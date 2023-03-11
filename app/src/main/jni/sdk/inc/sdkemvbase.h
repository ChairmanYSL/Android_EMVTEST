#ifndef SDKEMVBASE_H
#define SDKEMVBASE_H

#include "sdktypedef.h"

#ifndef SDK_EMVBASE_EMV_OK_BASE
#define SDK_EMVBASE_EMV_OK_BASE (300)
#endif

#ifndef SDK_EMVBASE_EMV_ERR_BASE
#define SDK_EMVBASE_EMV_ERR_BASE (-300)
#endif


typedef enum
{
	//max up to SDK_EMVBASE_EMV_ERR_BASE - 50
	SDK_EMV_UseOtherCard =          (SDK_EMVBASE_EMV_ERR_BASE - 31),
	SDK_EMV_ENDAPPLICATION = 		(SDK_EMVBASE_EMV_ERR_BASE - 30),
    SDK_EMV_TORN =  				(SDK_EMVBASE_EMV_ERR_BASE - 29),
    SDK_EMV_TransTryAgain =  		(SDK_EMVBASE_EMV_ERR_BASE - 28),
	SDK_EMV_UserSelect =            (SDK_EMVBASE_EMV_ERR_BASE - 27),
    SDK_EMV_ReadLogEnd =            (SDK_EMVBASE_EMV_ERR_BASE - 26),
    SDK_EMV_SeePhone =          	(SDK_EMVBASE_EMV_ERR_BASE - 25),
    SDK_EMV_CardInException =       (SDK_EMVBASE_EMV_ERR_BASE - 24),
	SDK_EMV_PPSERev6A82 = 			(SDK_EMVBASE_EMV_ERR_BASE - 23),
	SDK_EMV_PPSENotFound =       	(SDK_EMVBASE_EMV_ERR_BASE - 22),
	SDK_EMV_NoLog =              	(SDK_EMVBASE_EMV_ERR_BASE - 21),
    SDK_EMV_FixedAidAbsent =     	(SDK_EMVBASE_EMV_ERR_BASE - 20),
    SDK_EMV_SwitchInterface =		(SDK_EMVBASE_EMV_ERR_BASE - 19),
    SDK_EMV_ReadCardAgain =   		(SDK_EMVBASE_EMV_ERR_BASE - 18),
    SDK_EMV_AppSelectTryAgain =  	(SDK_EMVBASE_EMV_ERR_BASE - 17),
    SDK_EMV_InitAppNotAccept =   	(SDK_EMVBASE_EMV_ERR_BASE - 16),
    SDK_EMV_AppBlock =           	(SDK_EMVBASE_EMV_ERR_BASE - 15),
    SDK_EMV_NeedMagCard =        	(SDK_EMVBASE_EMV_ERR_BASE - 14),
    SDK_EMV_AppTimeOut =         	(SDK_EMVBASE_EMV_ERR_BASE - 13),
    SDK_EMV_CardBlock =          	(SDK_EMVBASE_EMV_ERR_BASE - 12),
    SDK_EMV_CancelTrans =        	(SDK_EMVBASE_EMV_ERR_BASE - 11),
    SDK_EMV_NotAccept =          	(SDK_EMVBASE_EMV_ERR_BASE - 10),
    SDK_EMV_IccDataRedund =      	(SDK_EMVBASE_EMV_ERR_BASE - 9),
    SDK_EMV_NoAppSel =           	(SDK_EMVBASE_EMV_ERR_BASE - 8),
    SDK_EMV_IccReturnErr =          (SDK_EMVBASE_EMV_ERR_BASE - 7),
    SDK_EMV_IccCommandErr =         (SDK_EMVBASE_EMV_ERR_BASE - 6),
    SDK_EMV_IccDataFormatErr =      (SDK_EMVBASE_EMV_ERR_BASE - 5),
    SDK_EMV_TransNotAccept = 		(SDK_EMVBASE_EMV_ERR_BASE - 4),
	SDK_EMV_TransTerminate =	   	(SDK_EMVBASE_EMV_ERR_BASE - 3),
    SDK_EMV_TransOnlineDecline =  	(SDK_EMVBASE_EMV_ERR_BASE - 2),
    SDK_EMV_TransOfflineDecline = 	(SDK_EMVBASE_EMV_ERR_BASE - 1),


    SDK_EMV_TransOfflineApprove =   (SDK_EMVBASE_EMV_OK_BASE + 1),
    SDK_EMV_TransOnlineApprove =    (SDK_EMVBASE_EMV_OK_BASE + 2),
    SDK_EMV_TransOnlineWait =       (SDK_EMVBASE_EMV_OK_BASE + 3),
    SDK_EMV_TransReadAppDataComplete = (SDK_EMVBASE_EMV_OK_BASE + 4),
    //max up to SDK_EMVBASE_EMV_OK_BASE + 50
}SDK_EMVBASE_RLT;



typedef struct
{
    unsigned char AidLen;
    unsigned char Aid[16];		//5-16
    unsigned char Asi;			//Application Selection Indicator.0-needn't match exactly(partial match up to the length);1-match exactly
    unsigned char AppVerNum[2];
    unsigned char TacDefault[5];
    unsigned char TacOnline[5];
    unsigned char TacDecline[5];
    unsigned char FloorLimit[4];
    unsigned char Threshold[4];
    unsigned char MaxTargetPercent;
    unsigned char TargetPercent;
    unsigned char TermDDOLLen;
    unsigned char TermDDOL[128];
    unsigned char TermPinCap;
    unsigned char ectranslimit[6];		//for contact PBOC use only - low value payment - electronic cash
    unsigned char cl_cvmlimit[6];
    unsigned char cl_translimit[6];
    unsigned char cl_offlinelimit[6];

	unsigned char transvaule;		//if for all transcation type set transvaule 0xFF, otherwise set transvaule with tag9C
	unsigned char contactorcontactless;	//if contactorcontactless is 0,for both contact and contactless; if is 1,for contact only; if is 2,for contactless only
}SDK_EMVBASE_AID_STRUCT;

typedef struct
{
    unsigned char RID[5];
    unsigned char CAPKI;
    unsigned char HashInd;
    unsigned char ArithInd;
    unsigned char ModulLen;
    unsigned char Modul[248];
    unsigned char ExponentLen;
    unsigned char Exponent[3];
    unsigned char CheckSumLen;
    unsigned char CheckSum[20];
    unsigned char ExpireDate[4];
}SDK_EMVBASE_CAPK_STRUCT;


typedef struct{
	unsigned char aidlen;
	unsigned char aid[16];
	unsigned char kernelidlen;
	unsigned char kernelid[8];
}SDK_EMVBASE_CL_HIGHESTAID;


typedef struct
{
    unsigned short LenOut;
    unsigned char DataOut[256];
    unsigned char SW1;
    unsigned char SW2;
}SDK_EMVBASE_LOGDATA;


typedef struct
{
    unsigned char ucAidLen;
    unsigned char asAid[32];    //5-16
    unsigned char ucAppLabelLen;
    unsigned char asAppLabel[32];
    unsigned char ucPreferNameLen;
    unsigned char asPreferName[32];
    unsigned char ucPriority;     //tag'87'
    unsigned char ucLangPreferLen;
    unsigned char asLangPrefer[16];    //2-8
    unsigned char ucIcti;    	 //Issuer Code Table Index.lang used for display app list according to ISO8859.but not include Chinese,Korea,etc.
    unsigned char ucLocalName;   //If display app list using local language considerless of info in card.0-use card info;1-use local language.
    unsigned char ucAppLocalNameLen;
    unsigned char asAppLocalName[32];
}SDK_EMVBASE_C_AIDLIST;


typedef struct
{
    unsigned char AIDLen;
    unsigned char AID[16];
    unsigned char AppLabelLen;
    unsigned char AppLabel[16];
    unsigned char PreferNameLen;
    unsigned char PreferName[16];
    unsigned char Priority;
    unsigned char LangPreferLen;
    unsigned char LangPrefer[8];
    unsigned char AidInTermLen;
    unsigned char AidInTerm[16];
    unsigned char KernelIdentifierLen;
    unsigned char KernelIdentifier[8];
}SDK_EMVBASE_CL_AIDLIST;


typedef enum
{
	SDKEMVBASE_CVM_NA=0,
	SDKEMVBASE_CVM_NOCVMREQ,
	SDKEMVBASE_CVM_OBTAINSIGNATURE,
	SDKEMVBASE_CVM_ONLINEPIN,
	SDKEMVBASE_CVM_CONFVERIFIED,
	SDKEMVBASE_CVM_MOBILE,
	SDKEMVBASE_CVM_PLAINTEXTPIN,
	SDKEMVBASE_CVM_ENCIPHEREDPIN,
 	SDKEMVBASE_CVM_PLAINTEXTPIN_AND_SIGN,
	SDKEMVBASE_CVM_ENCIPHEREDPIN_AND_SIGN,
	SDKEMVBASE_CVM_SKIPCVM,
}SDK_EMVBASE_CVM_RESULT;

extern s32 sdkemvbaseInitDDI(void);
extern void sdkEMVBase_SetEMVLogOutput(bool enable);

extern s32 sdkEMVBasePowerStartInit(void);
extern s32 sdkEMVBaseDelOneAID(const u8 *pheInAID, s32 siAIDLen);
extern void sdkEMVBaseDelAllAIDLists(void);
extern s32 sdkEMVBaseAddAnyAIDList(const SDK_EMVBASE_AID_STRUCT *pstInAIDList, s32 siAIDNum);
extern s32 sdkEMVBaseGetAnyAIDList(s32 siStartIndex, s32 siAIDNum, SDK_EMVBASE_AID_STRUCT *pstOutAIDList, s32 *psiOutAIDNum);
extern s32 sdkEMVBaseGetAIDListNum(s32 *psiInAIDListNum);
extern s32 sdkEMVbaseGetMatchAID(const u8 *pheInAID, s32 siAIDLen, SDK_EMVBASE_AID_STRUCT *pstOutAID,s32 MaxOutAIDNum,s32 *psiOutAIDNum);

extern s32 sdkEMVBaseDelOneCAPK(const u8 *pheInRID, s32 siCAPKIndex);
extern void sdkEMVBaseDelAllCAPKLists(void);
extern s32 sdkEMVBaseAddAnyCAPKList(const SDK_EMVBASE_CAPK_STRUCT *pstInCAPKList, s32 siCAPKNum);
extern s32 sdkEMVBaseGetAnyCAPKList(s32 siStartIndex, s32 siCAPKNum, SDK_EMVBASE_CAPK_STRUCT *pstOutCAPKList, s32 *psiOutCAPKNum);
extern s32 sdkEMVBaseGetCAPKListNum(s32 *psiInCAPKListNum);
extern s32 sdkEMVBaseGetMatchCAPK(const u8 *pheInRID, s32 uiCAPKIndex, SDK_EMVBASE_CAPK_STRUCT *pstOutCAPK);

extern s32 sdkEMVBaseDelOneGMCAPK(const u8 *pheInRID, s32 siCAPKIndex);
extern void sdkEMVBaseDelAllGMCAPKLists(void);
extern s32 sdkEMVBaseAddAnyGMCAPKList(const SDK_EMVBASE_CAPK_STRUCT *pstInCAPKList, s32 siCAPKNum);
extern s32 sdkEMVBaseGetAnyGMCAPKList(s32 siStartIndex, s32 siCAPKNum, SDK_EMVBASE_CAPK_STRUCT *pstOutCAPKList, s32 *psiOutCAPKNum);
extern s32 sdkEMVBaseGetGMCAPKListNum(s32 *psiInCAPKListNum);
extern s32 sdkEMVBaseGetMatchGMCAPK(const u8 *pheInRID, s32 uiCAPKIndex, SDK_EMVBASE_CAPK_STRUCT *pstOutCAPK);
extern bool sdkEMVBaseCheckCAPKExit(u8 *RID, u8 capki);

extern s32 sdkEMVBaseTransInit();
extern s32 sdkEMVBaseEntryPoint(SDK_EMVBASE_CL_HIGHESTAID *tempHighestAID);

extern s32 sdkEMVBaseConfigTLV(const u8 *pheTag, const u8 *pheInData, s32 siInLen);
extern s32 sdkEMVBaseReadTLV(const u8 *pheTag, u8 *pheOutData, s32 *psiOutLen);
extern bool sdkEMVBaseCheckTagExit(u8 *tag);
extern s32 sdkEMVBaseDelTLV(const u8 *pheTag);

extern s32 sdkEMVBaseSetInputPINFun(s32(*fun_inputpin)(u8 ucIccEncryptWay, u8 ucPINTryCount, u8 *pheCreditPwd));
extern s32 sdkEMVBaseSetSpTermCheck(bool enable);		//default is false
extern s32 sdkEMVBaseGetScriptResult(s32 * psiScriptResultLen, u8 * pheScriptResult);
extern bool sdkEMVBaseNeedSignature();
extern s32 sdkEMVBaseGetEntryPointAIDList(SDK_EMVBASE_CL_AIDLIST *entrypointlist ,s32 silistnum ,s32 *psiOutlistnum);		//must call after sdkEMVBaseEntryPoint
extern s32 sdkEMVBaseSetUserSelcetFixedAid(u8 *aidincard, u8 aidlen);	//must call after sdkEMVBaseEntryPoint

extern s32 sdkEMVBaseSetRevocationKey(s32(*fun_cmprevocation)(u8 *key));

extern s32 sdkEMVBaseReSelectApp(SDK_EMVBASE_CL_HIGHESTAID *tempHighestAID);

extern s32 sdkEMVBaseSetTransAmount(u8 *bcdAmount);
extern s32 sdkEMVBaseSetTwoTransAmount(u8 *bcdAmount, u8 *bcdOtherAmount);
extern s32 sdkEMVBasePreTrans();
extern s32 sdkEMVBasePreTransGetTransType();

extern s32 sdkEMVBaseSelectFromTermSelect(u8 *aidnum);
extern s32 sdkEMVBaseGetFromTermSelectAIDList(SDK_EMVBASE_CL_AIDLIST *entrypointlist ,s32 silistnum ,s32 *psiOutlistnum);

extern SDK_EMVBASE_CVM_RESULT sdkEMVBaseGetCVMresult(void);

extern s32 sdkEMVBaseSetSupportExternSelect(u8 flag);


extern s32 sdkEMVBaseSetAppListCandicateMaxNum(u8 maxnum);


#endif

