/*
 ***************** File mfm814x250m.cpp ***********************
 *
 * BRD Driver for FM814x250M 
 * Data Acquisition Daughter Boards
 *
 * (C) InSys by Ekkore. Aug. 2011
 *
 ******************************************************
*/

#define	SIDE_API_EXPORTS
#include "mfm814x250m.h"

#define	CURRFILE _BRDC("MFM814x250M")

//******************************************
Cmfm814x250m::Cmfm814x250m() :
	CAdcSubModule()
{
	for(int i = 0; i < MAX_ADC_SRV; i++)
		m_pAdcSrv[i] = NULL;
	m_pIniData = NULL;
	m_nVersion = 0x10;
}

//******************************************
Cmfm814x250m::Cmfm814x250m( BRD_InitData *pBrdInitData, long sizeInitData ) :
	CAdcSubModule( pBrdInitData, sizeInitData )
{
	long		size = sizeInitData;
	INIT_Data	*pInitData = (INIT_Data*)pBrdInitData;

	for(int i = 0; i < MAX_ADC_SRV; i++)
		m_pAdcSrv[i] = NULL;
	m_nVersion = 0x10;
	m_pIniData = new ADM_Cfg;

	//
	// Извлечь из BRD.INI имя диалоговой DLL и другие параметры
	//
	SetDlgDLLName( m_pIniData->sDlgDllName, pBrdInitData, size );

	FindKeyWord( _BRDC("adccnt"),  m_pIniData->nAdcCnt, pInitData, size );
	FindKeyWord( _BRDC("version"), m_pIniData->nVersion, pInitData, size );
	FindKeyWord( _BRDC("inptype"), m_pIniData->nInpType, pInitData, size );
	if( m_pIniData->nInpType != 1 )
		m_pIniData->nInpType = 0;


	//
	// Извлечь из BRD.INI стандартные параметры АЦП (6 штук):
	// Bits, Encoding, MinRate, MaxRate, InpRange, FifoSize (см. adm2ifcfg.h)
	//
	GetAdcIni( &m_pIniData->rAdcCfg, pInitData, size );
}

//******************************************
Cmfm814x250m::~Cmfm814x250m()
{
	for(int i = 0; i < MAX_ADC_SRV; i++)
	{
		if( m_pAdcSrv[i] )
			delete m_pAdcSrv[i];
	}
	if( m_pIniData )
		delete m_pIniData;
}

//***************************************************************************************
S32 Cmfm814x250m::Init(PUCHAR pSubmodEEPROM, PTSTR pIniString)
{
	S32				err = BRDerr_OK;
	ICR_CfgAdm		cfgAdm;
	ICR_CfgAdc		cfgAdc;
	ICR_CfgAdcFifo	cfgAdcFifo;

	 err = CSubModule::Init( pSubmodEEPROM, pIniString );

	memset( &cfgAdm, 0xff, sizeof(ICR_CfgAdm) );
	memset( &cfgAdc, 0xff, sizeof(ICR_CfgAdc) );
	memset( &cfgAdcFifo, 0xff, sizeof(ICR_CfgAdcFifo) );
	
	if(pSubmodEEPROM)
	{
		ULONG	admCfgSizeb = *((PUSHORT)pSubmodEEPROM + 2);

		// Извлечение из ICR'а нужных структур,
		// а также заполнение переменных-членов m_nVersion и m_nSubunitId
		GetICR( pSubmodEEPROM, admCfgSizeb, &cfgAdm, &cfgAdc, &cfgAdcFifo );
	}

	m_puCnt = 0;

	//
	// Заполнение структуры m_rSubCfg на основе данных из
	// ICR'а и файла BRD.INI
	//
	err = SetConfig( &cfgAdm, &cfgAdc, &cfgAdcFifo );

	// 
	// Формирование службы m_pAdcSrv и занесение ее в список служб m_SrvList
	//
	err = SetServices();

	return SUBERR(err);
}

//***************************************************************************************
void Cmfm814x250m::GetDeviceInfo(BRD_Info* pInfo)
{
	pInfo->boardType	= (m_Type << 16) | (m_nVersion & 0xFFFF);	
	pInfo->boardType	= m_Type;
	pInfo->pid			= m_PID;
	BRDC_strcpy( pInfo->name, _BRDC("FM814x250M") );
	pInfo->verMajor		= VER_MAJOR;	
	pInfo->verMinor		= VER_MINOR;
}

//***************************************************************************************
void Cmfm814x250m::GetICR( void *pCfgMem, ULONG nRealCfgSize, 
				ICR_CfgAdm     *pCfgAdm, 
				ICR_CfgAdc     *pCfgAdc, 
				ICR_CfgAdcFifo *pCfgAdcFifo)
{
	void	*pEndCfgMem = (PVOID)((PUCHAR)pCfgMem + nRealCfgSize);
	int		endFlag = 0;

	//
	// Вызывается только из Cmfm814x250m::Init()
	//
	// Поиск в ICR'е базового модуля описания
	// 1. внешнего FIFO для АЦП (ADC_FIFO_TAG=0x200)
	//
	for( endFlag = 0; (0==endFlag) && (pCfgMem < pEndCfgMem); )
	{
		USHORT sign = *((USHORT*)pCfgMem);
		USHORT size = 0;

		switch(sign)
		{
			case END_TAG:
			case ALT_END_TAG:
				size = 2;
				endFlag = 1;
				break;
			case ADC_FIFO_TAG:
			{
				ICR_CfgAdcFifo	*pAdcFifo;
				
				pAdcFifo = (ICR_CfgAdcFifo*)pCfgMem;
				memcpy( pCfgAdcFifo, pAdcFifo, sizeof(ICR_CfgAdcFifo) );
				size = sizeof(ICR_CfgAdcFifo);
				break;
			}
			default:
				size = *((USHORT*)pCfgMem + 1);
				size += 4;
				break;
		}
		pCfgMem = (PUCHAR)pCfgMem + size;
	} 

	//
	// Поиск в ICR'е субмодуля описания 
	//  1. корневой структуры субмодуля      (ADM_ID_TAG = 0x80)
	//  2. специфической структуры субмодуля (ADM_CFG_TAG = 0x90)
	//  3. стандартной структуры АЦП         (ADC_CFG_TAG = 0x100)
	//
	for( endFlag = 0; (0==endFlag) && (pCfgMem < pEndCfgMem); )
	{
		USHORT sign = *((USHORT*)pCfgMem);
		USHORT size = 0;

		switch(sign)
		{
			case END_TAG:
			case ALT_END_TAG:
				endFlag = 1;
				break;
			case ADM_ID_TAG:
			{
				//
				// Извлечение из ICR корневой структуры субмодуля
				// (см. icr.h)
				ICR_IdAdm	*pAdmId;

				pAdmId = (ICR_IdAdm*)pCfgMem;
				nRealCfgSize = pAdmId->wSizeAll; // это ненужная операция?
				m_nVersion = pAdmId->bVersion;
				m_nSubunitId = pAdmId->wType;
				size = sizeof(ICR_IdAdm);
				break;
			}
			case ADM_CFG_TAG:
			case 0x93:			// Старый ткг от FM814x125M
			{
				//
				// Извлечение из ICR специфической структуры субмодуля
				// (см. icr009D.h)
				ICR_CfgAdm *pAdmCfg;
				
				pAdmCfg = (ICR_CfgAdm*)pCfgMem;
				memcpy(pCfgAdm, pAdmCfg, sizeof(ICR_CfgAdm));

				pCfgAdm->wTag = ADM_CFG_TAG;
				if( pCfgAdm->bInpType != 1 )
					pCfgAdm->bInpType = 0;

				size = pAdmCfg->wSize;
				size += 4;
				break;
			}
			case ADC_CFG_TAG:
			{
				//
				// Извлечение из ICR стандартной структуры АЦП
				// (см. icr.h)
				ICR_CfgAdc *pAdc;
				
				pAdc = (ICR_CfgAdc*)pCfgMem;
				memcpy(pCfgAdc, pAdc, sizeof(ICR_CfgAdc));
				size = sizeof(ICR_CfgAdc);
				break;
			}
			default:
					size = *((USHORT*)pCfgMem + 1);
					size += 4;
					break;
		}
		pCfgMem = (PUCHAR)pCfgMem + size;
	}
}

//***************************************************************************************
S32		Cmfm814x250m::SetConfig( ICR_CfgAdm *pCfgAdm,
							  ICR_CfgAdc *pAdc,
							  ICR_CfgAdcFifo *pAdcFifo )
{
	//
	// Вызывается только из Cmfm814x250m::Init()
	//
	// Заполнение структуры m_rSubCfg на основе данных из
	// ICR'а и файла BRD.INI
	//

	//
	// Заполняем значениями по умолчанию
	//
	memcpy(&m_rSubCfg, &AdmCfg_dflt, sizeof(ADM_Cfg));

	//
	// Заполняем параметрами из ICR'а
	//
	if( pCfgAdm->wTag == ADM_CFG_TAG )
	{	
		m_rSubCfg.nAdcCnt    = pCfgAdm->bAdcCnt;
		m_rSubCfg.nVersion   = m_nVersion;			// версия субмодуля
		m_rSubCfg.nAdcType   = pCfgAdm->bAdcType;	// тип кристалла АЦП: 0-всегда
		m_rSubCfg.nDacType   = pCfgAdm->bDacType;	// тип кристалла ЦАП: 0-всегда
		m_rSubCfg.nDacRange  = pCfgAdm->wDacRange;	// шкала преобразования ЦАП: (мВ)
		m_rSubCfg.nGenType   = pCfgAdm->bGenType;	// тип кристалла внутр. генератора: 0-не програм-ый, 1-Si571
		m_rSubCfg.nGenAdr    = pCfgAdm->bGenAdr;	// адресный код внутр. генератора: 0x49 по умолчанию
		m_rSubCfg.nGenRef    = pCfgAdm->nGenRef;    // заводская установка частоты внутр. генератора (Гц)
		m_rSubCfg.nGenRefMax = pCfgAdm->nGenRefMax; // максимальная частота внутр. генератора (Гц)
		m_rSubCfg.nInpType   = pCfgAdm->bInpType; 
	}

	//
	// Заполняем параметрами из BRD.INI
	//
	if(m_pIniData)
	{	
		if( BRDC_strlen(m_pIniData->sDlgDllName) )
			BRDC_strcpy(m_rSubCfg.sDlgDllName, m_pIniData->sDlgDllName);
		if( m_pIniData->nAdcCnt != 0xffffffff )
			m_rSubCfg.nAdcCnt = m_pIniData->nAdcCnt;
		if( m_pIniData->nVersion != 0xffffffff )
			m_rSubCfg.nVersion = m_pIniData->nVersion;
		if( m_pIniData->nInpType != 0xffffffff )
			m_rSubCfg.nInpType = m_pIniData->nInpType;
	}

	//
	// Заполнение структуры m_rSubCfg.rAdcCfg на основе данных из
	// ICR'а и файла BRD.INI
	//
	if(m_pIniData == NULL)
		SetAdcConfig(&m_rSubCfg.rAdcCfg, pAdc, pAdcFifo, pCfgAdm->wTag == ADM_CFG_TAG, NULL);
	else
		SetAdcConfig(&m_rSubCfg.rAdcCfg, pAdc, pAdcFifo, pCfgAdm->wTag == ADM_CFG_TAG, &m_pIniData->rAdcCfg);

	return BRDerr_OK;
}

//***************************************************************************************
S32 Cmfm814x250m::SetServices()
{
	//
	// Вызывается только из Cmfm814x250m::Init()
	//
	// Поместить недосформированную службу "FM814x250M" в контейнер m_SrvList
	//
//	if(m_rSubCfg.nAdcCnt)
//	{
	for(int i = 0; i < MAX_ADC_SRV; i++)
	{
		m_pAdcSrv[i] = new CFm814x250mSrv (-1, -1, this, NULL);
		m_SrvList.push_back(m_pAdcSrv[i]);
	}	

	return BRDerr_OK;
}

//***************************************************************************************
void Cmfm814x250m::SetSrvList( DEV_CMD_ServListItem *paServList )
{
	//
	// Досформировать службу "FM814X250M0", 
	// которая лежит в контейнере m_SrvList[0]
	//
	int srvSize = (int)m_SrvList.size();

	for(int i = 0; i < srvSize; i++)
	{
		// Присвоить службе ее новое имя и ее индекс в базовом списке служб
		m_SrvList[i]->SetName( paServList[i].name );
		m_SrvList[i]->SetIndex( paServList[i].idxMain );

		FM814X250MSRV_Cfg rSrvCfg;

		rSrvCfg.AdcCfg.BaseRefGen[0]	= m_AdmIfCfg.RefGen[0];
		rSrvCfg.AdcCfg.BaseRefGen[1]	= m_AdmIfCfg.RefGen[1];
		rSrvCfg.AdcCfg.SysRefGen		= m_AdmIfCfg.SysRefGen;
		rSrvCfg.AdcCfg.BaseExtClk		= m_AdmIfCfg.ExtClk;
		rSrvCfg.AdcCfg.RefPVS	=	m_AdmIfCfg.RefPVS;
		BRDC_strcpy(rSrvCfg.AdcCfg.DlgDllName, m_rSubCfg.sDlgDllName);

		rSrvCfg.AdcCfg.isAlreadyInit = 0;
		rSrvCfg.AdcCfg.MaxChan	= m_rSubCfg.nAdcCnt;
		rSrvCfg.nVersion		= m_rSubCfg.nVersion;	
		rSrvCfg.nAdcType		= m_rSubCfg.nAdcType;	
		rSrvCfg.nDacType		= m_rSubCfg.nDacType;	
		rSrvCfg.nDacRange		= m_rSubCfg.nDacRange;	
		rSrvCfg.nGenType		= m_rSubCfg.nGenType;	
		rSrvCfg.nGenAdr			= m_rSubCfg.nGenAdr;	
		rSrvCfg.nGenRef	    	= m_rSubCfg.nGenRef;    
		rSrvCfg.nGenRefMax	 	= m_rSubCfg.nGenRefMax; 
		rSrvCfg.nInpType		= m_rSubCfg.nInpType;	

		// Скопировать все 6 полей из одной структуры в другую
		SetAdcCfg(&rSrvCfg.AdcCfg, &m_rSubCfg.rAdcCfg);

		// Создать в службе разделяемую память с указателем void *CServise::m_pConfig
		// и скопировать в нее целиком структуру rSrvCfg
		m_SrvList[i]->SetCfgMem(&rSrvCfg, sizeof(FM814X250MSRV_Cfg));

		// Эта операция мне не понятна
#ifndef __linux__
		m_SrvList[i]->SetPropDlg(SIDE_propDlg);
#endif
	}
}

//***************** End of file mfm814x250m.cpp *****************
