//****************** File Icr008B.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодуля ADM28x800M
//
//*********************************************************

#ifndef _ICR008B_H
 #define _ICR008B_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x008B

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg008B {
	USHORT	wTag;			// тэг структуры (ADM_CFG_TAG)
	USHORT	wSize;			// размер всех следующих полей структуры
	UCHAR	bAdmIfNum;		// номер интерфейса ADM
	ULONG	dMainGen;		// значение основного опорного генератора
	ULONG	dMaxFreqVco;	// максимальная частота дополнительного ГУН
	ULONG	dMinFreqVco;	// минимальная частота дополнительного ГУН
	UCHAR	bIsClbrValue;	// наличие калибровочных коэффициентов
	UCHAR	bAdcCnt;		// количество АЦП
} ICR_Cfg008B, *PICR_Cfg008B, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR008B_H

// ****************** End of file Icr008B.h **********************
