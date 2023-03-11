#ifndef _DLLJCB_H_
#define _DLLJCB_H_

#include "dllemvbase.h"

#define JCB_READAIDPARAMETERS		51
#define JCB_EXTRACTMATCH			52
#define JCB_PARTICALMATCH			53
#define JCB_NOTMATCH 				54
#define JCB_ERR_CVMFailALWAYS		55
#define JCB_ERR_CVMNoSupport		56
#define JCB_ERR_CVMFail				57
#define JCB_ERR_EMV_ScriptFormat	58
#define JCB_ERR_NEEDREVERSAL		59
#define JCB_ERR_EC_QUANCUNERR		60
#define JCB_ERR_READCARDAGAIN		61




//=电子现金 NowTask ectrade huxiang add for EC-------
#define EMV_EC_CASH                             0x2000                                          //现金充值
#define EMV_EC_QCACC                            0x2001                                          //指定账户圈存
#define EMV_EC_QCACCNO                  0x2002                                          //非指定账户圈存
#define EMV_READLOG                     0x2003                                          //读日志


#define APDU_PPSE 0x01
#define APDU_SELECT 0x02
#define APDU_GPO	0x03
#define APDU_READRECORD 0x04

#define APDU_CLEARLOG       0x00
#define APDU_IDLE           0x10

/*============ jcb zwb 180101===============*/
#define JCB_OPS_STATUS_APPROVED   1
#define JCB_OPS_STATUS_ONLINE     2
#define JCB_OPS_STATUS_ONLINE_TWOPRESENTMENTS		3
#define JCB_OPS_STATUS_ONLINE_PREMENTANDHOLD		4
#define JCB_OPS_STATUS_DECLINED   5
#define JCB_OPS_STATUS_TRYANOTHERINTERFACE     6
#define JCB_OPS_STATUS_ENDAPPLICATION 	7
#define JCB_OPS_STATUS_ENDAPPLICATION_COMMUNICATIONERROR 	8
#define JCB_OPS_STATUS_ENDAPPLICATION_ONDEVICECVM 	9
#define JCB_OPS_STATUS_SELECTNEXT 		0x0A
#define JCB_OPS_STATUS_NA 		0x0F

#define JCB_OPS_START_A  0x00
#define JCB_OPS_START_B  0x01
#define JCB_OPS_START_C  0x02
#define JCB_OPS_START_D  0x03
#define JCB_OPS_START_NA 0x0F  

#define JCB_OPS_CVM_NOCVMREQ        0
#define JCB_OPS_CVM_OBTAINSIGNATURE 1
#define JCB_OPS_CVM_ONLINEPIN       2
#define JCB_OPS_CVM_CONFVERIFIED    3
#define JCB_OPS_CVM_NA              0x0F

#define JCB_CVMRESULT_DEFAULT  0X3F
#define JCB_CVMRESULT_NOCVMPERFORMED 0x1F
#define JCB_CVMRESULT_SIGBATURE 0x1E
#define JCB_CVMRESULT_ONLINEPIN 0x02
#define JCB_CVMRESULT_PLAINTEXTPINVERIFICATION 0x01


#define JCB_CVMRESULT_UNKNOWN 0x00
#define JCB_CVMRESULT_SUCCESSFUL 0x02

#define JCB_USERREQDATA_MSGID_APPROVED        0x03
#define JCB_USERREQDATA_MSGID_APPROVEDSIGN    0x1A   //26
#define JCB_USERREQDATA_MSGID_AUTHORISINGPLSWAIT  0x1B   // 27
#define JCB_USERREQDATA_MSGID_PLSENTERPIN  0x09   // 9
#define JCB_USERREQDATA_MSGID_NOTAUTHORISED     0x07
#define JCB_USERREQDATA_MSGID_INSERTCARD      0x1D   // 29
#define JCB_USERREQDATA_MSGID_TRYAGAIN        0x21   //33
#define JCB_USERREQDATA_MSGID_SEEPHONE        0x20   // 32
#define JCB_USERREQDATA_MSGID_PROCESSING        0x16   // 22
#define JCB_USERREQDATA_MSGID_CARDREADOK      0x17    // 23
#define JCB_USERREQDATA_MSGID_PRESENT_ONECARD	0x19
#define JCB_USERREQDATA_MSGID_PRESENTCARD	0x15
#define JCB_USERREQDATA_MSGID_ERROR_TRYOTHERCARD	 0x1C   // 28
//#define JCB_USERREQDATA_MSGID_CLEARDISP       0x1E   // 30

//#define JCB_USERREQDATA_STATUS_NOTREADY    0
//#define JCB_USERREQDATA_STATUS_IDLE        1
#define JCB_USERREQDATA_STATUS_READYTOREAD 2
#define JCB_USERREQDATA_STATUS_PROCESSING  3
#define JCB_USERREQDATA_STATUS_CARDREADSUCCESS 4
#define JCB_USERREQDATA_STATUS_PROCESSINGERR   5
#define JCB_USERREQDATA_STATUS_COLLISION_DETECTED 6

#define JCB_USERREQDATA_VALUEQUALIFIER_NA	0
#define JCB_USERREQDATA_VALUEQUALIFIER_BALANCE 1
/*================end==================*/




typedef struct _JCBOUTPARAMETERSET
{
	unsigned char OutcomeParSet[8+1];
    unsigned char status;
    unsigned char start;
    unsigned char CVM;
	unsigned char OnlineResponseData;
    unsigned char UIRequestOnOutPresent;
    unsigned char UIRequestOnRestartPresent;
    unsigned char DataRecordPresent;
    unsigned char DisDataPresent;
	unsigned char AlternateInterfacePreference;
    unsigned char Receipt;
    unsigned char FieldOffRequest;
    unsigned char RemovalTimeout[2];
	unsigned char opssendflag;
}JCBOUTPARAMETERSET;

typedef struct _JCBUSERINTERFACERESTARTDATA
{
    unsigned char MessageID;
    unsigned char Status;
    unsigned char sendMsgFlag;
}JCBUSERINTERFACERESTARTDATA;

typedef struct _JCBUSERINTERFACEREQDATA
{
    unsigned char MessageID;
    unsigned char Status;
    unsigned char HoldTime;
    unsigned char ValueQualifier;
    unsigned char value[6];
    unsigned char CurrencyCode[2];
    unsigned char sendMsgFlag;
}JCBUSERINTERFACEREQDATA;

typedef struct _JCBUSERINTERFACEREQUESTDATA
{
    unsigned char MessageID;
    unsigned char Status;
	unsigned char RemovalTimeout[2];
    unsigned char sendMsgFlag;
}JCBUSERINTERFACEREQUESTDATA;

typedef struct
{
	EMVBASE_CAPK_STRUCT *CAPK;
    EMVBASE_LISTAPPDATA *SelectedApp;
	EMVBASE_LISTAPPDATA *AppListCandidate;
	unsigned char AppListCandidatenum;
	
	unsigned char bPrintReceipt:1;
	unsigned char bErrSDATL:1;
	unsigned char bErrAuthData:1;
	unsigned char bCDOL1HasNoUnpredictNum:1;
	unsigned char bCDOL2HasNoUnpredictNum:1;

    unsigned char bAbleOnline:1;
	unsigned char bReversal:1;
	unsigned char bAdvice:1;
	unsigned char IfNeedRefer:1;
	
    unsigned char JcbIUPperformed:1;
	unsigned char startdealiup:1;
    unsigned char canceldealiup:1;

	unsigned short AuthDataMaxLen;
	unsigned short AuthDataLen;
    unsigned char *AuthData;  //SM 算法的时候这里可能缺资源,使用AuthData
    unsigned char IPKModulMaxLen;
	unsigned char IPKModulLen;
	unsigned char *IPKModul;       //zcl 2048 - > 248
	unsigned char ICCPKModulMaxLen;
    unsigned char ICCPKModulLen;
	unsigned char *ICCPKModul;
	
	unsigned char ICCPINPKModulLen;
	unsigned char *ICCPINPKModul;
	
	unsigned short RedundantDataMaxLen;
	unsigned short RedundantDataLen;
	unsigned char *RedundantData;
	unsigned char TransResult;
	unsigned char qPBOCOrMSD;
	unsigned char SelectedAppNo;
	unsigned char JcbDRRI;
	unsigned char onlinetradestatus;
	unsigned char TransResultBak;
	unsigned char GenerateACRetDataLen;
	unsigned char GenerateACRetData[255];

	unsigned char JcbTransactionMode;
	unsigned char JcbImplementationOption;//1111 1000 EMV/Legacy Mode 1.Offline Data Authentication 2.Exception File Check 3.Issuer Update
	unsigned char JcbIsLegacyCard;
	unsigned char JcbIssuerUpdate;
	unsigned char JcbTornTransactionFlag;
	unsigned char JcbTornTrack2Len;
	unsigned char JcbTornTrack2Data[19];
	unsigned char JcbTornCDAHashDatalen;
	unsigned char JcbTornCDAHashDataBuffer[507];
	unsigned char JcbCandidateListEmptyFlag; //for jcb contactless terminal spec V1.3
	unsigned char JcbCVMParameter;
	unsigned char secondtap;
	JCBOUTPARAMETERSET *pJcbOutParameterSet;
	JCBUSERINTERFACEREQDATA *pJcbUserInterfaceReqData;
	JCBUSERINTERFACERESTARTDATA *pJcbUserInterfaceRestartData;
	JCBUSERINTERFACEREQUESTDATA *pJcbUserInterfaceRequestData;
}JCBTRADEPARAMETER;;


typedef unsigned char (*JCB_InputCreditPwd)(unsigned char pintype,unsigned char *ASCCreditPwd);
typedef void (*JCB_RandomNum)(unsigned char *RandomNum,unsigned int RandomNumLen);
typedef unsigned char (*JCB_VerifyCardNo)(unsigned char *asPAN);


typedef void (*JCB_SendOutComeParameterSet)(JCBTRADEPARAMETER *EMVTradeParam,unsigned char status);
typedef void (*JCB_senduserinterfacerequestdata)(JCBTRADEPARAMETER *EMVTradeParam,unsigned char step);
typedef unsigned char (*JCB_Dispuserinterfacerequestdata)(JCBTRADEPARAMETER *EMVTradeParam,unsigned char step, JCBUSERINTERFACEREQDATA *tempuserinterfacereqdata);
typedef void (*JCB_SendUserInterfaceRestartData)(JCBTRADEPARAMETER *EMVTradeParam,unsigned char step);
typedef unsigned char (*JCB_DispUserInterfaceRestartData)(JCBTRADEPARAMETER *EMVTradeParam,unsigned char step, JCBUSERINTERFACERESTARTDATA *tempuserinterfacereqdata);
typedef void (*JCB_SendUserInterfaceRequestParameter)(JCBTRADEPARAMETER *EMVTradeParam,unsigned char step);
typedef unsigned char (*JCB_DispUserInterfaceRequestParamere)(JCBTRADEPARAMETER *EMVTradeParam,unsigned char step, JCBUSERINTERFACEREQUESTDATA *tempuserinterfacerequestdata);
typedef unsigned char (*JCB_CheckCAPKExit)(JCBTRADEPARAMETER *EMVTradeParam);

typedef struct{

    JCBTRADEPARAMETER *EMVTradeParam;
    
	unsigned char *pasTradeAmount;
	EMVBASE_ContactlessIsoCommand IsoCommand;
	JCB_InputCreditPwd EMVB_InputCreditPwd;

	JCB_RandomNum EMVB_RandomNum;

	EMVBase_termipkrevokecheck termipkrevokecheck;
	EMVBASE_CheckMatchTermAID CheckMatchTermAID;
 	EMVBASE_ReadTermAID ReadTermAID;

	JCB_VerifyCardNo VerifyCardNo;
	JCB_CheckCAPKExit CheckCapkExit;
	JCB_SendOutComeParameterSet SendOutComeParameterSet;
	JCB_senduserinterfacerequestdata senduserinterfacerequestdata;
	JCB_Dispuserinterfacerequestdata Dispuserinterfacerequestdata;
	JCB_SendUserInterfaceRestartData SendUserInterfaceRestartData;
	JCB_DispUserInterfaceRestartData DispUserInterfaceRestartData;
	JCB_SendUserInterfaceRequestParameter SendUserInterfaceRequestParameter;
	JCB_DispUserInterfaceRequestParamere DispUserInterfaceRequestParamer;
	
}JCBTradeUnionStruct;

typedef struct _JCBTornRecoveryContext
{
	unsigned char RecoveryTornEMVTransactionFlag;
	unsigned char TornTrack2len;
	unsigned char TornTrack2Data[19];
	unsigned char TornCDAHashDatalen;
	unsigned char TornCDAHashDataBuffer[507];
	unsigned char ReferenceControlParameter;
	unsigned char UnpredictNum[4];
	unsigned char crc;
}JCBTORNRECOVERYCONTEXT;


extern unsigned char jcb_FinalSelectedApp(JCBTradeUnionStruct *App_UnionStruct);
extern unsigned char jcb_InitialApp(JCBTradeUnionStruct *tempApp_UnionStruct);
extern unsigned char jcb_RebuildApplist(JCBTradeUnionStruct *App_UnionStruct);
extern unsigned char jcb_ChooseApp(JCBTradeUnionStruct *App_UnionStruct);
extern unsigned char jcb_ReadAppData(JCBTradeUnionStruct *tempApp_UnionStruct);
extern unsigned char jcb_ProcessRestrict(JCBTradeUnionStruct * tempApp_UnionStruct);
extern unsigned char jcb_DataAuth(JCBTradeUnionStruct *tempApp_UnionStruct);
extern void jcb_GetLibVersion(unsigned char *version);

#endif


