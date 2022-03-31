//****************** File Icr009C.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодуля ЦАП FM214x2G5B (ДЖПД)
//
//*********************************************************

#ifndef _ICR009C_H
 #define _ICR009C_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x009C

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg009C {
	U16	wTag;		// тэг структуры (ADM_CFG_TAG)
	U16	wSize;		// размер всех следующих полей структуры
	U08	bAdmIfNum;	// номер интерфейса ADM
	U08	bSubType;		// тип субмодуля: 0-Standard 
	U08	bDacCnt;		// количество ЦАП: 2
	U08	bDacType;		// тип кристалла ЦАП: 0-нет, 1-AD57x4
	U32	dLpfCutoff;	// частота среза ФНЧ0 (Гц)
	U08	bOutResist;	// выходное сопротивление (0 - 50 Ом, 1 - 75 Ом)
	U08	reserve;		// резерв

	U08	bGenType;	// тип кристалла внутр. генератора G1: 0-не програм-ый, 1-Si571
	U08	bGenAdr;	// адресный код внутр. генератора G1: 0x49 по умолчанию
	U32	nGenRef;   // заводская установка частоты внутр. генератора G1 (Гц)
	U32	nGenRefMax; // максимальная частота внутр. генератора G1 (Гц)
	U08	bQmCnt;		// количество квадратурных модуляторов: 0, 1, 2
	U08	bQmType;		// тип кристалла квадратурных модуляторов: 0-нет, 1-есть
	U08	bBiasDacType;	// тип кристалла ЦАП смещения: 0-нет, 1-AD5621
	U08	bStartDacType;	// тип кристалла ЦАП старта: 0-нет, 1-AD5621

	U08	bIsExtClockContact;		// наличие разъема внешнего такта: 0-нет, 1-есть
	U08	bIsExtStartContact;		// наличие разъема внешнего старта: 0-нет, 1-есть
	U08	bIsIndustrial;	// индустриальное исполнение: 0-нет, 1-да
	U08	bPllType;		// тип кристалла синтезатора ФАПЧ: 0-нет, 1-ADF4350
	U32	nFreqDataMax; // максимальная частота потока данных - sampling rate (Гц)
} ICR_Cfg009C, *PICR_Cfg009C, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR009C_H

// ****************** End of file Icr009C.h **********************
