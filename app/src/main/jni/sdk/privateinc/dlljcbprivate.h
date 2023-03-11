#ifndef _DLLJCBPRIVATE_H_
#define _DLLJCBPRIVATE_H_



#define Undefined_Mode	0
#define EMV_Mode  		1
#define Magstripe_Mode	2
#define Legacy_Mode		3 





typedef struct{
	unsigned char method;
	unsigned char condition;
}CVMR;

typedef struct{
	CVMR CVRList[120];
	unsigned char CVRListLen;
	unsigned char CVM_X[6];
	unsigned char CVM_Y[6];
}CVMSTRCUT;


#define CVMR_UNKNOWN             0
#define CVMR_FAIL                1
#define CVMR_SUCCESS             2


#define GenerateAC1  1
#define GenerateAC2  2


#define MAXSCRIPTLEN 128


//taa result
#define TAA_OFFLINE 0x40
#define TAA_ONLINE  0x80
#define TAA_DENIAL  0x00


//dol
#define typeCDOL1       1
#define typeCDOL2       2
#define typeDDOL        3
#define typeTDOL        4


unsigned char jcb_CheckDataMissDDA(JCBTradeUnionStruct *tempApp_UnionStruct);
unsigned char jcb_CombineDDA_FirstHalf(JCBTradeUnionStruct *tempApp_UnionStruct);

//9C
#define NEW_TRANS_GOODS  	0x00
#define NEW_TRANS_CASH   	0x01
#define NEW_TRANS_CASHBACK 	0x09
#define NEW_TRANS_REFUND	0x20

#endif

