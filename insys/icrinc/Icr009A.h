//****************** File Icr009A.h ***********************
//
//  Определения констант и структур 
//	для работы с конфигурационным ППЗУ
//  для субмодуля FM404V с тетстовой прошивкой
//
//*********************************************************

#ifndef _ICR009A_H
 #define _ICR009A_H

#pragma pack(push, 1)    

#define ADM_CFG_TAG 0x009A

// Конфигурационные параметры субмодуля
typedef struct _ICR_Cfg009A {
	U16	wTag;		// тэг структуры (ADM_CFG_TAG)
	U16	wSize;	// размер всех следующих полей структуры
	U08	bAdmIfNum;	// номер интерфейса ADM
	U08	bGen1Type;	// тип кристалла внутр. генератора G1: 0-не програм-ый, 1-Si571
	U08	bGen1Adr;	// адресный код внутр. генератора G1: 0x49 по умолчанию
	U32	nGen1Ref;   // заводская установка частоты внутр. генератора G1 (Гц)
	U32	nGen1RefMax; // максимальная частота внутр. генератора G1 (Гц)
	U32	nGen2Ref;		// значения опорного генератора G2 (Гц): 0-отсутствует
	U32	nGen3Ref;		// значения опорного генератора G3 (Гц): 0-отсутствует
	U08 reserve[9];	// резерв
} ICR_Cfg009A, *PICR_Cfg009A, ICR_CfgAdm, *PICR_CfgAdm;

#pragma pack(pop)    

#endif // _ICR009A_H

// ****************** End of file Icr009A.h **********************