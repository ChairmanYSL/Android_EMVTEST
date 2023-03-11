#include "dllemvbase.h"
#include "dlljcb.h"
#include "sdkemvbaseprivate.h"
#include "dlljcbprivate.h"
#include "sdkjcbprivate.h"
#include "sdkDebug.h"
#include "sdktypedef.h"
#include "sdkped.h"


JCBTRADEPARAMETER *gstJcbTradeParam = NULL;
JCBTradeUnionStruct *gstJcbTradeUnionStruct = NULL;

SDK_JCB_TRANSFLOW_MODE gstJCBTransFlowModeStatus;



typedef struct
{
    bool bIsForceOnline;    // force online for qpboc
    bool bIsForcePIN;       //force need online pin for china,overseas not need force online pin

    s32 (*ReadCard)();
    s32 (*DispRemoveCard)(void);        //display remove card
    s32 (*DispCardExpire)(void);        //dispplay card expire
    s32 (*DispOnlineODA)(u8 Authtype, bool result);     //dispplay online oda result
    s32 (*setBeforeGPO)(void);      //set something before GPO
    s32 (*VerifyCardNo)(u8* asPAN);//Exception File Check
    s32 (*PromptSoundAndLed)(int mode);//100ms 提示声音/灯
} SDK_JCB_TRADE_PARAM;

SDK_JCB_TRADE_PARAM *gstsdkJcbTradeTable;

s32 sdkJcbGetTransMode(void)
{
	return gstJCBTransFlowModeStatus;
}

s32 sdkJcbSetOption(u8 option)
{
	if(gstJcbTradeParam == NULL)
	{
		return SDK_PARA_ERR;
	}

	gstJcbTradeParam->JcbImplementationOption = option;
	return SDK_OK;
}

s32 sdkJcbSetForceOnline(bool bIsForceOnline)
{
	if(gstsdkJcbTradeTable == NULL)
	{
		return SDK_PARA_ERR;
	}

	gstsdkJcbTradeTable->bIsForceOnline = bIsForceOnline;
	if(bIsForceOnline)
	{
		emvbase_avl_settag(EMVTAG_bForceOnline, 0x01);
		emvbase_avl_setvalue_or(EMVTAG_TVR, 3, 0x08);
	}
	else
	{
		emvbase_avl_settag(EMVTAG_bForceOnline, 0x00);
		emvbase_avl_setvalue_and(EMVTAG_TVR, 3, 0xF7);
	}
	return SDK_OK;
}


s32 sdkJcbSetDispTapCardAgain(s32(*fun_setDispTapCardAgain)())
{
    if(gstsdkJcbTradeTable == NULL)
    {
        return SDK_PARA_ERR;
    }

    gstsdkJcbTradeTable->ReadCard = fun_setDispTapCardAgain;
    return SDK_OK;
}

s32 sdkJcbSetDispRemoveCard(s32(*fun_setdispremovecard)())
{
    if(gstsdkJcbTradeTable == NULL)
    {
        return SDK_PARA_ERR;
    }

    gstsdkJcbTradeTable->DispRemoveCard = fun_setdispremovecard;
    return SDK_OK;
}

s32 sdkJcbSetBeforeGPO(s32(*fun_setbeforegpo)())
{
    if(gstsdkJcbTradeTable == NULL)
    {
        return SDK_PARA_ERR;
    }

    gstsdkJcbTradeTable->setBeforeGPO = fun_setbeforegpo;
    return SDK_OK;
}

s32 sdkJcbSetVerifyCardNo(s32(*fun_setVerifyCardNo)(u8* asPAN))
{
    if(gstsdkJcbTradeTable == NULL)
    {
        return SDK_PARA_ERR;
    }

    gstsdkJcbTradeTable->VerifyCardNo = fun_setVerifyCardNo;
    return SDK_OK;
}

s32 sdkJcbSetPromptSoundAndLed( s32 (*fun_setPromptSoundAndLed)(int mode))
{
    if(gstsdkJcbTradeTable == NULL)
    {
        return SDK_PARA_ERR;
    }

    gstsdkJcbTradeTable->PromptSoundAndLed = fun_setPromptSoundAndLed;

    return SDK_OK;
}

s32 sdkJcbSetRevocationKey(s32(*fun_cmprevocation)(u8 *key))
{
    if(gstJcbTradeUnionStruct == NULL)
    {
        return SDK_PARA_ERR;
    }

    gstJcbTradeUnionStruct->termipkrevokecheck = fun_cmprevocation;
    return SDK_OK;
}

s32 sdkJcbSetOutComeParam(
JCB_SendOutComeParameterSet SendOutComeParameterSet,
JCB_senduserinterfacerequestdata senduserinterfacerequestdata,
JCB_Dispuserinterfacerequestdata Dispuserinterfacerequestdata,
JCB_SendUserInterfaceRestartData SendUserInterfaceRestartData,
JCB_DispUserInterfaceRestartData DispUserInterfaceRestartData,
JCB_SendUserInterfaceRequestParameter SendUserInterfaceRequestParameter,
JCB_DispUserInterfaceRequestParamere DispUserInterfaceRequestParamer
)
{
	if(gstJcbTradeUnionStruct == NULL)
	{
		return SDK_PARA_ERR;
	}

	gstJcbTradeUnionStruct->SendOutComeParameterSet = SendOutComeParameterSet;
	gstJcbTradeUnionStruct->senduserinterfacerequestdata = senduserinterfacerequestdata;
	gstJcbTradeUnionStruct->Dispuserinterfacerequestdata = Dispuserinterfacerequestdata;
	gstJcbTradeUnionStruct->SendUserInterfaceRestartData = SendUserInterfaceRestartData;
	gstJcbTradeUnionStruct->DispUserInterfaceRestartData = DispUserInterfaceRestartData;
	gstJcbTradeUnionStruct->SendUserInterfaceRequestParameter = SendUserInterfaceRequestParameter;
	gstJcbTradeUnionStruct->DispUserInterfaceRequestParamer = DispUserInterfaceRequestParamer;

}

void sdkJcbAidToTermInfo(unsigned char i, SDK_EMVBASE_AID_STRUCT * termaidparam, unsigned char onlinepinsup)
{
	unsigned int Threshold;
	unsigned char JcbThreshold[6];

    emvbase_avl_createsettagvalue(EMVTAG_TermAID, (termaidparam + i)->Aid, (termaidparam + i)->AidLen);
    emvbase_avl_createsettagvalue(EMVTAG_AppVerNum, (termaidparam + i)->AppVerNum, 2);

	TraceHex("emv","TacDefault",(termaidparam+i)->TacDefault,5);
	if(memcmp((termaidparam+i)->TacDefault, "\xFF\xFF\xFF\xFF\xFF", 5) == 0)
	{
		emvbase_avl_deletetag(EMVTAG_TACDefault);//not configured
	}
	else
	{
	    emvbase_avl_createsettagvalue(EMVTAG_TACDefault, (termaidparam + i)->TacDefault, 5);
	}

	TraceHex("emv","TacOnline",(termaidparam+i)->TacOnline,5);
	if(memcmp((termaidparam+i)->TacOnline, "\xFF\xFF\xFF\xFF\xFF", 5) == 0)
	{
		emvbase_avl_deletetag(EMVTAG_TACOnline);//not configured
	}
	else
	{
	    emvbase_avl_createsettagvalue(EMVTAG_TACOnline, (termaidparam + i)->TacOnline, 5);
	}

	TraceHex("emv","TacDecline",(termaidparam+i)->TacDecline,5);
	if(memcmp((termaidparam+i)->TacDecline, "\xFF\xFF\xFF\xFF\xFF", 5) == 0)
	{
		emvbase_avl_deletetag(EMVTAG_TACDenial);//not configured
	}
	else
	{
	    emvbase_avl_createsettagvalue(EMVTAG_TACDenial, (termaidparam + i)->TacDecline, 5);
	}

	TraceHex("emv","cl_cvmlimit",(termaidparam+i)->cl_cvmlimit,6);
	if(memcmp((termaidparam+i)->cl_cvmlimit, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0)
	{
		emvbase_avl_deletetag(EMVTAG_termcvm_limit);//not configured
	}
	else
	{
	    emvbase_avl_createsettagvalue(EMVTAG_termcvm_limit, (termaidparam + i)->cl_cvmlimit, 6);
	}

	TraceHex("emv","cl_translimit",(termaidparam+i)->cl_translimit,6);
	if(memcmp((termaidparam+i)->cl_translimit, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0)
	{
		emvbase_avl_deletetag(EMVTAG_clessofflineamt);//not configured
	}
	else
	{
	    emvbase_avl_createsettagvalue(EMVTAG_clessofflineamt, (termaidparam + i)->cl_translimit, 6);
	}

	TraceHex("emv","cl_offlinelimit",(termaidparam+i)->cl_offlinelimit,6);
	if(memcmp((termaidparam+i)->cl_offlinelimit, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0)
	{
		emvbase_avl_deletetag(EMVTAG_clessofflinelimitamt);//not configured
	}
	else
	{
	    emvbase_avl_createsettagvalue(EMVTAG_clessofflinelimitamt, (termaidparam + i)->cl_offlinelimit, 6);
	}

    emvbase_avl_settag(EMVTAG_MaxTargetPercent, (termaidparam + i)->MaxTargetPercent);
    emvbase_avl_settag(EMVTAG_TargetPercent, (termaidparam + i)->TargetPercent);
    emvbase_avl_createsettagvalue(EMVTAG_Threshold, (termaidparam + i)->Threshold, 4);

	emvbase_avl_createsettagvalue(EMVTAG_Threshold, (termaidparam + i)->Threshold, 4);
	memset(JcbThreshold, 0x00, 6);
	memcpy(&JcbThreshold[2], (termaidparam + i)->Threshold, 4);
	emvbase_avl_createsettagvalue(EMVTAG_JCBThresholdRandomSelect, JcbThreshold, 6);
}


s32 sdkJcbReadAIDParameters(JCBTradeUnionStruct * temp_UnionStruct)
{
    u8 i;
    s32 ret;
    u8 type = 0;

    EMVBASE_AID_STRUCT *tempaidparam;
    EMVBASE_LISTAPPDATA *tempappdata;
    unsigned char tempselectedappno;

    tempappdata = temp_UnionStruct->EMVTradeParam->AppListCandidate;
    tempselectedappno = temp_UnionStruct->EMVTradeParam->SelectedAppNo;
    tempaidparam = (EMVBASE_AID_STRUCT *)emvbase_malloc(sizeof(EMVBASE_AID_STRUCT));

	type = emvbase_avl_gettagvalue(EMVTAG_TransTypeValue);
    Trace("emv", "type=0x%x\n", type);

    ret = sdkEMVBaseReadAIDParameters((tempappdata + tempselectedappno)->AidInTerm,(tempappdata + tempselectedappno)->AidInTermLen,tempaidparam,type,2);

	Trace("emv","sdkEMVBaseReadAIDParameters = %d\r\n",ret);

    if(ret != SDK_OK)
    {
        emvbase_free(tempaidparam);
        return ret;
    }
    sdkJcbAidToTermInfo(0, tempaidparam, 0);

    emvbase_free(tempaidparam);

    if(emvbase_avl_checkiftagexist(EMVTAG_CardAID) == 0)
    {
        if((tempappdata + tempselectedappno)->AIDLen)
        {
            emvbase_avl_createsettagvalue(EMVTAG_CardAID, (tempappdata + tempselectedappno)->AID, (tempappdata + tempselectedappno)->AIDLen);
        }
    }

    if(emvbase_avl_checkiftagexist(EMVTAG_AppLabel) == 0)
    {
        if((tempappdata + tempselectedappno)->AppLabelLen)
        {
            emvbase_avl_createsettagvalue(EMVTAG_AppLabel, (tempappdata + tempselectedappno)->AppLabel, (tempappdata + tempselectedappno)->AppLabelLen);
        }
    }

    if(emvbase_avl_checkiftagexist(EMVTAG_AppPreferName) == 0)
    {
        if((tempappdata + tempselectedappno)->PreferNameLen)
        {
            emvbase_avl_createsettagvalue(EMVTAG_AppPreferName, (tempappdata + tempselectedappno)->PreferName, (tempappdata + tempselectedappno)->PreferNameLen);
        }
    }

    if(emvbase_avl_checkiftagexist(EMVTAG_LangPrefer) == 0)
    {
        if((tempappdata + tempselectedappno)->LangPreferLen)
        {
            emvbase_avl_createsettagvalue(EMVTAG_LangPrefer, (tempappdata + tempselectedappno)->LangPrefer, (tempappdata + tempselectedappno)->LangPreferLen);
        }
    }

    return SDK_OK;
}


static void sdkJcbTradeParamDestory(void)
{
    if(gstJcbTradeParam != NULL)
    {
        if(gstJcbTradeParam->CAPK != NULL)
        {
            emvbase_free(gstJcbTradeParam->CAPK);
            gstJcbTradeParam->CAPK = NULL;
        }

        if(gstJcbTradeParam->AuthData != NULL)
        {
            emvbase_free(gstJcbTradeParam->AuthData);
            gstJcbTradeParam->AuthData = NULL;
        }
        if(gstJcbTradeParam->IPKModul != NULL)
        {
            emvbase_free(gstJcbTradeParam->IPKModul);
            gstJcbTradeParam->IPKModul = NULL;
        }
        if(gstJcbTradeParam->ICCPKModul != NULL)
        {
            emvbase_free(gstJcbTradeParam->ICCPKModul);
            gstJcbTradeParam->ICCPKModul = NULL;
        }
        if(gstJcbTradeParam->RedundantData != NULL)
        {
            emvbase_free(gstJcbTradeParam->RedundantData);
            gstJcbTradeParam->RedundantData = NULL;
        }


		//sjz20191024 add
		if(gstJcbTradeParam->pJcbOutParameterSet != NULL)
        {
            emvbase_free(gstJcbTradeParam->pJcbOutParameterSet);
            gstJcbTradeParam->pJcbOutParameterSet = NULL;
        }
		if(gstJcbTradeParam->pJcbUserInterfaceReqData != NULL)
        {
            emvbase_free(gstJcbTradeParam->pJcbUserInterfaceReqData);
            gstJcbTradeParam->pJcbUserInterfaceReqData = NULL;
        }
		if(gstJcbTradeParam->pJcbUserInterfaceRestartData != NULL)
        {
            emvbase_free(gstJcbTradeParam->pJcbUserInterfaceRestartData);
            gstJcbTradeParam->pJcbUserInterfaceRestartData = NULL;
        }
		if(gstJcbTradeParam->pJcbUserInterfaceRequestData != NULL)
        {
            emvbase_free(gstJcbTradeParam->pJcbUserInterfaceRequestData);
            gstJcbTradeParam->pJcbUserInterfaceRequestData = NULL;
        }
        emvbase_free(gstJcbTradeParam);
        gstJcbTradeParam = NULL;
    }

    if(gstJcbTradeUnionStruct != NULL)
    {
        emvbase_free(gstJcbTradeUnionStruct);
        gstJcbTradeUnionStruct = NULL;
    }

    if(gstsdkJcbTradeTable != NULL)
    {
        emvbase_free(gstsdkJcbTradeTable);
        gstsdkJcbTradeTable = NULL;
    }

}

void sdkJcbMalloctradememory()
{
    s32 aidnum=0;

	sdkJcbTradeParamDestory();

    if(gstJcbTradeUnionStruct == NULL)
    {
        gstJcbTradeUnionStruct = (JCBTradeUnionStruct *)emvbase_malloc(sizeof(JCBTradeUnionStruct));
    }
    memset(gstJcbTradeUnionStruct,0,(sizeof(JCBTradeUnionStruct)));

	if(gstsdkJcbTradeTable == NULL)
	{
		gstsdkJcbTradeTable = (SDK_JCB_TRADE_PARAM *)emvbase_malloc(sizeof(SDK_JCB_TRADE_PARAM));
	}
	memset(gstsdkJcbTradeTable,0,(sizeof(SDK_JCB_TRADE_PARAM)));


    if(gstJcbTradeParam == NULL)
    {
        gstJcbTradeParam = (JCBTRADEPARAMETER *)emvbase_malloc(sizeof(JCBTRADEPARAMETER));
    }
    memset(gstJcbTradeParam,0,sizeof(JCBTRADEPARAMETER));

    sdkEMVBaseGetAIDListNum(&aidnum);

	if(gstJcbTradeParam->pJcbOutParameterSet == NULL)
    {
        gstJcbTradeParam->pJcbOutParameterSet = emvbase_malloc(sizeof(JCBOUTPARAMETERSET));
    }
	memset(gstJcbTradeParam->pJcbOutParameterSet, 0, sizeof(JCBOUTPARAMETERSET));

	if(gstJcbTradeParam->pJcbUserInterfaceReqData == NULL)
    {
        gstJcbTradeParam->pJcbUserInterfaceReqData = emvbase_malloc(sizeof(JCBUSERINTERFACEREQDATA));
    }
	memset(gstJcbTradeParam->pJcbUserInterfaceReqData, 0, sizeof(JCBUSERINTERFACEREQDATA));

	if(gstJcbTradeParam->pJcbUserInterfaceRestartData == NULL)
    {
        gstJcbTradeParam->pJcbUserInterfaceRestartData = emvbase_malloc(sizeof(JCBUSERINTERFACERESTARTDATA));
    }
	memset(gstJcbTradeParam->pJcbUserInterfaceRestartData, 0, sizeof(JCBUSERINTERFACERESTARTDATA));
	if(gstJcbTradeParam->pJcbUserInterfaceRequestData == NULL)
	{
		gstJcbTradeParam->pJcbUserInterfaceRequestData = emvbase_malloc(sizeof(JCBUSERINTERFACEREQUESTDATA));
	}
	memset(gstJcbTradeParam->pJcbUserInterfaceRequestData, 0, sizeof(JCBUSERINTERFACEREQUESTDATA));
}


unsigned char sdkJcbFunCB(unsigned char pintype, unsigned char *ASCCreditPwd)
{
    unsigned char rslt;
    unsigned char tCreditPwd[64], tICCEncryptWay;
    s32 rlt;
    u8 PINTryCount = 0;

    Trace("emv", "pintype=%d\r\n", pintype);
    if(gstEMVBase_TradeParam->InputPWD == NULL)
    {
        Trace("emv","ERROR!!!USER NOT SET INPUTPIN FUNCTION\r\n");
        return RLT_EMV_ERR;
    }

    if(pintype == EMVB_DATAAUTH_INPUTPINSTATUS)
    {
        return RLT_EMV_OK;
    }
    else if(pintype == EMVB_DATAAUTH_VERIFYIDCARD)
    {
        return RLT_EMV_OK;
    }
    else if(pintype == EMVB_NEEDREFER)
    {
        return RLT_EMV_OK;
    }
    memset(tCreditPwd, 0, sizeof(tCreditPwd));

    if(pintype == EMVB_DATAAUTH_OFFLINEENCPIN || pintype == EMVB_DATAAUTH_OFFLINEPLAINTEXTPIN)
    {
        tICCEncryptWay = SDK_PED_IC_OFFLINE_PIN;
    }
    else
    {
        tICCEncryptWay = SDK_PED_IC_ONLINE_PIN;
    }

    rlt = gstEMVBase_TradeParam->InputPWD(tICCEncryptWay, PINTryCount, tCreditPwd);
    Trace("emv","InputPWD=%d\r\n",rlt);
    if(rlt != SDK_OK && rlt != SDK_PED_NOPIN)
    {
        Trace("emv","WARNING!!!USER CANCEL INPUT PIN\r\n");
        return RLT_ERR_EMV_CancelTrans;
    }

    if(rlt == SDK_OK)
    {
        if(tCreditPwd[0])
        {
            memcpy(ASCCreditPwd, &tCreditPwd[1], tCreditPwd[0]);
            return RLT_EMV_OK;
        }
        else
        {
            return RLT_ERR_EMV_InputBYPASS;
        }
    }
    else if(rlt == SDK_PED_NOPIN)
    {
        return RLT_ERR_EMV_InputBYPASS;
    }
    else
    {
        return RLT_ERR_EMV_CancelTrans;
    }
}

void sdkJcbGetRandom(unsigned char* randpad, unsigned int randLen)
{
    sdkGetRandom(randpad, randLen);
}

static void sdkJcbIccIsoCommand(EMVBASE_APDU_SEND * ApduSend, EMVBASE_APDU_RESP * apdu_r)
{
    sdkEMVBase_ContactlessIsoCommand(ApduSend, apdu_r);
}

static unsigned char sdkJcbVerifyCardNo(unsigned char *asPAN)
{
	s32 ret;
	if(gstsdkJcbTradeTable->VerifyCardNo)
	{
		ret = gstsdkJcbTradeTable->VerifyCardNo(asPAN);
		if(ret != SDK_OK)
		{
			return RLT_EMV_ERR;
		}
	}

	return RLT_EMV_OK;
}

static unsigned char sdkJcbCheckCAPK(JCBTRADEPARAMETER *EMVTradeParam)
{
    EMVBASE_LISTAPPDATA *tempselectedapp;
    unsigned char i, RID[5];
    unsigned char CAPKI;
    unsigned char ret;

    Trace("emv", "sdkJcbCheckCAPK\r\n");
    tempselectedapp = EMVTradeParam->SelectedApp;
	memcpy(RID, (unsigned char*)tempselectedapp->AID, 5);

    ret = emvbase_avl_gettagvalue_spec(EMVTAG_CAPKI, &CAPKI, 0, 1);

    if(ret != 0)
    {
        return RLT_EMV_ERR;
    }

	if( sdkEMVBaseCheckCAPKExit(RID,CAPKI))
	{
		return RLT_EMV_OK;
	}

    return RLT_EMV_ERR;
}

void sdkJcbTransTermDataInit(void)
{
	gstJcbTradeParam->bAbleOnline=1;//initial value: 1
    gstJcbTradeParam->SelectedAppNo = 0xff;
	gstJcbTradeParam->JcbIsLegacyCard = 1;
	gstJcbTradeParam->JcbCVMParameter = 0xFF;
	gstJcbTradeParam->JcbImplementationOption = 0xFC;//F8->FC,add bit3:MSD support//sjz 2020.6.22
//	gstJcbTradeParam->paywavetrackdataup = 0x03;
	gstJcbTradeParam->JcbCandidateListEmptyFlag = 1;

    gstJcbTradeUnionStruct->EMVTradeParam = gstJcbTradeParam;
    gstJcbTradeUnionStruct->IsoCommand = sdkJcbIccIsoCommand;
    gstJcbTradeUnionStruct->EMVB_InputCreditPwd = sdkJcbFunCB;
    gstJcbTradeUnionStruct->EMVB_RandomNum = sdkJcbGetRandom;
    gstJcbTradeUnionStruct->CheckMatchTermAID = sdkEMVBase_CheckMatchTermAID_CL;
    gstJcbTradeUnionStruct->ReadTermAID = sdkEMVBase_ReadTermAID_CL;
	gstJcbTradeUnionStruct->VerifyCardNo = sdkJcbVerifyCardNo;
	gstJcbTradeUnionStruct->CheckCapkExit = sdkJcbCheckCAPK;
}


s32 sdkJcbTransInit()
{
	gstJCBTransFlowModeStatus = 0;
    sdkJcbTradeParamDestory();
	sdkJcbMalloctradememory();
    sdkJcbTransTermDataInit();
	if(gstJcbTradeUnionStruct)
	{
		jcb_TornRecovery(gstJcbTradeUnionStruct);
	}

	return SDK_OK;
}

s32 sdkJcbFinalSelectedApp(const SDK_JCB_TRADE_PARAM * pstTradeParam)
{
    u8 retCode = RLT_EMV_ERR;
    s32 rlt = SDK_ERR;

    if((NULL == pstTradeParam) || (gstJcbTradeUnionStruct == NULL) || (gstJcbTradeParam == NULL))
    {
        return SDK_PARA_ERR;
    }

    retCode = jcb_FinalSelectedApp(gstJcbTradeUnionStruct);
    Trace("emv", "jcb_FinalSelectedApp retCode = %02d\r\n", retCode);
	if(retCode == JCB_ERR_READCARDAGAIN)
	{
		return SDK_EMV_ReadCardAgain;
	}
    else if(retCode == JCB_READAIDPARAMETERS)
    {
        rlt = sdkJcbReadAIDParameters(gstJcbTradeUnionStruct);

        if(rlt != SDK_OK)
        {
            retCode = RLT_EMV_APPSELECTTRYAGAIN;
        }
        else
        {
            retCode = RLT_EMV_OK;
        }
    }

    sdkEMVBaseRltToSdkRlt(retCode, &rlt);
	Trace("emv", "sdkJcbFinalSelectedApp ret = %02d\r\n", rlt);

    return rlt;
}

s32 sdkJcbInitialApp(const SDK_JCB_TRADE_PARAM * pstTradeParam)
{
    u8 retCode = RLT_EMV_ERR;
    s32 rlt = SDK_ERR;
    u8 tagAIPflag, tagAFLflag;


    if((NULL == pstTradeParam) || (gstJcbTradeUnionStruct == NULL) || (gstJcbTradeParam == NULL))
    {
        return SDK_PARA_ERR;
    }

//	retCode = jcb_VariableInitialisation(gstJcbTradeUnionStruct);//要在导入aid参数后判断

    if(gstsdkJcbTradeTable->setBeforeGPO != NULL)
    {
        (gstsdkJcbTradeTable->setBeforeGPO)();
    }


    while(1)
    {
        retCode = jcb_InitialApp(gstJcbTradeUnionStruct);
        Trace("emv", "jcb_InitialApp retCode = %02d\r\n", retCode);

		if(EMV_Mode == gstJcbTradeUnionStruct->EMVTradeParam->JcbTransactionMode)
		{
			gstJCBTransFlowModeStatus = SDKJCB_TRANSFLOW_EMV_MODE;
		}
		else if(Magstripe_Mode == gstJcbTradeUnionStruct->EMVTradeParam->JcbTransactionMode)
		{
			gstJCBTransFlowModeStatus = SDKJCB_TRANSFLOW_MSD_MODE;
		}
		else if(Legacy_Mode == gstJcbTradeUnionStruct->EMVTradeParam->JcbTransactionMode)
		{
			gstJCBTransFlowModeStatus = SDKJCB_TRANSFLOW_LEGACY_MODE;
		}
		else
		{
			gstJCBTransFlowModeStatus = SDKJCB_TRANSFLOW_UNDEF_MODE;
		}

        if(retCode == RLT_ERR_EMV_APDUTIMEOUT)
        {
            return SDK_EMV_ReadCardAgain;
        }
        else if(retCode == RLT_ERR_INITAPP_NOTACCEPED)
        {
            retCode = RLT_EMV_APPSELECTTRYAGAIN;
        }

        sdkEMVBaseRltToSdkRlt(retCode, &rlt);
		Trace("emv", "sdkJcbInitialApp ret = %02d\r\n", rlt);
        return rlt;
    }
}

s32 sdkJcbReadAppData(const SDK_JCB_TRADE_PARAM * pstTradeParam)
{
    u8 retCode = RLT_EMV_ERR, k;
    s32 rlt = SDK_ERR;

    if((NULL == pstTradeParam) || (gstJcbTradeUnionStruct == NULL) || (gstJcbTradeParam == NULL))
    {
        return SDK_PARA_ERR;
    }

    retCode = jcb_ReadAppData(gstJcbTradeUnionStruct);
	Trace("emv", "jcb_ReadAppData retCode = %02d\r\n", retCode);

    if(retCode == RLT_EMV_OK)
    {
        if(gstJcbTradeParam->JcbTransactionMode==EMV_Mode)
		{
			if(gstJcbTradeParam->JcbTornTransactionFlag == true)
			{
				retCode = jcb_TornProcessCryptInfo(gstJcbTradeUnionStruct);
				Trace("emv", "jcb_TornProcessCryptInfo ret = %02d\r\n", retCode);
			}
			if(retCode == RLT_EMV_OK)
			{
				Trace("", "goto check data after read record!\r\n");
				emvbase_avl_printtagallvalue("TVR", EMVTAG_TVR);
				retCode = jcb_ODACheckMandotoryData(gstJcbTradeUnionStruct);
				Trace("", "jcb_ODACheckMandotoryData = %d\r\n",retCode);
				emvbase_avl_printtagallvalue("TVR", EMVTAG_TVR);
				jcb_DealTrackData(gstJcbTradeUnionStruct);
			}
		}
		else
		{
			retCode = jcb_ODACheckMandotoryData(gstJcbTradeUnionStruct);
			Trace("", "jcb_ODACheckMandotoryData = %d\r\n",retCode);
			jcb_DealTrackData(gstJcbTradeUnionStruct);
		}
    }
	else if(retCode == RLT_ERR_EMV_APDUTIMEOUT)
	{
	    return SDK_EMV_ReadCardAgain;
    }


    sdkEMVBaseRltToSdkRlt(retCode, &rlt);
	Trace("emv", "sdkJcbReadAppData ret = %02d\r\n", rlt);
    return rlt;
}

s32 sdkJcbTermRiskManage(const SDK_JCB_TRADE_PARAM * pstTradeParam)
{
	u8 retCode = RLT_EMV_ERR, k;
	s32 rlt = SDK_ERR;

	if((NULL == pstTradeParam) || (gstJcbTradeUnionStruct == NULL) || (gstJcbTradeParam == NULL))
	{
		return SDK_PARA_ERR;
	}

	retCode = jcb_TermRiskManage(gstJcbTradeUnionStruct);
	Trace("emv", "jcb_TermRiskManage retCode = %02d\r\n", retCode);

	sdkEMVBaseRltToSdkRlt(retCode, &rlt);
	Trace("emv", "sdkJcbTermRiskManage ret = %02d\r\n", rlt);
	return rlt;
}


s32 sdkJcbProcessRestrict(const SDK_JCB_TRADE_PARAM * pstTradeParam)
{
    u8 retCode = RLT_EMV_ERR, k;
    s32 rlt = SDK_ERR;

    if((NULL == pstTradeParam) || (gstJcbTradeUnionStruct == NULL) || (gstJcbTradeParam == NULL))
    {
        return SDK_PARA_ERR;
    }

    retCode = jcb_ProcessRestrict(gstJcbTradeUnionStruct);
	Trace("emv", "jcb_ProcessRestrict retCode = %02d\r\n", retCode);

    sdkEMVBaseRltToSdkRlt(retCode, &rlt);
	Trace("emv", "sdkJcbProcessRestrict ret = %02d\r\n", rlt);
    return rlt;
}


s32 sdkJcbTermiAnalys(const SDK_JCB_TRADE_PARAM * pstTradeParam)
{
	u8 retCode = RLT_EMV_ERR, k;
	s32 rlt = SDK_ERR;

	if((NULL == pstTradeParam) || (gstJcbTradeUnionStruct == NULL) || (gstJcbTradeParam == NULL))
	{
		return SDK_PARA_ERR;
	}

	retCode = jcb_TermiAnalys(gstJcbTradeUnionStruct);

	Trace("emv", "jcb_TermiAnalys retCode = %02d\r\n", retCode);
	if(retCode == RLT_EMV_OK)
	{
		retCode = jcb_TransProcess(gstJcbTradeUnionStruct);
		Trace("emv", "jcb_TransProcess retCode = %02d\r\n", retCode);
	}
	sdkEMVBaseRltToSdkRlt(retCode, &rlt);
	Trace("emv", "sdkJcbTermiAnalys ret = %02d\r\n", rlt);
	return rlt;
}


s32 sdkJcbReadCAPK(const SDK_JCB_TRADE_PARAM * pstTradeParam)
{
    EMVBASE_CAPK_STRUCT *tempcapk;
    EMVBASE_LISTAPPDATA *tempselectedapp;
    unsigned char i, RID[5];
    unsigned char CAPKI;
    unsigned char ret;


	if((NULL == pstTradeParam) || (gstJcbTradeUnionStruct == NULL) || (gstJcbTradeParam == NULL))
    {
        return SDK_PARA_ERR;
    }

    Trace("emv", "sdkJcbReadCAPK\r\n");
	if(gstJcbTradeUnionStruct->EMVTradeParam->CAPK == NULL)
    {
        gstJcbTradeUnionStruct->EMVTradeParam->CAPK = (EMVBASE_CAPK_STRUCT *)emvbase_malloc(sizeof(EMVBASE_CAPK_STRUCT));
        memset(gstJcbTradeUnionStruct->EMVTradeParam->CAPK, 0, sizeof(EMVBASE_CAPK_STRUCT));
    }

    tempcapk = gstJcbTradeUnionStruct->EMVTradeParam->CAPK;
    tempselectedapp = gstJcbTradeUnionStruct->EMVTradeParam->SelectedApp;



    tempcapk->ModulLen = 0;


    ret = emvbase_avl_gettagvalue_spec(EMVTAG_CAPKI, &CAPKI, 0, 1);

    if(ret != 0)
    {
        return -1;
    }

    if(tempcapk->ModulLen == 0)
    {
        memcpy(RID, (unsigned char*)tempselectedapp->AID, 5);

        sdkEMVBase_ReadCAPK(RID,CAPKI,tempcapk);
    }
    return SDK_OK;
}


s32 sdkJcbDataAuth(const SDK_JCB_TRADE_PARAM * pstTradeParam)
{
    u8 retCode = RLT_EMV_ERR, k;
    s32 rlt = SDK_ERR;

    if((NULL == pstTradeParam) || (gstJcbTradeUnionStruct == NULL) || (gstJcbTradeParam == NULL))
    {
        return SDK_PARA_ERR;
    }

	#if 0
    if(gstJcbTradeUnionStruct->EMVTradeParam->CAPK == NULL)
    {
        gstJcbTradeUnionStruct->EMVTradeParam->CAPK = (EMVBASE_CAPK_STRUCT *)emvbase_malloc(sizeof(EMVBASE_CAPK_STRUCT));
        memset(gstJcbTradeUnionStruct->EMVTradeParam->CAPK,0,sizeof(EMVBASE_CAPK_STRUCT));
    }


    sdkJcbReadCAPK(gstJcbTradeUnionStruct);
	#endif


    retCode = jcb_DataAuth(gstJcbTradeUnionStruct);



    Trace("emv", "jcb_DataAuth retCode = %02d\r\n", retCode);
    emvbase_avl_printtagallvalue("Paywaveb_DataAuth TVR",EMVTAG_TVR);
    sdkEMVBaseRltToSdkRlt(retCode, &rlt);
	Trace("emv", "sdkJcbDataAuth ret = %02d\r\n", rlt);
    return rlt;
}

s32 sdkJcbCardHolderVerf(const SDK_JCB_TRADE_PARAM * pstTradeParam)
{
    u8 retCode = RLT_EMV_ERR, k;
    s32 rlt = SDK_ERR;
	unsigned char IUParameter;

    if((NULL == pstTradeParam) || (gstJcbTradeUnionStruct == NULL) || (gstJcbTradeParam == NULL))
    {
        return SDK_PARA_ERR;
    }

    retCode = jcb_CardHolderVerf(gstJcbTradeUnionStruct);
    Trace("emv", "jcb_CardHolderVerf retCode = %02d\r\n", retCode);
    emvbase_avl_printtagallvalue("Emv_CardHolderVerf TTQ", &EMVTAG_TermTransPredicable);
	if(gstJcbTradeUnionStruct->EMVTradeParam->bPrintReceipt)
	{
		gstemvbaseneedsign = 1;
	}
    sdkEMVBaseRltToSdkRlt(retCode, &rlt);
	Trace("emv", "sdkJcbCardHolderVerf ret = %02d\r\n", rlt);
    return rlt;
}

s32 sdkJcbTransProcess(const SDK_JCB_TRADE_PARAM * pstTradeParam)
{
    u8 retCode = RLT_EMV_ERR, k;
    s32 rlt = SDK_ERR;

    if((NULL == pstTradeParam) || (gstJcbTradeUnionStruct == NULL) || (gstJcbTradeParam == NULL))
    {
        return SDK_PARA_ERR;
    }

    if(gstJcbTradeParam->TransResult == RESULT_ONLINE_WAIT)   //锟斤拷锟阶等达拷
    {
        Trace("emv","Emv_OnLineDeal tempterminfo->inputpinstatus = %d  \r\n", gstJcbTradeParam->onlinetradestatus);

        if(gstJcbTradeParam->onlinetradestatus != RLT_EMV_OK)
        {
            gstJcbTradeParam->bReversal = 1;
            retCode = jcb_UnableGoOnlineDeal(gstJcbTradeUnionStruct);
			Trace("emv", "jcb_UnableGoOnlineDeal ret = %02d\r\n", retCode);
        }
        else
        {
            retCode = jcb_OnLineDeal(gstJcbTradeUnionStruct);
			Trace("emv", "jcb_OnLineDeal ret = %02d\r\n", retCode);
        }

        Trace("emv","\r\nEmv_OnLineDeal retCode = %d\r\n",retCode);
        Trace("emv","\r\nEmv_OnLineDeal JcbIssuerUpdate = %d \r\n ",gstJcbTradeParam->JcbIssuerUpdate);
		Trace("emv","gstJcbTradeParam->TransResult = %02x\r\n",gstJcbTradeParam->TransResult);

        if(retCode == RLT_EMV_OK)
        {
            if(gstJcbTradeParam->JcbIssuerUpdate)  //锟斤拷锟斤拷ISSUER UPDATE PROCESSING
            {
                return SDK_EMV_ReadCardAgain;
            }
			else
			{
				if(gstJcbTradeParam->JcbTransactionMode == EMV_Mode)
				{
					retCode = jcb_PreTransComplete(gstJcbTradeUnionStruct);
					Trace("emv", "jcb_PreTransComplete ret = %02d\r\n", retCode);
					if(retCode == JCB_ERR_NEEDREVERSAL)
					{
						//*emvtask = EMVERRDEAL;
					}

				}
			}
        }
    }
	else
    {
		retCode = RLT_EMV_OK;
	}


    sdkEMVBaseRltToSdkRlt(retCode, &rlt);
	Trace("emv", "sdkJcbTransProcess ret = %02d\r\n", rlt);
    return rlt;
}

s32 sdkJcbTransComplete(const SDK_JCB_TRADE_PARAM * pstTradeParam)
{
    u8 retCode = RLT_EMV_ERR;
    s32 rlt = SDK_ERR;
	s32 len=0;
	u8 ret;
	u8 DTIP[3];
 	u8 ImpleOption = gstJcbTradeParam->JcbImplementationOption;
	u8 CombOptions;
	u8 TransactionMode;
	u8 IUParamter;
	u8 IUParamterExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_JCBIssuerUpdateParameter, &IUParamter, 0, 1));

    if((NULL == pstTradeParam) || (gstJcbTradeUnionStruct == NULL) || (gstJcbTradeParam == NULL) || (gstsdkJcbTradeTable==NULL))
    {
        return SDK_PARA_ERR;
    }

	Trace("emv","gstJcbTradeParam->TransResult = %02x\r\n",gstJcbTradeParam->TransResult);
    switch (gstJcbTradeParam->TransResult)
    {

        case RESULT_OFFLINE_APPROVE:
			jcb_SendTransactionOutCome(gstJcbTradeUnionStruct, JCB_OPS_STATUS_APPROVED);
            retCode = RLT_EMV_OFFLINE_APPROVE;
            break;

        case RESULT_OFFLINE_DECLINE:
			emvbase_avl_createsettagvalue(EMVTAG_AuthRespCode,"Z1",2);
            retCode = RLT_EMV_OFFLINE_DECLINE;

			if(emvbase_avl_gettagvalue(EMVTAG_TransTypeValue)== 0x20)  //refund
			{
				gstJcbTradeParam->TransResult = RESULT_OFFLINE_APPROVE;
				retCode = RLT_EMV_OFFLINE_APPROVE;
			}

            break;


        case RESULT_ONLINE_WAIT:

		if(gstsdkJcbTradeTable->PromptSoundAndLed)
		{
			gstsdkJcbTradeTable->PromptSoundAndLed(0);
		}
			memset(DTIP, 0x00, 3);
			emvbase_avl_gettagvalue_spec(EMVTAG_JCBDynamicTIP, DTIP, 0, 3);

			emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, &CombOptions, 0, 1);
			TransactionMode = gstJcbTradeParam->JcbTransactionMode;

			EMVBase_Trace("IUParamter=%02x ImpleOption=%02x CombOptions=%02x DTIP[1]=%02x\r\n", IUParamter, ImpleOption, CombOptions, DTIP[1]);

			if((IUParamterExist == 1) && (TransactionMode==EMV_Mode) && ((ImpleOption & 0x88) == 0x88) && ((CombOptions & 0x02) == 0x02) && ((DTIP[1]&0x80) == 0x80))
			{
				EMVBase_Trace( "IUParamter = %x\r\n",IUParamter);
				if(IUParamter == 0x01)
				{
					jcb_SendTransactionOutCome(gstJcbTradeUnionStruct, JCB_OPS_STATUS_ONLINE_PREMENTANDHOLD);
				}
				else if(IUParamter == 0x02)
				{
					jcb_SendTransactionOutCome(gstJcbTradeUnionStruct, JCB_OPS_STATUS_ONLINE_TWOPRESENTMENTS);
				}
				else
				{
					jcb_SendTransactionOutCome(gstJcbTradeUnionStruct, JCB_OPS_STATUS_ONLINE);
				}
			}

			else
			{
				jcb_SendTransactionOutCome(gstJcbTradeUnionStruct, JCB_OPS_STATUS_ONLINE);

			}
            retCode = RLT_EMV_ONLINE_WAIT;
            break;

        case RESULT_ONLINE_APPROVE:
            retCode = RLT_EMV_ONLINE_APPROVE;
            break;

        case RESULT_ONLINE_DECLINE:
            retCode = RLT_EMV_ONLINE_DECLINE;
            break;

        default:
            retCode = RLT_EMV_TERMINATE_TRANSERR;
            break;
    }

    sdkEMVBaseRltToSdkRlt(retCode, &rlt);
    return rlt;
}

s32 sdkJcbDifferEMVBase()
{
	emvbase_avl_settagtype(EMVTAGTYPE_JSPEEDY);

    emvbase_avl_createsettagvalue(EMVTAG_CVMResult,"\x1F\x00\x00",3);
	emvbase_avl_createsettagvalue(EMVTAG_JCBCombinationOptions, "\x7F\x00", 2);//bit3-msd, sjz 2020.6.22
	emvbase_avl_createsettagvalue(EMVTAG_JCBRemovalTimeout, "\x01\x00", 2);
	emvbase_avl_createsettagvalue(EMVTAG_JCBStaticTIP, "\x73\x80\x00", 3);
}

SDK_EMVBASE_CVM_RESULT sdkJcbGetCVMresult()
{
	SDK_EMVBASE_CVM_RESULT ret=0;


	switch (gstJcbTradeUnionStruct->EMVTradeParam->JcbCVMParameter)
	{
	   case JCB_OPS_CVM_NOCVMREQ:
			ret = SDKEMVBASE_CVM_NOCVMREQ;
	   break;
	   case JCB_OPS_CVM_OBTAINSIGNATURE:
		   ret = SDKEMVBASE_CVM_OBTAINSIGNATURE;
	  break;
	   case JCB_OPS_CVM_ONLINEPIN:
			ret = SDKEMVBASE_CVM_ONLINEPIN;
	   break;
	   case JCB_OPS_CVM_CONFVERIFIED:
		   ret = SDKEMVBASE_CVM_CONFVERIFIED;
	  break;

	   default:
			ret = SDKEMVBASE_CVM_NA;
	   break;
	}

	return ret;
}

s32 sdkJcbTransFlow1()
{
    s32 ret;
    SDK_EMVBASE_CL_HIGHESTAID tempHighestAID;

    if((gstEMVBase_UnionStruct == NULL) || (gstEMVBase_EntryPoint == NULL) || (gstJcbTradeParam == NULL)
            || (gstsdkJcbTradeTable == NULL))
    {
        return SDK_PARA_ERR;
    }

    if(gstEMVBase_UnionStruct->rapdu)
    {
        emvbase_free(gstEMVBase_UnionStruct->rapdu);
        gstEMVBase_UnionStruct->rapdu = NULL;
    }

    gstJcbTradeParam->SelectedApp = gstEMVBase_EntryPoint->SelectedApp;
    gstJcbTradeParam->SelectedAppNo = gstEMVBase_UnionStruct->SelectedAppNo;
    gstJcbTradeParam->AppListCandidate = gstEMVBase_EntryPoint->AppListCandidate;
    gstJcbTradeParam->AppListCandidatenum = gstEMVBase_EntryPoint->AppListCandidatenum;

	gstJcbTradeParam->JcbCandidateListEmptyFlag = 0;

	sdkJcbDifferEMVBase();

    while(1)
    {
        ret = sdkJcbFinalSelectedApp(gstsdkJcbTradeTable);

        if(ret == SDK_OK)
        {
            ret = sdkJcbInitialApp(gstsdkJcbTradeTable);
        }

        if(ret == SDK_EMV_AppSelectTryAgain)
        {
       		#if 0		//此处为送检处理,量产跳到app处理重选择
            ret = sdkJcbReSelectApp(gstJcbTradeUnionStruct);
            if(SDK_OK == ret)
            {
                continue;
            }
			#endif
            return ret;
        }
        else if(ret != SDK_OK)
        {
            return ret;
        }

        ret = sdkJcbReadAppData(gstsdkJcbTradeTable);

		if(ret == SDK_EMV_AppSelectTryAgain)
        {
        	#if 0		//此处为送检处理,量产跳到app处理重选择
            ret = sdkJcbReSelectApp(gstJcbTradeUnionStruct);
            if(SDK_OK == ret)
            {
                continue;
            }
			#endif
            return ret;
        }
        else if(ret != SDK_OK)
        {
            return ret;
        }

		if(gstJcbTradeParam->JcbTransactionMode == EMV_Mode)
        {
			sdkJcbReadCAPK(gstsdkJcbTradeTable); // added for recovery of the IPK and ICCPK before GAC Command--20210420
        }

		if(!gstJcbTradeParam->JcbTornTransactionFlag)
		{
			ret = sdkJcbTermRiskManage(gstsdkJcbTradeTable);
			if(ret == SDK_EMV_AppSelectTryAgain)
			{
				#if 0		//此处为送检处理,量产跳到app处理重选择
				ret = sdkJcbReSelectApp(gstJcbTradeUnionStruct);
				if(SDK_OK == ret)
				{
					continue;
				}
				#endif
				return ret;
			}
			else if(ret != SDK_OK)
			{
				return ret;
			}

			ret = sdkJcbProcessRestrict(gstsdkJcbTradeTable);

			if(ret == SDK_EMV_AppSelectTryAgain)
			{
				#if 0		//此处为送检处理,量产跳到app处理重选择
				ret = sdkJcbReSelectApp(gstJcbTradeUnionStruct);
				if(SDK_OK == ret)
				{
					continue;
				}
				#endif
				return ret;
			}
			else if(ret != SDK_OK)
			{
				return ret;
			}

			ret = sdkJcbTermiAnalys(gstsdkJcbTradeTable);
			if(ret == SDK_EMV_AppSelectTryAgain)
			{
				#if 0		//此处为送检处理,量产跳到app处理重选择
				ret = sdkJcbReSelectApp(gstJcbTradeUnionStruct);
				if(SDK_OK == ret)
				{
					continue;
				}
				#endif
				return ret;
			}
			else if(ret != SDK_OK)
			{
				return ret;
			}
		}

        if(gstJcbTradeParam->JcbTransactionMode == EMV_Mode)
        {
			if(gstsdkJcbTradeTable->PromptSoundAndLed)
			{
				gstsdkJcbTradeTable->PromptSoundAndLed(0);
			}

            ret = sdkJcbDataAuth(gstsdkJcbTradeTable);
            gstJcbTradeParam->JcbTornTransactionFlag = 0;
            if(ret != SDK_OK)
            {
                return ret;
            }
        }

		if( ( RESULT_ONLINE_WAIT == gstJcbTradeParam->TransResult || RESULT_OFFLINE_APPROVE == gstJcbTradeParam->TransResult )
				&& (gstsdkJcbTradeTable->DispRemoveCard ) )
		{
			gstsdkJcbTradeTable->DispRemoveCard();
		}

        ret = sdkJcbCardHolderVerf(gstsdkJcbTradeTable);

		gstemvbaseCVMresult = sdkJcbGetCVMresult();
		Trace("emv", "CVM Result code = %d\r\n", gstemvbaseCVMresult);

		if(ret == SDK_EMV_TransOnlineWait)
		{
			break;
		}
        else if(ret != SDK_OK)
        {
            return ret;
        }

        break;
    }



    ret = sdkJcbTransComplete(gstsdkJcbTradeTable);
	Trace("emv", "sdkJcbTransFlow1 ret = %02d\r\n", ret);
	return ret;

}
s32 sdkJcbTransFlow2()
{
    s32 ret;

    if((gstEMVBase_UnionStruct == NULL) || (gstEMVBase_EntryPoint == NULL) || (gstJcbTradeParam == NULL)
            || (gstsdkJcbTradeTable == NULL))
    {
        return SDK_PARA_ERR;
    }


    while(1)
    {
        if(gstJcbTradeParam->JcbIssuerUpdate)
        {
            if(gstsdkJcbTradeTable->ReadCard)
            {
                ret = gstsdkJcbTradeTable->ReadCard();
            }
            else
            {
                ret = SDK_EMV_ReadCardAgain;
            }
			if(SDK_OK != ret)
			{
				return ret;
			}

            ret = sdkJcbFinalSelectedApp(gstsdkJcbTradeTable);
			Trace("emv","sdkJcbFinalSelectedApp = %d\r\n",ret);
            if(ret != SDK_OK)
            {
                //continue;
                return ret;
            }
        }

        ret = sdkJcbTransProcess(gstsdkJcbTradeTable);
        if(ret == SDK_EMV_ReadCardAgain)
        {
			continue;
        }
        else if(ret != SDK_OK)
        {
            return ret;
        }

        break;
    }



    ret =  sdkJcbTransComplete(gstsdkJcbTradeTable);                                               //锟斤拷锟斤拷锟斤拷锟?

	Trace("emv", "sdkJcbTransFlow2 ret = %02d\r\n", ret);
	return ret;
}

void sdkJcbSendOutcome(u8 step)
{
	jcb_SendTransactionOutCome(gstJcbTradeUnionStruct, step);
}

void sdkJcbSetCandidateListEmptyFlag(bool flag)
{
	gstJcbTradeUnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = flag;
}

bool sdkJcbNeedIssuerUpdate()
{
	if(gstJcbTradeParam)
	{
		Trace("","gstJcbTradeParam->secondtap = %d\r\n",gstJcbTradeParam->secondtap);
	}
	if(gstJcbTradeParam && gstJcbTradeParam->secondtap)
	{
		return true;
	}
	else
	{
		return false;
	}
}

s32 sdkJcbImportOnlineResult(s32 ucOnlineResult, const u8 *pheRspCode)
{
	if(NULL == gstJcbTradeParam)
	{
		return SDK_ERR;
	}
    if(ucOnlineResult == SDK_OK)
    {
        gstJcbTradeParam->onlinetradestatus = RLT_EMV_OK;
        if(pheRspCode != NULL)
        {
            emvbase_avl_createsettagvalue(EMVTAG_AuthRespCode, pheRspCode, 2);
            TraceHex("emv", "App Set AuthCode 8A:", pheRspCode, 2);
        }
    }
    else
    {
        gstJcbTradeParam->onlinetradestatus = RLT_EMV_ERR;
    }

    Trace("emv", "App Set Online Result:%d,ucOnlineResult=%d\r\n", gstJcbTradeParam->onlinetradestatus, ucOnlineResult);
    return SDK_OK;
}

s32 sdkJcbSetStatic9F53(u8 *data)
{
	if(NULL == data)
	{
		return SDK_ERR;
	}

	if(0 == emvbase_avl_createsettagvalue(EMVTAG_JCBStaticTIP, data, 3))
	{
		return SDK_OK;
	}
	else
	{
		return SDK_ERR;
	}
}

s32 sdkJcbGetStatic9F53(u8 *data)
{
	if(NULL == data)
	{
		return SDK_ERR;
	}

	if(0 == emvbase_avl_gettagvalue_spec(EMVTAG_JCBStaticTIP, data, 0, 3))
	{
		return SDK_OK;
	}
	else
	{
		return SDK_ERR;
	}

}

s32 sdkJcbGetLibVersion(u8 *version)
{
	if(version == NULL)
	{
		return SDK_PARA_ERR;
	}

	jcb_GetLibVersion(version);
	return SDK_OK;
}

s32 sdkJcbSendUIRequest(JCBTradeUnionStruct *tempApp_UnionStruct, u8 MessageID, u8 Status, u8 *RemovalTimeout)
{
	if(NULL == tempApp_UnionStruct || NULL == tempApp_UnionStruct->EMVTradeParam || NULL == tempApp_UnionStruct->EMVTradeParam->pJcbUserInterfaceRequestData)
	{
		return SDK_PARA_ERR;
	}
	jcb_SendUIRequest(tempApp_UnionStruct, MessageID, Status, RemovalTimeout);
}

