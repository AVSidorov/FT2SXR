//****************** File Icr0096.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодулей FM212x1G
//
//*********************************************************

#ifndef _ICR0096_H
 #define _ICR0096_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x0096

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg0096 {
	U16	wTag;		// тэг структуры (ADM_CFG_TAG)
	U16	wSize;		// размер всех следующих полей структуры
	U08	bAdmIfNum;	// номер интерфейса ADM
	U08	bAdcCnt;	// количество АЦП: 2
	U08	bAdcType;	// тип кристалла АЦП: 0-нет, 1-ADC10D1000, 1-ADC10D1500, 1-ADC12D1000, 1-ADC12D1600, 1-ADC12D1800,
	U08	bDacType;	// тип кристалла ЦАП: 0-нет, 1-AD57x4
	U16	wDacRange;	// шкала преобразования ЦАП: (мВ)
	U08	bGenType;	// тип кристалла внутр. генератора: 0-не програм-ый, 1-Si571
	U08	bGenAdr;	// адресный код внутр. генератора: 0x49 по умолчанию
	U32	nGenRef;    // заводская установка частоты внутр. генератора (Гц)
	U32	nGenRefMax; // максимальная частота внутр. генератора (Гц)
	U08	bSyntType;	// тип кристалла синтезатора: 0-отсутствует, 1-ADF4350
	U08	bAttType;	// тип кристалла аттенюатора: 0-отсутствует, 1-DAT-31R5
	U08	bTempType;	// тип кристалла датчика температуры: 0-отсутствует, 1-TMP442
	U08	bTempAdr;	// адресный код датчика температуры: 0x4C по умолчанию

	U08 reserve[20];// резерв для выравнивания

	S16	awRangeDeviation[2][2][4];	// отклонение ШП от номинала (разы) (default 10000)
									// [тип входа][номер АЦП][номер ШП]
	S16 awBiasDeviation[2][2][4];	// отклонения смещения нуля (разы) (default 0)
									// [тип входа][номер АЦП][номер ШП]
} ICR_Cfg0096, *PICR_Cfg0096, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR0096_H

// ****************** End of file Icr0096.h **********************
