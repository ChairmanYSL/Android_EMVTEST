#include "dlljcb.h"
#include "dlljcbprivate.h"
#include "stdlib.h"

JCBTORNRECOVERYCONTEXT gJCBTornRecoveryContext;

static const EMVBASETAG JcbDDAmandatoryTag[] =
{
    {EMVTAG_CAPKI},
    {EMVTAG_ICCPKCert},
    {EMVTAG_ICCPKExp},
    {EMVTAG_IPKCert},
    {EMVTAG_IPKExp},
    {"\x00\x00\x00"}
};



static int GetCompileTime(char *pasCompileTime, const char *pasDate, const char *pasTime)
{
	char temp_date[64] = {0},str_year[5] = {0}, str_month[4] = {0}, str_day[3] = {0};
	char temp_time[64] = {0},str_hour[2] = {0}, str_min[2] = {0}, str_sec[2] = {0};
	char en_month[12][4]={ "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	char tempCompileTime[20] = {0};
	int i = 0;

	if(pasCompileTime == NULL || pasDate == NULL || pasTime == NULL)
	{
		return 0;
	}

	sprintf(temp_date,"%s",pasDate);    //"Sep 7 2012"
	sscanf(temp_date,"%s %s %s",str_month, str_day, str_year);

	for(i=0; i < 12; i++)
	{
		if(strncmp(str_month,en_month[i],3)==0)
		{
		    memset(str_month, 0, sizeof(str_month));
	  		sprintf(str_month, "%02d", i + 1);
	        break;
	    }
	}

	if(strlen(str_day)==1)//ÔøΩÔøΩÔøΩÔøΩÔøΩÔøΩŒ™1ŒªÔøΩÔøΩ“™«∞ÔøΩÔøΩ0x30
	{
		str_day[1]=str_day[0];
		str_day[0]=0x30;
	}

	sprintf(pasCompileTime, "%s%s%s", str_year, str_month, str_day);
	sprintf(tempCompileTime, " %s", pasTime);
	strcat(pasCompileTime, tempCompileTime);

	return 0;
}

void jcb_GetLibVersion(unsigned char *version)
{
	GetCompileTime(version, __DATE__, __TIME__);
}

void jcb_SendUIRequest(JCBTradeUnionStruct *tempApp_UnionStruct, unsigned char MessageID, unsigned char Status, unsigned char *RemovalTimeout)
{
	JCBUSERINTERFACEREQUESTDATA *tempuserinterfacerequestdata;
	unsigned char *tempRemovalTimeout;
	unsigned short templen = 0;

    tempuserinterfacerequestdata = tempApp_UnionStruct->EMVTradeParam->pJcbUserInterfaceRequestData;

	tempuserinterfacerequestdata->sendMsgFlag = 1;
	tempuserinterfacerequestdata->MessageID = MessageID;
	tempuserinterfacerequestdata->Status = Status;
	if(RemovalTimeout)
	{
		memcpy(tempuserinterfacerequestdata->RemovalTimeout, RemovalTimeout, 2);
	}
	else
	{
		emvbase_avl_gettagvalue_all(EMVTAG_JCBRemovalTimeout, tempRemovalTimeout, &templen);
	}

	if(tempApp_UnionStruct->DispUserInterfaceRequestParamer)
	{
	  tempApp_UnionStruct->DispUserInterfaceRequestParamer(tempApp_UnionStruct->EMVTradeParam, JCB_OPS_STATUS_ONLINE_PREMENTANDHOLD, tempuserinterfacerequestdata);
	}

	if(tempApp_UnionStruct->SendUserInterfaceRequestParameter)
	{
	  tempApp_UnionStruct->SendUserInterfaceRequestParameter(tempApp_UnionStruct->EMVTradeParam, JCB_OPS_STATUS_ONLINE_PREMENTANDHOLD);
	}

}


//zwb 180102
void jcb_SendTransactionOutCome(JCBTradeUnionStruct *tempApp_UnionStruct, unsigned char step)
{

	JCBOUTPARAMETERSET *tempoutparameterset;
	JCBUSERINTERFACEREQDATA *tempuserinterfacereqdata;
	JCBUSERINTERFACERESTARTDATA *tempuserinterfacerestartdata;
	JCBUSERINTERFACEREQUESTDATA *tempuserinterfacerequestdata;
	unsigned char temp[10];

	tempoutparameterset = tempApp_UnionStruct->EMVTradeParam->pJcbOutParameterSet;
	tempuserinterfacereqdata = tempApp_UnionStruct->EMVTradeParam->pJcbUserInterfaceReqData;
	tempuserinterfacerestartdata = tempApp_UnionStruct->EMVTradeParam->pJcbUserInterfaceRestartData;
    tempuserinterfacerequestdata = tempApp_UnionStruct->EMVTradeParam->pJcbUserInterfaceRequestData;

	memset(temp,0,sizeof(temp));
    EMVBase_Trace("\r\nstart Send Outcome\r\n");
    EMVBase_Trace("\r\nseterr step = %d\r\n",step);

	#if 0
	if(tempuserinterfacerequestdata->sendMsgFlag && (step == JCB_OPS_STATUS_ENDAPPLICATION))
	{
		if(tempApp_UnionStruct->DispUserInterfaceRequestParamer)
		{
			tempApp_UnionStruct->DispUserInterfaceRequestParamer(tempApp_UnionStruct->EMVTradeParam,JCB_OPS_STATUS_ONLINE_PREMENTANDHOLD,tempuserinterfacerequestdata);
		}

		memset(tempuserinterfacereqdata, 0x00, sizeof(JCBUSERINTERFACEREQDATA));

		tempuserinterfacereqdata->MessageID = tempuserinterfacerequestdata->MessageID;
		tempuserinterfacereqdata->Status = tempuserinterfacerequestdata->Status;

		if(tempApp_UnionStruct->senduserinterfacerequestdata)
		{
			tempApp_UnionStruct->senduserinterfacerequestdata(tempApp_UnionStruct->EMVTradeParam, JCB_OPS_STATUS_ONLINE_PREMENTANDHOLD);
		}
	}
	#endif

	memset(tempoutparameterset, 0xFF, sizeof(JCBOUTPARAMETERSET));
	memset(tempuserinterfacereqdata, 0x00, sizeof(JCBUSERINTERFACEREQDATA));
	memset(tempuserinterfacerestartdata, 0x00, sizeof(JCBUSERINTERFACERESTARTDATA));

	memcpy(tempoutparameterset->RemovalTimeout, "\x00\x00", 2);

	if(step == JCB_OPS_STATUS_APPROVED) // 1
	{
		tempoutparameterset->status = step;
		//emvbase_avl_gettagvalue_spec(TAG_JCBIssuerUpdateParameter, (unsigned char *)&(tempoutparameterset->CVM), 0,1);
		tempoutparameterset->CVM = tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter;
		tempoutparameterset->UIRequestOnOutPresent = 1;
		tempoutparameterset->DataRecordPresent = 1;
		tempoutparameterset->Receipt = 1;
		memcpy(tempoutparameterset->RemovalTimeout, "\x00\x00", 2);

		if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode)
		{
			if(tempoutparameterset->CVM == JCB_OPS_CVM_OBTAINSIGNATURE)
			{
				tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_APPROVEDSIGN;
			}
			else
			{
				tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_APPROVED;
			}
		}
		tempuserinterfacereqdata->Status = JCB_USERREQDATA_STATUS_CARDREADSUCCESS;
		if(emvbase_avl_checkiftagexist(EMVTAG_JCBOfflineBalance))
		{
			tempuserinterfacereqdata->ValueQualifier = JCB_USERREQDATA_VALUEQUALIFIER_BALANCE;// balance
			memset(temp, 0x00, 10);
			emvbase_avl_gettagvalue_spec(EMVTAG_JCBOfflineBalance, temp, 0, 6);
			memcpy(tempuserinterfacereqdata->value, temp, 6);
			emvbase_avl_gettagvalue_spec(EMVTAG_CountryCode,&temp[6], 0, 2);
			memcpy(tempuserinterfacereqdata->CurrencyCode, &temp[6], 2);
		}
		tempuserinterfacereqdata->sendMsgFlag = 1;

	}
	else if(step == JCB_OPS_STATUS_ONLINE) // 2
	{
		EMVBase_Trace("temp CVM == %x\r\n", tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter);
		EMVBase_Trace("tempoutparameterset->CVM == %x\r\n", tempoutparameterset->CVM);
		tempoutparameterset->status = step;
		tempoutparameterset->CVM = tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter;
		EMVBase_Trace("tempoutparameterset->CVM1 == %d\r\n", tempoutparameterset->CVM);
		tempoutparameterset->UIRequestOnOutPresent = 1;
		tempoutparameterset->DataRecordPresent = 1;
		memcpy(tempoutparameterset->RemovalTimeout, "\x00\x00", 2);


//		emvbase_avl_gettagvalue_spec(EMVTAG_JCBIssuerUpdateParameter, (unsigned char *)&(tempoutparameterset->CVM), 0,1);


		EMVBase_Trace("tempoutparameterset->CVM2 == %d\r\n", tempoutparameterset->CVM);
		if(tempoutparameterset->CVM == JCB_OPS_CVM_ONLINEPIN)
		{
			tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_PLSENTERPIN;
		}
		else
		{
			tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_AUTHORISINGPLSWAIT;
		}

		EMVBase_Trace("MessageID = %d\r\n", tempuserinterfacereqdata->MessageID);

		tempuserinterfacereqdata->Status = JCB_USERREQDATA_STATUS_CARDREADSUCCESS;
		if(emvbase_avl_checkiftagexist(EMVTAG_JCBOfflineBalance))
		{
			tempuserinterfacereqdata->ValueQualifier = JCB_USERREQDATA_VALUEQUALIFIER_BALANCE;// balance
			memset(temp, 0x00, 10);
			emvbase_avl_gettagvalue_spec(EMVTAG_JCBOfflineBalance, temp, 0, 6);
			memcpy(tempuserinterfacereqdata->value, temp, 6);
			emvbase_avl_gettagvalue_spec(EMVTAG_CountryCode,&temp[6], 0, 2);
			memcpy(tempuserinterfacereqdata->CurrencyCode, &temp[6], 2);
		}
		tempuserinterfacereqdata->sendMsgFlag = 1;

	}
	else if(step == JCB_OPS_STATUS_ONLINE_TWOPRESENTMENTS)// 3
	{
		tempoutparameterset->status = step;
		tempoutparameterset->start = JCB_OPS_START_B;
		//tempoutparameterset->OnlineResponseData = 1;
		tempoutparameterset->CVM = tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter;
		tempoutparameterset->UIRequestOnOutPresent = 1;
		tempoutparameterset->UIRequestOnRestartPresent = 1;
		tempoutparameterset->DataRecordPresent = 1;
		memcpy(tempoutparameterset->RemovalTimeout, "\x00\x00", 2);
		//emvbase_avl_gettagvalue_spec(EMVTAG_JCBIssuerUpdateParameter, (unsigned char *)&(tempoutparameterset->CVM), 0,1);

		if(tempoutparameterset->CVM == JCB_OPS_CVM_ONLINEPIN)
		{
			tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_PLSENTERPIN;
		}
		else
		{
			tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_AUTHORISINGPLSWAIT;
		}
		//tempuserinterfacereqdata->MessageID = gJCBUIOnOutcomeMsgID;
		tempuserinterfacereqdata->Status = JCB_USERREQDATA_STATUS_CARDREADSUCCESS;
		if(emvbase_avl_checkiftagexist(EMVTAG_JCBOfflineBalance))
		{
			tempuserinterfacereqdata->ValueQualifier = JCB_USERREQDATA_VALUEQUALIFIER_BALANCE;// balance
			memset(temp, 0x00, 10);
			emvbase_avl_gettagvalue_spec(EMVTAG_JCBOfflineBalance, temp, 0, 6);
			memcpy(tempuserinterfacereqdata->value, temp, 6);
			emvbase_avl_gettagvalue_spec(EMVTAG_CountryCode,&temp[6], 0, 2);
			memcpy(tempuserinterfacereqdata->CurrencyCode, &temp[6], 2);
		}
		tempuserinterfacereqdata->sendMsgFlag = 1;

		tempuserinterfacerestartdata->MessageID = JCB_USERREQDATA_MSGID_TRYAGAIN;
		tempuserinterfacerestartdata->Status = JCB_USERREQDATA_STATUS_READYTOREAD;
		tempuserinterfacerestartdata->sendMsgFlag = 1;
	}
	else if(step == JCB_OPS_STATUS_ONLINE_PREMENTANDHOLD) // 4
	{
		tempoutparameterset->status = step;
		tempoutparameterset->start = JCB_OPS_START_D;
		tempoutparameterset->CVM = tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter;
		//tempoutparameterset->OnlineResponseData = 1;
		tempoutparameterset->UIRequestOnOutPresent = 1;
		tempoutparameterset->UIRequestOnRestartPresent = 1;
		tempoutparameterset->DataRecordPresent = 1;
		emvbase_avl_gettagvalue_spec(EMVTAG_JCBRemovalTimeout,tempoutparameterset->RemovalTimeout, 0, 2);
		EMVBase_TraceHex("EMVTAG_JCBRemovalTimeout",tempoutparameterset->RemovalTimeout,2);
		//memcpy(tempoutparameterset->RemovalTimeout, "\x00\x00", 2);
		//emvbase_avl_gettagvalue_spec(EMVTAG_JCBIssuerUpdateParameter, (unsigned char *)&(tempoutparameterset->CVM), 0,1);


		if(tempoutparameterset->CVM == JCB_OPS_CVM_ONLINEPIN)
		{
			tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_PLSENTERPIN;
		}
		else
		{
			tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_AUTHORISINGPLSWAIT;
		}
		//tempuserinterfacereqdata->MessageID = gJCBUIOnOutcomeMsgID;
		tempuserinterfacereqdata->Status = JCB_USERREQDATA_STATUS_PROCESSING;
		if(emvbase_avl_checkiftagexist(EMVTAG_JCBOfflineBalance))
		{
			tempuserinterfacereqdata->ValueQualifier = JCB_USERREQDATA_VALUEQUALIFIER_BALANCE;// balance
			memset(temp, 0x00, 10);
			emvbase_avl_gettagvalue_spec(EMVTAG_JCBOfflineBalance, temp, 0, 6);
			memcpy(tempuserinterfacereqdata->value, temp, 6);
			emvbase_avl_gettagvalue_spec(EMVTAG_CountryCode,&temp[6], 0, 2);
			memcpy(tempuserinterfacereqdata->CurrencyCode, &temp[6], 2);
		}
		tempuserinterfacereqdata->sendMsgFlag = 1;

		tempuserinterfacerestartdata->MessageID = JCB_USERREQDATA_MSGID_PROCESSING;
		tempuserinterfacerestartdata->Status = JCB_USERREQDATA_STATUS_PROCESSING;
		tempuserinterfacerestartdata->sendMsgFlag = 1;

		if(memcmp(tempoutparameterset->RemovalTimeout, "\x00\x00", 2))
		{
			tempuserinterfacerequestdata->MessageID = JCB_USERREQDATA_MSGID_CARDREADOK;
			tempuserinterfacerequestdata->Status = JCB_USERREQDATA_STATUS_CARDREADSUCCESS;
			memcpy(tempuserinterfacerequestdata->RemovalTimeout, tempoutparameterset->RemovalTimeout, 2);
			tempuserinterfacerequestdata->sendMsgFlag = 1;
		}
	}
	else if(step == JCB_OPS_STATUS_DECLINED)// 5
	{
		tempoutparameterset->status = step;
		tempoutparameterset->UIRequestOnOutPresent = 1;
		tempoutparameterset->DataRecordPresent = 1;
		//tempoutparameterset->opssendflag = 1;
		memcpy(tempoutparameterset->RemovalTimeout, "\x00\x00", 2);

		tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_NOTAUTHORISED;
		tempuserinterfacereqdata->Status = JCB_USERREQDATA_STATUS_CARDREADSUCCESS;
		if(emvbase_avl_checkiftagexist(EMVTAG_JCBOfflineBalance))
		{
			tempuserinterfacereqdata->ValueQualifier = JCB_USERREQDATA_VALUEQUALIFIER_BALANCE;// balance
			memset(temp, 0x00, 10);
			emvbase_avl_gettagvalue_spec(EMVTAG_JCBOfflineBalance, temp, 0, 6);
			memcpy(tempuserinterfacereqdata->value, temp, 6);
			emvbase_avl_gettagvalue_spec(EMVTAG_CountryCode,&temp[6], 0, 2);
			memcpy(tempuserinterfacereqdata->CurrencyCode, &temp[6], 2);
		}
		tempuserinterfacereqdata->sendMsgFlag = 1;

	}
	else if(step == JCB_OPS_STATUS_TRYANOTHERINTERFACE)// 6
	{
		tempoutparameterset->status = step;
		tempoutparameterset->UIRequestOnOutPresent = 1;
		memcpy(tempoutparameterset->RemovalTimeout, "\x00\x00", 2);

		tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_INSERTCARD;
		tempuserinterfacereqdata->Status = JCB_USERREQDATA_STATUS_READYTOREAD;
		tempuserinterfacereqdata->sendMsgFlag = 1;
	}
	else if(step == JCB_OPS_STATUS_ENDAPPLICATION)// 7
	{
		tempoutparameterset->status = step;
		EMVBase_Trace("JcbCandidateListEmptyFlag = %d\r\n", tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag);
		if(tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag)
		{
			tempoutparameterset->UIRequestOnOutPresent = 1;
			tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_ERROR_TRYOTHERCARD;
			tempuserinterfacereqdata->Status = JCB_USERREQDATA_STATUS_READYTOREAD;
			tempuserinterfacereqdata->sendMsgFlag = 1;
		}
		memcpy(tempoutparameterset->RemovalTimeout, "\x00\x00", 2);
	}
	else if(step == JCB_OPS_STATUS_ENDAPPLICATION_COMMUNICATIONERROR)// 8
	{
		tempoutparameterset->status = step;
		tempoutparameterset->start = JCB_OPS_START_B;
		tempoutparameterset->UIRequestOnOutPresent = 1;
		tempoutparameterset->UIRequestOnRestartPresent = 1;
		memcpy(tempoutparameterset->RemovalTimeout, "\x00\x00", 2);

		tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_TRYAGAIN;
		tempuserinterfacereqdata->Status = JCB_USERREQDATA_STATUS_PROCESSINGERR;
		tempuserinterfacereqdata->HoldTime = 0x13;
		tempuserinterfacereqdata->sendMsgFlag = 1;

		tempuserinterfacerestartdata->MessageID = JCB_USERREQDATA_MSGID_TRYAGAIN;
		tempuserinterfacerestartdata->Status = JCB_USERREQDATA_STATUS_READYTOREAD;
		tempuserinterfacerestartdata->sendMsgFlag = 1;


	}
	else if(step == JCB_OPS_STATUS_ENDAPPLICATION_ONDEVICECVM)// 9
	{
		tempoutparameterset->status = step;
		tempoutparameterset->start = JCB_OPS_START_B;
		tempoutparameterset->UIRequestOnOutPresent = 1;
		tempoutparameterset->UIRequestOnRestartPresent = 1;
		tempoutparameterset->FieldOffRequest = 0x13;
		memcpy(tempoutparameterset->RemovalTimeout, "\x00\x00", 2);

		tempuserinterfacereqdata->MessageID = JCB_USERREQDATA_MSGID_SEEPHONE;
		tempuserinterfacereqdata->Status = JCB_USERREQDATA_STATUS_PROCESSINGERR;
		tempuserinterfacereqdata->HoldTime = 0x13;
		tempuserinterfacereqdata->sendMsgFlag = 1;

		tempuserinterfacerestartdata->MessageID = JCB_USERREQDATA_MSGID_TRYAGAIN;
		tempuserinterfacerestartdata->Status = JCB_USERREQDATA_STATUS_READYTOREAD;
		tempuserinterfacerestartdata->sendMsgFlag = 1;
	}
	else if(step == JCB_OPS_STATUS_SELECTNEXT)// 10
	{
		tempoutparameterset->status = step;
		tempoutparameterset->start = JCB_OPS_START_C;
		memcpy(tempoutparameterset->RemovalTimeout, "\x00\x00", 2);
	}


	if(tempApp_UnionStruct->SendOutComeParameterSet)
	{
		tempApp_UnionStruct->SendOutComeParameterSet(tempApp_UnionStruct->EMVTradeParam,step);
	}

	if(tempuserinterfacereqdata->sendMsgFlag)
	{
		if(tempApp_UnionStruct->senduserinterfacerequestdata)
		{
			tempApp_UnionStruct->senduserinterfacerequestdata(tempApp_UnionStruct->EMVTradeParam,step);
		}
		if(tempApp_UnionStruct->Dispuserinterfacerequestdata)
		{
			tempApp_UnionStruct->Dispuserinterfacerequestdata(tempApp_UnionStruct->EMVTradeParam,step,tempuserinterfacereqdata);
		}
		tempuserinterfacereqdata->sendMsgFlag = 0;
	}


	if(tempuserinterfacerestartdata->sendMsgFlag)
	{
		if(tempApp_UnionStruct->SendUserInterfaceRestartData)
		{
			tempApp_UnionStruct->SendUserInterfaceRestartData(tempApp_UnionStruct->EMVTradeParam,step);
		}
		if(tempApp_UnionStruct->DispUserInterfaceRestartData)
		{
			tempApp_UnionStruct->DispUserInterfaceRestartData(tempApp_UnionStruct->EMVTradeParam,step,tempuserinterfacerestartdata);
		}
		tempuserinterfacerestartdata->sendMsgFlag = 0;
	}
	#if 0
	EMVBase_Trace("tempuserinterfacerequestdata->sendMsgFlag = %d\r\n",tempuserinterfacerequestdata->sendMsgFlag);
	//present & hold µƒ ±∫Ú“™«ÛΩª“◊Ω· ¯÷Æ∫Û≤≈ƒ‹Ã· æ“∆ø®£¨∂¯≤ª «‘⁄ ‰»Îpresent & hold outcomeµƒ ±∫Ú, À˘“‘‘⁄present & hold outcomeµƒ ±∫ÚsetŒ™1,‘Ÿ ‰≥ˆΩª“◊Ω·π˚µƒ ±∫Ú≤≈ª·Ã· æ
	if((step == JCB_OPS_STATUS_APPROVED || step == JCB_OPS_STATUS_DECLINED || step == JCB_OPS_STATUS_ENDAPPLICATION) && (tempuserinterfacerequestdata->sendMsgFlag))
	{
		if(tempApp_UnionStruct->DispUserInterfaceRequestParamer)
		{
			tempApp_UnionStruct->DispUserInterfaceRequestParamer(tempApp_UnionStruct->EMVTradeParam,JCB_OPS_STATUS_ONLINE_PREMENTANDHOLD,tempuserinterfacerequestdata);
		}

		tempuserinterfacerequestdata->sendMsgFlag = 0;
	}
	#endif
}

unsigned char jcb_TornRecovery(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	EMVBase_Trace( "TornFlag=%d\r\n", gJCBTornRecoveryContext.RecoveryTornEMVTransactionFlag);
	if(gJCBTornRecoveryContext.RecoveryTornEMVTransactionFlag == 1)
	{
		EMVBase_Trace("JcbTornTransactionFlag\r\n");
		gJCBTornRecoveryContext.RecoveryTornEMVTransactionFlag = 0;
		tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag = 1;
		if(!emvbase_avl_checkiftagexist(EMVTAG_ReferenceControlParameter))
		{
			emvbase_avl_createsettagvalue(EMVTAG_ReferenceControlParameter, &gJCBTornRecoveryContext.ReferenceControlParameter,1);
		}

		emvbase_avl_createsettagvalue(EMVTAG_UnpredictNum,gJCBTornRecoveryContext.UnpredictNum, 4);

		tempApp_UnionStruct->EMVTradeParam->JcbTornTrack2Len = gJCBTornRecoveryContext.TornTrack2len;
		memcpy(tempApp_UnionStruct->EMVTradeParam->JcbTornTrack2Data, gJCBTornRecoveryContext.TornTrack2Data, tempApp_UnionStruct->EMVTradeParam->JcbTornTrack2Len);

		tempApp_UnionStruct->EMVTradeParam->JcbTornCDAHashDatalen = gJCBTornRecoveryContext.TornCDAHashDatalen;
		memcpy(tempApp_UnionStruct->EMVTradeParam->JcbTornCDAHashDataBuffer, gJCBTornRecoveryContext.TornCDAHashDataBuffer, tempApp_UnionStruct->EMVTradeParam->JcbTornCDAHashDatalen);

		return RLT_EMV_OK;
	}

	return RLT_EMV_ERR;
}

/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝π¶ƒ‹: Ω‚ŒˆSELECT√¸¡Ó∑µªÿµƒø® ˝æ›
   »Îø⁄≤Œ ˝: selectRet - ø®Œƒº˛—°‘Ò”¶¥Ω·ππÃÂ
          DataOut -  ø®∑µªÿµƒ ˝æ›
          LenOut -   ø®∑µªÿµƒ ˝æ›≥§∂»
   ∑µ ªÿ ÷µ: EMV_OK£®0£©-≥…π¶
          ERR_EMV_IccDataFormat£®34£© - ICø® ˝æ›∏Ò Ω¥Ì
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:       20131010
 ********************************************************************/
unsigned char jcb_ReadSelectRetData(EMVBASE_SELECT_RET* selectRet, unsigned char* DataOut, unsigned short LenOut, JCBTradeUnionStruct *tempAppUnionStruct)
{
	  int matchnum;
    EMVBASE_LISTAPPDATA *tempAppAppData;
    EMVBASE_TERMAPP tempAppTermApp;
    unsigned char *tempappnum;
    //unsigned char *temptermlistnum;
    unsigned char j, k;     //i,
    unsigned short index, indexFCI, indexFCIProp, len, lenFCI, lenFCIProp;
    int indexIssuerDiscret, lenIssuerDiscret;
    int indexAppApp, lenAppApp;
    unsigned char priorityexist;
	unsigned char templen, ExtendedLen;
	unsigned char tempindex;
	unsigned char errindex = 0,i;
	unsigned char errindex2 = 0;
	unsigned short tempBF0Clen;


    tempappnum = &(tempAppUnionStruct->EMVTradeParam->AppListCandidatenum);
    tempAppAppData = tempAppUnionStruct->EMVTradeParam->AppListCandidate;
    *tempappnum = 0;


    index = 0;

    if(DataOut[index] != 0x6F)          //FCIƒ£∞Â±Í ∂
    {
        return RLT_ERR_EMV_IccDataFormat;         //FCI template
    }
    index++;

    if(EMVBase_ParseExtLen(DataOut, &index, &lenFCI))
    {
        return RLT_ERR_EMV_IccDataFormat;
    }
    indexFCI = index;

    while(index < indexFCI + lenFCI)
    {
        if(index >= LenOut) { return RLT_ERR_EMV_IccDataFormat; }

        if(DataOut[index] == 0xFF || DataOut[index] == 0x00) { index++; continue; }
        else if(DataOut[index] == 0x84)         //DF√˚
        {
            if(selectRet->DFNameExist == 1) { return RLT_ERR_EMV_IccDataFormat; }
            index++;
            selectRet->DFNameLen = DataOut[index];

            if(selectRet->DFNameLen > 16) { return RLT_ERR_EMV_IccDataFormat; }
            memcpy(selectRet->DFName, DataOut + index + 1, selectRet->DFNameLen);
            index += selectRet->DFNameLen + 1;
            selectRet->DFNameExist = 1;
        }
        else if(DataOut[index] == 0xA5)         //FCI ˝æ›◊®”√ƒ£∞Â
        {
            selectRet->FCIPropExist = 1;
            index++;

            if(EMVBase_ParseExtLen(DataOut, &index, &lenFCIProp))
            {
                return RLT_ERR_EMV_IccDataFormat;
            }
            indexFCIProp = index;

			EMVBase_Trace( "indexFCIProp=%d lenFCIProp=%d\r\n",indexFCIProp, lenFCIProp);
            while(index < indexFCIProp + lenFCIProp)
            {
                if(index >= LenOut) { return RLT_ERR_EMV_IccDataFormat; }

                if(DataOut[index] == 0xFF || DataOut[index] == 0x00) { index++; continue; }
                else if(!memcmp(DataOut + index, "\xBF\x0C", 2))             //FCI ∑¢ø®∑Ω◊‘∂®“Â ˝æ›
                {
                    if(selectRet->IssuerDiscretExist == 1) { return RLT_ERR_EMV_IccDataFormat; }
                    index += 2;

                    if(EMVBase_ParseExtLen(DataOut, &index, &len))
                    {
                    	EMVBase_Trace( "bf0c error!\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }

                    if(len > 253) { return RLT_ERR_EMV_IccDataFormat; }
                    selectRet->IssuerDiscretLen = len;                     //sxl 090211
                    EMVBase_Trace( "IssuerDiscretLen=%d index=%d\r\n", len, index);
                    //memcpy(selectRet->IssuerDiscret,DataOut+index,len);
                    selectRet->IssuerDiscretExist = 1;

                    indexIssuerDiscret = index;
                    lenIssuerDiscret = len;

                    while(index < indexIssuerDiscret + lenIssuerDiscret)
                    {
                    	EMVBase_Trace( "index=%d indexIssuerDiscret=%d lenIssuerDiscret=%d",index, indexIssuerDiscret,lenIssuerDiscret);
                        if(index >= LenOut) { return RLT_ERR_EMV_IccDataFormat; }

                        if(DataOut[index] == 0xFF || DataOut[index] == 0x00) { index++; continue; }
                        else if(DataOut[index] == 0x61)
                        {
                            memset((unsigned char *)&tempAppAppData[*tempappnum], 0, sizeof(EMVBASE_LISTAPPDATA));                           //«Â”¶”√
                            priorityexist = 0;
                            index += 1;

                            if(EMVBase_ParseExtLen(DataOut, &index, &len))
                            {
                            	EMVBase_Trace( "tag 61 error\r\n");
                                return RLT_ERR_EMV_IccDataFormat;
                            }
                            indexAppApp = index;
                            lenAppApp = len;

							errindex = index;
							if(len == 0)
							{
								EMVBase_Trace("tag 61 length zero!\r\n");
								return RLT_ERR_EMV_IccDataFormat;
							}

                            while(index < indexAppApp + lenAppApp)
                            {
                            	EMVBase_Trace( "index=%d LenOut=%d\r\n", index, LenOut);
                                if(index >= LenOut)
								{
									EMVBase_Trace( "error11111");
									//return ERR_EMV_IccDataFormat;
									break;
								}

                                if(DataOut[index] == 0xFF || DataOut[index] == 0x00) { index++; continue; }
                                else if(DataOut[index] == 0x4F)                                 //AID
                                {
                                    index += 1;
                                    (tempAppAppData + (*tempappnum))->AIDLen = DataOut[index];
#if 1
									//if((tempAppAppData + (*tempappnum))->AIDLen > 16) { (tempAppAppData + (*tempappnum))->AIDLen = 16; }
									if((tempAppAppData + (*tempappnum))->AIDLen > 16)
									{
										(tempAppAppData + (*tempappnum))->AIDLen = 0;
									}

#else
									if((tempAppAppData + (*tempappnum))->AIDLen > 16) //zwb  for jcb 180312
									{
										//return ERR_EMV_IccDataFormat;
										index = indexIssuerDiscret + lenIssuerDiscret;
										break;
									}
#endif


									index++;
                                    j = (tempAppAppData + (*tempappnum))->AIDLen;
                                    memcpy((tempAppAppData + (*tempappnum))->AID, DataOut + index, j);
                                    EMVBase_TraceHex( "4F aid &len", DataOut + index, j);
                                    index += j;
                                }
                                else if(DataOut[index] == 0x50)                                 //AppLabel
                                {//500B 4A434220437265646974870101
                                	EMVBase_Trace("goto deal tag 50!\r\n");
                                    index += 1;
                                    (tempAppAppData + (*tempappnum))->AppLabelLen = DataOut[index];

                                    if((tempAppAppData + (*tempappnum))->AppLabelLen > 16) { (tempAppAppData + (*tempappnum))->AppLabelLen = 16; }
                                    index++;
                                    j = (tempAppAppData + (*tempappnum))->AppLabelLen;
                                    memcpy((tempAppAppData + (*tempappnum))->AppLabel, DataOut + index, j);
                                    index += j;
                                }
                                else if(DataOut[index] == 0x87)    //AppPriority
                                {
									EMVBase_Trace("goto deal tag 87!\r\n");
									index ++;
									templen = DataOut[index];
									if(templen == 1)//870201   870001	87020001   870101
									{
										index++;
										(tempAppAppData + (*tempappnum))->Priority = DataOut[index];
										index++;
										priorityexist = 1;

									}
									else
									{
										index++;
										templen = 0;
										index +=templen;
									}

                                }
								else if(memcmp(DataOut + index, "\x9F\x2A", 2) == 0)//kernel ID
								{//9F2A0105  9F2A04050001  9F2A0305000001  9F2A0005

									index += 2;
									templen = DataOut[index];
									EMVBase_Trace( "tag9F2A templen=%d\r\n", templen);
									if(templen == 1)
									{
										index++;
										EMVBase_Trace( "tag9F2A 1byte=%02x\r\n", DataOut[index]);
										if(DataOut[index] != 0x05)
										{
											templen = 0;
											(tempAppAppData + (*tempappnum))->AIDLen = 0;
											//index--;
										}
										index += templen;
									}
									else
									{
										index++;
										templen = 0;
										index += templen;
									}
								}
								#if 1
								else if(memcmp(DataOut + index, "\x9F\x29", 2) == 0)//Extened Selection zwb 180202
								{  //9F29021122  9F29031122  9F29011122  9F29001122

									EMVBase_Trace( "goto deal tag 9F29!\r\n");
									index += 2;
									templen = DataOut[index];
									if(templen == 2)
									{
										index++;
										index += templen;
									}
									else
									{
										index++;
										templen =  0;
										index += templen;
									}
								}
								#endif
                                else
                                {
									errindex = index;

                                    k = DataOut[index];
									EMVBase_Trace( "xxx k=%02x\r\n", k);

                                    if((k & 0x1F) == 0x1F)
                                    {
                                        index++;
                                    }
                                    index++;


									EMVBase_Trace( "index111=%d indexAppApp + lenAppApp=%d\r\n", index, indexAppApp + lenAppApp);
									EMVBase_Trace( "DataOut[index]=%02x\r\n", DataOut[index]);
									if(index < indexAppApp + lenAppApp)
									{
										if(EMVBase_ParseExtLen(DataOut, &index, &len))
										{
											break;
										}
										EMVBase_Trace( "test111111");
									}
									else
									{
										break;
									}

                                    index += len;
                                }

                            }

							EMVBase_Trace("00000index=%d LenOut=%d",index,LenOut);

							EMVBase_Trace("00000index=%d indexAppApp + lenAppApp=%d\r\n", index, indexAppApp + lenAppApp);
							if(index >  indexAppApp + lenAppApp /*&& index < LenOut*/)	// parase err
							{
								EMVBase_Trace("errindex=%d\r\n", errindex);
								index = errindex;
								for(i = errindex ; i < indexAppApp + lenAppApp ;  i++)
								{
									if(DataOut[i] == 0x61)
									{
										EMVBase_Trace("another tag 61");
										break;
									}
								}
								index = i;

							}

                            if((tempAppAppData + (*tempappnum))->AIDLen != 0)                            //»∑±£√ø∏ˆƒø¬º»Îø⁄∂º”–AID
                            {
                                //paywaveµƒ√ª”–Àµ«Â≥˛
                                if(priorityexist == 0)
                                {
                                    (tempAppAppData + (*tempappnum))->Priority = 0;
                                }
                                //sxl paywaveµƒ≤ª≈–∂œpriority◊Ó∏ﬂŒª
                                //if(((tempAppAppData + (*tempappnum))->Priority & 0x80) == 0 )
                                {
                                    //matchnum = tempAppUnionStruct->check_MatchTermAID((tempAppAppData + (*tempappnum))->ReqKernelID, (tempAppAppData + (*tempappnum))->AID, (tempAppAppData + (*tempappnum))->AIDLen, &tempAppTermApp, PAYWAVEAPP);
								#if 1
									//matchnum = tempAppUnionStruct->check_MatchTermAID((tempAppAppData + (*tempappnum))->ReqKernelID, (tempAppAppData + (*tempappnum))->AID, (tempAppAppData + (*tempappnum))->AIDLen, &tempAppTermApp, EMVAPP);

									matchnum = tempAppUnionStruct->CheckMatchTermAID((tempAppAppData + (*tempappnum))->AID, (tempAppAppData + (*tempappnum))->AIDLen, &tempAppTermApp,NULL,0);
									EMVBase_Trace( "matchnum=%d tempappnum=%d\r\n", matchnum, *tempappnum);

                                    if(matchnum >= 0)
                                    {
                                        (tempAppAppData + (*tempappnum))->AidInTermLen = tempAppTermApp.AIDLen;
                                        memcpy((tempAppAppData + (*tempappnum))->AidInTerm, tempAppTermApp.AID, (tempAppAppData + (*tempappnum))->AidInTermLen);
                                        (*tempappnum)++;
                                    }
								#endif
                                }

                                //}
                            }
                        }
                        else if(!memcmp(DataOut + index, "\x9F\x4D", 2))                    //Log Entry»’÷æ»Îø⁄
                        {
                            if(selectRet->LogEntryExist == 1) { return RLT_ERR_EMV_IccDataFormat; }
                            index += 2;

                            if(EMVBase_ParseExtLen(DataOut, &index, &len))
                            {
                                return RLT_ERR_EMV_IccDataFormat;
                            }
                            selectRet->LogEntryLen = len;
                            //memcpy(selectRet->LogEntry,DataOut+index,selectRet->LogEntryLen);
                            index += selectRet->LogEntryLen;
                            selectRet->LogEntryExist = 1;
                        }
                        else
                        {
                            if(errindex2 == 0)
                            {
                            	errindex2 = index;
                            }
                            k = DataOut[index];
							EMVBase_Trace("errindex2 k=%02x\r\n", k);

                            if((k & 0x1F) == 0x1F)
                            {
                                index++;
                            }
                            index++;

							EMVBase_Trace("index2222=%d indexIssuerDiscret + lenIssuerDiscret=%d\r\n", index, indexIssuerDiscret + lenIssuerDiscret);

							if(index < indexIssuerDiscret + lenIssuerDiscret)
							{
								EMVBase_Trace("DataOut[index]=%02x\r\n", DataOut[index]);
								if(EMVBase_ParseExtLen(DataOut, &index, &len))
								{
									break;
								}
							}
							else
							{
								break;
							}

                            index += len;
                        }
                    }

					EMVBase_Trace("index3333=%d indexIssuerDiscret + lenIssuerDiscret=%d\r\n", index, indexIssuerDiscret + lenIssuerDiscret);
					if(index >	indexIssuerDiscret + lenIssuerDiscret)	// parase err
					{
						EMVBase_Trace("errindex2=%d\r\n", errindex2);
						index = errindex2;
						for(i = errindex2 ; i < indexIssuerDiscret + lenIssuerDiscret ; i++)
						{
							if(DataOut[i] == 0x61)
							{
								break;
							}
						}
						index = i;

					}
				}
                else                //other unknown TLV data
                {
                    k = DataOut[index];
					EMVBase_Trace( "2222k=%d\r\n",k);

                    if((k & 0x1F) == 0x1F)
                    {
                        index++;
                    }
                    index++;

                    if(EMVBase_ParseExtLen(DataOut, &index, &len))
                    {
						EMVBase_Trace( "yyyyyyyyyyyy\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }
                    index += len;
                }
            }

			EMVBase_Trace("IssuerDiscretLen=%d index=%d indexIssuerDiscret=%d", selectRet->IssuerDiscretLen, index, indexIssuerDiscret);
			if( selectRet->IssuerDiscretLen != (index - indexIssuerDiscret))// BF0C length value
			{// the length  must correspond to the length of value
				EMVBase_Trace("BF0C length value error!\r\n");
				return RLT_ERR_EMV_IccDataFormat;
			}

            if(index != indexFCIProp + lenFCIProp)
            {
				EMVBase_Trace( "zzzzzzzzzzzzz\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
        }
        else
        {
            k = DataOut[index];

            if((k & 0x1F) == 0x1F)
            {
                index++;
            }
            index++;

            if(EMVBase_ParseExtLen(DataOut, &index, &len))
            {
				EMVBase_Trace( "aaaaaaaaaa\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
            index += len;
        }
    }

    if(index != indexFCI + lenFCI)
    {
		EMVBase_Trace( "bbbbbbbbbbb\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }
    return RLT_EMV_OK;
}

/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_SelectDFRetData(unsigned char indexDF,EMVBASE_SELECT_RET* selectRet,EMVBASE_APDU_RESP *apdu_r,AppUnionStruct *tempAppUnionStruct)
   ∫Ø ˝π¶ƒ‹: Ω‚ŒˆSELECT√¸¡Ó∑µªÿµƒø® ˝æ›
   »Îø⁄≤Œ ˝: indexDF - DFÀ˜“˝
          EMVBASE_SELECT_RET -  —°‘ÒŒƒº˛Ω·ππÃÂ
          apdu_r -   ø®∑µªÿµƒ ˝æ›
   ∑µ ªÿ ÷µ: EMV_OK£®0£©-≥…π¶
          ERR_EMV_IccDataFormat£®34£© - ICø® ˝æ›∏Ò Ω¥Ì
          RLT_ERR_EMV_IccReturn -∆‰À˚¥ÌŒÛ∑µªÿ
          ERR_BLOCKORFORMATERR -À¯ø®
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:        20131010
 ********************************************************************/
unsigned char jcb_SelectDFRetData(EMVBASE_APDU_RESP *apdu_r, JCBTradeUnionStruct *tempAppUnionStruct)
{
    EMVBASE_SELECT_RET selectRet;


    if(apdu_r->SW1 == 0x90 && apdu_r->SW2 == 0x00)
    {
        memset((unsigned char*)&selectRet, 0, sizeof(EMVBASE_SELECT_RET));

        if(jcb_ReadSelectRetData(&selectRet, apdu_r->DataOut, apdu_r->LenOut, tempAppUnionStruct) != RLT_EMV_OK)
        {
            return RLT_ERR_EMV_IccDataFormat;
        }

        if(/*selectRet.DFNameExist == 0 ||*/ selectRet.FCIPropExist == 0) //84 or A5 not exist
        {//DF not exist continue transaction    jcb zwb 20180201
            return RLT_ERR_EMV_IccDataFormat;
        }
        //Insert DF name into tree
        //emvbase_avl_createsettagvalue(EMVEMVTAG_DFName,selectRet.DFName,selectRet.DFNameLen);

        if(selectRet.SFIExist == 1)
        {
            if(selectRet.SFI < 1 || selectRet.SFI > 10 )
            {
                return RLT_ERR_EMV_IccDataFormat;
            }
        }
        /*
           if(selectRet.LangPreferExist==1)
           {
                emvbase_avl_createsettagvalue(EMVTAG_LangPrefer,selectRet.LangPrefer,selectRet.LangPreferLen);
           }
           if(selectRet.ICTIExist==1)
           {
                emvbase_avl_createsettagvalue(EMVTAG_ICTI,&selectRet.ICTI,1);
           }
         */


        return RLT_EMV_OK;
    }
    else
    {
    	//jcb_SendTransactionOutCome(tempAppUnionStruct,JCB_OPS_STATUS_ENDAPPLICATION);
        return RLT_ERR_EMV_IccReturn;
    }
}

/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_SelectApp(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: paywave—°‘Ò”¶”√
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: Œﬁ
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:         sxl0221
 ********************************************************************/
unsigned char jcb_SelectApp(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    EMVBASE_APDU_SEND apdu_s;
    EMVBASE_APDU_RESP apdu_r;
    unsigned char retCode;



    EMVBase_COMMAND_SELECT("2PAY.SYS.DDF01", 14, 0, &apdu_s);

    tempApp_UnionStruct->IsoCommand(&apdu_s, &apdu_r);

    if(apdu_r.ReadCardDataOk != RLT_EMV_OK)
    {
        return RLT_ERR_EMV_APDUTIMEOUT;
    }
    retCode = jcb_SelectDFRetData(&apdu_r, tempApp_UnionStruct);
	if(retCode == RLT_ERR_EMV_IccDataFormat || retCode == RLT_ERR_EMV_IccReturn)
	{
		EMVBase_Trace( "PPSE ERROR!\r\n");
		EMVBase_Trace( "AppListCandidatenum=%d\r\n", tempApp_UnionStruct->EMVTradeParam->AppListCandidatenum);
			if(tempApp_UnionStruct->EMVTradeParam->AppListCandidatenum < 1)
		{
			tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = 1; //
		}
		jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
	}
	EMVBase_Trace( "jcb_SelectDFRetData retCode=%d\r\n", retCode);
	return retCode;
}

//ªÒ»°”¶”√—°‘Ò¡–±Ì
/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywaveb_GetAppCandidate(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: PaywaveªÒ»°”¶”√¡–±Ì
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: Œﬁ
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:         sxl0221
 ********************************************************************/
unsigned char jcb_GetAppCandidate(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char retcode = 0;

    retcode = jcb_SelectApp(tempApp_UnionStruct);          //Ω´÷’∂À”¶”√¡–±Ì¥´œ¬¿¥

    return retcode;
}

unsigned char jcb_checkifwrongtag(JCBTradeUnionStruct *tempApp_UnionStruct, unsigned char *tag, unsigned char taglen, EMVBASETAGCVLITEM *retrunitem, unsigned char Redundantflag)
{
	int i;
	unsigned char present = 0;
	unsigned char updateRA = 0;
	unsigned char temptag[3];
	unsigned char temptaglen;
	EMVBASETAGCVLITEM *item;
	unsigned char ret = 2;
	int ret1;
	unsigned int rundatabuflen;

	memset(temptag, 0, sizeof(temptag));
	temptaglen = (taglen > 3) ? 3 : (taglen);
	memcpy(temptag, tag, temptaglen);
	item = emvbase_avl_gettagitempointer(temptag);

	if(item != NULL)
	{
		present = 1;
		updateRA = item->updatecondition & EMVTAGUPDATECONDITION_RA;
	}

	if(present == 0)
	{
		ret1 = emvbase_TagBaseLib_read(tag, taglen, retrunitem,EMVTAGTYPE_JSPEEDY);

		if(ret1 == 0)
		{
			updateRA = retrunitem->updatecondition & EMVTAGUPDATECONDITION_RA;
		}
	}
	else
	{
		memcpy(retrunitem, item, sizeof(EMVBASETAGCVLITEM));
	}


	if(updateRA)
	{
		ret = 1;
	}

	if(Redundantflag)
	{
		rundatabuflen = tempApp_UnionStruct->EMVTradeParam->RedundantDataLen;

		if(tempApp_UnionStruct->EMVTradeParam->RedundantData == NULL)
		{
			tempApp_UnionStruct->EMVTradeParam->RedundantDataMaxLen = 256;
			tempApp_UnionStruct->EMVTradeParam->RedundantData = (unsigned char *)emvbase_malloc(256);
			if(tempApp_UnionStruct->EMVTradeParam->RedundantData)
			{
				memset(tempApp_UnionStruct->EMVTradeParam->RedundantData,0,256);
			}
		}

		if(rundatabuflen && tempApp_UnionStruct->EMVTradeParam->RedundantData)
		{
			if(EMVBase_CheckifRedundantData(temptag, tempApp_UnionStruct->EMVTradeParam->RedundantData, rundatabuflen))
			{
				ret = 0;
			}
		}
		if(tempApp_UnionStruct->EMVTradeParam->RedundantData)
		{
			if((temptaglen + rundatabuflen) < tempApp_UnionStruct->EMVTradeParam->RedundantDataMaxLen)
			{
				memcpy(&tempApp_UnionStruct->EMVTradeParam->RedundantData[rundatabuflen], temptag, temptaglen);
				tempApp_UnionStruct->EMVTradeParam->RedundantDataLen = (rundatabuflen + temptaglen);
			}
			else
			{
				if(tempApp_UnionStruct->EMVTradeParam->RedundantDataMaxLen < 512)
				{
					unsigned char *p = (unsigned char *)emvbase_malloc(512);
					if(p)
					{
						memset(p,0,512);
						memcpy(p,tempApp_UnionStruct->EMVTradeParam->RedundantData,rundatabuflen);
						if(tempApp_UnionStruct->EMVTradeParam->RedundantData)
						{
							emvbase_free(tempApp_UnionStruct->EMVTradeParam->RedundantData);
						}
						tempApp_UnionStruct->EMVTradeParam->RedundantDataMaxLen = 512;
						tempApp_UnionStruct->EMVTradeParam->RedundantData = p;
						memcpy(&tempApp_UnionStruct->EMVTradeParam->RedundantData[rundatabuflen], temptag, temptaglen);
						tempApp_UnionStruct->EMVTradeParam->RedundantDataLen = (rundatabuflen + temptaglen);
					}
				}
			}
		}
	}

	return ret;
}


unsigned char jcb_ParseAndStoreCardResponse(JCBTradeUnionStruct *tempApp_UnionStruct, unsigned char *DataOut, unsigned short *DataOutIndex, unsigned char *parsebInTable, unsigned char readstep)
{
    //unsigned char k;
    unsigned short index, len, tagdatalen;
    //unsigned char ifemvtag;  //tmpdata,tagindex,tmpval,addvalue,
    unsigned char bInTable;
    unsigned char tag[4], taglen;
    unsigned char ret;
    //unsigned char needdonextstep = 1;
    EMVBASETAGCVLITEM tagitem;

#ifdef EMVB_DEBUG
    EMVBASETAGCVLITEM *item;
    unsigned char i;
#endif

    index = *DataOutIndex;
    bInTable = 0;



    if(DataOut[index] == 0xFF || DataOut[index] == 0x00)
    {
        index++;
        *DataOutIndex = index;
        *parsebInTable = bInTable;

        return RLT_EMV_OK;
    }
    memset(tag, 0, sizeof(tag));
    taglen = 0;
    tag[taglen++] = DataOut[index];

    if((tag[0] & 0x1f) == 0x1f)
    {
        tag[taglen++] = DataOut[index + 1];

        if(tag[1] & 0x80)
        {
            tag[taglen++] = DataOut[index + 2];
        }
    }

	EMVBase_Trace("JCB-info: Tag%02x%02x parsing\r\n", tag[0], tag[1]);

    ret = jcb_checkifwrongtag(tempApp_UnionStruct, tag, taglen, &tagitem, readstep);

    if(ret == 0)
    {
    	EMVBase_Trace("JCB-error: Tag already existed\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }
    else if(ret == 2)
    {
       	if(memcmp(tag, "\x9F\x02", 2)== 0)
    	{
			if(taglen != 6)
			{
				EMVBase_Trace("tag 9F02 length error!\r\n");
				return RLT_ERR_EMV_IccDataFormat;
			}
		}
    }
    else
    {
        index += taglen;

        if(EMVBase_ParseExtLen(DataOut, &index, &len))
        {
            EMVBase_Trace("JCB-error: Tag parsing error\r\n");
            return RLT_ERR_EMV_IccDataFormat;
        }
        tagdatalen = len;

        if(len)
        {
            if(len > tagitem.maxlen || len < tagitem.minlen)
            {
            	EMVBase_Trace("JCB-info: Tag value len range: min(%d)-max(%d)\r\n", tagitem.minlen, tagitem.maxlen);
				EMVBase_Trace("JCB-info: Tag value len in card: %d\r\n", len);

                if(readstep == 0)
                {
                    if(memcmp(tag, EMVTAG_AppLabel, taglen) == 0 || memcmp(tag, EMVTAG_AppPreferName, taglen) == 0 ||
                       memcmp(tag, EMVTAG_ICTI, taglen) == 0 || memcmp(tag, EMVTAG_LangPrefer, taglen) == 0)
                    {
                        if(len > tagitem.maxlen)
                        {
                            tagdatalen = tagitem.maxlen;
                        }
                    }
                    else
                    {
                    	EMVBase_Trace("JCB-error: Tag value len error\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }
                }
                else
                {
                	EMVBase_Trace("JCB-error: Tag value len error\r\n");
                    return RLT_ERR_EMV_IccDataFormat;
                }
            }
        }

        emvbase_avl_createsettagvalue(tag, &DataOut[index], tagdatalen);

        index += len;
        bInTable = 1;
    }

    if(bInTable == 0)
    {
        index += taglen;

        if(EMVBase_ParseExtLen(DataOut, &index, &len))
        {
			EMVBase_Trace("JCB-error: Tag parsing error\r\n");
            return RLT_ERR_EMV_IccDataFormat;
        }
        index += len;

        bInTable = 1;
    }

    *DataOutIndex = index;
    *parsebInTable = bInTable;

    return RLT_EMV_OK;
}




/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝π¶ƒ‹: Ω‚ŒˆSELECT√¸¡Ó∑µªÿµƒø® ˝æ›
   »Îø⁄≤Œ ˝: selectRet - ø®Œƒº˛—°‘Ò”¶¥Ω·ππÃÂ
          DataOut -  ø®∑µªÿµƒ ˝æ›
          LenOut -   ø®∑µªÿµƒ ˝æ›≥§∂»
   ∑µ ªÿ ÷µ: EMV_OK£®0£©-≥…π¶
          ERR_EMV_IccDataFormat£®34£© - ICø® ˝æ›∏Ò Ω¥Ì
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:    sxl0221
 ********************************************************************/
unsigned char jcb_FinalReadSelectRetData(EMVBASE_SELECT_RET* selectRet, unsigned char* DataOut, unsigned short LenOut, JCBTradeUnionStruct *tempAppUnionStruct)
{
    unsigned char k;      //j,
    unsigned short index, indexFCI, indexFCIProp, len, lenFCI, lenFCIProp, templen;
    unsigned short indexIssuerDiscret;    //,lenIssuerDiscret;
    //unsigned short indexAppApp,lenAppApp;
    unsigned char ret;
    unsigned char bIntable;
    EMVBASETAGCVLITEM *emvitem,*emvitem1;
    unsigned char *temp;       //[300]


	EMVBase_Trace("JCB-info: apdu response parsing\r\n");

    index = 0;

    if(DataOut[index] != 0x6F)          //FCIƒ£∞Â±Í ∂
    {
    	EMVBase_Trace("JCB-error: FCI template error(r-apdu first byte != 6F)\r\n");
        return RLT_ERR_EMV_IccDataFormat;         //FCI template
    }
    index++;

    if(EMVBase_ParseExtLen(DataOut, &index, &lenFCI))
    {
    	EMVBase_Trace("JCB-error: FCI template(Tag6F) parsing error\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }

    indexFCI = index;

    while(index < indexFCI + lenFCI)
    {
    	EMVBase_Trace("JCB-info: sub Tags in FCI template(Tag6F) parsing\r\n");

        if(index >= LenOut)
		{
        	EMVBase_Trace("JCB-error: r-apdu parsing error\r\n");
			EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
            return RLT_ERR_EMV_IccDataFormat;
        }

        if(DataOut[index] == 0xFF || DataOut[index] == 0x00) { index++; continue; }
        else if(DataOut[index] == 0x84)         //DF√˚
        {
			EMVBase_Trace("JCB-info: DF Name(Tag84) parsing\r\n");

            if(selectRet->DFNameExist == 1)
			{
            	EMVBase_Trace("JCB-error: DF Name(Tag84) already existed\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
            index++;

            if(EMVBase_ParseExtLen(DataOut, &index, &templen))
            {
            	EMVBase_Trace("JCB-error: DF Name(Tag84) parsing error\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }

            selectRet->DFNameLen = templen;

			if(selectRet->DFNameLen == 0)
			{
				EMVBase_Trace("JCB-error: DF Name(Tag84)'s length is zero!\r\n");
				return RLT_ERR_EMV_IccDataFormat;
			}


            if(selectRet->DFNameLen > 16)
			{
            	EMVBase_Trace("JCB-error: DF Name(Tag84)'s len(%d) > 16\r\n", selectRet->DFNameLen);
                return RLT_ERR_EMV_IccDataFormat;
            }
            memcpy(selectRet->DFName, DataOut + index, selectRet->DFNameLen);
            selectRet->DFNameExist = 1;

            index += templen;

            //termpcardinfo->DFNameLen = selectRet->DFNameLen;
            //memcpy(termpcardinfo->DFName,selectRet->DFName,termpcardinfo->DFNameLen);
            emvbase_avl_createsettagvalue(EMVTAG_DFName, selectRet->DFName, selectRet->DFNameLen);
        }
        else if(DataOut[index] == 0xA5)         //FCI ˝æ›◊®”√ƒ£∞Â
        {
        	EMVBase_Trace("JCB-info: FCI Proprietary Template(TagA5) parsing\r\n");

            selectRet->FCIPropExist = 1;
            index++;

            if(EMVBase_ParseExtLen(DataOut, &index, &lenFCIProp))
            {
            	EMVBase_Trace("JCB-error: FCI Proprietary Template(TagA5) parsing error\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
            indexFCIProp = index;

			emvbase_avl_deletetag(EMVTAG_AppLabel);//SELECT DF must contain 50
			emvbase_avl_deletetag(EMVTAG_PDOL);

            while(index < indexFCIProp + lenFCIProp)
            {
            	EMVBase_Trace("JCB-info: sub Tags in FCI Proprietary Template(TagA5) parsing\r\n");

                ret = jcb_ParseAndStoreCardResponse(tempAppUnionStruct, DataOut, &index, &bIntable, 0);

                if(ret != RLT_EMV_OK)
                {
					EMVBase_Trace("JCB-info: sub Tags in FCI Proprietary Template(TagA5) parsing error\r\n");
                    return RLT_ERR_EMV_IccDataFormat;
                }
            }

            if(index != indexFCIProp + lenFCIProp)
            {
				EMVBase_Trace("JCB-error: FCI Proprietary Template(TagA5)'s value parsing error\r\n");
				EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }


			emvitem = emvbase_avl_gettagitempointer(EMVTAG_AppLabel);
			if(emvitem != NULL)
			{
				emvitem1 = emvbase_avl_gettagitempointer(EMVTAG_PDOL);
				if(emvitem1 == NULL)
				{
					EMVBase_Trace("JCB-error: PDOL(Tag9F38) missing\r\n");
					return RLT_ERR_EMV_IccDataFormat;
				}
			}
			else
			{
				EMVBase_Trace("JCB-error: App Label(Tag50) missing\r\n");
				return RLT_ERR_EMV_IccDataFormat;
			}

			emvitem = emvbase_avl_gettagitempointer(EMVTAG_PDOL);//
			if(emvitem != NULL)
			{
				if(emvitem->len == 0)
				{
					EMVBase_Trace("JCB-error: PDOL(Tag9F38) missing\r\n");
					return RLT_ERR_EMV_IccDataFormat;
				}
			}

            //Ω‚ŒˆBF0C
            indexIssuerDiscret = 0;
            emvitem = emvbase_avl_gettagitempointer(EMVTAG_FCIDisData);

            if(emvitem != NULL)
            {
            	EMVBase_Trace("JCB-info: sub Tags in FCI Issuer Discretionary Data(TagBF0C) parsing\r\n");

				if(!emvitem->len)
				{
					EMVBase_Trace("JCB-error: FCI Issuer Discretionary Data(TagBF0C)'s len is 0\r\n");
					return RLT_ERR_EMV_IccDataFormat;
				}
                temp = (unsigned char *)emvbase_malloc(300);
                memset(temp, 0, 300);
                memcpy(temp, emvitem->data, emvitem->len);

                while(indexIssuerDiscret < emvitem->len)
                {
                    ret = jcb_ParseAndStoreCardResponse(tempAppUnionStruct, temp, &indexIssuerDiscret, &bIntable, 0);

                    if(ret != RLT_EMV_OK)
                    {
                    	EMVBase_Trace("JCB-error: sub Tags in FCI Issuer Discretionary Data(TagBF0C) parsing error\r\n");
                        emvbase_free(temp);
                        return RLT_ERR_EMV_IccDataFormat;
                    }
                }

                emvbase_free(temp);

                if(indexIssuerDiscret != emvitem->len)
                {
                	EMVBase_Trace("JCB-error: FCI Issuer Discretionary Data(TagBF0C)'s value parsing error\r\n");
					EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
                    return RLT_ERR_EMV_IccDataFormat;
                }
                emvbase_avl_deletetag(EMVTAG_FCIDisData);
            }
        }
        else
        {
            k = DataOut[index];
			EMVBase_Trace( "k=%02x\r\n", k);
            if((k & 0x1F) == 0x1F)
            {
            	EMVBase_Trace("JCB-info: Tag%02x%02x parsing\r\n", DataOut[index], DataOut[index+1]);
                index++;
            }
			else
			{
				EMVBase_Trace("JCB-info: Tag%02x parsing\r\n", DataOut[index]);
			}
            index++;

            if(EMVBase_ParseExtLen(DataOut, &index, &len))
            {
            	EMVBase_Trace("JCB-error: Tag parsing error\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
            index += len;
        }
    }

    if(index != indexFCI + lenFCI)
    {
    	EMVBase_Trace("JCB-error: FCI template(Tag6F)'s value parsing error\r\n");
		EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }
    return RLT_EMV_OK;
}

/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_FinalSelectRetData(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: Ω‚ŒˆPaywave—°‘Ò”¶”√∑µªÿ ˝æ›
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: Œﬁ
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:         sxl0221
 ********************************************************************/
unsigned char  jcb_FinalSelectRetData(EMVBASE_APDU_RESP *apdu_r, JCBTradeUnionStruct *tempApp_UnionStruct)
{
    //unsigned char i;	//k,
    EMVBASE_SELECT_RET selectRet;
    EMVBASE_LISTAPPDATA *tempselectedapp;

    EMVBASE_LISTAPPDATA *tempappdata;
    unsigned char tempselectedappno;

    //unsigned char tempselectappno;
    //unsigned char indexPODL,TermTranFlag;
    unsigned char *tempappnum;
    unsigned char tempaidlen;

#ifdef EMVB_DEBUG
    unsigned char i;
#endif


    tempappnum = &(tempApp_UnionStruct->EMVTradeParam->AppListCandidatenum);
    tempselectedapp = tempApp_UnionStruct->EMVTradeParam->SelectedApp;
    tempappdata = tempApp_UnionStruct->EMVTradeParam->AppListCandidate;
    tempselectedappno = tempApp_UnionStruct->EMVTradeParam->SelectedAppNo;
	//EMVBase_Trace( "jcb_FinalReadSelectRetData111111 *tempappnum=%d\r\n", *tempappnum);


    if(apdu_r->SW1 == 0x90 && apdu_r->SW2 == 0x00) //Select OK
    {
		if(tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate)
        {
            //return RLT_EMV_OK;
        }
        memset((unsigned char*)&selectRet, 0, sizeof(EMVBASE_SELECT_RET));

        if(jcb_FinalReadSelectRetData(&selectRet, apdu_r->DataOut, apdu_r->LenOut, tempApp_UnionStruct) != RLT_EMV_OK)
        {
			//jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
            //if(*tempappnum > 1)
            if(1)
            {
            	EMVBase_Trace("JCB-info: select next aid\r\n");
                return RLT_EMV_APPSELECTTRYAGAIN;
            }
            else
            {
            	EMVBase_Trace("JCB-error: no more aid\r\n");
            	if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag == 0)
            	{
            		tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = 1;
            	}
                return RLT_ERR_EMV_IccDataFormat;
            }
        }

        if(selectRet.DFNameExist == 0 || selectRet.FCIPropExist == 0)
        {
        	EMVBase_Trace("JCB-info: DF Name(Tag84) or FCI Proprietary Template(TagA5) missing\r\n");

            if(*tempappnum > 1)
            {
            	EMVBase_Trace("JCB-info: select next aid\r\n");
                return RLT_EMV_APPSELECTTRYAGAIN;
            }
            else
            {
				EMVBase_Trace("JCB-error: no more aid\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
        }

        //±»Ωœ—°‘Òµƒ√˚≥∆
		if(tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate == 0)
        {
            tempaidlen = (tempappdata + tempselectedappno)->AIDLen;

            if(tempaidlen > selectRet.DFNameLen)
            {
                tempaidlen = selectRet.DFNameLen;
            }

#if 0 //del by sujianzhong 2016.10.19
            if(memcmp((tempappdata + tempselectedappno)->AID, selectRet.DFName, tempaidlen))
            {
                if(*tempappnum > 1)
                {
                    return RLT_EMV_APPSELECTTRYAGAIN;
                }
                else
                {
                    EMVBase_Trace("\r\nPaywave_FinalReadSelectRetData444\r\n");
                    return RLT_ERR_EMV_IccDataFormat;
                }
            }
#endif
            memcpy(tempselectedapp->AID, (tempappdata + tempselectedappno)->AID, (tempappdata + tempselectedappno)->AIDLen);
            tempselectedapp->AIDLen = (tempappdata + tempselectedappno)->AIDLen;
        }
        /*
           if(tempselectedapp->AppLabelLen == 0)   //sxl 090211
           {
            if((tempAppAppData+tempselectappno)->AppLabelLen)
            {
                        memcpy(tempselectedapp->AppLabel,(tempappdata+tempselectedappno)->AppLabel,(tempappdata+tempselectedappno)->AppLabelLen);
                        tempselectedapp->AppLabelLen=(tempappdata+tempselectedappno)->AppLabelLen;
            }
           }
         */
    }
    else    //current app selected fail,delete it from app list and select again.
    {
    	EMVBase_Trace("JCB-error: SW != 9000\r\n");

		if(tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate)
		{
			return RLT_ERR_EMV_SWITCHINTERFACE;
		}
		//tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = 1;
		//jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
        if(*tempappnum > 1)
        {
        	EMVBase_Trace("JCB-info: select next aid\r\n");
            return RLT_EMV_APPSELECTTRYAGAIN;
        }
        else
        {
        	EMVBase_Trace("JCB-error: no more aid\r\n");
        	if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag == 0)
        	{
        		tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = 1;
        	}
            return RLT_ERR_EMV_SWITCHINTERFACE;               //20110804
        }
    }
    //±»ΩœAID£¨Ω´—°÷–µƒAID∏¥÷∆∏¯TermInfo
    return JCB_READAIDPARAMETERS;
}

/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_FinalSelect(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: Paywave”¶”√◊Ó÷’—°‘Ò
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: Œﬁ
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:         sxl0221
 ********************************************************************/
unsigned char jcb_FinalSelect(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char retCode;
    EMVBASE_APDU_SEND apdu_s;
    EMVBASE_APDU_RESP apdu_r;
    unsigned char tempselectappno;
    EMVBASE_LISTAPPDATA *tempAppAppData;


    tempAppAppData = tempApp_UnionStruct->EMVTradeParam->AppListCandidate;
    tempselectappno = tempApp_UnionStruct->EMVTradeParam->SelectedAppNo;
    EMVBase_COMMAND_SELECT((tempAppAppData + tempselectappno)->AID, (tempAppAppData + tempselectappno)->AIDLen, 0, &apdu_s);
    tempApp_UnionStruct->IsoCommand(&apdu_s, &apdu_r);

    if(apdu_r.ReadCardDataOk != RLT_EMV_OK)
    {
		EMVBase_Trace("JCB-error: apdu timeout or other error\r\n");
        return RLT_ERR_EMV_APDUTIMEOUT;
    }

    retCode = jcb_FinalSelectRetData(&apdu_r, tempApp_UnionStruct);
	EMVBase_Trace("Second Tap !!!jcb_FinalSelectRetData ret = %d\r\n", retCode);
	if(retCode != JCB_READAIDPARAMETERS && retCode != RLT_EMV_OK)
	{
		if(tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate)
		{
			tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate = 0;
			tempApp_UnionStruct->EMVTradeParam->JcbIUPperformed = 0;
			tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = 0;
			//if(tempApp_UnionStruct->EMVTradeParam->AppListCandidatenum > 1)
			tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = 1;

			//tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = 1;
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
			return RLT_ERR_EMV_IccDataFormat;

		}

		if(apdu_r.SW1 == 0x90 && apdu_r.SW2 == 0x00)
		{
			if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag == 0)
			{
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);

				#if 0
				if(tempApp_UnionStruct->EMVTradeParam->AppListCandidatenum <= 1)
				{
					tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = 1;

					jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
				}
				#endif
				return RLT_EMV_APPSELECTTRYAGAIN;
			}
			else
			{
				memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
				return RLT_ERR_EMV_IccDataFormat;
			}
		}
		else
		{
			if(tempApp_UnionStruct->EMVTradeParam->AppListCandidatenum <= 1)
			{
				tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = 1;
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
			}
		}
	}

    return retCode;
}

unsigned char jcb_FindTagFromPDOL(unsigned char *pheTag)
{
	int tag_len;
	unsigned int uiIndex;
	unsigned char i, k, bInTable;
	EMVBASETAGCVLITEM *PDOLdataitem = NULL;
	unsigned short PDOLLen;
	unsigned char *PDOLData;

	uiIndex = 0;

	PDOLdataitem = emvbase_avl_gettagitempointer(EMVTAG_PDOL);

	if(NULL == pheTag || PDOLdataitem == NULL)
	{
		return 0;
	}
	tag_len = 1;

	if((0x1F & pheTag[0]) == 0x1F)
	{
		tag_len = 2;
	}
	PDOLLen = PDOLdataitem->len;
	PDOLLen = (PDOLLen>255) ? 255 :(PDOLLen);

	PDOLData = (unsigned char *)emvbase_malloc(255);
	memcpy(PDOLData, PDOLdataitem->data, PDOLLen);

	while(uiIndex < PDOLLen)	 //Process PDOL
	{
		if(PDOLData[uiIndex] == 0xFF || PDOLData[uiIndex] == 0x00)
		{
			uiIndex++; continue;
		}
		//bInTable = 0;
		k = PDOLData[uiIndex];
		if(memcmp(&PDOLData[uiIndex], pheTag, tag_len) == 0)
		{
			emvbase_free(PDOLData);
			return 1;
		}

		if((k & 0x1F) == 0x1F)
		{
			uiIndex++;
		}
		uiIndex += 2;
	}

	emvbase_free(PDOLData);
	return 0;
}


//zwb 171030
unsigned char jcb_DealTornECHOCommand(JCBTradeUnionStruct *tempApp_UnionStruct, EMVBASE_APDU_RESP *apdu_r)
{
    unsigned char ret;
	unsigned char k,j,t,indexRet = 0;
	unsigned short index,indexTemp,len,lenTemp,orgindex,tmplen,startpos;
	unsigned char Primitivetype = 0;
	unsigned char countpadding = 0;
	unsigned char bIntable;
	unsigned char GenerateACRetDataflag;
	unsigned char CryptInfo,reqCrpt;
	unsigned char VLPAvailableFund[6];
    EMVBASETAGCVLITEM *item;
	unsigned char AppCryptExist, CryptInfoExist, ATCExist,CardholderVerifStatus,SDADExist;
	unsigned char TransactionMode, ImpleOption, AcquiOption, AIP[2];
	unsigned char Track2Exist;

    tempApp_UnionStruct->EMVTradeParam->GenerateACRetDataLen = 0;
	memset(tempApp_UnionStruct->EMVTradeParam->GenerateACRetData,0,255);

	TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;

	//if(TransactionMode == EMV_Mode)
	{
		if(!(apdu_r->SW1==0x90 && apdu_r->SW2==0x00))
		{
			EMVBase_Trace("JCB-error: SW != 9000\r\n");
			tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag = 0;
			return RLT_EMV_OFFLINE_DECLINE;
			//jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
			//return JCB_OPS_STATUS_ENDAPPLICATION;
		}
	}
	emvbase_avl_deletetag(EMVTAG_SignDynAppData);
	emvbase_avl_deletetag(EMVTAG_CryptInfo);
	emvbase_avl_deletetag(EMVTAG_ATC);
	emvbase_avl_deletetag(EMVTAG_AppCrypt);

	//if(TransactionMode == EMV_Mode)
		//emvbase_avl_deletetag(EMVTAG_JCBCardholderVerifyStatus);


	if(TransactionMode == EMV_Mode)
	{
		if(apdu_r->DataOut[0] != 0x77)
		{
			EMVBase_Trace("JCB-info: r-apdu template error (first byte != 0x77)\r\n");
			//jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}

    index=0;
	if(apdu_r->DataOut[index]==0x77)//TLV coded data
	{
	    Primitivetype = 1;
		index++;
		//EMVBase_Trace( "apdu_r->LenOut=%02x\r\n", apdu_r->LenOut);
		//EMVBase_TraceHex( "apdu_r->DataOut--->",apdu_r->DataOut,apdu_r->LenOut);
		if(EMVBase_ParseExtLen(apdu_r->DataOut,&index,&lenTemp))
		{
			EMVBase_Trace("JCB-info: Tag77 parsing error\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
		indexTemp=index;
		indexRet=0;
		while(index<indexTemp+lenTemp)
		{
			if(index>=apdu_r->LenOut)
			{
				EMVBase_Trace("JCB-info: r-apdu parsing error\r\n");
				EMVBase_Trace("JCB-info: sub Tags' len error\r\n");
				return RLT_EMV_OFFLINE_DECLINE;
			}
			if(apdu_r->DataOut[index]==0xFF || apdu_r->DataOut[index]==0x00)
			{
				index++;
				continue;
			}

            orgindex = index;

			bIntable = 0;
			GenerateACRetDataflag = 1;

			if(!memcmp((unsigned char*)&apdu_r->DataOut[index],"\x9F\x4B",2))
			{
				GenerateACRetDataflag = 0;
			}

			ret = jcb_ParseAndStoreCardResponse(tempApp_UnionStruct,apdu_r->DataOut,&index,&bIntable,0);
			if(ret != RLT_EMV_OK)
			{
				EMVBase_Trace("JCB-info: sub Tags in Tag77 parsing error\r\n");
				ret = RLT_EMV_OFFLINE_DECLINE;
				return ret;
			}

			//sxl20110406
			if(GenerateACRetDataflag&&(indexRet+index-orgindex <=255))
			{
				memcpy(tempApp_UnionStruct->EMVTradeParam->GenerateACRetData+indexRet,(unsigned char*)&apdu_r->DataOut[orgindex],index-orgindex);
				indexRet+=index-orgindex;
			}

		}	//endwhile
		if(index!=indexTemp+lenTemp)
		{
			EMVBase_Trace("JCB-error: Tag70's value parsing error\r\n");
			EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}
	else
	{
		EMVBase_Trace("JCB-info: r-apdu template error (first byte != 0x77)\r\n");
		return RLT_EMV_OFFLINE_DECLINE;
	}

	//unsigned char AppCryptExist, CryptInfoExist, ATCExist,CardholderVerifStatus,SDADExist;
	AppCryptExist = emvbase_avl_checkiftagexist(EMVTAG_AppCrypt);//9F26
	CryptInfoExist = emvbase_avl_checkiftagexist(EMVTAG_CryptInfo);//9F27
	ATCExist = emvbase_avl_checkiftagexist(EMVTAG_ATC);//9F36
	SDADExist = emvbase_avl_checkiftagexist(EMVTAG_SignDynAppData);//9F4B
	CardholderVerifStatus = emvbase_avl_checkiftagexist(EMVTAG_JCBCardholderVerifyStatus);//9F50

	if(TransactionMode == EMV_Mode)
	{
		if((CryptInfoExist == 0)  || (ATCExist == 0))
		{
			EMVBase_Trace("JCB-info: CID(Tag9F27) missing or ATC(Tag9F36) missing\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}

	tempApp_UnionStruct->EMVTradeParam->GenerateACRetDataLen = indexRet;

	CryptInfo = emvbase_avl_gettagvalue(EMVTAG_CryptInfo);
	reqCrpt = emvbase_avl_gettagvalue(EMVTAG_ReferenceControlParameter);
	//TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, &AcquiOption, 0, 1);
	ImpleOption = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;
	memset(AIP, 0x00, 2);
	emvbase_avl_gettagvalue_spec(EMVTAG_AIP, AIP, 0, 2);

	if(TransactionMode == EMV_Mode)
	{
		if((CryptInfo & 0xc0) == 0x40 && (reqCrpt & TAA_OFFLINE))
		{

		}
		else if((CryptInfo & 0xc0) == 0x80 && ((reqCrpt & TAA_OFFLINE) || (reqCrpt & TAA_ONLINE)))
		{

		}
		else if((CryptInfo & 0xc0) == 0x00)
		{
			//return JCB_OPS_STATUS_DECLINED;
		}
		else
		{
			EMVBase_Trace("JCB-info: CID(Tag9F27) error\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}

		if((CryptInfo&0xc0) == 0x00)//AAC
		{
			if(AppCryptExist == 0)
			{
				return RLT_EMV_OFFLINE_DECLINE;
			}
		}

		//TC return or ARQC return(CDA Request)
		if((CryptInfo&0xc0) == 0x40 || ((CryptInfo&0xc0) == 0x80 && (reqCrpt&0x10) == 0x10))
		{
			if((SDADExist == 0) || (CardholderVerifStatus == 0))
			{
				return RLT_EMV_OFFLINE_DECLINE;
			}
		}

		//ARQC return(CDA not Request)
		if((CryptInfo&0xc0) == 0x80 && ((reqCrpt&0x10) != 0x10))
		{
			if((AppCryptExist == 0) || (CardholderVerifStatus == 0))
			{
				return RLT_EMV_OFFLINE_DECLINE;
			}
		}
	}

	//NO CDA
	if(emvbase_avl_checkiftagexist(EMVTAG_SignDynAppData)==0 && TransactionMode == EMV_Mode)
	{
		if(emvbase_avl_checkiftagexist(EMVTAG_AppCrypt)==0)
		{
			return RLT_ERR_EMV_IccDataFormat;
		}

		if((CryptInfo&0xc0) == 0x00) //AAC
		{
			if((reqCrpt&0xC0) == 0x00)
			{
				if(reqCrpt&0x10)
				{
					return RLT_EMV_OFFLINE_DECLINE;
				}
			}
		}
		else
		{
			if(reqCrpt&0x10)
			{
				return RLT_EMV_OFFLINE_DECLINE;
			}
		}


	}

    ret = RLT_EMV_OK;

	//emvbase_avl_setvalue_or(EMVTAG_TSI,0,0x20);


	if((CryptInfo&0x07)==0x01) // Service not allowed
	{
		emvbase_avl_setvalue_or(EMVTAG_TVR,1,0x10);
		ret = RLT_ERR_EMV_NotAccept;
	}

	return ret;


}

unsigned char jcb_TronECHOProcess(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	EMVBASE_APDU_RESP apdu_r;
	EMVBASE_APDU_SEND apdu_s;
	unsigned char retCode;


	memcpy(apdu_s.Command, "\x80\xDF\x00\x00", 4);
	apdu_s.Lc = 0x00;
	apdu_s.Le = 256;
	apdu_s.EnableCancel = 0x01;

	tempApp_UnionStruct->IsoCommand(&apdu_s, &apdu_r);

	if(apdu_r.ReadCardDataOk == RLT_EMV_OK)
	{
		retCode = jcb_DealTornECHOCommand(tempApp_UnionStruct, &apdu_r);
		EMVBase_Trace( "Jcb_DealTornECHOCommand retcode=%d\r\n", retCode);



		if(retCode == RLT_EMV_OFFLINE_DECLINE)
		{
			//jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
			//return retCode;
		}

		if(retCode != RLT_EMV_OK)
		{
			memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));
			tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag = 0;
			retCode = RLT_EMV_OK;
		}
		return retCode;
	}
	else
	{
		return RLT_ERR_EMV_APDUTIMEOUT;
	}

}


//process after select command before gpo
unsigned char jcb_VariableInitialisation(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char retCode, Impletation, CombOption;
	unsigned char STIP[3], DTIP[3];
	unsigned char TermCmpIndicator;


//	emvbase_avl_createsettagvalue(EMVTAG_TVR, "\x00\x00\x00\x00\x00", 5);
	emvbase_avl_settag(EMVTAG_JCBTermcmpIndicator, 0x00);//sjz 2020.6.22
	//memset(&gJCBOnlineTransactionContext, 0x00, sizeof(JCBONLINETRANSACTIONCONTEXT));
	tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode = Undefined_Mode;

	Impletation = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, &CombOption, 0, 1);
	EMVBase_Trace( "JCB-info: Implementation Option: %02x\r\n", Impletation);
	EMVBase_Trace( "JCB-info: Combination Option: %02x\r\n", CombOption);

	if((Impletation & 0x80) == 0x80 && (CombOption & 0x02) == 0x02 )
	{
		EMVBase_Trace( "JCB-info: EMV mode supported by terminal and acquirer\r\n");
		emvbase_avl_setvalue_or(EMVTAG_JCBTermcmpIndicator, 0,0x02);
	}
//1.5aÁâàÊú¨Âº∫Âà∂ÊîØÊåÅMSDÔºå9F52 bit1Âº∫Âà∂ËÆæ‰∏∫0	2021.7.26 lsy
//	if((Impletation & 0x04) == 0x04 && (CombOption & 0x04) == 0x04 )//msd //sjz 2020.6.22
//	{
//		emvbase_avl_setvalue_or(EMVTAG_JCBTermcmpIndicator, 0,0x01);
//	}

	memset(STIP, 0x00, 3);
	memset(DTIP, 0x00, 3);
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBStaticTIP, STIP, 0, 3);
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBDynamicTIP, DTIP, 0, 3);
	memcpy(DTIP, STIP, 3);
	DTIP[0] &= 0x7F;
	if((Impletation & 0x88) != 0x88)	//if not support Issuer Update supported,bit 8 = 0
	{
		DTIP[1] &= 0x7F;//why BF?//sjz 2020.6.23 modify BF->7F
	}

//	if((DTIP[0] & 0x02) != 0x02)//EMV CONTACT SUPPORT
//	{
//		DTIP[0] |= 0x02;
//	}

	emvbase_avl_createsettagvalue(EMVTAG_JCBDynamicTIP, DTIP, 3);
	EMVBase_TraceHex("after jcb_VariableInitialisation STIP", STIP, 3);
	EMVBase_TraceHex("after jcb_VariableInitialisation DTIP(9F53)", DTIP, 3);

	EMVBase_Trace( "after jcb_VariableInitialisation 9F52: %02x\r\n", TermCmpIndicator);

	return RLT_EMV_OK;
}

/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_FinalSelectedApp(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: ”¶”√◊Ó÷’—°‘Ò
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: Œﬁ
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:        sxl0221
 ********************************************************************/
unsigned char jcb_FinalSelectedApp(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char retCode;

    //unsigned short tempnowtask;


    retCode = jcb_FinalSelect(tempApp_UnionStruct);          //—°÷–”¶”√
	EMVBase_Trace("Second Tap!!! jcb_FinalSelect ret = %d\r\n", retCode);
	if(retCode == RLT_ERR_EMV_IccDataFormat || retCode == RLT_ERR_EMV_SWITCHINTERFACE)
	{
		if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode
			&& tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag)
		{
			memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_ENDAPPLICATION);
		}
		return retCode;
	}
    else if(retCode == RLT_EMV_OK || retCode == RLT_ERR_EMV_IccCommand || retCode == RLT_ERR_EMV_APDUTIMEOUT)
    {
        return retCode;
    }
    else if(retCode == JCB_READAIDPARAMETERS)
    {
//		if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag && tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode)
		if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag)//’‚¿ÔŒ™…∂“™≈–emv_mode,“™gpo÷Æ∫Û≤≈÷™µ¿ «∑ÒŒ™emv_mode
		{
			if(jcb_FindTagFromPDOL("\x9F\x52") == 0)
			{
				EMVBase_Trace("don't contain 9F52! --->legacy card\r\n");
				memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
				return RLT_EMV_TERMINATE_TRANSERR;
			}

			retCode = jcb_TronECHOProcess(tempApp_UnionStruct);
			EMVBase_Trace( "Jcb_TronECHOProcess retCode=%d\r\n", retCode);
			if(retCode == JCB_OPS_STATUS_DECLINED)
			{
			}
			else if(retCode == RLT_ERR_EMV_APDUTIMEOUT)
			{
				EMVBase_Trace( "ECHO command errror goto restart!\r\n");
				memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION_COMMUNICATIONERROR);

				return JCB_ERR_READCARDAGAIN;
			}

		}

        return JCB_READAIDPARAMETERS;
    }
    return RLT_EMV_APPSELECTTRYAGAIN;
}

/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_PDOLProcess(AppUnionStruct *tempApp_UnionStruct,EMVBASE_APDU_SEND *apdu_s)
   ∫Ø ˝π¶ƒ‹: ◊È÷Ø∑¢ÀÕGPO ˝æ›
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: Œﬁ
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:    sxl0221
 ********************************************************************/
unsigned char jcb_PDOLProcess(JCBTradeUnionStruct *tempApp_UnionStruct, EMVBASE_APDU_SEND *apdu_s)
{
    unsigned char *pdolData;       //[255]
    unsigned short index, indexOut;
    unsigned char k, m, *buf, bInTable;   //i,  [255]
    unsigned char len;
    //unsigned char ttqtagexist = 0;
    EMVBASETAGCVLITEM *item;
    unsigned char *PDOL;       //[300]
    unsigned char tag[4], taglen;
    EMVBASETAGCVLITEM *pdoldataitem;
    unsigned char tempTermTransPredicable;
    unsigned short templen, tempPDOLlen;
    int ret1;
    EMVBASETAGCVLITEM emvtagitem;


#ifdef EMVB_DEBUG

    unsigned char i;
#endif


    emvbase_avl_settag(EMVTAG_VLPIndicator, 0);


    item = emvbase_avl_gettagitempointer(EMVTAG_PDOL);

    if(item == NULL || item->len == 0)
    {
    	EMVBase_Trace("JCB-error: PDOL(Tag9F38) missing\r\n");
		jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
		return RLT_EMV_APPSELECTTRYAGAIN;;
    }

    //sxl20130626  ◊¢“‚÷∏’Î
    //PDOL Œ™ø’
    pdolData = (unsigned char *)emvbase_malloc(255);
    buf = (unsigned char *)emvbase_malloc(255);
    PDOL = (unsigned char *)emvbase_malloc(300);
    memset(pdolData, 0, 255);

    index = 0;
    indexOut = 0;

    memset(PDOL, 0, 300);

    if(item->len > 255)
    {
        tempPDOLlen = 255;
    }
    else
    {
        tempPDOLlen = item->len;
    }
    memcpy(PDOL, item->data, tempPDOLlen);

    index = 0;

    while(index < tempPDOLlen)  //Process PDOL
    {
        if(PDOL[index] == 0xFF || PDOL[index] == 0x00) {index++; continue; }
        memset(buf, 0, 255);
        bInTable = 0;


        memset(tag, 0, sizeof(tag));
        taglen = 0;
        tag[taglen++] = PDOL[index];

        if((tag[0] & 0x1f) == 0x1f)
        {
            tag[taglen++] = PDOL[index + 1];

            if(tag[1] & 0x80)
            {
                tag[taglen++] = PDOL[index + 2];
            }
        }

		//jcb check tag9F52 exist zwb 171102
		if(!memcmp(tag, "\x9F\x52", 2)) //exist? not legacy card
		{
			tempApp_UnionStruct->EMVTradeParam->JcbIsLegacyCard = 0;
			if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag == 1)
			{
				//tempApp_UnionStruct->EMVTradeParam->JcbIsLegacyCard = 1;
			}
		}

        /*if(!memcmp(tag, "\x9F\x66", 2))
        {
            ttqtagexist = 1;
        }*/
        pdoldataitem = emvbase_avl_gettagitempointer(tag);


        if(pdoldataitem != NULL)
        {
            ret1 = emvbase_TagBaseLib_read(tag, taglen, &emvtagitem, EMVTAGTYPE_JSPEEDY);

            if(ret1)
            {
                ret1 = emvbase_TagBaseLib_read(tag, taglen, &emvtagitem, EMVTAGTYPE_EMV);
            }

            if(ret1 == 0)
            {
                index += taglen;

                if(EMVBase_ParseExtLen(PDOL, &index, &templen))                //sxl?DOL ≥§∂»÷ª”–“ª∏ˆ◊÷Ω⁄
                {
                	EMVBase_Trace("JCB-error: PDOL(Tag9F38)'s value error\r\n");
                    emvbase_free(pdolData);
                    emvbase_free(buf);
                    emvbase_free(PDOL);
                    return RLT_ERR_EMV_IccDataFormat;
                }
                k = templen;       // 1
                m = pdoldataitem->len;               // 4

                if(pdoldataitem->datafomat & EMVTAGFORMAT_N)              //numeric
                {
                    if(k >= m)
                    {
                        if(m)
                        {
                            memcpy(&buf[k - m], pdoldataitem->data, m);
                        }
                        memcpy(&pdolData[indexOut], buf, k);
                    }
                    else
                    {
                        if(m)
                        {
                            memcpy(buf, pdoldataitem->data, m);
                        }
                        memcpy(&pdolData[indexOut], &buf[m - k], k);
                    }
                }
                else if(pdoldataitem->datafomat & EMVTAGFORMAT_CN)              //compact numeric
                {
                    if(m)
                    {
                        memset(buf, 0xFF, 255);
                        memcpy(buf, pdoldataitem->data, m);
                        memcpy(&pdolData[indexOut], buf, k);
                    }
                    else
                    {
                        memset(buf, 0x00, 255);
                        memcpy(&pdolData[indexOut], buf, k);
                    }
                }
                else                //other formats
                {
                    if(m)
                    {
                        memcpy(buf, pdoldataitem->data, m);
                    }
                    memcpy(&pdolData[indexOut], buf, k);
                }
                indexOut += k;
                bInTable = 1;
            }
        }

        if(!bInTable)
        {
            index += taglen;

            if(EMVBase_ParseExtLen(PDOL, &index, &templen))
            {
            	EMVBase_Trace("JCB-error: PDOL(Tag9F38)'s value error\r\n");
                emvbase_free(pdolData);
                emvbase_free(buf);
                emvbase_free(PDOL);
                return RLT_ERR_EMV_IccDataFormat;
            }
            k = templen;
            memcpy(&pdolData[indexOut], buf, k);
            indexOut += k;
        }
    }

    // Process PDOL end here

    emvbase_avl_createsettagvalue(EMVTAG_PDOLData, pdolData, indexOut);

    //tempterminfo->PDOLDataLen=indexOut;
    //memcpy(tempterminfo->PDOLData,pdolData,indexOut);


    memcpy(apdu_s->Command, "\x80\xA8\x00\x00", 4);
	//memcpy(apdu_s->Command,"\x80\xCA\x9F\x68",4);
    apdu_s->Lc = indexOut + 2;
    len = 0;
    apdu_s->DataIn[len++] = 0x83;

    if((indexOut & 0x80) > 0)                            //”–¡Ω∏ˆ◊÷Ω⁄
    {
        (apdu_s->Lc)++;
        apdu_s->DataIn[len++] = 0x81;
        apdu_s->DataIn[len++] = indexOut;
    }
    else
    {
        apdu_s->DataIn[len++] = indexOut;         //÷ª”–“ª∏ˆ◊÷Ω⁄
    }
    memcpy((unsigned char*)&apdu_s->DataIn[len], pdolData, indexOut);
    apdu_s->Le = 256;

    //apdu_s->Le = 0;
    apdu_s->EnableCancel = 0;        //‘ –Ì»°œ˚

    emvbase_free(pdolData);
    emvbase_free(buf);
    emvbase_free(PDOL);
    return RLT_EMV_OK;
}

unsigned char jcb_DelGPORetData(JCBTradeUnionStruct *tempApp_UnionStruct, EMVBASE_APDU_RESP *apdu_r)
{
	unsigned char  ATCbExist, IssuAppDatabExist;
	unsigned char AppCryptbExist, Track2EquivalentbExist;
	unsigned char TermTransPredicable[4];
	unsigned char AFL_num = 0;
	EMVBASETAGCVLITEM *AFLitem;
	unsigned char IssuAppData[5];
	unsigned char CIDbExist, CID;
	unsigned char AIPTagExist = 1, AIP[2];
	unsigned char retCode;
	unsigned char TransanctionMode, ImpleOption, TermcmpIndicator=0;


	emvbase_avl_gettagvalue_spec(EMVTAG_JCBTermcmpIndicator, &TermcmpIndicator, 0, 1);
	memset(AIP, 0, sizeof(AIP));

    if(emvbase_avl_gettagvalue_spec(EMVTAG_AIP, AIP, 0, 2))//82
    {
        //return RLT_EMV_ERR;
        AIPTagExist = 0;
    }

	//jcb zwb 171102
	if(AIPTagExist == 0)
	{
		EMVBase_Trace("JCB-error: AIP(Tag82) missing\r\n");
		return RLT_EMV_TERMINATE_TRANSERR;
	}

	EMVBase_Trace( "JCB-info: AIP(Tag82): %02x %02x\r\n", AIP[0], AIP[1]);
	EMVBase_Trace( "JCB-info: Terminal Compatibility Indicator(Tag9F52): %02x\r\n", TermcmpIndicator);
	EMVBase_Trace( "JCB-info: Transaction Mode: %d\r\n", tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode);
	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Undefined_Mode)
	{
		if(AIPTagExist==1)
		{
			if((AIP[1] & 0x80) && (TermcmpIndicator&0x02))
			{
				EMVBase_Trace( "JCB-info: EMV Mode has been selected(AIP(Tag82) byte2bit8 is '1')-->EMV mode\r\n");
				tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode = EMV_Mode;
			}
			else if(((AIP[1] & 0x80) == 0x00)) //1.5a 9F52 bit1 force set to 0,Msd Mandatory support 2021.8.16 lsy
			{
				EMVBase_Trace( "JCB-info: EMV Mode has not been selected(AIP(Tag82) byte2bit8 is '0')-->Trans Terminate\r\n");
//				tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode = Magstripe_Mode;
				tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode = Undefined_Mode;
				return RLT_EMV_TERMINATE_TRANSERR;
			}
			else
			{
				EMVBase_Trace("JCB-error: EMV Mode not supported and MS mode not supported\r\n");
				return RLT_EMV_TERMINATE_TRANSERR;
			}
		}
	}

	TransanctionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;
	if(TransanctionMode == Legacy_Mode)
	{
		if(apdu_r->DataOut[0] != 0x80)
		{
			EMVBase_Trace( "JCB-error: Legacy mode but GPO template(r-apdu data byte1) != '80'\r\n");
			return RLT_EMV_TERMINATE_TRANSERR;
		}
	}
	else if(TransanctionMode == EMV_Mode || TransanctionMode == Magstripe_Mode)
	{
		if(apdu_r->DataOut[0] != 0x77)
		{
			EMVBase_Trace( "JCB-error: EMV or MS mode but GPO template(r-apdu data byte1) != '77'\r\n");
			return RLT_EMV_TERMINATE_TRANSERR;
		}
	}

	AFLitem = emvbase_avl_gettagitempointer(EMVTAG_AFL);

	if(AFLitem != NULL)
	{
		if(AFLitem->len)
		{
			//AFL_num = (AFLitem->len) >> 2;
			if(AFLitem->len < 4 || (AFLitem->len % 4 != 0))
			{
				EMVBase_Trace("JCB-error: len of AFL(Tag94) error\r\n");
				return RLT_EMV_TERMINATE_TRANSERR;
			}
		}
		else
		{
			EMVBase_Trace("JCB-error: len of AFL(Tag94) is 0\r\n");
			//if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode || tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Legacy_Mode)
			{
				return RLT_EMV_TERMINATE_TRANSERR;
			}
		}
	}
	else
	{
		if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode || tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Legacy_Mode)
		{
			EMVBase_Trace( "JCB-error: EMV or Legacy mode but AFL(Tag94) missing\r\n");
			return RLT_EMV_TERMINATE_TRANSERR;
		}
	}

#if 0
	TransanctionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;
	ImpleOption = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, &CombOptions, 0, 1);
	if(TransactionMode == Magstripe_Mode || TransactionMode == Legacy_Mode ||  \
		((TransactionMode == EMV_Mode)&&(((ImpleOption&0xa0) != 0xa0)||((CombOptions&0x20) != 0x20))) \
		|| ((TransactionMode == EMV_Mode) && ((CombOptions&0x01) != 0x01)))
	{
		emvbase_avl_setvalue_or(EMVTAG_TVR, 0, 0x80);
	}
#endif
	//end

	return RLT_EMV_OK;
}


/*******************************************************************
   ◊˜    ’ﬂ: ÀŒ–¢¡¡
   ∫Ø ˝√˚≥∆: unsigned char Emv_GPORetData(AppUnionStruct *tempApp_UnionStruct,EMVBASE_APDU_RESP *apdu_r)
   ∫Ø ˝π¶ƒ‹: Ω‚ŒˆGPOªÿµƒ ˝æ›
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: Œﬁ
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:         20130731
 ********************************************************************/
unsigned char jcb_GPORetData(JCBTradeUnionStruct *tempApp_UnionStruct, EMVBASE_APDU_RESP *apdu_r)
{
    unsigned short index, indexTemp, len, lenTemp, cplen, j;
    unsigned char k;  //i,
    unsigned char bInTable;
    //unsigned int unknowlen;
    //unsigned char tag;
    unsigned char ret;
	unsigned char TransanctionMode;
	unsigned char TornFlag;

	EMVBase_Trace("JCB-info: apdu response parsing\r\n");

	TransanctionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;
	TornFlag = tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag;

    if(((apdu_r->SW1 == 0x90 && apdu_r->SW2 == 0x00) /*&& (TornFlag==0)*/) || ((TornFlag==1) && (apdu_r->SW1 == 0x62 && apdu_r->SW2 == 0x00))) //Get Processing Options OK
    {
        index = 0;

        // ˝æ›∏Ò ΩŒ™:°Æ80°Ø+≥§∂»+AIP(”¶”√Ωªª•Ãÿ’˜)+AFL(√ø4∏ˆ◊÷Ω⁄ «“ª◊ÈŒƒº˛∂®Œª∆˜)
        if(apdu_r->DataOut[index] == 0x80)      //primitive data
        {
			EMVBase_Trace("JCB-info: GPO template 80\r\n");

            if(apdu_r->LenOut < 8)
            {
            	EMVBase_Trace("JCB-error: r-apdu's len < 8\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
            index++;

            if(EMVBase_ParseExtLen(apdu_r->DataOut, &index, &len))
            {
            	EMVBase_Trace("JCB-error: Tag80 parsing error\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }

            if(index + len != apdu_r->LenOut)
            {
            	EMVBase_Trace("JCB-error: Tag80's len error\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
            emvbase_avl_createsettagvalue(EMVTAG_AIP, apdu_r->DataOut + index, 2);


            index += 2;
            k = (len - 2) % 4;

            if(k != 0)
            {
            	EMVBase_Trace("JCB-error: AFL's len != Multiple of 4\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
            k = (len - 2) / 4;      //AFL4◊÷Ω⁄≥§∂»
            cplen = ((unsigned short)k) * 4;


            emvbase_avl_createsettagvalue(EMVTAG_AFL, apdu_r->DataOut + index, cplen);


            for(j = 0; j < k; j++)
            {
                if(apdu_r->DataOut[index + j * 4] == 0)
                {
                	EMVBase_Trace("JCB-error: AFL[%d]'s SFI is 0\r\n", j+1);
                    return RLT_ERR_EMV_IccDataFormat;
                }
            }

        }
        else if(apdu_r->DataOut[index] == 0x77)      //TLV coded data
        {
			EMVBase_Trace("JCB-info: GPO template 77\r\n");

            index++;

            if(EMVBase_ParseExtLen(apdu_r->DataOut, &index, &lenTemp))
            {
            	EMVBase_Trace("JCB-error: Tag77 parsing error\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
            indexTemp = index;

            while(index < indexTemp + lenTemp)
            {
                if(index >= apdu_r->LenOut)
                {
                	EMVBase_Trace("JCB-error: r-apdu parsing error\r\n");
					EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
                    return RLT_ERR_EMV_IccDataFormat;
                }

                if(apdu_r->DataOut[index] == 0xFF || apdu_r->DataOut[index] == 0x00) { index++; continue; }
                ret = jcb_ParseAndStoreCardResponse(tempApp_UnionStruct, apdu_r->DataOut, &index, &bInTable, 1);

                if(ret != RLT_EMV_OK)
                {
                	EMVBase_Trace("JCB-error: sub Tags in Tag77 parsing error\r\n");
                    return RLT_ERR_EMV_IccDataFormat;
                }
            }

            if(index != indexTemp + lenTemp)
            {
            	EMVBase_Trace("JCB-error: Tag77's value parsing error\r\n");
				EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
        }
        else
		{
        	EMVBase_Trace("JCB-error: GPO template error (first byte != 77 or 80)\r\n");
            return RLT_ERR_EMV_IccDataFormat;
        }
        //sxl20110325 ≤ª‘⁄¥À≈–∂œAIP «∑Ò¥Ê‘⁄
        //if((tempiccdatatable+MV_AIP)->bExist==0) return ERR_EMV_IccDataMissing;//AIP or AFL missing.//sxl20110318 || tempterminfo->AFL_Num<1
    }
    else if(apdu_r->SW1 == 0x69 && apdu_r->SW2 == 0x85)
    {
    	EMVBase_Trace("JCB-error: SW == 6985\r\n");
    	jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
		//if(tempApp_UnionStruct->EMVTradeParam->AppListCandidatenum >1)
		//{
			//jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
		//}
		//else
		//{
			//jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
		//}
        return RLT_EMV_APPSELECTTRYAGAIN;
    }
	#if 0
    else if(apdu_r->SW1 == 0x69 && apdu_r->SW2 == 0x84)  //20110804
    {
        return RLT_ERR_EMV_SWITCHINTERFACE;
    }
    else if(apdu_r->SW1 == 0x69 && apdu_r->SW2 == 0x86)
    {
        return RLT_ERR_EMV_SEEPHONE;
    }
	#endif
    else
	{
		EMVBase_Trace("JCB-error: SW != 9000\r\n");
		jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
		if(tempApp_UnionStruct->EMVTradeParam->AppListCandidatenum >1)	//2021.9.22 lishiyao modify for 1.5a
		{
			return RLT_EMV_APPSELECTTRYAGAIN;
		}
		else
		{
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
			return RLT_EMV_TERMINATE_TRANSERR;
		}
	}
    return RLT_EMV_OK;
}


/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_InitialApp(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: ”¶”√≥ı ºªØ
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: Œﬁ
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:         sxl0221
 ********************************************************************/
unsigned char jcb_InitialApp_Inline(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char retCode;
    EMVBASE_APDU_SEND apdu_s;
    EMVBASE_APDU_RESP apdu_r;
    unsigned char tempTermTransPredicable;
    unsigned char AIP[2];
    unsigned char AIPTagExist = 1;
	unsigned char AFLTagExist = 1;
	unsigned char ImpleOption, CombOptions[2];

	retCode = jcb_PDOLProcess(tempApp_UnionStruct, &apdu_s);

	if(retCode != RLT_EMV_OK)
	{
	    if(RLT_ERR_EMV_IccDataFormat == retCode)
    	{
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
			return RLT_EMV_APPSELECTTRYAGAIN;
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_ENDAPPLICATION);
			if(tempApp_UnionStruct->EMVTradeParam->AppListCandidatenum <= 1)
			{
				tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = 1;
			}
			retCode = RLT_EMV_TERMINATE_TRANSERR;
		}
		return retCode;
	}
	//jcb zwb 171102
	ImpleOption = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, CombOptions,0, 2);
	EMVBase_Trace( "JCB-info: Implementation Option: %02x\r\n", ImpleOption);
	EMVBase_Trace( "JCB-info: Combination Option: %02x\r\n", CombOptions[0]);

	if(tempApp_UnionStruct->EMVTradeParam->JcbIsLegacyCard) // PDOL doesn't contain 9F52
	{
		EMVBase_Trace( "JCB-info: PDOL doesn't contain 9F52--->Legacy card\r\n");
		if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag)
		{
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
			return RLT_EMV_TERMINATE_TRANSERR;
		}

		if((ImpleOption & 0x40) == 0x40 && (CombOptions[0] & 0x01) == 0x01)
		{
			EMVBase_Trace( "JCB-info: Legacy mode supported--->Legacy mode\r\n");
			tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode = Legacy_Mode;
			//tempApp_UnionStruct->EMVTradeParam->JcbIsLegacyCard = 0;
		}
		else
		{
			EMVBase_Trace( "JCB-error: Legacy mode not supported\r\n");
			if(tempApp_UnionStruct->EMVTradeParam->AppListCandidatenum <= 1)
			{
				tempApp_UnionStruct->EMVTradeParam->JcbCandidateListEmptyFlag = 1;
			}
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
			return RLT_EMV_APPSELECTTRYAGAIN;
		}
	}


	tempApp_UnionStruct->IsoCommand(&apdu_s, &apdu_r);

	if(apdu_r.ReadCardDataOk != RLT_EMV_OK)
	{
		EMVBase_Trace("JCB-error: apdu timeout or other error\r\n");

		jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION_COMMUNICATIONERROR);
		retCode = RLT_ERR_EMV_APDUTIMEOUT;
		return retCode;
	}

	retCode = jcb_GPORetData(tempApp_UnionStruct, &apdu_r);

	if(retCode == RLT_ERR_EMV_IccDataFormat || retCode == RLT_ERR_EMV_IccReturn)
	{
		if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag == 0)
		{
			//jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
			//jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_ENDAPPLICATION);
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
			return RLT_EMV_APPSELECTTRYAGAIN;
		}
		else
		{
			memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_ENDAPPLICATION);
			return RLT_EMV_TERMINATE_TRANSERR;
		}
	}

	if(retCode != RLT_EMV_OK && retCode != RLT_ERR_EMV_SEEPHONE)
	{
		return retCode;
	}


	retCode = jcb_DelGPORetData(tempApp_UnionStruct, &apdu_r);
	if(retCode != RLT_EMV_OK)
	{
		if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag == 0)
		{
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
			if(retCode == RLT_EMV_TERMINATE_TRANSERR)
			{
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
				return retCode;
			}
			return RLT_EMV_APPSELECTTRYAGAIN;
		}
		else
		{
			memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_ENDAPPLICATION);
			return RLT_EMV_TERMINATE_TRANSERR;
		}
	}

	if(retCode == RLT_EMV_OK)
	{
		if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Magstripe_Mode
			&& tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag)
		{
			memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_ENDAPPLICATION);
			return RLT_EMV_TERMINATE_TRANSERR;
		}
		else if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode==EMV_Mode
			&& tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag == 1)
		{
			tempApp_UnionStruct->EMVTradeParam->JcbTornTrack2Len = gJCBTornRecoveryContext.TornTrack2len;
			memcpy(tempApp_UnionStruct->EMVTradeParam->JcbTornTrack2Data, gJCBTornRecoveryContext.TornTrack2Data, tempApp_UnionStruct->EMVTradeParam->JcbTornTrack2Len);
		}
	}

	return retCode;

}

unsigned char jcb_InitialApp(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char retCode;

	retCode = jcb_VariableInitialisation(tempApp_UnionStruct);//“™‘⁄µº»Îaid≤Œ ˝∫Û≈–∂œ
	retCode = jcb_InitialApp_Inline(tempApp_UnionStruct);
	if(retCode != RLT_EMV_OK && retCode != RLT_ERR_EMV_APDUTIMEOUT)
	{
		if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag)
		{
			memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));
		}
	}
	return retCode;

}

/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_DealAFLData(AppUnionStruct *tempApp_UnionStruct,EMVBASE_APDU_SEND *apdu_s,unsigned char *tempRead_AFL_Num,unsigned char *tempAFL_RecordNum)
   ∫Ø ˝π¶ƒ‹: ∂¡AFL
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: Œﬁ
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:
 ********************************************************************/
unsigned char jcb_DealAFLData(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char temp[50];      //i,j,,temp2[38]
    EMVBASETAGCVLITEM *item, *item1;
    unsigned char i, j, temp2[50];

    unsigned char AIPbExist = 0;
    unsigned char Track2EquivalentbExist, IssuAppDatabExist, AppCryptbExist, ATCbExist, AFLbExist;
    unsigned char CryptInfobExist;
    unsigned char *CryptInfo, tempCryptInfo;
    unsigned char TransTypeValue;
    //unsigned char TermTransPredicable1;
	unsigned char CDOL1Exist=0,PanExist=0,ExdateExist=0;
	unsigned char TornCompareFlag = 0;
	unsigned char Track2Data[30];
	unsigned short tmpTrack2Len;


	CDOL1Exist = emvbase_avl_checkiftagexist(EMVTAG_CDOL1);//8C
	PanExist = emvbase_avl_checkiftagexist(EMVTAG_Track2Equ);//57
	ExdateExist = emvbase_avl_checkiftagexist(EMVTAG_AppExpireDate);//5F24

	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode || tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Legacy_Mode)
	{
		if(CDOL1Exist==0 || PanExist == 0 || ExdateExist == 0)
		{
			if(0 == CDOL1Exist)
			{
				EMVBase_Trace("JCB-error: CDOL1(Tag8C) missing\r\n");
			}
			if(0 == PanExist)
			{
				EMVBase_Trace("JCB-error: Track2(Tag57) missing\r\n");
			}
			if(0 == ExdateExist)
			{
				EMVBase_Trace("JCB-error: Expiry Date(Tag5F24) missing\r\n");
			}

			if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag == 0)
			{
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
				return RLT_EMV_APPSELECTTRYAGAIN;
			}

			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
			return RLT_EMV_TERMINATE_TRANSERR;
		}
	}

	//jcb torn transaction compare track2
	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode && tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag)
	{
		if(TornCompareFlag == 0)
		{
			TornCompareFlag = 1;
			memset(Track2Data, 0x00, 30);
			emvbase_avl_gettagvalue_all(EMVTAG_Track2Equ, Track2Data, &tmpTrack2Len);

			if(!(tempApp_UnionStruct->EMVTradeParam->JcbTornTrack2Len == tmpTrack2Len &&
				(memcmp(tempApp_UnionStruct->EMVTradeParam->JcbTornTrack2Data, Track2Data, tmpTrack2Len) == 0)))
			{
				memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));

				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
				return RLT_EMV_TERMINATE_TRANSERR;
			}
		}
	}

    tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 0;
    item = emvbase_avl_gettagitempointer(EMVTAG_Track2Equ);
    item1 = emvbase_avl_gettagitempointer(EMVTAG_PAN);;

    item = emvbase_avl_gettagitempointer(EMVTAG_SDATagList);

    if(item != NULL)     //if SDA_TL exist
    {
        if(item->len)
        {
        	EMVBase_Trace("JCB-info: SDA Tag List(Tag9F4A) len: %d\r\n", item->len);
    		EMVBase_TraceHex("JCB-info: SDA Tag List(Tag9F4A):", item->data, item->len);
            tempApp_UnionStruct->EMVTradeParam->bErrSDATL = 1;

            if(item->len == 1)
            {
                if(item->data[0] == 0x82)
                {
                    if(emvbase_avl_gettagvalue_spec(EMVTAG_AIP, temp, 0, 2))
                    {
                    	EMVBase_Trace("JCB-error: AIP(Tag82) missing\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }

					if(tempApp_UnionStruct->EMVTradeParam->AuthData == NULL)
	                {
	                    tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen = 0;
	                    tempApp_UnionStruct->EMVTradeParam->AuthData = (unsigned char *)emvbase_malloc(2);

	                    if(tempApp_UnionStruct->EMVTradeParam->AuthData == NULL)
	                    {
	                    	EMVBase_Trace("JCB-error: memory error\r\n");
	                        return RLT_EMV_ERR;
	                    }

	                    tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen = 2;
	                    memset(tempApp_UnionStruct->EMVTradeParam->AuthData,0,2);
	                }
                    memcpy((unsigned char*)&tempApp_UnionStruct->EMVTradeParam->AuthData[tempApp_UnionStruct->EMVTradeParam->AuthDataLen], temp, 2);
                    tempApp_UnionStruct->EMVTradeParam->AuthDataLen += 2;
                    tempApp_UnionStruct->EMVTradeParam->bErrSDATL = 0;
                }
            }
        }
    }
    //CVM list exist
    item = emvbase_avl_gettagitempointer(EMVTAG_CVMList);

    if(item != NULL)
    {
        if(item->len % 2 != 0)
        {
			EMVBase_Trace("JCB-error: len of CVM List(Tag8E) error\r\n");
            return RLT_ERR_EMV_IccDataFormat;
        }
    }

    return RLT_EMV_OK;
}

/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_ReadAFLRetData(AppUnionStruct *tempApp_UnionStruct,EMVBASE_APDU_RESP *apdu_r,unsigned char *tempRead_AFL_Num,unsigned char *tempAFL_RecordNum)
   ∫Ø ˝π¶ƒ‹: ∂¡AFL∑µªÿµƒ ˝æ›Ω‚Œˆ
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: Œﬁ
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:
 ********************************************************************/
unsigned char jcb_ReadAFLRetData(JCBTradeUnionStruct *tempApp_UnionStruct, EMVBASE_APDU_RESP *apdu_r, unsigned char tempt, unsigned char authdataflag)
{
    unsigned char t;
    unsigned char bInTable;      //i,k,
    unsigned short index, indexAEF, lenAEF;    //len,
    //unsigned char tempreadaflnum;
    //unsigned char tag;
    unsigned char ret;
	unsigned short indexlen, lenTemp, indexTemp, len;
	unsigned char k = 0;


	EMVBase_Trace("JCB-info: apdu response parsing\r\n");

    if(!(apdu_r->SW1 == 0x90 && apdu_r->SW2 == 0x00)) // ICC parameter Records found fail.
    {
		EMVBase_Trace("JCB-error: SW != 9000\r\n");
        return RLT_ERR_EMV_IccReturn;
    }
    index = 0;

    t = tempt;
    t >>= 3;

    if((t >= 11) && (t <= 30)) //mofified according to SU Bullitin No.12
    {
        if(authdataflag)
        {
            if(apdu_r->DataOut[index] == 0x70)           //∂‘”⁄SFI¥”11µΩ30µƒº«¬ºTAG==70∫Õº«¬º≥§∂»”√”⁄Õ—ª˙»œ÷§
            {
				if(tempApp_UnionStruct->EMVTradeParam->AuthData == NULL)
                {
                    tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen = 0;
                    tempApp_UnionStruct->EMVTradeParam->AuthData = (unsigned char *)emvbase_malloc(512);

                    if(tempApp_UnionStruct->EMVTradeParam->AuthData == NULL)
                    {
                    	EMVBase_Trace("JCB-error: memory error\r\n");
                        return RLT_EMV_ERR;
                    }

                    tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen = 512;
                    memset(tempApp_UnionStruct->EMVTradeParam->AuthData,0,512);
                }

                if((tempApp_UnionStruct->EMVTradeParam->AuthDataLen + apdu_r->LenOut) > tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen)
                {
                    if(tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen < 1024)
                    {
                        unsigned char *p = (unsigned char *)emvbase_malloc(1024);
                        if(p == NULL)
                        {
                        	EMVBase_Trace("JCB-error: memory error\r\n");
                            return RLT_EMV_ERR;
                        }
                        memcpy(p,tempApp_UnionStruct->EMVTradeParam->AuthData,tempApp_UnionStruct->EMVTradeParam->AuthDataLen);
                        emvbase_free(tempApp_UnionStruct->EMVTradeParam->AuthData);
                        tempApp_UnionStruct->EMVTradeParam->AuthData = p;
                        tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen = 1024;
                    }
                    else if(tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen < 2048)
                    {
                        unsigned char *p = (unsigned char *)emvbase_malloc(2048);
                        if(p == NULL)
                        {
                        	EMVBase_Trace("JCB-error: memory error\r\n");
                            return RLT_EMV_ERR;
                        }
                        memcpy(p,tempApp_UnionStruct->EMVTradeParam->AuthData,tempApp_UnionStruct->EMVTradeParam->AuthDataLen);
                        emvbase_free(tempApp_UnionStruct->EMVTradeParam->AuthData);
                        tempApp_UnionStruct->EMVTradeParam->AuthData = p;
                        tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen = 2048;
                    }
                    else
                    {
                    	EMVBase_Trace("JCB-error: data len of Offline data authentication error\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }
                }
                memcpy((unsigned char*)&tempApp_UnionStruct->EMVTradeParam->AuthData[tempApp_UnionStruct->EMVTradeParam->AuthDataLen], apdu_r->DataOut, apdu_r->LenOut);
                tempApp_UnionStruct->EMVTradeParam->AuthDataLen += apdu_r->LenOut;

            }
            else            //≤ªµ»”⁄70‘Ú»œŒ™Õ—ª˙ ˝æ›»œ÷§“—æ≠÷¥––≤¢ ß∞‹
            {
                tempApp_UnionStruct->EMVTradeParam->bErrAuthData = 1;
            }
        }

		// for tag9F24 9F19 9F6E 9F7C  20200707
		indexlen = 0;
		if(apdu_r->DataOut[indexlen] == 0x70)
		{
            indexlen++;

            if(EMVBase_ParseExtLen(apdu_r->DataOut, &indexlen, &lenTemp))
            {
            	EMVBase_Trace("JCB-error: Tag70 parsing error\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
            indexTemp = indexlen;

            while(indexlen < indexTemp + lenTemp)
            {
                if(indexlen >= apdu_r->LenOut)
				{
		        	EMVBase_Trace("JCB-error: r-apdu parsing error\r\n");
					EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
		            return RLT_ERR_EMV_IccDataFormat;
		        }

                if(apdu_r->DataOut[indexlen] == 0xFF || apdu_r->DataOut[indexlen] == 0x00) { indexlen++; continue; }

                else if(!memcmp(&apdu_r->DataOut[indexlen], "\x9F\x24", 2))
                {
                    indexlen += 2;

                    if(EMVBase_ParseExtLen(apdu_r->DataOut, &indexlen, &len))
                    {
                    	EMVBase_Trace("JCB-error: Tag9F24 parsing error\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }

                    if(len == 0)
                    {
                    	EMVBase_Trace("JCB-error: Tag9F24's len is 0\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }
                    emvbase_avl_createsettagvalue(EMVTAG_PayAccountRefer, &apdu_r->DataOut[indexlen], len);
                    indexlen += len;
                }
				else if(!memcmp(&apdu_r->DataOut[indexlen], "\x9F\x19", 2))
                {
                    indexlen += 2;

                    if(EMVBase_ParseExtLen(apdu_r->DataOut, &indexlen, &len))
                    {
                    	EMVBase_Trace("JCB-error: Tag9F19 parsing error\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }

                    if(len == 0)
                    {
                    	EMVBase_Trace("JCB-error: Tag9F19's len is 0\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }
                    emvbase_avl_createsettagvalue(EMVTAG_TOKENREQUESTID, &apdu_r->DataOut[indexlen], len);
                    indexlen += len;
                }
				else if(!memcmp(&apdu_r->DataOut[indexlen], "\x9F\x6E", 2))
                {
                    indexlen += 2;

                    if(EMVBase_ParseExtLen(apdu_r->DataOut, &indexlen, &len))
                    {
                    	EMVBase_Trace("JCB-error: Tag9F6E parsing error\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }

                    if(len == 0)
                    {
                    	EMVBase_Trace("JCB-error: Tag9F6E's len is 0\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }
                    emvbase_avl_createsettagvalue(EMVTAG_JCBDeviceInfo, &apdu_r->DataOut[indexlen], len);
                    indexlen += len;
                }
				else if(!memcmp(&apdu_r->DataOut[indexlen], "\x9F\x7C", 2))
                {
                    indexlen += 2;

                    if(EMVBase_ParseExtLen(apdu_r->DataOut, &indexlen, &len))
                    {
                    	EMVBase_Trace("JCB-error: Tag9F7C parsing error\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }

                    if(len == 0)
                    {
                    	EMVBase_Trace("JCB-error: Tag9F7C's len is 0\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }
                    emvbase_avl_createsettagvalue(EMVTAG_JCBPartnerDiscretionaryData, &apdu_r->DataOut[indexlen], len);
                    indexlen += len;
                }
                else		//unknown data,out of this command.
                {
                    k = apdu_r->DataOut[indexlen];

                    if((k & 0x1F) == 0x1F)
                    {
                    	EMVBase_Trace("JCB-info: Tag%02x%02x parsing\r\n", apdu_r->DataOut[index], apdu_r->DataOut[index+1]);
                        indexlen++;
                    }
					else
					{
						EMVBase_Trace("JCB-info: Tag%02x parsing\r\n", apdu_r->DataOut[index]);
					}
                    indexlen++;

                    if(EMVBase_ParseExtLen(apdu_r->DataOut, &indexlen, &len))
                    {
                    	EMVBase_Trace("JCB-error: Tag parsing error\r\n");
                        return RLT_ERR_EMV_IccDataFormat;
                    }
                    indexlen += len;
                }
            }

            if(indexlen != indexTemp + lenTemp)
            {
            	EMVBase_Trace("JCB-error: Tag70's value parsing error\r\n");
				EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
        }

        return RLT_EMV_OK;
    }

    while(apdu_r->DataOut[index] == 0xFF || apdu_r->DataOut[index] == 0x00)
    {
        index++;
    }

    if(apdu_r->DataOut[index] != 0x70)
    {
    	EMVBase_Trace("JCB-error: r-apdu template error (first byte != 0x70)\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }
    index++;

    if(EMVBase_ParseExtLen(apdu_r->DataOut, &index, &lenAEF))
    {
    	EMVBase_Trace("JCB-error: Tag70 parsing error\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }

    if(authdataflag)     //add record to authentication data string if it is required.
    {
		if(tempApp_UnionStruct->EMVTradeParam->AuthData == NULL)
        {
            tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen = 0;
            tempApp_UnionStruct->EMVTradeParam->AuthData = (unsigned char *)emvbase_malloc(512);

            if(tempApp_UnionStruct->EMVTradeParam->AuthData == NULL)
            {
            	EMVBase_Trace("JCB-error: memory error\r\n");
                return RLT_EMV_ERR;
            }

            tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen = 512;
            memset(tempApp_UnionStruct->EMVTradeParam->AuthData,0,512);
        }

        if((tempApp_UnionStruct->EMVTradeParam->AuthDataLen + apdu_r->LenOut) > tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen)
        {
            if(tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen < 1024)
            {
                unsigned char *p = (unsigned char *)emvbase_malloc(1024);
                if(p == NULL)
                {
                	EMVBase_Trace("JCB-error: memory error\r\n");
                    return RLT_EMV_ERR;
                }
                memcpy(p,tempApp_UnionStruct->EMVTradeParam->AuthData,tempApp_UnionStruct->EMVTradeParam->AuthDataLen);
                emvbase_free(tempApp_UnionStruct->EMVTradeParam->AuthData);
                tempApp_UnionStruct->EMVTradeParam->AuthData = p;
                tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen = 1024;
            }
            else if(tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen < 2048)
            {
                unsigned char *p = (unsigned char *)emvbase_malloc(2048);
                if(p == NULL)
                {
                	EMVBase_Trace("JCB-error: memory error\r\n");
                    return RLT_EMV_ERR;
                }
                memcpy(p,tempApp_UnionStruct->EMVTradeParam->AuthData,tempApp_UnionStruct->EMVTradeParam->AuthDataLen);
                emvbase_free(tempApp_UnionStruct->EMVTradeParam->AuthData);
                tempApp_UnionStruct->EMVTradeParam->AuthData = p;
                tempApp_UnionStruct->EMVTradeParam->AuthDataMaxLen = 2048;
            }
            else
            {
            	EMVBase_Trace("JCB-error: data len of Offline data authentication error\r\n");
                return RLT_ERR_EMV_IccDataFormat;
            }
        }

        memcpy((unsigned char*)&tempApp_UnionStruct->EMVTradeParam->AuthData[tempApp_UnionStruct->EMVTradeParam->AuthDataLen], (unsigned char*)&apdu_r->DataOut[index], lenAEF);
        tempApp_UnionStruct->EMVTradeParam->AuthDataLen += lenAEF;

    }
    indexAEF = index;


    while(index < indexAEF + lenAEF)
    {
        if(index >= apdu_r->LenOut)
        {
        	EMVBase_Trace("JCB-error: r-apdu parsing error\r\n");
			EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
            return RLT_ERR_EMV_IccDataFormat;
        }

        if(apdu_r->DataOut[index] == 0xFF || apdu_r->DataOut[index] == 0x00)
        {
            index++;
            continue;
        }
        ret = jcb_ParseAndStoreCardResponse(tempApp_UnionStruct, apdu_r->DataOut, &index, &bInTable, 1);

        if(ret != RLT_EMV_OK)
        {
        	EMVBase_Trace("JCB-error: sub Tags in Tag70 parsing error\r\n");
            return RLT_ERR_EMV_IccDataFormat;
        }
    }

    if(index != indexAEF + lenAEF)
    {
    	EMVBase_Trace("JCB-error: Tag70's value parsing error\r\n");
		EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }

    //sxl20110325
    if(index < apdu_r->LenOut)
    {
    	EMVBase_Trace("JCB-error: r-apdu parsing error\r\n");
		EMVBase_Trace("JCB-error: sub Tags' len error\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }

    return RLT_EMV_OK;
}

static unsigned char jcb_ReadAppData_Inline(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char retCode;
	EMVBASETAGCVLITEM *item;
	unsigned char aflexist = 0;
	unsigned char Read_AFL_Num = 0;
	unsigned char AFL_RecordNum = 0;
	unsigned char AFL_Num;
	EMVBASE_APDU_SEND apdu_s;
	EMVBASE_APDU_RESP apdu_r;
	unsigned char authdataflag;
	unsigned char TornCompareFlag = 0;
	unsigned char Track2Data[30];
	unsigned short tmpTrack2Len, Track2Exist;


	tempApp_UnionStruct->EMVTradeParam->AuthDataLen = 0;
	tempApp_UnionStruct->EMVTradeParam->bErrAuthData = 0;
	tempApp_UnionStruct->EMVTradeParam->bErrSDATL = 0;

	item = emvbase_avl_gettagitempointer(EMVTAG_AFL);

	if(item != NULL)
	{
		if(item->len >> 2)
		{
			aflexist = 1;
		}
	}

	if(aflexist == 0)
	{
		retCode = jcb_DealAFLData(tempApp_UnionStruct);

		return retCode;
	}
	retCode = EMVBase_CheckReadAFL(item);

	if(retCode != RLT_EMV_OK)
	{
		EMVBase_Trace("JCB-error: data of AFL(Tag94) error\r\n");
		jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
		//jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
		return RLT_EMV_APPSELECTTRYAGAIN;		   //huxiang add for case CLQ.S.018.00
	}
	AFL_Num = item->len >> 2;
	tempApp_UnionStruct->EMVTradeParam->AuthDataLen = 0;
	tempApp_UnionStruct->EMVTradeParam->bErrAuthData = 0;

	for(Read_AFL_Num = 0; Read_AFL_Num < AFL_Num; Read_AFL_Num++)
	{
		AFL_RecordNum = item->data[(Read_AFL_Num << 2) + 1];

		while(AFL_RecordNum <= item->data[(Read_AFL_Num << 2) + 2])
		{
			EMVBase_FormReadAflData(&apdu_s, AFL_RecordNum, item->data[(Read_AFL_Num << 2) + 0]);

			//final record ≤ª‘ –Ì»°œ˚
			if(Read_AFL_Num == AFL_Num - 1 && AFL_RecordNum == item->data[(Read_AFL_Num << 2) + 2])
			{
                //apdu_s.EnableCancel = 0;
                apdu_s.EnableCancel = 1;
			}
			tempApp_UnionStruct->IsoCommand(&apdu_s, &apdu_r);
			if(apdu_r.ReadCardDataOk != RLT_EMV_OK)
			{
				if(apdu_r.ReadCardDataOk == APDUCOMMCANCEL)
            	{
            		EMVBase_Trace("JCB-error: apdu cancel\r\n");
					jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_ENDAPPLICATION);
//					return RLT_EMV_TERMINATE_TRANSERR;
					return RLT_ERR_EMV_CancelTrans;
				}

				EMVBase_Trace("JCB-error: apdu timeout or other error\r\n");
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION_COMMUNICATIONERROR);
				return RLT_ERR_EMV_APDUTIMEOUT;
			}

			if(AFL_RecordNum < item->data[(Read_AFL_Num << 2) + 1] + item->data[(Read_AFL_Num << 2) + 3])
			{
				authdataflag = 1;
			}
			else
			{
				authdataflag = 0;
			}
			retCode = jcb_ReadAFLRetData(tempApp_UnionStruct, &apdu_r, item->data[(Read_AFL_Num << 2) + 0], authdataflag);

			if(retCode != RLT_EMV_OK)
			{
				if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag == 0)
				{
					jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
					return RLT_EMV_APPSELECTTRYAGAIN;
				}
				else
				{
					memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));
					jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
					return RLT_ERR_EMV_IccDataFormat;
				}
			}

			//jcb torn transaction compare track2
			if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode && tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag)
			{
				memset(Track2Data, 0x00, 30);
				Track2Exist = !(emvbase_avl_gettagvalue_all(EMVTAG_Track2Equ, Track2Data, &tmpTrack2Len));
				if(TornCompareFlag == 0 && Track2Exist)
				{
					TornCompareFlag = 1;
					//memset(Track2Data, 0x00, 30);
					//emvbase_avl_gettagvalue_all(EMVTAG_Track2Equ, Track2Data, &tmpTrack2Len);
					if(!(tempApp_UnionStruct->EMVTradeParam->JcbTornTrack2Len == tmpTrack2Len &&
						(memcmp(tempApp_UnionStruct->EMVTradeParam->JcbTornTrack2Data, Track2Data, tmpTrack2Len) == 0)))
					{
						memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));

						jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
						return RLT_EMV_TERMINATE_TRANSERR;
					}
				}
			}

			AFL_RecordNum++;
		}
	}

	retCode = jcb_DealAFLData(tempApp_UnionStruct);
	return retCode;
}


/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_ReadAppData(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: ∂¡”¶”√ ˝æ›
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: Œﬁ
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:         sxl 0221
 ********************************************************************/
unsigned char jcb_ReadAppData(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char retCode;
	retCode = jcb_ReadAppData_Inline(tempApp_UnionStruct);

	if(retCode != RLT_EMV_OK && retCode != RLT_ERR_EMV_APDUTIMEOUT)
	{
		if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag)
		{
			memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));
		}
	}

	return retCode;

}

unsigned char jcb_TornProcessCryptInfo(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char CryptInfo,reqCrpt,TransactionMode;
	unsigned char AppCryptExist, CryptInfoExist, ATCExist,CardholderVerifStatus,SDADExist;

	TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;
	AppCryptExist = emvbase_avl_checkiftagexist(EMVTAG_AppCrypt);//9F26
	CryptInfoExist = emvbase_avl_checkiftagexist(EMVTAG_CryptInfo);//9F27
	ATCExist = emvbase_avl_checkiftagexist(EMVTAG_ATC);//9F36
	SDADExist = emvbase_avl_checkiftagexist(EMVTAG_SignDynAppData);//9F4B
	CardholderVerifStatus = emvbase_avl_checkiftagexist(EMVTAG_JCBCardholderVerifyStatus);//9F50
	reqCrpt = emvbase_avl_gettagvalue(EMVTAG_ReferenceControlParameter);

	if(TransactionMode == EMV_Mode)
	{
		if((CryptInfoExist == 0)  || (ATCExist == 0))
		{
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}


	CryptInfo = emvbase_avl_gettagvalue(EMVTAG_CryptInfo);
	//Trace("", "Jcb_TornProcessCryptInfo =%02x\r\n", CryptInfo);
	//TC return or ARQC return(CDA Request)
	if((CryptInfo&0xc0) == 0x40 || ((CryptInfo&0xc0) == 0x80 && (reqCrpt&0x10) == 0x10))
	{
		if((SDADExist == 0) || (CardholderVerifStatus == 0))
		{
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}

	//ARQC return(CDA not Request)
	if((CryptInfo&0xc0) == 0x80 && ((reqCrpt&0x10) != 0x10))
	{
		if((AppCryptExist == 0) || (CardholderVerifStatus == 0))
		{
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}

	if((CryptInfo & 0x80) == 0x80)
	{
		tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 2;
	}
	else if((CryptInfo & 0x40) == 0x40)
	{
		tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 0;
	}
	else
	{
		tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 1;
		tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
		jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
		return RLT_EMV_OFFLINE_DECLINE;
	}


	return RLT_EMV_OK;
}

#if 1
unsigned char jcb_ODACheckMandotoryData(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	//unsigned char retCode = RLT_EMV_OK;
	unsigned char AIP[2];
	//unsigned char TermCapab[3];
	//EMVBASE_APDU_SEND apdu_s;
	//EMVBASE_APDU_RESP apdu_r;
	unsigned char ODAFail = 0;
	unsigned char ImpleOption,CombOptions;
	unsigned char TransactionMode;


	ImpleOption = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, &CombOptions, 0, 1);
	TransactionMode =tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;
	EMVBase_Trace( "JCB-info: Implementation Option: %02x\r\n", ImpleOption);
	EMVBase_Trace( "JCB-info: Combination Option: %02x\r\n", CombOptions);

	memset(AIP, 0, sizeof(AIP));
	//memset(TermCapab, 0, sizeof(TermCapab));

	emvbase_avl_gettagvalue_spec(EMVTAG_AIP, AIP, 0, 2);
	EMVBase_Trace("JCB-info: AIP(Tag82): %02x %02x\r\n", AIP[0], AIP[1]);

	//emvbase_avl_gettagvalue_spec(EMVTAG_TermCapab, TermCapab, 0, 3);
	//jcb_CheckIfdoCDA(tempApp_UnionStruct);

	if((TransactionMode==EMV_Mode) && (AIP[0] & 0x01) && ((ImpleOption & 0xa0) == 0xa0) && ((CombOptions & 0x22) == 0x22))//ICC and terminal support Combined DDA/AC
	{
		EMVBase_Trace( "JCB-info: CDA supported by terminal and card\r\n");

		if(tempApp_UnionStruct->EMVTradeParam->bErrSDATL || tempApp_UnionStruct->EMVTradeParam->bErrAuthData)		   //SDA_TL.tag not EMV; represents data not available at the moment; refers to constructed Data Object;//processing of SDA Tag List failed
		{
			EMVBase_Trace("JCB-info: SDA Tag List(Tag9F4A) error(len is not equal to 1 or value is not equal to '82')\r\n");
			EMVBase_Trace("or data need to be authenticated returned by read app data is not template '70'\r\n");
			ODAFail = 1;
		}
		else
		{
			//retCode = jcb_CombineDDA_FirstHalf(tempApp_UnionStruct);//Get CAPK,IPK,ICCPK
			//if(Emvb_CombineDDA_FirstHalf(tempApp_UnionStruct) != RLT_EMV_OK)			   //Get CAPK,IPK,ICCPK //∏¥∫œ∂ØÃ¨»œ÷§œ»»°µ√CAPK£¨IPK£¨ICCPK.
			//EMVBase_Trace( "jcb_CombineDDA_FirstHalf = %d\r\n", retCode);
			//retCode = RLT_EMV_OK;

			if(jcb_CheckDataMissDDA(tempApp_UnionStruct)!=RLT_EMV_OK)
		    {
		    	ODAFail = 1;
		        emvbase_avl_setvalue_or(EMVTAG_TVR,0,0x20); //ICC data missing
		        //retCode = RLT_ERR_EMV_IccDataMissing;
		    }
			else
			{
				unsigned char ipkcertlen = 0,iccpkcertlen = 0;

				EMVBASETAGCVLITEM *item, *item1;

				item = emvbase_avl_gettagitempointer(EMVTAG_IPKCert);

				if(item != NULL)
				{
					if(item->len)
					{
						ipkcertlen = item->len;
					}
				}

				item = emvbase_avl_gettagitempointer(EMVTAG_ICCPKCert);

				if(item != NULL)
				{
					if(item->len)
					{
						iccpkcertlen = item->len;
					}
				}

				item = emvbase_avl_gettagitempointer(EMVTAG_IPKRem);

				if(item != NULL)
				{
					if(item->len)
					{
					}
				}


				if((iccpkcertlen + 36) > ipkcertlen)
				{
					if(!emvbase_avl_checkiftagexist(EMVTAG_IPKRem))
					{
						ODAFail = 1;
					    emvbase_avl_setvalue_or(EMVTAG_TVR,0,0x20); //ICC data missing
					    //retCode = RLT_ERR_EMV_IccDataMissing;
				    }
				}

			}
			if(emvbase_avl_checkiftagexist(EMVTAG_CAPKI))
			{
				if(tempApp_UnionStruct->CheckCapkExit)
				{
					if(tempApp_UnionStruct->CheckCapkExit(tempApp_UnionStruct->EMVTradeParam)!=RLT_EMV_OK)
					{
						ODAFail = 1;
					}
				}
			}
			#if 0
				if(RLT_ERR_EMV_IccDataMissing == retCode)
					{
						tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
						jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
						retCode = RLT_EMV_OFFLINE_DECLINE;
					}
			#endif
			}

		if(ODAFail)
		{
			emvbase_avl_setvalue_or(EMVTAG_TVR, 0, 0x04);		//Offline Combined DDA/AC Generation failed
			//emvbase_avl_setvalue_or(EMVTAG_TSI, 0, 0x80); 	//set bit 'Offline Data Authentication was performed' bit 1
		}
		//return RLT_EMV_OK;
	}
	else	//all dynamic and static authentication and DDA/AC are not supported.
	{
		EMVBase_Trace("JCB-info: CDA not supported by card or terminal");
		//The bit should be set to 1 according to test script 2CI.023.00
		emvbase_avl_setvalue_or(EMVTAG_TVR, 0, 0x80);								//Offline Data Authentication not performed
		//retCode = RLT_EMV_OK;
	}

	return RLT_EMV_OK;
}


#endif


/*******************************************************************
Author:   sxl
Function Name:   void Paywave_DealTrackData(AppUnionStruct *tempApp_UnionStruct)
Function Purpose:Deal Track Data
Input Paramters:
Output Paramters:N/A
Return Value:       N/A
Remark: N/A
Modify: N/A  Á£ÅÈÅìÊï∞ÊçÆËΩ¨ÂåñËøòÈúÄË¶ÅÊµã???
********************************************************************/
unsigned char jcb_DealTrackData(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char trackdata[200],trackdatalen,field3len,field5len,pvvexist = 0;
	unsigned char i;
	unsigned char tmpbuf[200],tmpbuflen,Dposition = 0,maxlen;
	unsigned char lrc;
	unsigned char *tmppan,tmppanlen,tmpdata[20];
	EMVBASETAGCVLITEM *item;
    unsigned char CardHoldNameexist,Track2Equexist,Track1Discretexist;

	unsigned char uptrack1 = 0,uptrack2 = 0;

	#ifdef TEMPTEST
	unsigned char testlen;
	#endif


    emvbase_avl_createsettagvalue(EMVTAG_Paypassconverttrack1,NULL,0);
	emvbase_avl_createsettagvalue(EMVTAG_Paypassconverttrack2,NULL,0);



//	if(tempApp_UnionStruct->EMVTradeParam->VisaAPFlag)
//	{
//
//	    if(tempApp_UnionStruct->EMVTradeParam->VisaAPFlag  == 0x02)
//		{
//			uptrack1 = 1;
//		}
//
//
//		uptrack2 = 1;
//
//	}
//	else
	{
		if(tempApp_UnionStruct->EMVTradeParam->qPBOCOrMSD == TRANSFLOW_MSDMODE)
		{
//			if(tempApp_UnionStruct->EMVTradeParam->paywavetrackdataup&0x02)
			{
				uptrack2 = 1;
			}

//			if(tempApp_UnionStruct->EMVTradeParam->paywavetrackdataup&0x01)
			{
				uptrack1 = 1;
			}
		}
		else
		{
//			if(tempApp_UnionStruct->EMVTradeParam->paywavetrackdataup&0x02)
			{
				uptrack2 = 1;
			}
		}


	}

	item = emvbase_avl_gettagitempointer(EMVTAG_Track2Equ);
	//Creating Track 1
	if(item!= NULL)
	{
		if(item->len== 0)
		{
			EMVBase_Trace("JCB-error: Track2(Tag57)'s len is 0");
			return RLT_ERR_EMV_IccDataFormat;
		}

        tmpbuflen = item->len;
		tmpbuflen = tmpbuflen<<1;

        memset(tmpbuf,0,sizeof(tmpbuf));
		EMVBaseBcdToAsc(tmpbuf,item->data,item->len);
		Dposition = 0;
		for(i = 0;i < tmpbuflen;i++)
		{
			if(tmpbuf[i] == 'D')
			{
				Dposition = i;
				break;
			}
		}

		if((tmpbuf[tmpbuflen-1] == 'F') ||(tmpbuf[tmpbuflen-1] == 'f'))
		{
			tmpbuflen -= 1;
		}
	}
	else
	{
		EMVBase_Trace("JCB-error: Track2(Tag57) missing");
		return RLT_ERR_EMV_IccDataFormat;
	}

    if(uptrack1)
    {
	    trackdatalen = 0;
		trackdata[trackdatalen++] = '%';
		trackdata[trackdatalen++] = 'B';
		memcpy(&trackdata[trackdatalen],tmpbuf,Dposition);
		trackdatalen += Dposition;
		field3len = Dposition;

		trackdata[trackdatalen++] = '^';

	    //check track format
		if((Dposition == 0)||(Dposition > 19))
		{
			EMVBase_Trace("JCB-error: Track2(Tag57) data error");
			return RLT_ERR_EMV_IccDataFormat;
		}

	    //check if card name exist
	    item = emvbase_avl_gettagitempointer(EMVTAG_CardHoldName);
		CardHoldNameexist = 0;

		if(item != NULL)
		{
			if(item->len)
			{
				CardHoldNameexist = 1;
			}
		}

		if(CardHoldNameexist)
		{

			if(item->len <2 ||item->len >26)
			{
				EMVBase_Trace("JCB-error: Card Holder Name(Tag5F20) data error");
				return RLT_ERR_EMV_IccDataFormat;
			}
			memcpy(&trackdata[trackdatalen],item->data,item->len);
			trackdatalen += item->len;
			field5len = item->len;
		}
		else
		{
			trackdata[trackdatalen++] = 0x20;
			trackdata[trackdatalen++] = 0x2F;
			field5len = 2;
		}
		trackdata[trackdatalen++] = '^';

		//check tag '57'
		item = emvbase_avl_gettagitempointer(EMVTAG_Track2Equ);
        Track2Equexist = 0;
		if(item != NULL)
		{
			if(item->len)
			{
				Track2Equexist = 1;
			}
		}

		if(Track2Equexist)
		{
		    //(iccdatatable+MV_PAN)->bExist = 1;
            //(iccdatatable+MV_AppExpireDate)->bExist = 1;
            tmppan = (unsigned char *)emvbase_malloc(200);

			memset(tmppan,0,200);
            memset(tmppan,'F',200);
			memcpy(tmppan,tmpbuf,Dposition);
			tmppanlen = Dposition;
			tmppanlen = ((tmppanlen+1)>>1)<<1;
			EMVBaseAscToBcd(tmpdata,tmppan,tmppanlen);
			emvbase_avl_createsettagvalue(EMVTAG_PAN,tmpdata,tmppanlen>>1);
			//tempcardinfo->PANLen = tmppanlen>>1;
			emvbase_free(tmppan);

			EMVBaseAscToBcd(tmpdata,&tmpbuf[Dposition+1],4);
			emvbase_avl_createsettagvalue(EMVTAG_AppExpireDate,tmpdata,2);

		    //card Expiration Date
			if((Dposition+1+4) > tmpbuflen)
			{
				return RLT_ERR_EMV_IccDataFormat;
			}
			memcpy(&trackdata[trackdatalen],&tmpbuf[Dposition+1],4);
			trackdatalen += 4;

	        //Service Code
			if((Dposition+1+4+3) > tmpbuflen)
			{
				return RLT_ERR_EMV_IccDataFormat;
			}
			memcpy(&trackdata[trackdatalen],&tmpbuf[Dposition+1+4],3);
			trackdatalen += 3;

			//PVKI&PVV
			if((Dposition+1+4+3+5+8) == tmpbuflen)  //add 8 bytes other data
			{
				memcpy(&trackdata[trackdatalen],&tmpbuf[Dposition+1+4+3],5);
				#ifdef TEMPTEST
                printf("\r\nPVKI&PVV\r\n");
				for(testlen = 0;testlen<5;testlen++)
				{
					printf("%02x ",trackdata[trackdatalen+testlen]);
				}
				printf("\r\n");
				#endif
				trackdatalen += 5;
				pvvexist = 1;
			}
			else
			{
				//trackdatalen += 5;
			}

		}
		if(pvvexist)
		{
			if((Dposition+1+20) > tmpbuflen)
			{
				return RLT_ERR_EMV_IccDataFormat;
			}
		}
		else
		{
			if((Dposition+1+7) > tmpbuflen)
			{
				return RLT_ERR_EMV_IccDataFormat;
			}
		}


		//track 1 discretionary data tag '9f1f'
		item = emvbase_avl_gettagitempointer(EMVTAG_Track1Discret);
        Track1Discretexist = 0;
		if(item != NULL)
		{
			if(item->len)
			{
				Track1Discretexist = 1;
			}
		}

	    if(Track1Discretexist)
	    {
	    	maxlen = 79 - 30 - field3len - field5len;
			if(item->len < maxlen)
			{
				maxlen = item->len;
			}
            EMVBase_Trace("9F1F maxlen = %d\r\n",maxlen);

			memcpy(&trackdata[trackdatalen],item->data,maxlen);
			trackdatalen += maxlen;
	    }
		else
		{
			//trackdata[trackdatalen++] = 0;
		}

		if((pvvexist == 0)&&((Dposition+15+1) != tmpbuflen))
		{
			trackdata[trackdatalen++] = 0x31;
			memset(&trackdata[trackdatalen],0x30,7);
			trackdatalen += 7;

		}
		else
		{
			//Contactless Indicator 1
			if(pvvexist)
			{
				trackdata[trackdatalen++] = tmpbuf[Dposition+20];
			}
			else
			{
			    if((Dposition+15+1)<tmpbuflen)
			    {
			    	trackdata[trackdatalen++] = tmpbuf[Dposition+15];
			    }
				else
				{
					trackdata[trackdatalen++] = 0x31;
				}

			}

			//ATC part1 2
			if(pvvexist)
			{
		        memcpy(&trackdata[trackdatalen],&tmpbuf[Dposition+16],2);
				trackdatalen += 2;
			}
			else
			{
			    if((Dposition+11+4)<tmpbuflen)
			    {
					memcpy(&trackdata[trackdatalen],&tmpbuf[Dposition+11],2);
			    }
				else
				{
					memcpy(&trackdata[trackdatalen],"\x30\x30",2);
				}
				trackdatalen += 2;
			}

			//dCVV 3
			if(pvvexist)
			{
				memcpy(&trackdata[trackdatalen],&tmpbuf[Dposition+13],3);
				trackdatalen += 3;
			}
			else
			{
			    if((Dposition+8+3)<tmpbuflen)
			    {
					memcpy(&trackdata[trackdatalen],&tmpbuf[Dposition+8],3);
			    }
				else
				{
					memcpy(&trackdata[trackdatalen],"\x30\x30\x30",3);
				}
				trackdatalen += 3;
			}

			//ATC part2 2
		    if(pvvexist)
			{
		        memcpy(&trackdata[trackdatalen],&tmpbuf[Dposition+18],2);
				trackdatalen += 2;
			}
			else
			{
			    if((Dposition+11+4)<tmpbuflen)
			    {
					memcpy(&trackdata[trackdatalen],&tmpbuf[Dposition+13],2);
			    }
				else
				{
					memcpy(&trackdata[trackdatalen],"\x30\x30",2);
				}
				trackdatalen += 2;
			}
		}


		//11.4 Visa reserved
		trackdata[trackdatalen++] = '0';

		//11.5 Visa reserved
		memcpy(&trackdata[trackdatalen],"000",3);
		trackdatalen += 3;

		//field12 End Sentinel
	    trackdata[trackdatalen++] = '?';

		//field13 LRC
		if(trackdatalen > 80)
		{
			return RLT_ERR_EMV_IccDataFormat;
		}

	    lrc = 0;
		for(i = 0;i < trackdatalen;i++)
		{
			lrc ^= trackdata[i];
		}

		trackdata[trackdatalen++] = lrc;

	    EMVBase_TraceHex( "EMVTAG_Paypassconverttrack1 ", trackdata, trackdatalen);
        emvbase_avl_createsettagvalue(EMVTAG_Paypassconverttrack1,trackdata,trackdatalen);

    }

	//Creating Track 2
	if(uptrack2)
	{
	    //#ifdef PAYWAVEL3TEST
		//item = emvbase_avl_gettagitempointer(EMVTAG_Track2Equ);
		//Creating Track 2
		//if(item!= NULL)
		//{
		//	emvbase_avl_createsettagvalue(EMVTAG_Paypassconverttrack2,item->data,item->len);
		//}
		//#else
		trackdatalen = 0;
		trackdata[trackdatalen++] = ';';  //sxl?  visa ap not allow
		memcpy(&trackdata[trackdatalen],tmpbuf,tmpbuflen);
		trackdatalen += tmpbuflen;
		trackdata[1+Dposition] = '=';
	    trackdata[trackdatalen++] = '?';
		lrc = 0;
		for(i = 0;i < trackdatalen;i++)
		{
			lrc ^= trackdata[i];
		}

		if(lrc < 0x30)//zwb 170509   visa clqs0100002v1
		{
			lrc &= 0x0F;
			lrc += 0x30;
		}

		trackdata[trackdatalen++] = lrc;
		EMVBase_TraceHex( "tmpbuf ", tmpbuf, tmpbuflen);
		EMVBase_Trace( "trackdatalen=%d lrc=%d\r\n", trackdatalen, lrc);

		if(trackdatalen >= 50)
		{
			return RLT_ERR_EMV_IccDataFormat;
		}

		emvbase_avl_createsettagvalue(EMVTAG_Paypassconverttrack2,trackdata,trackdatalen);
		//#endif
	}


	return RLT_EMV_OK;
}

void jcb_RandTransSelect(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned int MinLimit,ThresholdMoney,TermAmtAuth;
	unsigned char TransTargetPercent;
	unsigned char bRandSelected;

	unsigned char FloorLimit[6];
	unsigned char Threshold[6];
	unsigned char RandomNum;
	unsigned char bShowRandNum;
	unsigned char AmtAuthNum[6];
	//unsigned char AmtAuthBin[4];
	unsigned char MaxTargetPercent,TargetPercent;
	unsigned char  TVR[5];


    memset(FloorLimit,0,sizeof(FloorLimit));
    emvbase_avl_gettagvalue_spec(EMVTAG_clessofflinelimitamt, FloorLimit, 0, 6);
	memset(Threshold,0,sizeof(Threshold));
    emvbase_avl_gettagvalue_spec(EMVTAG_JCBThresholdRandomSelect, Threshold,0,6);

	//memset(AmtAuthBin,0,sizeof(AmtAuthBin));
    //emvbase_avl_gettagvalue_spec(EMVTAG_AmtAuthBin,AmtAuthBin,0,4);


	memset(AmtAuthNum,0,sizeof(AmtAuthNum));
    emvbase_avl_gettagvalue_spec(EMVTAG_AmtAuthNum,AmtAuthNum,0,6);

#if 0
	MinLimit = EMVByteArrayToInt(FloorLimit,6);
	ThresholdMoney = EMVByteArrayToInt(Threshold,6);
	TermAmtAuth = EMVByteArrayToInt(AmtAuthNum, 6);
#else
	EMVBaseBcdToU32(&MinLimit, FloorLimit, 6);
	EMVBaseBcdToU32(&ThresholdMoney, Threshold, 6);
	EMVBaseBcdToU32(&TermAmtAuth, AmtAuthNum, 6);
#endif

	//EMVBase_Trace( "--------1111------->");
	if(TermAmtAuth > MinLimit)
	{
		return;
	}

	//zcl add
	memset(TVR, 0, sizeof(TVR));
	emvbase_avl_gettagvalue_spec(EMVTAG_TVR, TVR, 0, 5);
	if(TVR[3] & 0x80)
	{
		return;
	}
	//end

	//EMVBase_Trace( "--------22222------->");
	bRandSelected=0;
	if(tempApp_UnionStruct->EMVB_RandomNum == NULL)
	{
		return;
	}

	tempApp_UnionStruct->EMVB_RandomNum(&RandomNum,1);

	RandomNum = (unsigned char)(RandomNum%99+1);
    EMVBase_Trace( "RandomNum=%d\r\n", RandomNum);
	if(emvbase_avl_gettagvalue(EMVTAG_bShowRandNum))
	{
		emvbase_avl_settag(EMVTAG_bShowRandNum,RandomNum);
	}

	MaxTargetPercent = emvbase_avl_gettagvalue(EMVTAG_MaxTargetPercent);
	TargetPercent = emvbase_avl_gettagvalue(EMVTAG_TargetPercent);

    EMVBase_Trace( "Emvb_RandTransSelect Precent max=%d target=%d \r\n",MaxTargetPercent,TargetPercent);
	EMVBase_Trace( "Threshold=%d TermAmtAuth=%d MinLimit=%d\r\n",ThresholdMoney,TermAmtAuth, MinLimit);


	if(TermAmtAuth < ThresholdMoney)
	{
		if(RandomNum<=TargetPercent) bRandSelected=1;
	}
	else
	{
		//if(MinLimit<ThresholdMoney+1)//
		if(MinLimit < ThresholdMoney + 100) //  1÷∏????Œ™1?  1‘™◊™????Œ™100?
		{
			EMVBase_Trace( "Jcb_RandTransSelect1111111\r\n");
			return;
		}
		TransTargetPercent=(unsigned char)((MaxTargetPercent - TargetPercent)*(TermAmtAuth - ThresholdMoney)/(MinLimit-ThresholdMoney)+TargetPercent);
		EMVBase_Trace( "TransTargetPercent=%d\r\n", TransTargetPercent);
		if(RandomNum<=TransTargetPercent)
		{
			bRandSelected=1;
		}
	}

	if(bRandSelected==1)
	{
		EMVBase_Trace( "Jcb_RandTransSelect Selected!!!\r\n");
	    emvbase_avl_setvalue_or(EMVTAG_TVR,3,0x10); //set 'Transaction selected randomly for online processing' bit 1.
	}

}


int jcb_IccGetPAN(unsigned char *pasOutPAN)
{
    unsigned char temp[128] = {0};
    unsigned char bcPAN[128] = {0};
    unsigned int i, j, len;
    EMVBASETAGCVLITEM *item = NULL, *panitem = NULL;
    unsigned char tagexistflag, pantagexistflag;
    unsigned char Track2Len, Track2Data[20];


    if(NULL == pasOutPAN)
    {
        return RLT_EMV_ERR;
    }
    panitem = emvbase_avl_gettagitemandstatus(EMVTAG_PAN, &pantagexistflag);

    if(pantagexistflag)
    {
        memset(temp, 0, sizeof(temp));
        EMVBaseBcdToAsc(temp, panitem->data, panitem->len);

        if((panitem->data[panitem->len - 1] & 0x0F) == 0x0F)
        {
            temp[panitem->len * 2 - 1] = '\0';
        }
        memcpy(pasOutPAN, temp, strlen((char *)temp));
        return RLT_EMV_OK;
    }
    return RLT_EMV_ERR;
}

unsigned char jcb_TermRiskManage(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char TVR[5];
	int retCode;
	unsigned char asPAN[20] = {0};
	unsigned char TransactionMode;
	unsigned char AmtAuthNum[6], CLessOfflineAmt[6], CLessOfflineLimitAmt[6];
    unsigned char TermCVM_Limit[6];
	unsigned char TransType, TermType, CombOption[2], ImpleOption;
	unsigned char FLoorLimitExist = 0, TransactionLimitExist = 0, CVMLimitExist = 0;
	unsigned char TransCurExp = 0;

    memset(AmtAuthNum, 0, sizeof(AmtAuthNum));
    emvbase_avl_gettagvalue_spec(EMVTAG_AmtAuthNum, AmtAuthNum, 0, 6);

    memset(CLessOfflineAmt, 0, sizeof(CLessOfflineAmt)); //Transaction limit
    TransactionLimitExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_clessofflineamt, CLessOfflineAmt, 0, 6));

    memset(CLessOfflineLimitAmt, 0, sizeof(CLessOfflineLimitAmt));//floor limit
    FLoorLimitExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_clessofflinelimitamt, CLessOfflineLimitAmt, 0, 6));

    memset(TermCVM_Limit, 0, sizeof(TermCVM_Limit)); //CVM limit
    CVMLimitExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_termcvm_limit, TermCVM_Limit, 0, 6));

	EMVBase_Trace( "TransactionLimitExist=%d FLoorLimitExist=%d CVMLimitExist=%d\r\n", TransactionLimitExist, FLoorLimitExist, CVMLimitExist);

	TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;
	TransType = emvbase_avl_gettagvalue(EMVTAG_TransTypeValue);
	//1.amount limit
	if(memcmp(AmtAuthNum, CLessOfflineAmt, 6) >= 0 && (TransactionLimitExist == 1))
	{
		jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
		return RLT_EMV_APPSELECTTRYAGAIN;
	}

	if((TransType==0x00 || TransType==0x01 || TransType==0x09) && (memcmp(AmtAuthNum, TermCVM_Limit, 6) >= 0) && (CVMLimitExist == 1))
	{
		EMVBase_Trace( "CVM bit is setting !\r\n");
		emvbase_avl_setvalue_or(EMVTAG_JCBDynamicTIP, 0, 0x80);
	}

	TermType = emvbase_avl_gettagvalue(EMVTAG_TermType);
	TermType &= 0x0F;
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, CombOption, 0, 2);
	if((TermType == 0x01 || TermType == 0x04) || (TransactionMode == Magstripe_Mode || TransactionMode == Legacy_Mode))
	{//status check
		EMVBase_Trace( "EMVTAG_TVR 4byte 8 1\r\n");
		emvbase_avl_setvalue_or(EMVTAG_TVR, 3, 0x80);
	}

	emvbase_avl_gettagvalue_spec(EMVTAG_TransCurcyExp, &TransCurExp, 0, 1);
	EMVBase_Trace( "TransCurcyExp(5F36): %d\r\n", TransCurExp);
	EMVBase_TraceHex( "AmtAuthNum(9F02)", AmtAuthNum, 6);
	if((CombOption[0]&0x40) == 0x40)//Terminal support status check
	{
		if(((TransCurExp == 0) && (memcmp(AmtAuthNum, "\x00\x00\x00\x00\x00\x01", 6) == 0)) ||
			((TransCurExp == 1) && 	(memcmp(AmtAuthNum, "\x00\x00\x00\x00\x00\x10", 6) == 0)) ||
			((TransCurExp == 2) && 	(memcmp(AmtAuthNum, "\x00\x00\x00\x00\x01\x00", 6) == 0)))
			{
				EMVBase_Trace( "EMVTAG_TVR 4byte 8 1\r\n");
				emvbase_avl_setvalue_or(EMVTAG_TVR, 3, 0x80);
			}
	}
	EMVBase_Trace( "111==================\r\n");
	if(TransactionMode == EMV_Mode)
	{
		if(memcmp(AmtAuthNum, CLessOfflineLimitAmt, 6) >= 0 && (FLoorLimitExist == 1))
		{
			emvbase_avl_setvalue_or(EMVTAG_TVR, 3, 0x80);
		}
	}

	//2.Random Transaction Selection
	memset(TVR, 0, sizeof(TVR));
	emvbase_avl_gettagvalue_spec(EMVTAG_TVR, TVR, 0, 5);
	EMVBase_TraceHex( "After Status check TVR:", TVR, 5);
	if((TransactionMode == EMV_Mode) && (CombOption[0]&0x08) == 0x08 && (TVR[3]&0x80) != 0x80)
	{
		jcb_RandTransSelect(tempApp_UnionStruct);
	}

	EMVBase_Trace( "333333==================\r\n");
	//3.Exception File Check
	ImpleOption = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;
	EMVBase_Trace( "ImpleOption=%02x CombOption[0]=%02x\r\n", ImpleOption, CombOption[0]);
	if(TransactionMode == EMV_Mode && (ImpleOption & 0x90) == 0x90 && (CombOption[0]&0x10) == 0x10)
	{
    	if(tempApp_UnionStruct->VerifyCardNo != NULL)
    	{
        	if(emvbase_avl_checkiftagexist(EMVTAG_PAN))
        	{
            	if(jcb_IccGetPAN(asPAN) == RLT_EMV_OK)
	            {
	     			EMVBase_Trace( "begin to compare!\r\n");
	                retCode = (*tempApp_UnionStruct->VerifyCardNo)(asPAN);
					EMVBase_Trace( "retcode=%d\r\n", retCode);
	                if(retCode != RLT_EMV_OK)
	                {
	                	EMVBase_Trace( "card is in exception file!\r\n");
	                    emvbase_avl_setvalue_or(EMVTAG_TVR,0,0x10);
	                }
	            }
        	}
    	}
	}

	return RLT_EMV_OK;

}


/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_CheckExpDate(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: ≈–∂œø®∆¨…˙–ß»’∆⁄
   »Îø⁄≤Œ ˝: Œﬁ
   ∑µ ªÿ ÷µ: OK ”––ß.
                  ERR Œﬁ–ß.
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:
 ********************************************************************/
unsigned char jcb_CheckExpDate(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char currentDate[4], EffectDate[4], ExpireDate[4];   //buf[10],
    unsigned char AppEffectDateexist, AppExpireDateexist;
    EMVBASETAGCVLITEM *item;

    emvbase_avl_gettagvalue_spec(EMVTAG_TransDate, &currentDate[1], 0, 3);
	EMVBase_TraceHex("JCB-info: Transaction Date(Tag9A): ", &currentDate[1], 3);

    if(currentDate[1] > 0x49) { currentDate[0] = 0x19; }
    else{ currentDate[0] = 0x20; }
    AppEffectDateexist = 0;
    item = emvbase_avl_gettagitempointer(EMVTAG_AppEffectDate);

    if(item != NULL)
    {
        if(item->len >= 3)
        {
            AppEffectDateexist = 1;
        }
    }

    if(AppEffectDateexist)    //App effective date exist
    {
    	EMVBase_TraceHex("JCB-info: Effective Date(Tag5F25): ", item->data, 3);
        if(EMVBase_CheckDateFormat(item->data) == RLT_EMV_ERR)
		{
        	EMVBase_Trace("JCB-error: data of Effective Date(Tag5F25) error\r\n");
            return RLT_EMV_ERR;
        }

        if(item->data[0] > 0x49) {EffectDate[0] = 0x19; }
        else{ EffectDate[0] = 0x20; }
        memcpy((unsigned char*)&EffectDate[1], item->data, 3);

        if(memcmp(currentDate, EffectDate, 4) < 0)
        {
        	EMVBase_Trace("JCB-info: Application not yet effective \r\n");
            emvbase_avl_setvalue_or(EMVTAG_TVR, 1, 0x20);
        }
    }
    AppExpireDateexist = 0;
    item = emvbase_avl_gettagitempointer(EMVTAG_AppExpireDate);

    if(item != NULL)
    {
        if(item->len >= 3)
        {
            AppExpireDateexist = 1;
        }
    }

    if(AppExpireDateexist)    //App expiration date exist
    {
    	EMVBase_TraceHex("JCB-info: Expiry Date(Tag5F24):", item->data, 3);
        if(EMVBase_CheckDateFormat(item->data) == RLT_EMV_ERR)
		{
        	EMVBase_Trace("JCB-error: data of Expiry Date(Tag5F24) error\r\n");
            return RLT_EMV_ERR;
        }

        if(item->data[0] > 0x49) { ExpireDate[0] = 0x19; }
        else{ ExpireDate[0] = 0x20; }
        memcpy((unsigned char*)&ExpireDate[1], item->data, 3);

        if(memcmp(currentDate, ExpireDate, 4) > 0)
        {
        	EMVBase_Trace("JCB-info: Expired\r\n");
            emvbase_avl_setvalue_or(EMVTAG_TVR, 1, 0x40);
        }
    }
    return RLT_EMV_OK;
}

void jcb_CheckAUC(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char bTestFail;
    unsigned char TermType,AUC[2],AUCbExist;
    unsigned char IssuCountryCode[2],CountryCode[2],IssuCountryCodebExist;
    unsigned char TermAmtOther[6];
    unsigned char TermAddCapab[5];
    unsigned char TermAmtOtherBin = 0;
    unsigned char TransType;

    bTestFail=0;
    memset(AUC,0,sizeof(AUC));
    AUCbExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_AUC,AUC,0,2));

    TermType = emvbase_avl_gettagvalue(EMVTAG_TermType);
//  TransType = tempApp_UnionStruct->EMVTradeParam->TransType;
    TransType = emvbase_avl_gettagvalue(EMVTAG_TransTypeValue);

    memset(TermAddCapab,0,sizeof(TermAddCapab));
    emvbase_avl_gettagvalue_spec(EMVTAG_TermAddCapab,TermAddCapab,0,5);

    memset(IssuCountryCode,0,sizeof(IssuCountryCode));
    IssuCountryCodebExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_IssuCountryCode,IssuCountryCode,0,2));
    memset(CountryCode,0,sizeof(CountryCode));
    emvbase_avl_gettagvalue_spec(EMVTAG_CountryCode,CountryCode,0,2);

    memset(TermAmtOther,0,sizeof(TermAmtOther));
    emvbase_avl_gettagvalue_spec(EMVTAG_AmtOtherNum,TermAmtOther,0,6);
    if(memcmp(TermAmtOther,"\x00\x00\x00\x00\x00\x00",6))
    {
        TermAmtOtherBin = 1;
    }

    if(AUCbExist)   //AUC exist.
    {
        if((TermType==0x14 || TermType==0x15 || TermType==0x16 )
           && (TermAddCapab[0]&0x80))  //The termianl is ATM
        {
            if(!(AUC[0]&0x02))
                bTestFail=1;
        }
        else        //The terminal is not ATM
        {
            if(!(AUC[0]&0x01))
                bTestFail=1;
        }
        if(IssuCountryCodebExist)       //Issuer country code exist
        {
            if(!memcmp(IssuCountryCode,CountryCode,2))      //domestic
            {
                if(TransType == NEW_TRANS_CASH)
                {
                    if(!(AUC[0]&0x80))
                        bTestFail=1;
                }

                if(TransType == NEW_TRANS_GOODS)
                {
                    if( !(AUC[0]&0x28))
                        bTestFail=1;
                }
                if(TransType == NEW_TRANS_CASHBACK)
                {
                    if(!(AUC[1]&0x80))
                        bTestFail=1;
                }
            }
            else                            //international,terminal country code differ from issuer country code
            {
                if(TransType == NEW_TRANS_CASH)
                {
                    if(!(AUC[0]&0x40))
                        bTestFail=1;
                }

                if(TransType == NEW_TRANS_GOODS )
                {
                    if( !(AUC[0]&0x14))
                        bTestFail=1;
                }

                if(TransType == NEW_TRANS_CASHBACK)
                {
                    if(!(AUC[1]&0x40))
                        bTestFail=1;
                }
            }
        }
    }


    if(bTestFail)
    {
        emvbase_avl_setvalue_or(EMVTAG_TVR,1,0x10);
    }

}

unsigned char jcb_ProcessRestrict(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode)
	{
		jcb_CheckAUC(tempApp_UnionStruct);
	}

	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode || tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Legacy_Mode)
	{
		if(jcb_CheckExpDate(tempApp_UnionStruct)!=RLT_EMV_OK)
		{
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_SELECTNEXT);
			return RLT_EMV_APPSELECTTRYAGAIN;
		}
	}

	return RLT_EMV_OK;
}



unsigned char jcb_TermActAnaly(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char i, k;   //TermAnaResult,CardAnaResult;//0-Denial,1-Online,2-Offline
    unsigned char bFitIAC, bFitTAC;
    EMVBASE_Card_TAC CardTAC;

    unsigned char TVR[5];
    unsigned char TACDenial[5];
    unsigned char IACDenial[5], IACDenialbExist;
    unsigned char TermType;
    unsigned char TACOnline[5], IACOnline[5], IACOnlinebExist;
    unsigned char TACDefault[5], IACDefault[5], IACDefaultbExist;
    unsigned char bExist;



#if 1
    emvbase_avl_printtagallvalue("JCB-info: TVR: ", EMVTAG_TVR);
    emvbase_avl_printtagallvalue("JCB-info: TAC-online: ", EMVTAG_TACOnline);
    emvbase_avl_printtagallvalue("JCB-info: TAC-default: ", EMVTAG_TACDefault);
    emvbase_avl_printtagallvalue("JCB-info: TAC-Denial: ", EMVTAG_TACDenial);
    emvbase_avl_printtagallvalue("JCB-info: IAC-online: ", EMVTAG_IACOnline);
    emvbase_avl_printtagallvalue("JCB-info: IAC-default: ", EMVTAG_IACDefault);
    emvbase_avl_printtagallvalue("JCB-info: IAC-Denial: ", EMVTAG_IACDenial);
#endif

    IACDenialbExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_IACDenial, IACDenial, 0, 5));
    if(IACDenialbExist == 0)   //IAC-denial not exist
    {
        memset(IACDenial, 0, 5);
    }
    memcpy(CardTAC.IACDenial, IACDenial, 5);

    IACOnlinebExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_IACOnline, IACOnline, 0, 5));
    if(IACOnlinebExist == 0)   //IAC-online not exist
    {
        memset(CardTAC.IACOnline, 0xFF, 5);
        memset(IACOnline, 0, 5);
    }
    else
	{
		memcpy(CardTAC.IACOnline, IACOnline, 5);
	}

    IACDefaultbExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_IACDefault, IACDefault, 0, 5));
    if(IACDefaultbExist == 0)   //IAC-default not exist
    {
        memset(CardTAC.IACDefault, 0xFF, 5);
        memset(IACDefault, 0, 5);
    }
    else
    {
        memcpy(CardTAC.IACDefault, IACDefault, 5);   //lechan 20070207
    }

	//JCB Default TAC
	//Denial	0410000000
	//Online	9060009000
	//Default 9040008000
    bExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_TACDenial, TACDenial, 0, 5));
    if(bExist == 0)
    {
    	EMVBase_Trace("JCB-info: TAC-Denial missing-->use '04 10 00 00 00'\r\n");
        memset(TACDenial, 0, 5);
		memcpy(TACDenial, "\x04\x10\x00\x00\x00", 5);
    }

    bExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_TACOnline, TACOnline, 0, 5));
    if(bExist == 0)
    {
    	EMVBase_Trace("JCB-info: TAC-Online missing-->use '90 60 00 90 00'\r\n");
        memset(TACOnline, 0, 5);
        memcpy(TACOnline, "\x90\x60\x00\x90\x00", 5);
    }

    bExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_TACDefault, TACDefault, 0, 5));
    if(bExist == 0)
    {
    	EMVBase_Trace("JCB-info: TAC-Default missing-->use '90 40 00 80 00'\r\n");
        memset(TACDefault, 0, 5);
        memcpy(TACDefault, "\x90\x40\x00\x80\x00", 5);
    }

    memset(TVR, 0, sizeof(TVR));
    emvbase_avl_gettagvalue_spec(EMVTAG_TVR, TVR, 0, 5);
    if(!memcmp(TVR, "\x00\x00\x00\x00\x00", 5))
	{
		//return TAA_OFFLINE;
	}

#if 0
    emvbase_avl_printtagallvalue("Term online", EMVTAG_TACOnline);
    emvbase_avl_printtagallvalue("Term default", EMVTAG_TACDefault);
    emvbase_avl_printtagallvalue("Term declined", EMVTAG_TACDenial);
    emvbase_avl_printtagallvalue("ICC online", EMVTAG_IACOnline);
    emvbase_avl_printtagallvalue("ICC default", EMVTAG_IACDefault);
    emvbase_avl_printtagallvalue("ICC declined", EMVTAG_IACDenial);
#endif

	bFitIAC = 0;
    bFitTAC = 0;
    //ÊØîËæÉIACÂíåTVR,TACÂíåTVRÁöÑDENIAL‰Ω?Â¶ÇÊûúÊúâÁõ∏ÂêåÁöÑ‰ΩçÂàô‰∫§ÊòìDENIAL.
    for(i = 0; i < 5; i++)
    {
        k = TVR[i];

        if((k & CardTAC.IACDenial[i]) != 0)
        {
        	bFitIAC = 1;
        }

        if((k & TACDenial[i]) != 0)
        {
        	bFitTAC = 1;
        }
    }
    if(bFitIAC || bFitTAC)
	{
		EMVBase_Trace("JCB-info: AAC required\r\n");
		return TAA_DENIAL;
	}

    //k=tempterminfo->TermType&0x0F;
    TermType = emvbase_avl_gettagvalue(EMVTAG_TermType);
    TermType = TermType & 0x0F;

    //sxl20131024  bAbleOnlineËøòÈúÄË¶ÅÈáçÊñ∞Â§ÑÁê?
    if((TermType == 1 || TermType == 2 || TermType == 4 || TermType == 5) &&
       tempApp_UnionStruct->EMVTradeParam->bAbleOnline)          //Terminal has Online capability
    {           //ÊØîËæÉÊòØÊúâËÅîÊú∫ËÉΩÂäõÁöÑÁªàÁ´ØÁ±ªÂû?
                //ÊØîËæÉIACÂíåTVR,TACÂíåTVRÁöÑOnline‰Ω?Â¶ÇÊûúÊúâÁõ∏ÂêåÁöÑ‰ΩçÂàô‰∫§ÊòìOnline.Âê¶ÂàôOFFLINE
                //ÂΩì‰∫§ÊòìËÅîÊú∫Â§±Ë¥•Êó∂,bAbleOnlineË¢´Ê∏ÖÊà?.Âè™Âà§Êñ≠ÊâßË°åACCÊàñTC
        bFitIAC = 0;
        bFitTAC = 0;

        for(i = 0; i < 5; i++)
        {
            k = TVR[i];

            if((k & CardTAC.IACOnline[i]) != 0) { bFitIAC = 1; }

            if((k & TACOnline[i]) != 0) { bFitTAC = 1; }
        }

        if(bFitIAC || bFitTAC)
		{
			EMVBase_Trace("JCB-info: ARQC required\r\n");
			return TAA_ONLINE;
		}
        else
		{
			EMVBase_Trace("JCB-info: TC required\r\n");
			return TAA_OFFLINE;
		}
    }

    bFitIAC = 0;
    bFitTAC = 0;
    for(i = 0; i < 5; i++)
    {
        k = TVR[i];

        if((k & CardTAC.IACDefault[i]) != 0) { bFitIAC = 1; }

        if((k & TACDefault[i]) != 0) { bFitTAC = 1; }
    }

    if(bFitIAC || bFitTAC)
	{
		EMVBase_Trace("JCB-info: AAC required\r\n");
		return TAA_DENIAL;
	}
    else
	{
		EMVBase_Trace("JCB-info: TC required\r\n");
		return TAA_OFFLINE;
	}
}

//zwb 171102
unsigned char jcb_MDOLDefaultProcess(unsigned char *buf, unsigned short *len)
{//conside data format
#if 0
	*len = 19;
	memcpy(buf, "\x9F\x02\x06\x9F\x1A\x02\x5F\x2A\x02\x9A\x03\x9C\x01\x9F\x53\x03\x9F\x4E\x14", *len);
#else
	unsigned short taglen = 0;
	unsigned char *tagdata;
	//unsigned char MerchentName[41];

	tagdata = (unsigned char *)emvbase_malloc(300);
	emvbase_avl_gettagvalue_spec(EMVTAG_AmtAuthNum, tagdata, 0, 6);
	taglen += 6;

	emvbase_avl_gettagvalue_spec(EMVTAG_CountryCode, &tagdata[taglen], 0, 2);
	taglen += 2;

	emvbase_avl_gettagvalue_spec(EMVTAG_TransCurcyCode, &tagdata[taglen], 0, 2);
	taglen += 2;

	emvbase_avl_gettagvalue_spec(EMVTAG_TransDate, &tagdata[taglen], 0, 3);
	taglen += 3;

	emvbase_avl_gettagvalue_spec(EMVTAG_TransTypeValue, &tagdata[taglen], 0, 1);
	taglen += 1;

	emvbase_avl_gettagvalue_spec(EMVTAG_JCBDynamicTIP, &tagdata[taglen], 0, 3);
	taglen += 3;

	//memset(MerchentName, 0x00, 41);
	//memcpy(MerchentName, "\x58\x58\x20\x4D\x45\x52\x43\x48\x41\x4E\x54\x20\x59\x59\x20\x4C\x4F\x43\x41\x54\x49\x4F\x4E", 20);
	//memcpy(&tagdata[taglen], MerchentName, 20);
	emvbase_avl_gettagvalue_spec(EMVTAG_MerchantName, &tagdata[taglen], 0, 20);
	taglen += 20;

	memcpy(buf, tagdata, taglen);
	*len = taglen;

	emvbase_free(tagdata);

	return RLT_EMV_OK;

	/*unsigned char mdolbuf[60], buflen = 0;
	unsigned char i = 0;
	unsigned short copylen = 0;

	memset(mdolbuf, 0x00, 60);
	while(1)
	{
		if(memcmp(JCBMDOLDefaultTag[i].tag, "\x00\x00\x00\x00", 4) == 0)
		{
			break;
		}
		memcpy(&buf[copylen], JCBMDOLDefaultTag[i].tag, JCBMDOLDefaultTag[i].taglen)

		//emvbase_avl_gettagvalue_all((unsigned char *)JCBMDOLDefaultTag[i].tag, mdolbuf, &buflen);
		memcpy(&buf[copylen], mdolbuf, buflen);
		copylen += buflen;
		*len = copylen;
		buflen = 0;
		memset(mdolbuf, 0x00, 60);
		i++;
	}

	return RLT_EMV_OK;*/
#endif
}


unsigned char jcb_MDOLProcessFunc(unsigned char* CDDOL, unsigned short CDDOLLen, unsigned char* DOLData, unsigned short* DOLDataLen, JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned short index = 0;
    unsigned char *buf;   //[255]
    unsigned char bInTable;
    unsigned char tag[4], taglen;
    EMVBASETAGCVLITEM *DOLdataitem;
    unsigned short templen, indexOut = 0;
    //unsigned char bHasUnpredictNum = 0;
    unsigned char AIP[2], TermCapab[3];
    //unsigned char *TDOLData;       //[512]
    //unsigned short TDOLDataLen;
    //EMVBASETAGCVLITEM * item;
    //unsigned char CardTDOLDatabExist, TermTDOLDatabExist;
    //unsigned char TCHashValue[20];
    unsigned char *DOL;         //[300]
    unsigned short DOLLen;
    unsigned char i, k, m;
    //unsigned char CAPKI = 0;

    memset(AIP, 0, sizeof(AIP));
    emvbase_avl_gettagvalue_spec(EMVTAG_AIP, AIP, 0, 2);
    //memset(TermCapab, 0, sizeof(TermCapab));
    //emvbase_avl_gettagvalue_spec(EMVTAG_TermCapab, TermCapab, 0, 3);


    DOL = (unsigned char *)emvbase_malloc(300);
    buf = (unsigned char *)emvbase_malloc(255);



    memset(DOL, 0, 300);

    DOLLen = (CDDOLLen > 255) ? 255 : (CDDOLLen);
    memcpy(DOL, CDDOL, DOLLen);


    while(index < DOLLen)  //Process PDOL
    {
        if(DOL[index] == 0xFF || DOL[index] == 0x00) {index++; continue; }
        memset(buf, 0, 255);
        bInTable = 0;


        memset(tag, 0, sizeof(tag));
        taglen = 0;
        tag[taglen++] = DOL[index];

        if((tag[0] & 0x1f) == 0x1f)
        {
            tag[taglen++] = DOL[index + 1];

            if(tag[1] & 0x80)
            {
                tag[taglen++] = DOL[index + 2];
            }
        }

        DOLdataitem = emvbase_avl_gettagitempointer(tag);


        if(DOLdataitem != NULL)
        {
            index += taglen;



            if(EMVBase_ParseExtLen(DOL, &index, &templen))
            {
            	EMVBase_Trace("JCB-error: MDOL's value error\r\n");
                emvbase_free(DOL);
                emvbase_free(buf);
                return RLT_ERR_EMV_IccDataFormat;
            }
            k = templen;               // 1
            m = DOLdataitem->len;           // 4



            if(DOLdataitem->datafomat & EMVTAGFORMAT_N)          //numeric
            {
                if(k >= m)
                {
                    if(m)
                    {
                        memcpy(&buf[k - m], DOLdataitem->data, m);
                    }
                    memcpy(&DOLData[indexOut], buf, k);
                }
                else
                {
                    if(m)
                    {
                        memcpy(buf, DOLdataitem->data, m);
                    }
                    memcpy(&DOLData[indexOut], &buf[m - k], k);
                }
            }
            else if(DOLdataitem->datafomat & EMVTAGFORMAT_CN)          //compact numeric
            {
                if(m)
                {
                    memset(buf, 0xFF, 255);
                    memcpy(buf, DOLdataitem->data, m);
                    memcpy(&DOLData[indexOut], buf, k);
                }
                else
                {
                    memset(buf, 0x00, 255);
                    memcpy(&DOLData[indexOut], buf, k);
                }
            }
            else            //other formats
            {
                if(m)
                {
                    memcpy(buf, DOLdataitem->data, m);
                }
                memcpy(&DOLData[indexOut], buf, k);
            }
            indexOut += k;
            bInTable = 1;
        }

        if(!bInTable)
        {
            index += taglen;

            if(EMVBase_ParseExtLen(DOL, &index, &templen))
            {
            	EMVBase_Trace("JCB-error: MDOL's value error\r\n");
                emvbase_free(DOL);
                emvbase_free(buf);
                return RLT_ERR_EMV_IccDataFormat;
            }
            k = templen;
            memcpy(&DOLData[indexOut], buf, k);
            indexOut += k;
        }
    }


    emvbase_free(DOL);
    emvbase_free(buf);


    *DOLDataLen = indexOut;

    return RLT_EMV_OK;
}


unsigned char jcb_GetStripeDataCommand(JCBTradeUnionStruct *tempApp_UnionStruct, unsigned char TermAnaResult, EMVBASE_APDU_SEND *apdu_s)
{
	//unsigned char databuf[300];
	//unsigned short buflen;
	unsigned char MDOLExist;
	EMVBASETAGCVLITEM * item = NULL;
	unsigned char *Magmdol;
	unsigned short mdollen;
	unsigned char retCode;

	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode != Magstripe_Mode)
	{
		return RLT_EMV_ERR;
	}

	Magmdol = (unsigned char *)emvbase_malloc(300);

	item = emvbase_avl_gettagitemandstatus(EMVTAG_JCBMSDObjectList, &MDOLExist);

	EMVBase_Trace("JCBMSDObjectList exit = %d\r\n",MDOLExist);
	if(MDOLExist == 0)
	{
		//Jcb_MDOLDefaultProcess(item->data, item->len);
		jcb_MDOLDefaultProcess(Magmdol, &mdollen);
	}
	else
	{
		retCode = jcb_MDOLProcessFunc(item->data, item->len, Magmdol, &mdollen, tempApp_UnionStruct);
	}

	EMVBase_TraceHex("Jcb_GetStripeDataCommand ",Magmdol,mdollen);
	memcpy(apdu_s->Command, "\x80\xD0", 2);
	apdu_s->Command[2] = TermAnaResult;

	emvbase_avl_settag(EMVTAG_ReferenceControlParameter, apdu_s->Command[2]);

	apdu_s->Command[3] = 0x00;
	apdu_s->Lc = mdollen;
	memcpy(apdu_s->DataIn, Magmdol, mdollen);
	apdu_s->Le = 256;
	apdu_s->EnableCancel = 0;

	emvbase_free(Magmdol);

	return RLT_EMV_OK;
}


unsigned char jcb_CheckIfdoCDA(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char AIP[2];
    unsigned char TVR[5];
	unsigned char ImpleOption,CombOptions;

	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode != EMV_Mode)
	{
		return 0;
	}

	ImpleOption = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, &CombOptions, 0, 1);

    memset(AIP, 0, sizeof(AIP));
    emvbase_avl_gettagvalue_spec(EMVTAG_AIP, AIP, 0, 2);

    memset(TVR, 0, sizeof(TVR));
    emvbase_avl_gettagvalue_spec(EMVTAG_TVR, TVR, 0, 5);

    if((AIP[0] & 0x01) /*&& ((TVR[0] & 0x04) == 0)*/ && ((ImpleOption & 0xa0) == 0xa0) && ((CombOptions & 0x22) == 0x22))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned char jcb_TDOLProcess(unsigned char* TDOL, unsigned short TDOLLen, unsigned char* DOLData, unsigned short* DOLDataLen)
{
    unsigned short index = 0;
    unsigned char *buf;
    unsigned char bInTable;
    unsigned char tag[3], taglen;
    EMVBASETAGCVLITEM *DOLdataitem;
    unsigned short templen, indexOut = 0, DOLLen;

    unsigned char i, k, m;
    unsigned char *DOL;


    buf = (unsigned char *)emvbase_malloc(255);
    DOL = (unsigned char *)emvbase_malloc(300);


    memset(DOL, 0, 300);

    DOLLen = (TDOLLen > 255) ? 255 : (TDOLLen);
    memcpy(DOL, TDOL, DOLLen);

    while(index < DOLLen)  //Process PDOL
    {
        if(DOL[index] == 0xFF || DOL[index] == 0x00) {index++; continue; }
        memset(buf, 0, 255);
        bInTable = 0;


        memset(tag, 0, sizeof(tag));
        taglen = 0;
        tag[taglen++] = DOL[index];

        if((tag[0] & 0x1f) == 0x1f)
        {
            tag[taglen++] = DOL[index + 1];

            if(tag[1] & 0x80)
            {
                tag[taglen++] = DOL[index + 2];
            }
        }
        DOLdataitem = emvbase_avl_gettagitempointer(tag);


        if(DOLdataitem != NULL)
        {
            index += taglen;



            if(EMVBase_ParseExtLen(DOL, &index, &templen))
            {
            	EMVBase_Trace("JCB-error: TDOL's value error\r\n");
                emvbase_free(buf);
                emvbase_free(DOL);
                return RLT_ERR_EMV_IccDataFormat;
            }
            k = templen;
            m = DOLdataitem->len;


            if(DOLdataitem->datafomat & EMVTAGFORMAT_N)          //numeric
            {
                if(k >= m)
                {
                    if(m)
                    {
                        memcpy(&buf[k - m], DOLdataitem->data, m);
                    }
                    memcpy(&DOLData[indexOut], buf, k);
                }
                else
                {
                    if(m)
                    {
                        memcpy(buf, DOLdataitem->data, m);
                    }
                    memcpy(&DOLData[indexOut], &buf[m - k], k);
                }
            }
            else if(DOLdataitem->datafomat & EMVTAGFORMAT_CN)          //compact numeric
            {
                if(m)
                {
                    memset(buf, 0xFF, 255);
                    memcpy(buf, DOLdataitem->data, m);
                    memcpy(&DOLData[indexOut], buf, k);
                }
                else
                {
                    memset(buf, 0x00, 255);
                    memcpy(&DOLData[indexOut], buf, k);
                }
            }
            else            //other formats
            {
                if(m)
                {
                    memcpy(buf, DOLdataitem->data, m);
                }
                memcpy(&DOLData[indexOut], buf, k);
            }
            indexOut += k;
            bInTable = 1;
        }

        if(!bInTable)
        {
            index += taglen;

            if(EMVBase_ParseExtLen(DOL, &index, &templen))
            {
            	EMVBase_Trace("JCB-error: TDOL's value error\r\n");
                emvbase_free(buf);
                emvbase_free(DOL);
                return RLT_ERR_EMV_IccDataFormat;
            }
            k = templen;
            memcpy(&DOLData[indexOut], buf, k);
            indexOut += k;
        }
    }

    *DOLDataLen = indexOut;


    emvbase_free(buf);
    emvbase_free(DOL);
    return RLT_EMV_OK;
}

unsigned char jcb_GACDOLProcessFunc(unsigned char type, unsigned char* CDDOL, unsigned short CDDOLLen, unsigned char* DOLData, unsigned short* DOLDataLen, JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned short index = 0;
    unsigned char *buf;   //[255]
    unsigned char bInTable;
    unsigned char tag[4], taglen;
    EMVBASETAGCVLITEM *DOLdataitem;
    unsigned short templen, indexOut = 0;
    unsigned char bHasUnpredictNum = 0;
    unsigned char AIP[2], TermCapab[3];
    unsigned char *TDOLData;       //[512]
    unsigned short TDOLDataLen;
    EMVBASETAGCVLITEM * item;
    unsigned char CardTDOLDatabExist, TermTDOLDatabExist;
    unsigned char TCHashValue[20];
    unsigned char *DOL;         //[300]
    unsigned short DOLLen;
    unsigned char i, k, m;
    unsigned char CAPKI = 0;
	unsigned char TransactionMode;

    memset(AIP, 0, sizeof(AIP));
    emvbase_avl_gettagvalue_spec(EMVTAG_AIP, AIP, 0, 2);
    memset(TermCapab, 0, sizeof(TermCapab));
    emvbase_avl_gettagvalue_spec(EMVTAG_TermCapab, TermCapab, 0, 3);
	//EMVBase_TraceHex( "Emvb_DOLProcessFunc TAG_TermCapab 9F33", TermCapab, 3);
	EMVBase_Trace( "Emvb_DOLProcessFunc TermCapab=%02x %02x %02x\r\n", TermCapab[0], TermCapab[1], TermCapab[2]);

	TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;

    DOL = (unsigned char *)emvbase_malloc(300);
    buf = (unsigned char *)emvbase_malloc(255);



    memset(DOL, 0, 300);

    DOLLen = (CDDOLLen > 255) ? 255 : (CDDOLLen);
    memcpy(DOL, CDDOL, DOLLen);


    while(index < DOLLen)  //Process PDOL
    {
        if(DOL[index] == 0xFF || DOL[index] == 0x00) {index++; continue; }
        memset(buf, 0, 255);
        bInTable = 0;


        memset(tag, 0, sizeof(tag));
        taglen = 0;
        tag[taglen++] = DOL[index];

        if((tag[0] & 0x1f) == 0x1f)
        {
            tag[taglen++] = DOL[index + 1];

            if(tag[1] & 0x80)
            {
                tag[taglen++] = DOL[index + 2];
            }
        }

        if(!memcmp(tag, "\x9F\x37", 2))
        {
            bHasUnpredictNum = 1;
        }

        //4.3e add 20160330
        if(!memcmp((unsigned char*)&DOL[index], "\x9F\x22", 2))      //ÁªàÁ´ØCAPKI
        {
            index += 3;
            emvbase_avl_gettagvalue_spec(EMVTAG_CAPKI, &CAPKI, 0, 1);
            DOLData[indexOut] = CAPKI;
            indexOut += 1;
            continue;
        }
        //end

		EMVBase_Trace( "bHasUnpredictNum=%d tag[0]=%02x\r\n",bHasUnpredictNum, tag[0]);
        if(type == typeCDOL1 || type == typeCDOL2)
        {
            if(tag[0] == 0x98)
            {
                TDOLData = (unsigned char *)emvbase_malloc(512);
                memset(TDOLData, 0, 512);
                TDOLDataLen = 0;

                TermTDOLDatabExist = 0;
                item = emvbase_avl_gettagitemandstatus(EMVTAG_TDOL, &CardTDOLDatabExist);

                if(CardTDOLDatabExist == 0)
                {
                    item = emvbase_avl_gettagitemandstatus(EMVTAG_TermTDOL, &TermTDOLDatabExist);
                }

                if(item != NULL)
                {
                    if(item->len)
                    {
                        jcb_TDOLProcess(item->data, item->len, TDOLData, &TDOLDataLen);
                    }
                }
                EMVBase_Hash(TDOLData, TDOLDataLen, TCHashValue);

                emvbase_free(TDOLData);

                if(TermTDOLDatabExist)
                {
                    emvbase_avl_setvalue_or(EMVTAG_TVR, 4, 0x80);
                }
                emvbase_avl_createsettagvalue(EMVTAG_TCHashValue, TCHashValue, 20);
            }
        }
        DOLdataitem = emvbase_avl_gettagitempointer(tag);


        if(DOLdataitem != NULL)
        {
            index += taglen;



            if(EMVBase_ParseExtLen(DOL, &index, &templen))
            {
            	EMVBase_Trace("JCB-error: DOL's value error\r\n");
                emvbase_free(DOL);
                emvbase_free(buf);
                return RLT_ERR_EMV_IccDataFormat;
            }
            k = templen;               // 1
            m = DOLdataitem->len;           // 4



            if(DOLdataitem->datafomat & EMVTAGFORMAT_N)          //numeric
            {
                if(k >= m)
                {
                    if(m)
                    {
                        memcpy(&buf[k - m], DOLdataitem->data, m);
                    }
                    memcpy(&DOLData[indexOut], buf, k);
                }
                else
                {
                    if(m)
                    {
                        memcpy(buf, DOLdataitem->data, m);
                    }
                    memcpy(&DOLData[indexOut], &buf[m - k], k);
                }
            }
            else if(DOLdataitem->datafomat & EMVTAGFORMAT_CN)          //compact numeric
            {
                if(m)
                {
                    memset(buf, 0xFF, 255);
                    memcpy(buf, DOLdataitem->data, m);
                    memcpy(&DOLData[indexOut], buf, k);
                }
                else
                {
                    memset(buf, 0x00, 255);
                    memcpy(&DOLData[indexOut], buf, k);
                }
            }
            else            //other formats
            {
                if(m)
                {
                    memcpy(buf, DOLdataitem->data, m);
                }
                memcpy(&DOLData[indexOut], buf, k);
            }
            indexOut += k;
            bInTable = 1;
        }

        if(!bInTable)
        {
            index += taglen;

            if(EMVBase_ParseExtLen(DOL, &index, &templen))
            {
            	EMVBase_Trace("JCB-error: DOL's value error\r\n");
                emvbase_free(DOL);
                emvbase_free(buf);
                return RLT_ERR_EMV_IccDataFormat;
            }
            k = templen;
            memcpy(&DOLData[indexOut], buf, k);
            indexOut += k;
        }
		EMVBase_TraceHex("DOL tag", tag, taglen);
    }


    emvbase_free(DOL);
    emvbase_free(buf);


    *DOLDataLen = indexOut;

    if(bHasUnpredictNum == 0)
    {
        if(type == typeDDOL)
        {
			EMVBase_Trace( "--------->error2222\r\n");
            return RLT_EMV_ERR;
        }
        else if(type == typeCDOL1)
        {
            tempApp_UnionStruct->EMVTradeParam->bCDOL1HasNoUnpredictNum = 1;

			//if(TransactionMode == EMV_Mode)
			{
				if(jcb_CheckIfdoCDA(tempApp_UnionStruct))
				{
					return RLT_EMV_OK;
				}
			}

            if((AIP[0] & 0x01) && (TermCapab[2] & 0x08))
            {
				EMVBase_Trace( "1111111111\r\n");
                return RLT_EMV_OK;
            }
        }
        else if(type == typeCDOL2)
        {
            tempApp_UnionStruct->EMVTradeParam->bCDOL2HasNoUnpredictNum = 1;

			if(jcb_CheckIfdoCDA(tempApp_UnionStruct))
			{
				return RLT_EMV_OK;
			}

            if((AIP[0] & 0x01) && (TermCapab[2] & 0x08))
            {
				EMVBase_Trace( "222222222222\r\n");
                return RLT_EMV_OK;
            }
        }
		EMVBase_Trace( "--------->error3333\r\n");
        return RLT_EMV_ERR;
    }
    return RLT_EMV_OK;
}


unsigned char jcb_GACDOLProcess(unsigned char type, unsigned char * DOL, unsigned short DOLLen, unsigned char* DOLData, unsigned short* DOLDataLen, JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char retCode;
    unsigned char TVR[5];

    retCode = jcb_GACDOLProcessFunc(type, DOL, DOLLen, DOLData, DOLDataLen, tempApp_UnionStruct);
	EMVBase_Trace("Emvb_DOLProcessFunc retcode=%d\r\n", retCode);

    memset(TVR, 0, sizeof(TVR));
    emvbase_avl_gettagvalue_spec(EMVTAG_TVR, TVR, 0, 5);

    if(TVR[4] & 0x80)  //if 'Default TDOL used' bit is 1
    {
        jcb_GACDOLProcessFunc(type, DOL, DOLLen, DOLData, DOLDataLen, tempApp_UnionStruct);
    }
    return retCode;
}

unsigned char jcb_CommandGAC1(JCBTradeUnionStruct *tempApp_UnionStruct, unsigned char TermAnaResult, EMVBASE_APDU_SEND *apdu_s)
{
    unsigned char retCode;    //TermAnaResult,CardAnaResult;//0-Denial,1-Online,2-Offline
    unsigned short len;
    unsigned char SubAmount[6];
    unsigned char bCombineDDA = 0;
    unsigned char CDOL1bExist;
    EMVBASETAGCVLITEM *item;


    memcpy(apdu_s->Command, "\x80\xAE", 2);
	if(TermAnaResult == TAA_ONLINE)
    {
        memcpy(&apdu_s->Command[2], "\x80\x00", 2);      //get ARQC
    }
    else if(TermAnaResult == TAA_OFFLINE)
    {
    	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode)
    	{
    		memcpy(&apdu_s->Command[2], "\x40\x00", 2);          //get TC
        	//emvbase_avl_createsettagvalue(EMVTAG_AuthRespCode, "Y1", 2);
    	}
		else
		{
			EMVBase_Trace("JCB-error: MS Mode or Legacy Mode but TC required\r\n");
			return RLT_EMV_TERMINATE_TRANSERR;
		}
    }
    else
    {
        return RLT_EMV_TERMINATE_TRANSERR;//process transaction outcome
    }

    if(TermAnaResult == TAA_ONLINE || TermAnaResult == TAA_OFFLINE)
    {
        if(jcb_CheckIfdoCDA(tempApp_UnionStruct))
        {
            apdu_s->Command[2] |= 0x10;          //Combined DDA/AC requested.
        }
    }
    emvbase_avl_settag(EMVTAG_ReferenceControlParameter, apdu_s->Command[2]);


    item = emvbase_avl_gettagitemandstatus(EMVTAG_CDOL1, &CDOL1bExist);

    if(CDOL1bExist == 0)
    {
    	EMVBase_Trace("JCB-error: CDOL1(Tag8C) missing\r\n");
        return RLT_EMV_ERR;
    }
    retCode = jcb_GACDOLProcess(typeCDOL1, item->data, item->len, apdu_s->DataIn, &len, tempApp_UnionStruct);
	/*if(retCode != RLT_EMV_OK)
	{
		retCode = JCB_OPS_STATUS_DECLINED;
		return retCode;
	}*/


    emvbase_avl_createsettagvalue(EMVTAG_CDOL1RelatedData, apdu_s->DataIn, len);

    apdu_s->Lc = len;
    apdu_s->Le = 256;

    apdu_s->EnableCancel = 0;

    return retCode;
}



unsigned char jcb_DealGAC1Rsp(JCBTradeUnionStruct *tempApp_UnionStruct,EMVBASE_APDU_RESP *apdu_r)
{
    unsigned char ret;
	unsigned char k,j,t,indexRet = 0;
	unsigned short index,indexTemp,len,lenTemp,orgindex,tmplen,startpos;
	unsigned char Primitivetype = 0;
	unsigned char countpadding = 0;
	unsigned char bIntable;
	unsigned char GenerateACRetDataflag;
	unsigned char CryptInfo,reqCrpt;
	unsigned char VLPAvailableFund[6];
    EMVBASETAGCVLITEM *item;
	unsigned char AppCryptExist, CryptInfoExist, ATCExist,CardholderVerifStatus,SDADExist;
	unsigned char TransactionMode, ImpleOption, AcquiOption, AIP[2];
	unsigned char Track2Exist;

    tempApp_UnionStruct->EMVTradeParam->GenerateACRetDataLen = 0;
	memset(tempApp_UnionStruct->EMVTradeParam->GenerateACRetData,0,255);

	TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;

	if(TransactionMode == EMV_Mode)
	{
		if(apdu_r->SW1==0x69 && apdu_r->SW2==0x86)
		{
			EMVBase_Trace("JCB-error: SW == 6986\r\n");
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION_ONDEVICECVM);
			return RLT_ERR_EMV_SEEPHONE;
		}
		else if(apdu_r->SW1==0x69 && apdu_r->SW2==0x84)
		{
			EMVBase_Trace("JCB-error: SW == 6984\r\n");
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_TRYANOTHERINTERFACE);
			return RLT_ERR_EMV_SWITCHINTERFACE;
		}
		else if(!(apdu_r->SW1==0x90 && apdu_r->SW2==0x00))
		{
			EMVBase_Trace("JCB-error: SW != 9000\r\n");
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
			return RLT_EMV_APPSELECTTRYAGAIN;
		}
	}
	else if(TransactionMode == Magstripe_Mode)
	{
		if(apdu_r->SW1==0x69 && apdu_r->SW2==0x86)
		{
			EMVBase_Trace("JCB-error: SW == 6986\r\n");
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION_ONDEVICECVM);
			return RLT_ERR_EMV_SEEPHONE;
		}
		else if(apdu_r->SW1==0x63 && apdu_r->SW2==0x00)
		{//This Status Word indicates that the card refuses the Magstripe Mode transaction.
			EMVBase_Trace("JCB-info: SW == 6300\r\n");
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			return RLT_EMV_OFFLINE_DECLINE;
		}
		else if(!(apdu_r->SW1==0x90 && apdu_r->SW2==0x00))
		{
			EMVBase_Trace("JCB-error: SW != 9000\r\n");
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
			return RLT_EMV_APPSELECTTRYAGAIN;
		}
	}
	else if(TransactionMode == Legacy_Mode)
	{
		if(!(apdu_r->SW1==0x90 && apdu_r->SW2==0x00))
		{
			EMVBase_Trace("JCB-error: SW != 9000\r\n");
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
			return RLT_EMV_APPSELECTTRYAGAIN;
		}
	}


	emvbase_avl_deletetag(EMVTAG_SignDynAppData);
	emvbase_avl_deletetag(EMVTAG_CryptInfo);
	emvbase_avl_deletetag(EMVTAG_ATC);
	emvbase_avl_deletetag(EMVTAG_AppCrypt);

	//if(TransactionMode == EMV_Mode)
		//emvbase_avl_deletetag(EMVTAG_JCBCardholderVerifyStatus);

	if(TransactionMode == Magstripe_Mode)//return tag57 for magstripe mode
	{
		emvbase_avl_deletetag(EMVTAG_Track2Equ);
	}


	if(TransactionMode == EMV_Mode)
	{
		if(apdu_r->DataOut[0] != 0x77)
		{
			EMVBase_Trace("JCB-error: r-apdu template error (first byte != 0x77)\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}
	else if(TransactionMode == Legacy_Mode)
	{
		if(apdu_r->DataOut[0] != 0x80)
		{
			EMVBase_Trace("JCB-error: r-apdu template error (first byte != 0x80)\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}

	#if 0
	apdu_r->LenOut = 0x15;
	memset(apdu_r->DataOut, 0x00, 300);
	memcpy(apdu_r->DataOut, "\x57\x13\x35\x40\x82\x99\x99\x42\x10\x12\xD4\x91\x22\x01\x55\x55\x55\x55\x55\x55\x2F", apdu_r->LenOut);
	TransactionMode = Magstripe_Mode;
	emvbase_avl_deletetag(EMVTAG_Track2Equ);
	#endif

    index=0;
	if(apdu_r->DataOut[index]==0x80)//primitive data
	{
		EMVBase_Trace("JCB-info: GAC template 80\r\n");

		index++;
        if(EMVBase_ParseExtLen(apdu_r->DataOut,&index,&lenTemp))
		{
			EMVBase_Trace("JCB-info: Tag80 parsing error\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}

		if(apdu_r->LenOut != index + lenTemp)
		{
			EMVBase_Trace("JCB-info: Tag80's len error\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}


		if(lenTemp != 19)  //2021.9.15 lishiyao add for 1.5a,Legacy Data len is constant,19
		{
			EMVBase_Trace("JCB-info: When Legacy Mode Tag80's len != 19\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
		EMVBase_Trace("JCB-info:Tag 80 len = %d\r\n", lenTemp);

		while(apdu_r->DataOut[index+countpadding] ==0)
		{
			countpadding++;
			if(countpadding >2)
			{
				EMVBase_Trace("JCB-info: Tag80's value error\r\n");
				return RLT_EMV_OFFLINE_DECLINE;
			}
		}


		emvbase_avl_settag(EMVTAG_CryptInfo,apdu_r->DataOut[index]);
		index++;
        emvbase_avl_createsettagvalue(EMVTAG_ATC,&apdu_r->DataOut[index],2);
		index += 2;
		emvbase_avl_createsettagvalue(EMVTAG_AppCrypt,&apdu_r->DataOut[index],8);
		index += 8;

		if(apdu_r->LenOut>index)
		{
			k=apdu_r->LenOut-index;
			if(k>32)
			{
				EMVBase_Trace("JCB-info: Tag80's value error\r\n");
				return RLT_EMV_OFFLINE_DECLINE;
			}

			emvbase_avl_createsettagvalue(EMVTAG_IssuAppData,(unsigned char*)&apdu_r->DataOut[index],k);

		}

	}
	else if(apdu_r->DataOut[index]==0x77)//TLV coded data
	{
		EMVBase_Trace("JCB-info: GAC template 77\r\n");

	    Primitivetype = 1;
		index++;
		if(EMVBase_ParseExtLen(apdu_r->DataOut,&index,&lenTemp))
		{
			EMVBase_Trace("JCB-info: Tag77 parsing error\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
		indexTemp=index;
		indexRet=0;
		while(index<indexTemp+lenTemp)
		{
			if(index>=apdu_r->LenOut)
			{
				EMVBase_Trace("JCB-info: r-apdu parsing error\r\n");
				EMVBase_Trace("JCB-info: sub Tags' len error\r\n");
				return RLT_EMV_OFFLINE_DECLINE;
			}
			if(apdu_r->DataOut[index]==0xFF || apdu_r->DataOut[index]==0x00)
			{
				index++;
				continue;
			}

            orgindex = index;

			bIntable = 0;
			GenerateACRetDataflag = 1;

			if(!memcmp((unsigned char*)&apdu_r->DataOut[index],"\x9F\x4B",2))
			{
				GenerateACRetDataflag = 0;
			}

			ret = jcb_ParseAndStoreCardResponse(tempApp_UnionStruct,apdu_r->DataOut,&index,&bIntable,0);
			if(ret != RLT_EMV_OK)
			{
				EMVBase_Trace("JCB-info: sub Tags in Tag77 parsing error\r\n");
				ret = RLT_EMV_OFFLINE_DECLINE;
				return ret;
			}

			//sxl20110406
			if(GenerateACRetDataflag&&(indexRet+index-orgindex <=255))
			{
				memcpy(tempApp_UnionStruct->EMVTradeParam->GenerateACRetData+indexRet,(unsigned char*)&apdu_r->DataOut[orgindex],index-orgindex);
				indexRet+=index-orgindex;
			}

		}	//endwhile
		if(index!=indexTemp+lenTemp)
		{
			EMVBase_Trace("JCB-info: Tag77's value parsing error\r\n");
			EMVBase_Trace("JCB-info: sub Tags' len error\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}
	else if(apdu_r->DataOut[index]==0x57)//only for magstripe_mode
	{
		EMVBase_Trace("JCB-info: GAC template 57\r\n");

		if(TransactionMode != Magstripe_Mode)
		{
			EMVBase_Trace("JCB-info: EMV Mode or Legacy Mode but template 57 returned\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}

		index++;
		if(EMVBase_ParseExtLen(apdu_r->DataOut,&index,&lenTemp))
		{
			EMVBase_Trace("JCB-info: Tag57 parsing error\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
		indexTemp=index;
		indexRet=0;
		while(index<indexTemp+lenTemp)
		{
			if(index>=apdu_r->LenOut)
			{
				EMVBase_Trace("JCB-info: r-apdu parsing error\r\n");
				EMVBase_Trace("JCB-info: sub Tags' len error\r\n");
				return RLT_EMV_OFFLINE_DECLINE;
			}
			if(apdu_r->DataOut[index]==0xFF || apdu_r->DataOut[index]==0x00)
			{
				index++;
				continue;
			}

            orgindex = index;

			bIntable = 0;
			index = 0;

			ret = jcb_ParseAndStoreCardResponse(tempApp_UnionStruct,apdu_r->DataOut,&index,&bIntable,0);

			if(ret != RLT_EMV_OK)
			{
				EMVBase_Trace("JCB-info: sub Tags in Tag57 parsing error\r\n");
				ret = RLT_EMV_OFFLINE_DECLINE;
				return ret;
			}

		}	//endwhile
		if(index != (indexTemp+lenTemp))
		{
			EMVBase_Trace("JCB-info: Tag57's value parsing error\r\n");
			EMVBase_Trace("JCB-info: sub Tags' len error\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}
	else
	{
		EMVBase_Trace("JCB-info: GAC template error (first byte != 77 or 80 or 57)\r\n");

		if(TransactionMode == Magstripe_Mode)
		{
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
			//jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
			return RLT_EMV_APPSELECTTRYAGAIN;
		}
		return RLT_EMV_OFFLINE_DECLINE;

	}

	//unsigned char AppCryptExist, CryptInfoExist, ATCExist,CardholderVerifStatus,SDADExist;
	AppCryptExist = emvbase_avl_checkiftagexist(EMVTAG_AppCrypt);//9F26
	CryptInfoExist = emvbase_avl_checkiftagexist(EMVTAG_CryptInfo);//9F27
	ATCExist = emvbase_avl_checkiftagexist(EMVTAG_ATC);//9F36
	SDADExist = emvbase_avl_checkiftagexist(EMVTAG_SignDynAppData);//9F4B
	CardholderVerifStatus = emvbase_avl_checkiftagexist(EMVTAG_JCBCardholderVerifyStatus);//9F50
	EMVBase_Trace( "mode=%d 26=%d 27=%d 36=%d 4B=%d 50=%d\r\n", TransactionMode, AppCryptExist, CryptInfoExist, ATCExist, SDADExist, CardholderVerifStatus);

	if(TransactionMode == EMV_Mode)
	{
		if((CryptInfoExist == 0)  || (ATCExist == 0))
		{
			if(0 == CryptInfoExist)
			{
				EMVBase_Trace("JCB-info: CID(Tag9F27) missing\r\n");
			}
			if(0 == ATCExist)
			{
				EMVBase_Trace("JCB-info: ATC(Tag9F36) missing\r\n");
			}

			return RLT_EMV_OFFLINE_DECLINE;
		}
	}

	if(TransactionMode == Magstripe_Mode)
	{
		Track2Exist = emvbase_avl_checkiftagexist(EMVTAG_Track2Equ);
		if(Track2Exist == 0)
		{
			EMVBase_Trace("JCB-info: Track2(Tag57) missing\r\n");

			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_SELECTNEXT);
			//jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
			return RLT_EMV_APPSELECTTRYAGAIN;
		}
	}


    if(Primitivetype == 0)
    {
    	if(TransactionMode != Magstripe_Mode)
    	{
    		if(emvbase_avl_checkiftagexist(EMVTAG_AppCrypt)==0)
    		{
				EMVBase_Trace("JCB-info: Application Cryptogram(Tag9F26) missing\r\n");

    			return RLT_EMV_OFFLINE_DECLINE;
    		}
    	}
    }

	tempApp_UnionStruct->EMVTradeParam->GenerateACRetDataLen = indexRet;

	CryptInfo = emvbase_avl_gettagvalue(EMVTAG_CryptInfo);
	reqCrpt = emvbase_avl_gettagvalue(EMVTAG_ReferenceControlParameter);
	//TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, &AcquiOption, 0, 1);
	ImpleOption = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;
	memset(AIP, 0x00, 2);
	emvbase_avl_gettagvalue_spec(EMVTAG_AIP, AIP, 0, 2);

	EMVBase_Trace("JCB-info: CID(Tag9F27: %02x\r\n", CryptInfo);

	if(TransactionMode == EMV_Mode)
	{
		if((CryptInfo & 0xc0) == 0x40 && (reqCrpt & TAA_OFFLINE))
		{
			EMVBase_Trace("JCB-info: CID(Tag9F27)-TC(bit8 bit7 == '01')\r\n");
		}
		else if((CryptInfo & 0xc0) == 0x80 && ((reqCrpt & TAA_OFFLINE) || (reqCrpt & TAA_ONLINE)))
		{
			EMVBase_Trace("JCB-info: CID(Tag9F27)-ARQC(bit8 bit7 == '10')\r\n");
		}
		else if((CryptInfo & 0xc0) == 0x00)
		{
			EMVBase_Trace("JCB-info: CID(Tag9F27)-AAC(bit8 bit7 == '00')\r\n");
		}
		else
		{
			EMVBase_Trace("paywave-info: CID(Tag9F27) bit8 bit7 == '11'-->decline'\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}

		if((CryptInfo&0xc0) == 0x00)//AAC
		{
			//if(AppCryptExist == 0)
			{
				return RLT_EMV_OFFLINE_DECLINE;
			}
		}

		//TC return or ARQC return(CDA Request)
		if(((CryptInfo&0xc0) == 0x40 || (CryptInfo&0xc0) == 0x80) && (reqCrpt&0x10) == 0x10)
		{
			if((SDADExist == 0) || (CardholderVerifStatus == 0))
			{
				if(0 == SDADExist)
				{
					EMVBase_Trace("JCB-info: Signed Dynamic Application Data(Tag9F4B) missing\r\n");
				}
				if(0 == CardholderVerifStatus)
				{
					EMVBase_Trace("JCB-info: Cardholder Verification Status(Tag9F50) missing\r\n");
				}

				return RLT_EMV_OFFLINE_DECLINE;
			}
		}

		//ARQC return(CDA not Request)
		if(((CryptInfo&0xc0) == 0x80 || (CryptInfo&0xc0) == 0x40) && (reqCrpt&0x10) != 0x10)
		{
			if((AppCryptExist == 0) || (CardholderVerifStatus == 0))
			{
				if(0 == AppCryptExist)
				{
					EMVBase_Trace("JCB-info: Application Cryptogram(Tag9F26) missing\r\n");
				}
				if(0 == CardholderVerifStatus)
				{
					EMVBase_Trace("JCB-info: Cardholder Verification Status(Tag9F50) missing\r\n");
				}

				return RLT_EMV_OFFLINE_DECLINE;
			}
		}

	}
	else if(TransactionMode == Legacy_Mode)
	{
		if((CryptInfo&0xc0) != 0x80)//online only
		{
			EMVBase_Trace("JCB-info: Legacy mode but CID(Tag9F27) not ARQC(bit8 bit7 != '10')\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}
	else if(TransactionMode == Magstripe_Mode) // return only tag 57
	{
		if(emvbase_avl_checkiftagexist(EMVTAG_Track2Equ) == 0)
		{
			EMVBase_Trace("JCB-info: Track2(Tag57) missing\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}

		if((reqCrpt&TAA_ONLINE))
		{
		}
		else if((reqCrpt&TAA_DENIAL))
		{

		}
		else
		{
			EMVBase_Trace("JCB-info: MS Mode but TC required\r\n");
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}


	//NO CDA
	if(emvbase_avl_checkiftagexist(EMVTAG_SignDynAppData)==0 && TransactionMode == EMV_Mode)
	{
		if(emvbase_avl_checkiftagexist(EMVTAG_AppCrypt)==0)
		{
			EMVBase_Trace("JCB-error: Application Cryptogram(Tag9F26) missing\r\n");
			return RLT_ERR_EMV_IccDataFormat;
		}

		if((CryptInfo&0xc0) == 0x00) //AAC
		{
			if((reqCrpt&0xC0) == 0x00)
			{
				if(reqCrpt&0x10)
				{
					EMVBase_Trace("JCB-info: CDA required but Signed Dynamic Application Data(Tag9F4B) missing\r\n");
					return RLT_EMV_OFFLINE_DECLINE;
				}
			}
		}
		else
		{
			if(reqCrpt&0x10)
			{
				EMVBase_Trace("JCB-info: CDA required but Signed Dynamic Application Data(Tag9F4B) missing\r\n");
				return RLT_EMV_OFFLINE_DECLINE;
			}
		}


	}

    ret = RLT_EMV_OK;

	//emvbase_avl_setvalue_or(EMVTAG_TSI,0,0x20);


	if((CryptInfo&0x07)==0x01) // Service not allowed
	{
		EMVBase_Trace("JCB-error: Service not allowed( CID(Tag9F27) (bit3 bit2 bit1 == '001')\r\n");
		emvbase_avl_setvalue_or(EMVTAG_TVR,1,0x10);
		ret = RLT_ERR_EMV_NotAccept;
	}

	return ret;


}

unsigned char jcb_PrepareRecoveryContext(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char retCode;
	unsigned short templen;
	unsigned char *HashData;
	unsigned char crc;
	JCBTORNRECOVERYCONTEXT *gTronRecoveryContext;

	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode != EMV_Mode)
	{
		return RLT_EMV_OK;
	}

	gTronRecoveryContext = (JCBTORNRECOVERYCONTEXT *)emvbase_malloc(sizeof(JCBTORNRECOVERYCONTEXT));
	memset(&gJCBTornRecoveryContext, 0x00, sizeof(JCBTORNRECOVERYCONTEXT));

	tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag = 1;
	gJCBTornRecoveryContext.RecoveryTornEMVTransactionFlag = 1;
	emvbase_avl_gettagvalue_spec(EMVTAG_ReferenceControlParameter,&gJCBTornRecoveryContext.ReferenceControlParameter,0 ,1);


	emvbase_avl_gettagvalue_spec(EMVTAG_UnpredictNum,gJCBTornRecoveryContext.UnpredictNum,0 ,4);

	//Track2
	emvbase_avl_gettagvalue_all(EMVTAG_Track2Equ, gJCBTornRecoveryContext.TornTrack2Data, &templen);
	gJCBTornRecoveryContext.TornTrack2len = templen;

	//CDA EMVBase_Hash data
	HashData =(unsigned char *)emvbase_malloc(512);
	emvbase_avl_gettagvalue_all(EMVTAG_PDOLData, HashData,&templen);
	gJCBTornRecoveryContext.TornCDAHashDatalen +=templen;

	emvbase_avl_gettagvalue_all(EMVTAG_CDOL1RelatedData, &HashData[templen],&templen);
	gJCBTornRecoveryContext.TornCDAHashDatalen +=templen;

	memcpy(gJCBTornRecoveryContext.TornCDAHashDataBuffer, HashData, gJCBTornRecoveryContext.TornCDAHashDatalen);
	emvbase_free(HashData);

	emvbase_avl_printtagallvalue("PrepareRecoveryContext PDOLData: ", EMVTAG_PDOLData);
	emvbase_avl_printtagallvalue("PrepareRecoveryContext CDOL1RelatedData: ", EMVTAG_CDOL1RelatedData);
	EMVBase_Trace("gJCBTornRecoveryContext.TornCDAHashDatalen: %d", gJCBTornRecoveryContext.TornCDAHashDatalen);
	EMVBase_TraceHex("gJCBTornRecoveryContext.TornCDAHashDataBuffer: ", gJCBTornRecoveryContext.TornCDAHashDataBuffer, gJCBTornRecoveryContext.TornCDAHashDatalen);
#if 0
	memcpy(gTronRecoveryContext, (unsigned char *)&gJCBTornRecoveryContext, TORNRECOVERYCONTEXTLEN);
	//save to file
	if(kern_access_file(TORNRECOVERYCONTEXT) != 0)
	{
		retCode = kern_create_file(TORNRECOVERYCONTEXT,TORNRECOVERYCONTEXTLEN,0xFF);
		if(retCode != 0)
		{
			EMVBase_Trace( "kern_create_file tron error!!!!\r\n");
			emvbase_free(gTronRecoveryContext);
			return RLT_EMV_ERR;
		}
	}
	crc = arith_getcrc(gTronRecoveryContext,TORNRECOVERYCONTEXTLEN-1);
	gTronRecoveryContext->crc = crc;
	kern_write_file(TORNRECOVERYCONTEXT, gTronRecoveryContext,TORNRECOVERYCONTEXTLEN,0);

#endif
	emvbase_free(gTronRecoveryContext);

	return RLT_EMV_OK;

}


/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: void Paywave_DealDataAuthStatus(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: ¥¶¿ÌÕ—ª˙ ˝æ›»œ÷§Ω·π˚
   »Îø⁄≤Œ ˝:
   ∑µ ªÿ ÷µ:
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢: sxl0221
 ********************************************************************/
unsigned char  jcb_DealDataAuthStatus(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char k;
    unsigned char CardTransPredicable;

    CardTransPredicable = emvbase_avl_gettagvalue(EMVTAG_CardTransPredicable);

	EMVBase_Trace("JCB-info: CTQ(Tag9F6C) byte1: %02x\r\n", CardTransPredicable);


    if(CardTransPredicable & 0x20)  //Go online if ODA Fails
    {
        //sxlºÏ≤È «∑Ò÷ß≥÷onlineπ¶ƒ‹
        k = emvbase_avl_gettagvalue(EMVTAG_TermType) & 0x0F;

//        if(tempApp_UnionStruct->EMVTradeParam->VisaAPFlag)          //have online cap
//        {
//            k = 2;
//        }

        if((k == 1 || k == 2 || k == 4 || k == 5) && ((emvbase_avl_gettagvalue(EMVTAG_TermTransPredicable) & 0x08) != 0x08)) //online capable
        {
            //tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 2;
            tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 0x12;
        }
        else
        {
            tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 1;
        }
        EMVBase_Trace("\r\nauth status tempterminfo->JcbDRRI = %d\r\n", tempApp_UnionStruct->EMVTradeParam->JcbDRRI);
    }
    else if(CardTransPredicable & 0x10)
    {
//        EMVBase_Trace("\r\nauth tempterminfo->paywavetrackdataup = %02x\r\n", tempApp_UnionStruct->EMVTradeParam->paywavetrackdataup);

        return RLT_ERR_EMV_SWITCHINTERFACE;

#if 0
        // «∑Ò÷ß≥÷dual interface
#if (CONTACTLESSMACHINEMODEL == POSTERMINAL)             //÷’∂Àøœ∂®÷ß≥÷∆‰À˚ΩÁ√Ê∑Ω Ω
        return RLT_ERR_EMV_SWITCHINTERFACE;  //20110804

#else

        if(tempApp_UnionStruct->EMVTradeParam->paywavetrackdataup & 0x04)    //sxl?
        {
            return RLT_ERR_EMV_SWITCHINTERFACE;              //20110804
        }
        else
        {
            tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 1;
        }
#endif
#endif
    }
    else
    {
        EMVBase_Trace("\r\ntempterminfo->PaywaveDRRI1121\r\n");
        tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 1;
    }
    return RLT_EMV_OK;
}

/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char paywave_DynSignVerify(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: ±»∂‘∂ØÃ¨«©√˚ ˝æ›
   »Îø⁄≤Œ ˝:
   ∑µ ªÿ ÷µ:
   œ‡πÿµ˜”√:
   ±∏    ◊¢: ’˝ Ω≤‚ ‘µƒ ±∫Ú∫ÕEMV±Í◊º≤ª“ª—˘£¨ ˝æ›»œ÷§ ±÷ª“™º”…œÀÊª˙ ˝æÕ––
   –ﬁ∏ƒ–≈œ¢:
 ********************************************************************/
unsigned char jcb_DynSignVerify(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char *SDAToSign, SDAHash[20], *SDAData;    //[512]   [248]
    unsigned int index;
    EMVBASE_SIGN_DYNDATA_RECOVER *recovSDA;
    EMVBASETAGCVLITEM *item;
    unsigned char SignDynAppDataLen;
    unsigned char *SignDynAppData;
    unsigned char ICCPKExpexist, qPBOCUndfinedParamexist;
    unsigned char UnpredictNum[4];
    unsigned char AmtAuthNum[6];
    unsigned char TransCurcyCode[2];
    unsigned char DDAVersionType = 0;
    unsigned char CardUnpredictNum[20];

#ifdef EMVB_DEBUG
    unsigned char i;
    unsigned int j;
    i = 0;
#endif


    SignDynAppDataLen = 0;

    item = emvbase_avl_gettagitempointer(EMVTAG_SignDynAppData);

    if(item != NULL)
    {
        SignDynAppDataLen = item->len;
        SignDynAppData = item->data;

        if(SignDynAppData == NULL)
        {
            return RLT_EMV_ERR;
        }
    }
    else
    {
        return RLT_EMV_ERR;
    }

    if(SignDynAppDataLen != tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen)
    {
        return RLT_EMV_ERR;
    }
    ICCPKExpexist = 0;
    item = emvbase_avl_gettagitempointer(EMVTAG_ICCPKExp);

    if(item != NULL)
    {
        if(item->len)
        {
            ICCPKExpexist = 1;
        }
    }

    if(ICCPKExpexist == 0)
    {
        return RLT_EMV_ERR;
    }
	EMVBase_TraceHex("ICCPKModul", tempApp_UnionStruct->EMVTradeParam->ICCPKModul, tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen);
	EMVBase_TraceHex("CardInfo.ICCPKExp",item->data, item->len);
    emvbase_avl_printtagallvalue("SignDynAppData: ", EMVTAG_SignDynAppData);

    SDAData = (unsigned char *)emvbase_malloc(248);


    EMVBase_RSARecover(tempApp_UnionStruct->EMVTradeParam->ICCPKModul, tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen, item->data, item->len, SignDynAppData, SDAData);

	EMVBase_TraceHex("Recovered SDAD:", SDAData, tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen);


    recovSDA = (EMVBASE_SIGN_DYNDATA_RECOVER *)emvbase_malloc(sizeof(EMVBASE_SIGN_DYNDATA_RECOVER));

    if(EMVBase_FillSDA(SDAData, recovSDA, tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen) != RLT_EMV_OK)
    {
        EMVBase_Trace("\r\npaywave_DynSignVerify emv_fillsda fail\r\n");
        emvbase_free(SDAData);
        emvbase_free(recovSDA);
        return RLT_EMV_ERR;
    }

    if(recovSDA->DataTrail != 0xBC)             //If it is not °ÆBC°Ø, dynamic data authentication has failed.
    {
        EMVBase_Trace("\r\npaywave_DynSignVerify BC fail\r\n");
        emvbase_free(recovSDA);
        emvbase_free(SDAData);
        return RLT_EMV_ERR;
    }

    if(recovSDA->DataHead != 0x6A)              //If it is not °Æ6A°Ø, dynamic data authentication has failed.
    {
        EMVBase_Trace("\r\npaywave_DynSignVerify 6A fail\r\n");
        emvbase_free(recovSDA);
        emvbase_free(SDAData);
        return RLT_EMV_ERR;
    }

    //¡™ª˙∑µªÿµƒ «95
    if(recovSDA->DataFormat != 0x05)    //If it is not °Æ05°Ø, dynamic data authentication has failed.
    {
        EMVBase_Trace("\r\npaywave_DynSignVerify 05 fail\r\n");
        emvbase_free(recovSDA);
        emvbase_free(SDAData);
        return RLT_EMV_ERR;
    }
    index = 0;
    SDAToSign = (unsigned char *)emvbase_malloc(512);
    memcpy(SDAToSign, (unsigned char*)&SDAData[1], tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen - 22);
    index += tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen - 22;

    emvbase_free(SDAData);


    //VISA AP
#if 0

    if(tempApp_UnionStruct->EMVTradeParam->VisaAPFlag)
    {
        memset(CardUnpredictNum, 0, sizeof(CardUnpredictNum));

        //tag 9F 69 exist
        if(emvbase_avl_gettagvalue_spec(EMVTAG_PaywaveCardAuthData, CardUnpredictNum, 0, 5) == 0)
        {
            if(CardUnpredictNum[0] == 0x01)
            {
                DDAVersionType = 0x01;
            }
        }

        if(DDAVersionType == 0)
        {
            memset(UnpredictNum, 0, sizeof(UnpredictNum));
            emvbase_avl_gettagvalue_spec(EMVTAG_ATC, UnpredictNum, 0, 2);
            memcpy((unsigned char*)&SDAToSign[index], UnpredictNum, 2);
            index += 4;

            memset(UnpredictNum, 0, sizeof(UnpredictNum));
            emvbase_avl_gettagvalue_spec(EMVTAG_UnpredictNum, UnpredictNum, 0, 4);
            memcpy((unsigned char*)&SDAToSign[index], UnpredictNum, 4);
            index += 4;
        }
        else
        {
            memset(UnpredictNum, 0, sizeof(UnpredictNum));
            emvbase_avl_gettagvalue_spec(EMVTAG_ATC, UnpredictNum, 0, 2);
            memcpy((unsigned char*)&SDAToSign[index], UnpredictNum, 2);
            index += 4;

            memset(UnpredictNum, 0, sizeof(UnpredictNum));
            emvbase_avl_gettagvalue_spec(EMVTAG_UnpredictNum, UnpredictNum, 0, 4);
            memcpy((unsigned char*)&SDAToSign[index], UnpredictNum, 4);
            index += 4;

#ifdef EMVB_DEBUG
            EMVBase_Trace("\r\nUnpredictNum: ");

            for(i = 0; i < 4; i++)
            {
                EMVBase_Trace("%02X", UnpredictNum[i]);
            }

#endif

            memset(AmtAuthNum, 0, sizeof(AmtAuthNum));
            emvbase_avl_gettagvalue_spec(EMVTAG_AmtAuthNum, AmtAuthNum, 0, 6);
            memcpy((unsigned char*)&SDAToSign[index], AmtAuthNum, 6);
            index += 6;

#ifdef EMVB_DEBUG
            EMVBase_Trace("\r\nAmtAuthNum: ");

            for(i = 0; i < 6; i++)
            {
                EMVBase_Trace("%02X", AmtAuthNum[i]);
            }

#endif

            memset(TransCurcyCode, 0, sizeof(TransCurcyCode));
            emvbase_avl_gettagvalue_spec(EMVTAG_TransCurcyCode, TransCurcyCode, 0, 2);
            memcpy((unsigned char*)&SDAToSign[index], TransCurcyCode, 2);
            index += 2;

#ifdef EMVB_DEBUG
            EMVBase_Trace("\r\nTransCurcyCode: ");

            for(i = 0; i < 2; i++)
            {
                EMVBase_Trace("%02X", TransCurcyCode[i]);
            }

#endif

            memcpy((unsigned char*)&SDAToSign[index], CardUnpredictNum, 4);
            index += 4;
        }
    }
    else
#endif
    {
        memset(CardUnpredictNum, 0, sizeof(CardUnpredictNum));

        //tag 9F 69 exist
        if(emvbase_avl_gettagvalue_spec(EMVTAG_CardAuthData, CardUnpredictNum, 0, 5) == 0)
        {
            if(CardUnpredictNum[0] == 0x01)
            {
                DDAVersionType = 0x01;
            }
        }
        EMVBase_Trace("\r\nDDAVersionType = %d\r\n", DDAVersionType);

        if(DDAVersionType == 0)
        {
            //memset(UnpredictNum,0,sizeof(UnpredictNum));
            //emvbase_avl_gettagvalue_spec(EMVTAG_ATC,UnpredictNum,0,2);
            //memcpy((unsigned char*)&SDAToSign[index],UnpredictNum,2);
            //index+=4;

            memset(UnpredictNum, 0, sizeof(UnpredictNum));
            emvbase_avl_gettagvalue_spec(EMVTAG_UnpredictNum, UnpredictNum, 0, 4);
            memcpy((unsigned char*)&SDAToSign[index], UnpredictNum, 4);
            index += 4;
        }
        else
        {
            memset(UnpredictNum, 0, sizeof(UnpredictNum));
            emvbase_avl_gettagvalue_spec(EMVTAG_UnpredictNum, UnpredictNum, 0, 4);
            memcpy((unsigned char*)&SDAToSign[index], UnpredictNum, 4);
            index += 4;


			EMVBase_TraceHex("UnpredictNum:", UnpredictNum, 4);

            memset(AmtAuthNum, 0, sizeof(AmtAuthNum));
            emvbase_avl_gettagvalue_spec(EMVTAG_AmtAuthNum, AmtAuthNum, 0, 6);
            memcpy((unsigned char*)&SDAToSign[index], AmtAuthNum, 6);
            index += 6;

			EMVBase_TraceHex("AmtAuthNum:", AmtAuthNum, 6);

            memset(TransCurcyCode, 0, sizeof(TransCurcyCode));
            emvbase_avl_gettagvalue_spec(EMVTAG_TransCurcyCode, TransCurcyCode, 0, 2);
            memcpy((unsigned char*)&SDAToSign[index], TransCurcyCode, 2);
            index += 2;

			EMVBase_TraceHex("TransCurcyCode:", TransCurcyCode, 2);

        }
    }
    qPBOCUndfinedParamexist = 0;
    item = emvbase_avl_gettagitempointer(EMVTAG_CardAuthData);

    if(item != NULL)
    {
        if(item->len)
        {
            qPBOCUndfinedParamexist = 1;
        }
    }

    if(qPBOCUndfinedParamexist == 0)
    {
        EMVBase_Trace("\r\npaywave_DynSignVerify MV_UNPREDICTUDOL fail\r\n");
        emvbase_free(recovSDA);
        emvbase_free(SDAToSign);
        return RLT_EMV_ERR;
    }
    EMVBase_Trace("\r\ntempcardinfo->qPBOCUndfinedParam[0] = %d %d\r\n", item->data[0], item->len);

    if((item->data[0] != 0x01) || (item->len < 5) || (item->len > 16))
    {
        EMVBase_Trace("\r\npaywave_DynSignVerify qPBOCUndfinedParam 01 fail\r\n");
        emvbase_free(recovSDA);
        emvbase_free(SDAToSign);
        return RLT_EMV_ERR;
    }
    //SDAToSign[index++] = tempcardinfo->qPBOCUndefinedParamLen;

    memcpy((unsigned char*)&SDAToSign[index], item->data, item->len);
    index += item->len;

	EMVBase_TraceHex("AmtAuthNum:", item->data, item->len);
	EMVBase_TraceHex("SDAToSign:", SDAToSign, index);

    EMVBase_Hash(SDAToSign, index, SDAHash);

    emvbase_free(SDAToSign);

    if(recovSDA->HashInd == 0x01)  //SHA-1 algorithm
    {
		EMVBase_TraceHex("hash result", recovSDA->HashResult, 20);
		EMVBase_TraceHex("SDAHash", SDAHash, 20);

        if(memcmp(recovSDA->HashResult, SDAHash, 20))
        {
            EMVBase_Trace("\r\nhashresulterr111\r\n");
            emvbase_free(recovSDA);
            return RLT_EMV_ERR;
        }
    }
    else
    {
        EMVBase_Trace("\r\nhashresulterr112\r\n");
        emvbase_free(recovSDA);
        return RLT_EMV_ERR;
    }
    emvbase_avl_createsettagvalue(EMVTAG_ICCDynNum, &recovSDA->ICCDynData[1], recovSDA->ICCDynData[0]);

    emvbase_free(recovSDA);
    return RLT_EMV_OK;
}


unsigned char jcb_CheckDataMissDDA(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char i;
    i = 0;

    while(1)
    {
        if(memcmp(JcbDDAmandatoryTag[i].Tag, "\x00\x00\x00", 3) == 0)
        {
            break;
        }

        if(emvbase_avl_checkiftagexist((unsigned char *)JcbDDAmandatoryTag[i].Tag) == 0)
        {
        	EMVBase_Trace("JCB-info: Tag%02x%02x missing \r\n", JcbDDAmandatoryTag[i].Tag[0], JcbDDAmandatoryTag[i].Tag[1]);
            return RLT_ERR_EMV_IccDataMissing;
        }
        i++;
    }

    if(tempApp_UnionStruct->EMVTradeParam->bErrSDATL)
    {
    	EMVBase_Trace("JCB-info: SDA Tag List(Tag9F4A) error(len is not equal to 1 or value is not equal to '82')\r\n");
        return RLT_ERR_EMV_IccDataMissing;
    }

    return RLT_EMV_OK;
}

unsigned char jcb_GetCAPK(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    return RLT_EMV_ERR;
}

unsigned char jcb_GetIPK(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char *IPKData, IPKHash[20];
    unsigned char buf[9], bytePAN[8], byteIssuID[8];
    EMVBASE_IPK_RECOVER recovIPK;
    EMVBASETAGCVLITEM *item;
    unsigned char IPKCertexist;
    unsigned char IPKRemexist;
    unsigned char IPKExpexist;
    unsigned char PAN[10];
    unsigned short PANLen;
    unsigned char *IPKRem = NULL;
    int i, index;
    EMVBASE_CAPK_STRUCT *tempcapk;
    unsigned char tempcapkModulLen;
    int ret;
    unsigned int ipkdatalen;


	EMVBase_Trace("JCB-info: Recover Issuer Public Key\r\n");

    tempcapk = tempApp_UnionStruct->EMVTradeParam->CAPK;

    tempcapkModulLen = tempcapk->ModulLen;

    IPKCertexist = 0;

    ipkdatalen = 0;
    item = emvbase_avl_gettagitempointer(EMVTAG_IPKRem);

    if(item != NULL)
    {
        if(item->len)
        {
            ipkdatalen += (item->len);
        }
    }


    item = emvbase_avl_gettagitempointer(EMVTAG_IPKCert);

    if(item != NULL)
    {
        if(item->len)
        {
            IPKCertexist = 1;
        }
    }

    if(IPKCertexist == 0)
    {
    	EMVBase_Trace("JCB-info: Issuer Public Key Certificate(Tag90) missing\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }

    if(item->len != tempcapk->ModulLen)
    {
    	EMVBase_Trace("JCB-info: CA public key len: %d\r\n", tempcapk->ModulLen);
		EMVBase_Trace("JCB-info: Issuer Public Key Certificate(Tag90) value len: %d\r\n", item->len);
		EMVBase_Trace("JCB-info: different len\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }

    ipkdatalen += (item->len);
    ipkdatalen += 3;
    IPKData = (unsigned char *)emvbase_malloc(ipkdatalen);
    if(IPKData == NULL)
    {
    	EMVBase_Trace("JCB-info: memory error\r\n");
        return RLT_EMV_ERR;
    }

    memset(IPKData, 0, ipkdatalen);

    EMVBase_RSARecover(tempcapk->Modul, tempcapk->ModulLen, tempcapk->Exponent, tempcapk->ExponentLen, item->data, IPKData);
    EMVBase_FillIPK(IPKData, &recovIPK, tempcapk);

    if(recovIPK.DataTrail != 0xBC)
    {
    	EMVBase_Trace("JCB-info: Data Trailer != 0xBC\r\n");
        emvbase_free(IPKData);
        return RLT_ERR_EMV_IccDataFormat;
    }

    if(recovIPK.DataHead != 0x6A)
    {
    	EMVBase_Trace("JCB-info: Data Header != 0x6A\r\n");
        emvbase_free(IPKData);
        return RLT_ERR_EMV_IccDataFormat;
    }

    if(recovIPK.CertFormat != 0x02)
    {
    	EMVBase_Trace("JCB-info: Certificate Format != 0x02\r\n");
        emvbase_free(IPKData);
        return RLT_ERR_EMV_IccDataFormat;
    }
    //sign all the correspoding variables to the IPK
    index = 0;

    for(i = 0; i < tempcapk->ModulLen - 22; i++)
    {
        IPKData[i] = IPKData[i + 1];
    }

    index += tempcapk->ModulLen - 22;

    if(recovIPK.IPKLen > tempcapk->ModulLen - 36)
    {
        IPKRemexist = 0;
        item = emvbase_avl_gettagitempointer(EMVTAG_IPKRem);

        if(item != NULL)
        {
            if(item->len)
            {
                IPKRemexist = 1;
            }
        }

        if(IPKRemexist == 0)
        {
        	EMVBase_Trace("JCB-info: need Issuer Public Key Remainder(Tag92) but missing\r\n");
            emvbase_free(IPKData);
            emvbase_avl_setvalue_or(EMVTAG_TVR, 0, 0x20); //ICC data missing
            return RLT_ERR_EMV_IccDataMissing;
        }
        IPKRem = (unsigned char *)emvbase_malloc(item->len);
        if(IPKRem == NULL)
        {
        	EMVBase_Trace("JCB-info: memory error\r\n");
            emvbase_free(IPKData);
            return RLT_EMV_ERR;
        }
        memcpy(IPKRem, item->data, item->len);
        memcpy((unsigned char*)&IPKData[index], (unsigned char*)item->data, item->len);
        index += item->len;
    }
    IPKExpexist = 0;
    item = emvbase_avl_gettagitempointer(EMVTAG_IPKExp);

    if(item != NULL)
    {
        if(item->len)
        {
            IPKExpexist = 1;
        }
    }

    if(IPKExpexist)
    {
        memcpy((unsigned char*)&IPKData[index], (unsigned char*)item->data, item->len);
        index += item->len;
    }
    memset(IPKHash, 0, 20);

    EMVBase_Hash(IPKData, index, IPKHash);

    emvbase_free(IPKData);

    if(recovIPK.HashInd == 0x01)
    {
        if(memcmp(recovIPK.HashResult, IPKHash, 20))
        {
        	EMVBase_Trace("JCB-info: Hash Result verification fails\r\n");
            if(IPKRem)
            {
                emvbase_free(IPKRem);
            }
            return RLT_ERR_EMV_IccDataFormat;
        }
    }
    else
    {
    	EMVBase_Trace("JCB-info: Hash Algorithm Indicator != 0x01\r\n");
        if(IPKRem)
        {
            emvbase_free(IPKRem);
        }
        return RLT_ERR_EMV_IccDataFormat;
    }
    memset(PAN, 0, sizeof(PAN));
    emvbase_avl_gettagvalue_all(EMVTAG_PAN, PAN, &PANLen);

    //verify if leftmost 3-8 digits of PAN matches IssuID
    for(i = 0; i < 4; i++)
    {
        bytePAN[2 * i] = (PAN[i] & 0xF0) >> 4;
        bytePAN[2 * i + 1] = PAN[i] & 0x0F;
        byteIssuID[2 * i] = (recovIPK.IssuID[i] & 0xF0) >> 4;
        byteIssuID[2 * i + 1] = recovIPK.IssuID[i] & 0x0F;
    }

    for(i = 7; i >= 2; i--)
    {
        if(byteIssuID[i] != 0x0F)
        {
            if(memcmp(byteIssuID, bytePAN, i + 1))
            {
            	EMVBase_TraceHex("JCB-info: 3-8 digits of PAN: ", bytePAN, i + 1);
				EMVBase_TraceHex("JCB-info: Issuer Identifier: ", byteIssuID, i + 1);
                if(IPKRem)
                {
                	EMVBase_Trace("JCB-info: no match\r\n");
                    emvbase_free(IPKRem);
                }
                return RLT_ERR_EMV_IccDataFormat;
            }
            else
            {
                break;
            }
        }
    }

    if(i < 2)
    {
    	EMVBase_Trace("JCB-info: Issuer Identifier data error\r\n");
        if(IPKRem)
        {
            emvbase_free(IPKRem);
        }
        return RLT_ERR_EMV_IccDataFormat;
    }

    //verify if expiredate is later than current date
    if(EMVBase_ExpireDateVerify(recovIPK.ExpireDate) != RLT_EMV_OK)
    {
    	EMVBase_Trace("JCB-info: Expired\r\n");
        if(IPKRem)
        {
            emvbase_free(IPKRem);
        }
        return RLT_EMV_ERR;
    }
    //check if IPK is revoked.
    memcpy(buf, tempcapk->RID, 5);
    buf[5] = tempcapk->CAPKI;
    memcpy((unsigned char*)&buf[6], recovIPK.CertSerial, 3);

    emvbase_free(tempApp_UnionStruct->EMVTradeParam->CAPK);
    tempApp_UnionStruct->EMVTradeParam->CAPK = NULL;

    if(tempApp_UnionStruct->termipkrevokecheck != NULL)
    {
        ret = tempApp_UnionStruct->termipkrevokecheck(buf);//∑µªÿSDK_OK±Ì æ≤ª «ªÿ ’π´‘ø
        if(ret != 1)
        {
        	EMVBase_Trace("JCB-info: Issuer Public Key revoke\r\n");
            if(IPKRem)
            {
                emvbase_free(IPKRem);
            }
            return RLT_EMV_ERR;
        }
    }

    if(recovIPK.IPKAlgoInd != 0x01)  //other than '01' is not recognised.
    {
    	EMVBase_Trace("JCB-info: Issuer Public Key Algorithm Indicator != 0x01\r\n");
        if(IPKRem)
        {
            emvbase_free(IPKRem);
        }
        return RLT_ERR_EMV_IccDataFormat;
    }

    tempApp_UnionStruct->EMVTradeParam->IPKModulLen = recovIPK.IPKLen;

    if(tempApp_UnionStruct->EMVTradeParam->IPKModul == NULL)
    {
        tempApp_UnionStruct->EMVTradeParam->IPKModul = (unsigned char *)emvbase_malloc(recovIPK.IPKLen);
        if(tempApp_UnionStruct->EMVTradeParam->IPKModul == NULL)
        {
        	EMVBase_Trace("JCB-info: memory error\r\n");
            if(IPKRem)
            {
                emvbase_free(IPKRem);
            }
            return RLT_EMV_ERR;
        }
        memset(tempApp_UnionStruct->EMVTradeParam->IPKModul,0,recovIPK.IPKLen);

    }

    if(recovIPK.IPKLen <= tempcapkModulLen - 36)
    {
        memcpy(tempApp_UnionStruct->EMVTradeParam->IPKModul, recovIPK.IPKLeft, recovIPK.IPKLen);
    }
    else
    {
        memcpy(tempApp_UnionStruct->EMVTradeParam->IPKModul, recovIPK.IPKLeft, tempcapkModulLen - 36);
        memcpy((unsigned char*)&tempApp_UnionStruct->EMVTradeParam->IPKModul[tempcapkModulLen - 36], IPKRem, (recovIPK.IPKLen + 36) - tempcapkModulLen);
    }
    if(IPKRem)
    {
        emvbase_free(IPKRem);
    }

    return RLT_EMV_OK;
}

unsigned char jcb_GetICCPK(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char *ICCPKData, ICCPKHash[20];
    unsigned short index;
    EMVBASE_ICCPK_RECOVER recovICCPK;
    EMVBASETAGCVLITEM *item, *item1;
    unsigned char *ICCPKToSign;
    unsigned char testlen;
    unsigned char tmppan1[20], tmppan1len, tmppan2[20], tmppan2len;
    unsigned char IPKExpexist, ICCPKCertexist, ICCPKRemexist, PANexist;
    unsigned char *ICCPKRem = NULL;
    unsigned char CardIdentifyInfo;
    unsigned int ICCPKToSignLen = 0;


	EMVBase_Trace("JCB-info: Recover ICC Public Key\r\n");

	if(tempApp_UnionStruct->EMVTradeParam->AuthDataLen)
	{
		if(tempApp_UnionStruct->EMVTradeParam->AuthData == NULL)
		{
			EMVBase_Trace("EmvTradeParam->AuthData is freed\r\n");
			return RLT_EMV_ERR;
		}
	}

    ICCPKCertexist = 0;
    item = emvbase_avl_gettagitempointer(EMVTAG_ICCPKCert);

    if(item != NULL)
    {
        if(item->len)
        {
            ICCPKCertexist = 1;
        }
    }

    if(ICCPKCertexist == 0)
    {
    	EMVBase_Trace("JCB-info: ICC Public Key Certificate(Tag9F46) missing\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }

    if(item->len != tempApp_UnionStruct->EMVTradeParam->IPKModulLen)
    {
    	EMVBase_Trace("JCB-info: Issuer public key len: %d\r\n", tempApp_UnionStruct->EMVTradeParam->IPKModulLen);
		EMVBase_Trace("JCB-info: ICC Public Key Certificate(Tag9F46) value len: %d\r\n", item->len);
		EMVBase_Trace("JCB-info: different len\r\n");
        return RLT_EMV_ERR;
    }

    IPKExpexist = 0;
    item = emvbase_avl_gettagitempointer(EMVTAG_IPKExp);

    if(item != NULL)
    {
        if(item->len)
        {
            IPKExpexist = 1;
        }
    }

    if(IPKExpexist == 0)
    {
    	EMVBase_Trace("JCB-info: Issuer Public Key Exponent(Tag9F32) missing\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }
    ICCPKCertexist = 0;
    item1 = emvbase_avl_gettagitempointer(EMVTAG_ICCPKCert);

    if(item1 != NULL)
    {
        if(item1->len)
        {
            ICCPKCertexist = 1;
        }
    }

    if(ICCPKCertexist == 0)
    {
    	EMVBase_Trace("JCB-info: ICC Public Key Certificate(Tag9F46) missing\r\n");
        return RLT_ERR_EMV_IccDataFormat;
    }

    ICCPKData = (unsigned char *)emvbase_malloc(tempApp_UnionStruct->EMVTradeParam->IPKModulLen);
    if(ICCPKData == NULL)
    {
    	EMVBase_Trace("JCB-info: memory error\r\n");
        return RLT_EMV_ERR;
    }
    memset(ICCPKData,0,tempApp_UnionStruct->EMVTradeParam->IPKModulLen);

    EMVBase_RSARecover(tempApp_UnionStruct->EMVTradeParam->IPKModul, tempApp_UnionStruct->EMVTradeParam->IPKModulLen, item->data, item->len, item1->data, ICCPKData);

    emvbase_free(tempApp_UnionStruct->EMVTradeParam->IPKModul);
    tempApp_UnionStruct->EMVTradeParam->IPKModul = NULL;

    EMVBase_FillICCPK(ICCPKData, tempApp_UnionStruct->EMVTradeParam->IPKModulLen, &recovICCPK);

    if(recovICCPK.DataTrail != 0xBC)
    {
    	EMVBase_Trace("JCB-info: Data Trailer != 0xBC\r\n");
        emvbase_free(ICCPKData);
        return RLT_EMV_ERR;
    }

    if(recovICCPK.DataHead != 0x6A)
    {
    	EMVBase_Trace("JCB-info: Data Header != 0x6A\r\n");
        emvbase_free(ICCPKData);
        return RLT_EMV_ERR;
    }

    if(recovICCPK.CertFormat != 0x04)
    {
    	EMVBase_Trace("JCB-info: Certificate Format != 0x04\r\n");
        emvbase_free(ICCPKData);
        return RLT_EMV_ERR;
    }
    index = 0;


    ICCPKToSignLen = tempApp_UnionStruct->EMVTradeParam->IPKModulLen - 22;
    ICCPKToSignLen += 3;        //EMVTAG_ICCPKExp
    ICCPKToSignLen += tempApp_UnionStruct->EMVTradeParam->AuthDataLen;

    if(recovICCPK.ICCPKLen > (tempApp_UnionStruct->EMVTradeParam->IPKModulLen - 42))
    {
        ICCPKRemexist = 0;
        item1 = emvbase_avl_gettagitempointer(EMVTAG_ICCPKRem);

        if(item1 != NULL)
        {
            if(item1->len)
            {
                ICCPKRemexist = 1;
            }
        }

        if(ICCPKRemexist == 0)
        {
        	EMVBase_Trace("JCB-info: need ICC Public Key Remainder(Tag9F48) but missing\r\n");
            emvbase_free(ICCPKData);
            emvbase_avl_setvalue_or(EMVTAG_TVR, 0, 0x20);           //ICC data missing
            return RLT_ERR_EMV_IccDataMissing;
        }

        ICCPKRem = (unsigned char *)emvbase_malloc(item1->len);
        if(ICCPKRem == NULL)
        {
        	EMVBase_Trace("JCB-info: memory error\r\n");
            emvbase_free(ICCPKData);
            return RLT_EMV_ERR;
        }

        ICCPKToSignLen += item1->len;
        ICCPKToSign = (unsigned char *)emvbase_malloc(ICCPKToSignLen);

        if(ICCPKToSign == NULL)
        {
        	EMVBase_Trace("JCB-info: memory error\r\n");
            emvbase_free(ICCPKData);
            emvbase_free(ICCPKRem);
            return RLT_EMV_ERR;
        }
        memcpy(ICCPKToSign, (unsigned char*)&ICCPKData[1], tempApp_UnionStruct->EMVTradeParam->IPKModulLen - 22);
        index += tempApp_UnionStruct->EMVTradeParam->IPKModulLen - 22;

        memcpy(ICCPKRem, item1->data, item1->len);
        memcpy((unsigned char*)&ICCPKToSign[index], (unsigned char*)item1->data, item1->len);
        index += item1->len;
    }
    else
    {
        ICCPKToSign = (unsigned char *)emvbase_malloc(ICCPKToSignLen);
        if(ICCPKToSign == NULL)
        {
        	EMVBase_Trace("JCB-info: memory error\r\n");
            emvbase_free(ICCPKData);
            return RLT_EMV_ERR;
        }

        memcpy(ICCPKToSign, (unsigned char*)&ICCPKData[1], tempApp_UnionStruct->EMVTradeParam->IPKModulLen - 22);
        index += tempApp_UnionStruct->EMVTradeParam->IPKModulLen - 22;

    }

    emvbase_free(ICCPKData);

    ICCPKCertexist = 0;
    item = emvbase_avl_gettagitempointer(EMVTAG_ICCPKExp);

    if(item != NULL)
    {
        if(item->len)
        {
            ICCPKCertexist = 1;
        }
    }

    if(ICCPKCertexist == 0)
    {
    	EMVBase_Trace("JCB-info: ICC Public Key Exponent(Tag9F47) missing\r\n");
        emvbase_free(ICCPKToSign);
        if(ICCPKRem)
        {
            emvbase_free(ICCPKRem);
        }
        return RLT_ERR_EMV_IccDataFormat;
    }
    memcpy((unsigned char*)&ICCPKToSign[index], (unsigned char*)item->data, item->len);
    index += item->len;

	if(tempApp_UnionStruct->EMVTradeParam->AuthData)
    {
        memcpy((unsigned char*)&ICCPKToSign[index], tempApp_UnionStruct->EMVTradeParam->AuthData, tempApp_UnionStruct->EMVTradeParam->AuthDataLen);
	    index += tempApp_UnionStruct->EMVTradeParam->AuthDataLen;

	    emvbase_free(tempApp_UnionStruct->EMVTradeParam->AuthData);
	    tempApp_UnionStruct->EMVTradeParam->AuthData = NULL;
    }


    EMVBase_Hash(ICCPKToSign, index, ICCPKHash);

    emvbase_free(ICCPKToSign);


    if(recovICCPK.HashInd == 0x01)  //SHA-1 algorithm
    {
        if(memcmp(recovICCPK.HashResult, ICCPKHash, 20))
        {
        	EMVBase_Trace("JCB-info: Hash Result verification fails\r\n");
            if(ICCPKRem)
            {
                emvbase_free(ICCPKRem);
            }
            return RLT_EMV_ERR;
        }
    }
    else
    {
    	EMVBase_Trace("JCB-info: Hash Algorithm Indicator != 0x01\r\n");
        if(ICCPKRem)
        {
            emvbase_free(ICCPKRem);
        }
        return RLT_EMV_ERR;
    }
    PANexist = 0;
    item1 = emvbase_avl_gettagitempointer(EMVTAG_PAN);

    if(item1 != NULL)
    {
        if(item1->len)
        {
            PANexist = 1;
        }
    }

    if(PANexist == 0)
    {
    	EMVBase_Trace("JCB-info: Track2(Tag57) missing\r\n");
        if(ICCPKRem)
        {
            emvbase_free(ICCPKRem);
        }
        return RLT_ERR_EMV_IccDataFormat;
    }

    memset(tmppan1, 0, sizeof(tmppan1));
    EMVBaseBcdToAsc(tmppan1, recovICCPK.AppPAN, 10);
    tmppan1len = 20;

    for(testlen = 0; testlen < 20; testlen++)
    {
        if((tmppan1[testlen] == 'F') || (tmppan1[testlen] == 'f'))
        {
            tmppan1len = testlen;
            break;
        }
    }

    memset(tmppan2, 0, sizeof(tmppan2));
    EMVBaseBcdToAsc(tmppan2, item1->data, item1->len);
    tmppan2len = (item1->len) << 1;

    for(testlen = 0; testlen < tmppan2len; testlen++)
    {
        if((tmppan1[testlen] == 'F') || (tmppan2[testlen] == 'f'))
        {
            tmppan2len = testlen;
            break;
        }
    }

	EMVBase_Trace("JCB-info: PAN in Track2(Tag57): %s\r\n", tmppan1);
	EMVBase_Trace("JCB-info: APP PAN of ICC Public Key: %s\r\n", tmppan2);
    if(tmppan1len != tmppan2len)
    {
    	EMVBase_Trace("JCB-info: PAN different\r\n");
        if(ICCPKRem)
        {
            emvbase_free(ICCPKRem);
        }
        return RLT_EMV_ERR;
    }

    if(memcmp(tmppan1, tmppan2, tmppan1len))
    {
    	EMVBase_Trace("JCB-info: PAN different\r\n");
        if(ICCPKRem)
        {
            emvbase_free(ICCPKRem);
        }
        return RLT_EMV_ERR;
    }

    //verify expiredate is later than current date
    if(EMVBase_ExpireDateVerify(recovICCPK.ExpireDate) == RLT_EMV_ERR)
    {
    	EMVBase_Trace("JCB-info: Expired\r\n");
        if(ICCPKRem)
        {
            emvbase_free(ICCPKRem);
        }
        return RLT_EMV_ERR;
    }

    if(recovICCPK.ICCPKAlgoInd != 0x01)  //other than '01' is not recognised.
    {
    	EMVBase_Trace("JCB-info: ICC Public Key Algorithm Indicator != 0x01\r\n");
        if(ICCPKRem)
        {
            emvbase_free(ICCPKRem);
        }
        return RLT_EMV_ERR;
    }
    tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen = recovICCPK.ICCPKLen;

    if(tempApp_UnionStruct->EMVTradeParam->ICCPKModul == NULL)
    {
        tempApp_UnionStruct->EMVTradeParam->ICCPKModul = (unsigned char  *)emvbase_malloc(recovICCPK.ICCPKLen);

    }

    if(tempApp_UnionStruct->EMVTradeParam->ICCPKModul == NULL)
    {
    	EMVBase_Trace("JCB-info: memory error\r\n");
        if(ICCPKRem)
        {
            emvbase_free(ICCPKRem);
        }
        return RLT_EMV_ERR;
    }

    memset(tempApp_UnionStruct->EMVTradeParam->ICCPKModul,0,recovICCPK.ICCPKLen);

    if(recovICCPK.ICCPKLen <= (tempApp_UnionStruct->EMVTradeParam->IPKModulLen - 42))
    {
        memcpy(tempApp_UnionStruct->EMVTradeParam->ICCPKModul, recovICCPK.ICCPKLeft, recovICCPK.ICCPKLen);
    }
    else
    {
        memcpy(tempApp_UnionStruct->EMVTradeParam->ICCPKModul, recovICCPK.ICCPKLeft, tempApp_UnionStruct->EMVTradeParam->IPKModulLen - 42);
        memcpy((unsigned char*)&tempApp_UnionStruct->EMVTradeParam->ICCPKModul[tempApp_UnionStruct->EMVTradeParam->IPKModulLen - 42], ICCPKRem, recovICCPK.ICCPKLen - tempApp_UnionStruct->EMVTradeParam->IPKModulLen + 42);
    }

    if(ICCPKRem)
    {
        emvbase_free(ICCPKRem);
    }
    return RLT_EMV_OK;
}

unsigned char jcb_CombineDDA_FirstHalf(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char ret;

    EMVBASE_CAPK_STRUCT *tempcapk;

    tempcapk = tempApp_UnionStruct->EMVTradeParam->CAPK;

    if(jcb_CheckDataMissDDA(tempApp_UnionStruct)!=RLT_EMV_OK)
    {
        emvbase_avl_setvalue_or(EMVTAG_TVR,0,0x20); //ICC data missing
        return RLT_ERR_EMV_IccDataMissing;
    }

    if(tempcapk->ModulLen==0)
    {
    	EMVBase_Trace("JCB-info: CAPK missing\r\n");
        if(jcb_GetCAPK(tempApp_UnionStruct)!=RLT_EMV_OK)       //retrieval of the certificate authentication public key
        {
            return RLT_EMV_ERR;
        }
    }
    if(tempApp_UnionStruct->EMVTradeParam->IPKModulLen==0)
    {
    	ret = jcb_GetIPK(tempApp_UnionStruct);
        if(ret !=RLT_EMV_OK)        //retrieval of the issuer public key modulus
        {
            return ret;
        }
    }

    if(tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen==0)
    {
    	ret = jcb_GetICCPK(tempApp_UnionStruct);
        if(ret !=RLT_EMV_OK)          //retrieval of the ICC public key
        {
            return ret;
        }
    }

    return RLT_EMV_OK;
}


unsigned char jcb_AacRefuse(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char respCrpt;
    unsigned char CryptInfo = 0;

    respCrpt = emvbase_avl_gettagvalue(EMVTAG_ReferenceControlParameter);

    tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;

    CryptInfo = emvbase_avl_gettagvalue(EMVTAG_CryptInfo);

    if((CryptInfo & 0x08) == 0x08)		//Advice required
    {
        tempApp_UnionStruct->EMVTradeParam->bAdvice = 1;
    }

    if((respCrpt & 0x10) != 0)
    {
        emvbase_avl_setvalue_or(EMVTAG_TSI, 0, 0x80);
    }
    return RLT_EMV_OK;
}


void jcb_FillICCDynData(unsigned char* DynData, EMVBASE_ICC_DYN_DATA* ICCDynData)
{
    unsigned char Ldn;

    Ldn = DynData[0];
    ICCDynData->ICCDynNumLen = Ldn;
    memcpy(ICCDynData->ICCDynNum, DynData + 1, Ldn);
    ICCDynData->CryptInfo = DynData[Ldn + 1];
    memcpy(ICCDynData->AppCrypt, DynData + Ldn + 2, 8);
    memcpy(ICCDynData->HashResult, DynData + Ldn + 10, 20);
}

unsigned char jcb_CombineDDASignVerify(unsigned char type, JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char *SDAToSign, SDAHash[20], *SDAData, i = 0;
    unsigned int index;
    EMVBASE_SIGN_DYNDATA_RECOVER *recovSDA;
    EMVBASE_ICC_DYN_DATA ICCDynDataSign;
    EMVBASETAGCVLITEM *SignDynAppDataitem, *ICCPKExpitem, *DOLDataitem;
    unsigned char CryptInfo;
    unsigned char UnpredictNum[4];


	EMVBase_Trace("JCB-info: Signed Dynamic Application Data(Tag9F4B) verification\r\n");

    if((emvbase_avl_checkiftagexist(EMVTAG_CryptInfo) == 0) || (emvbase_avl_checkiftagexist(EMVTAG_SignDynAppData) == 0)
       || (emvbase_avl_checkiftagexist(EMVTAG_ATC) == 0))
    {
    	EMVBase_Trace("JCB-info: Signed Dynamic Application Data(Tag9F4B) or CID(Tag9F27) or ATC(Tag9F36) missing\r\n");
        return RLT_EMV_ERR;
    }
    memset((unsigned char*)&ICCDynDataSign, 0, sizeof(EMVBASE_ICC_DYN_DATA));

    memset(SDAHash, 0, 20);

    SignDynAppDataitem = emvbase_avl_gettagitempointer(EMVTAG_SignDynAppData);

    if(SignDynAppDataitem->len != tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen)
    {
    	EMVBase_Trace("JCB-info: ICC public key len: %d\r\n", tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen);
		EMVBase_Trace("JCB-info: Signed Dynamic Application Data(Tag9F4B) value len: %d\r\n", SignDynAppDataitem->len);
		EMVBase_Trace("JCB-info: different len\r\n");
        return RLT_EMV_ERR;
    }
    ICCPKExpitem = emvbase_avl_gettagitempointer(EMVTAG_ICCPKExp);

    SDAData = (unsigned char *)emvbase_malloc(248);
    EMVBase_RSARecover(tempApp_UnionStruct->EMVTradeParam->ICCPKModul, tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen, ICCPKExpitem->data, ICCPKExpitem->len, SignDynAppDataitem->data, SDAData);

    recovSDA = (EMVBASE_SIGN_DYNDATA_RECOVER *)emvbase_malloc(sizeof(EMVBASE_SIGN_DYNDATA_RECOVER));


    if(EMVBase_FillSDA(SDAData, recovSDA, tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen) != RLT_EMV_OK)
    {
    	EMVBase_Trace("JCB-info: Format of Data Recovered error\r\n");
        emvbase_free(recovSDA);
        emvbase_free(SDAData);
        return RLT_EMV_ERR;
    }

    if(recovSDA->DataTrail != 0xBC)
    {
    	EMVBase_Trace("JCB-info: Data Trailer != 0xBC\r\n");
        emvbase_free(recovSDA);
        emvbase_free(SDAData);
        return RLT_EMV_ERR;
    }

    if(recovSDA->DataHead != 0x6A)
    {
    	EMVBase_Trace("JCB-info: Data Header != 0x6A\r\n");
        emvbase_free(recovSDA);
        emvbase_free(SDAData);
        return RLT_EMV_ERR;
    }

    if(recovSDA->DataFormat != 0x05)
    {
    	EMVBase_Trace("JCB-info: Certificate Format != 0x05\r\n");
        emvbase_free(recovSDA);
        emvbase_free(SDAData);
        return RLT_EMV_ERR;
    }

    jcb_FillICCDynData(recovSDA->ICCDynData, &ICCDynDataSign);

    CryptInfo = emvbase_avl_gettagvalue(EMVTAG_CryptInfo);

    if(ICCDynDataSign.CryptInfo != CryptInfo)
    {
    	EMVBase_Trace("JCB-info: CID(Tag9F27) != CID in ICC Dynamic Data(recover data)\r\n");

        if((ICCDynDataSign.CryptInfo & 0xC0) == 0x80) // ??
        {
        	EMVBase_Trace("Emvb_CombineDDASignVerify 7\r\n");
            emvbase_free(recovSDA);
            emvbase_free(SDAData);
            return RLT_EMV_ERR;
        }
        else if((ICCDynDataSign.CryptInfo & 0xC0) == 0x40) // ??
        {
        	EMVBase_Trace("Emvb_CombineDDASignVerify 8\r\n");
            emvbase_free(recovSDA);
            emvbase_free(SDAData);
            return RLT_EMV_ERR;
        }
        else
        {
        	EMVBase_Trace("Emvb_CombineDDASignVerify 9\r\n");
            emvbase_free(recovSDA);
            emvbase_free(SDAData);
            return RLT_EMV_ERR;
        }
    }

    SDAToSign = (unsigned char *)emvbase_malloc(512);
    index = 0;
    memset(SDAToSign, 0, 512);
    memcpy(SDAToSign, (unsigned char*)&SDAData[1], tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen - 22);
    index += tempApp_UnionStruct->EMVTradeParam->ICCPKModulLen - 22;

    emvbase_free(SDAData);

    if(GenerateAC2 == type)
    {
        i = tempApp_UnionStruct->EMVTradeParam->bCDOL2HasNoUnpredictNum;
    }

    if(GenerateAC1 == type)
    {
        i = tempApp_UnionStruct->EMVTradeParam->bCDOL1HasNoUnpredictNum;
    }

    if(i == 0)
    {
        emvbase_avl_gettagvalue_spec(EMVTAG_UnpredictNum, UnpredictNum, 0, 4);
        memcpy((unsigned char*)&SDAToSign[index], UnpredictNum, 4);
        index += 4;
    }
    else
    {
    }

    if(recovSDA->HashInd != 0x01)		//SHA-1 algorithm
    {
    	EMVBase_Trace("JCB-info: Hash Algorithm Indicator != 0x01\r\n");
        emvbase_free(recovSDA);
        emvbase_free(SDAToSign);
        return RLT_EMV_ERR;
    }
    EMVBase_Hash(SDAToSign, index, SDAHash);


    if(memcmp(recovSDA->HashResult, SDAHash, 20))
    {
    	EMVBase_Trace("JCB-info: Hash Result verification fails\r\n");
        emvbase_free(recovSDA);
        emvbase_free(SDAToSign);
        return RLT_EMV_ERR;
    }
    emvbase_free(recovSDA);

    memset(SDAToSign, 0, 512);
    memset(SDAHash, 0, 20);
    index = 0;


	EMVBase_Trace("JcbTornTransactionFlag: %d", tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag);
	if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag)
	{
		memcpy(SDAToSign, tempApp_UnionStruct->EMVTradeParam->JcbTornCDAHashDataBuffer, tempApp_UnionStruct->EMVTradeParam->JcbTornCDAHashDatalen);
		index += tempApp_UnionStruct->EMVTradeParam->JcbTornCDAHashDatalen;
		emvbase_avl_printtagallvalue("cda SignVerify PDOLData: ", EMVTAG_PDOLData);
		emvbase_avl_printtagallvalue("cda SignVerify CDOL1RelatedData: ", EMVTAG_CDOL1RelatedData);
		EMVBase_Trace("tempApp_UnionStruct->EMVTradeParam->JcbTornCDAHashDatalen: %d", tempApp_UnionStruct->EMVTradeParam->JcbTornCDAHashDatalen);
		EMVBase_TraceHex("tempApp_UnionStruct->EMVTradeParam->JcbTornCDAHashDataBuffer: ", tempApp_UnionStruct->EMVTradeParam->JcbTornCDAHashDataBuffer,
			tempApp_UnionStruct->EMVTradeParam->JcbTornCDAHashDatalen);
	}
	else
	{

	    DOLDataitem = emvbase_avl_gettagitempointer(EMVTAG_PDOLData);

	    if(DOLDataitem != NULL)
	    {
	        if(DOLDataitem->len)
	        {
	            memcpy(SDAToSign, DOLDataitem->data, DOLDataitem->len);
	            index += DOLDataitem->len;
	        }
	    }

	    DOLDataitem = emvbase_avl_gettagitempointer(EMVTAG_CDOL1RelatedData);

	    if(DOLDataitem != NULL)
	    {
	        if(DOLDataitem->len)
	        {
	            memcpy(SDAToSign + index, DOLDataitem->data, DOLDataitem->len);
	            index += DOLDataitem->len;
	        }
	    }

    	if(type == GenerateAC2)
	    {
	        DOLDataitem = emvbase_avl_gettagitempointer(EMVTAG_CDOL2RelatedData);

	        if(DOLDataitem != NULL)
	        {
	            if(DOLDataitem->len)
	            {
	                memcpy(SDAToSign + index, DOLDataitem->data, DOLDataitem->len);
	                index += DOLDataitem->len;
	            }
	        }
	    }
	}

    memcpy(SDAToSign + index, tempApp_UnionStruct->EMVTradeParam->GenerateACRetData, tempApp_UnionStruct->EMVTradeParam->GenerateACRetDataLen);
    index += tempApp_UnionStruct->EMVTradeParam->GenerateACRetDataLen;


    EMVBase_Hash(SDAToSign, index, SDAHash);

    emvbase_free(SDAToSign);

    if(memcmp(ICCDynDataSign.HashResult, SDAHash, 20))
    {
    	EMVBase_Trace("JCB-info: Transaction Data Hash Code in ICC Dynamic Data(recover data) verification fails\r\n");
        return RLT_EMV_ERR;
    }
    emvbase_avl_createsettagvalue(EMVTAG_ICCDynNum, ICCDynDataSign.ICCDynNum, ICCDynDataSign.ICCDynNumLen);
    emvbase_avl_createsettagvalue(EMVTAG_AppCrypt, ICCDynDataSign.AppCrypt, 8);


    return RLT_EMV_OK;
}

unsigned char jcb_CombineDDA(unsigned char type, JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char retCode;

    retCode = jcb_CombineDDASignVerify(type, tempApp_UnionStruct);

    if(retCode != RLT_EMV_OK)
    {
        return retCode;
    }
    return RLT_EMV_OK;
}

unsigned char jcb_GAC1AndAnalys(JCBTradeUnionStruct *tempApp_UnionStruct, unsigned char TermAnaResult)
{
	unsigned char retCode;
	unsigned char TransactionMode;
	EMVBASE_APDU_RESP apdu_r;
	EMVBASE_APDU_SEND apdu_s;
	unsigned char AIP[2] = {0};
	unsigned char ODAFail = 0;
	unsigned char ImpleOption = 0,CombOptions = 0;


	TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;
	EMVBase_Trace( "Jcb_GAC1AndAnalys TransactionMode=%d\r\n", TransactionMode);

	ImpleOption = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, &CombOptions, 0, 1);
	memset(AIP, 0, sizeof(AIP));
	emvbase_avl_gettagvalue_spec(EMVTAG_AIP, AIP, 0, 2);
	EMVBase_Trace( "JCB-info: AIP: %02x %02x\r\n", AIP[0], AIP[1]);
	EMVBase_Trace( "JCB-info: Implementation Option: %02x\r\n", ImpleOption);
	EMVBase_Trace( "JCB-info: Combination Option: %02x\r\n", CombOptions);

	// shall recover IPK and ICCPK before GAC--20210420
	if((TransactionMode==EMV_Mode) && (AIP[0] & 0x01) && ((ImpleOption & 0xa0) == 0xa0) && ((CombOptions & 0x22) == 0x22))//ICC and terminal support Combined DDA/AC
	{
		EMVBase_Trace( "JCB-info: support ODA-CDA\r\n");

		if(tempApp_UnionStruct->EMVTradeParam->bErrSDATL || tempApp_UnionStruct->EMVTradeParam->bErrAuthData) //SDA_TL.tag not EMV; represents data not available at the moment; refers to constructed Data Object;//processing of SDA Tag List failed
		{
			EMVBase_Trace("JCB-info: Tag9F4A error(len is not equal to 1 or value is not equal to '82')\r\n");
			EMVBase_Trace("or data need to be authenticated returned by read app data is not format '70'\r\n");
			ODAFail = 1;
		}
		else
		{
			retCode = jcb_CombineDDA_FirstHalf(tempApp_UnionStruct); // Get CAPK,IPK,ICCPK
			if(retCode != RLT_EMV_OK)
			{
				ODAFail = 1;
			}
		}

		if(ODAFail)
		{
			emvbase_avl_setvalue_or(EMVTAG_TVR, 0, 0x04);		//Offline Combined DDA/AC Generation failed
		}
	}

	if(TransactionMode == Magstripe_Mode)
	{
		retCode = jcb_GetStripeDataCommand(tempApp_UnionStruct, TermAnaResult, &apdu_s);
	}
	else
	{
		retCode = jcb_CommandGAC1(tempApp_UnionStruct, TermAnaResult, &apdu_s);
	}

	if(retCode != RLT_EMV_OK)
	{
		return retCode;
	}

	tempApp_UnionStruct->IsoCommand(&apdu_s, &apdu_r);

	if(apdu_r.ReadCardDataOk != RLT_EMV_OK)
	{
		EMVBase_Trace("JCB-error: apdu timeout or other error\r\n");
		if(TransactionMode == EMV_Mode)//Tron Transaction process
		{
			jcb_PrepareRecoveryContext(tempApp_UnionStruct);
		}
		jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_ENDAPPLICATION_COMMUNICATIONERROR);
		return RLT_EMV_TORN;
	}
	retCode = jcb_DealGAC1Rsp(tempApp_UnionStruct, &apdu_r);

	if(retCode == RLT_EMV_OFFLINE_DECLINE)
	{
		tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
		jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
		retCode = RLT_EMV_OFFLINE_DECLINE;
	}
	return retCode;
	//return Jcb_DealGAC1Rsp(tempApp_UnionStruct, &apdu_r);
}

unsigned char jcb_TermiAnalys(JCBTradeUnionStruct *tempApp_UnionStruct)//no TSI
{
	unsigned char transtype, refundflag = 0;
	unsigned char IACDecline[5], IACOnline[5],IACDefault[5];
	unsigned char retCode = RLT_EMV_OK;
	unsigned char TermAnaResult;
	unsigned char DTIP[3],tvr[5];//sjz 2020.6.19

	transtype = emvbase_avl_gettagvalue(EMVTAG_TransTypeValue);
	if(transtype == 0x20)//refund
	{
		refundflag = 1;
	}

	if(refundflag == 0)
	{
		if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Magstripe_Mode || tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Legacy_Mode)
		{
			memset(IACDecline, 0x00, 5);
			memset(IACOnline, 0xFF, 5);
			memset(IACDefault, 0xFF, 5);
			emvbase_avl_createsettagvalue(EMVTAG_IACDenial, IACDecline, 5);
			emvbase_avl_createsettagvalue(EMVTAG_IACOnline, IACOnline, 5);
			emvbase_avl_createsettagvalue(EMVTAG_IACDefault, IACDefault, 5);
		}//no IAC values are provided by card, the Kernel shall use the default IAC values
		TermAnaResult = jcb_TermActAnaly(tempApp_UnionStruct);

		EMVBase_Trace( "Jcb_TermiAnalys TermAnaResult=%02x\r\n", TermAnaResult);
	}

	if(refundflag == 1)//the Kernel shall decline the transaction if refund
	{
		TermAnaResult = TAA_DENIAL;
	}

	memset(DTIP, 0, sizeof(DTIP));
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBDynamicTIP, DTIP, 0, 3);
	if(DTIP[0] & 0x04)//sjz 2020.6.19//transit reader,"MSD/Legacy mode/black CardNo" should declined;
	{
		if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Magstripe_Mode || tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Legacy_Mode)
		{
			TermAnaResult = TAA_DENIAL;
		}
		memset(tvr, 0, sizeof(tvr));
		emvbase_avl_gettagvalue_spec(EMVTAG_TVR, tvr, 0, 5);
		if(tvr[0] & 0x10)//black CardNo
		{
			TermAnaResult = TAA_DENIAL;
		}
		if(TermAnaResult == TAA_DENIAL)
		{
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
			//return RLT_EMV_OK;
			return RLT_EMV_OFFLINE_DECLINE;
		}
	}

	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode || tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Legacy_Mode)
	{

		if(TermAnaResult == TAA_DENIAL)
		{
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OK;
		}
	}
	//retCode = Jcb_TermiAnalysCompletion(tempApp_UnionStruct, TermAnaResult);

	retCode = jcb_GAC1AndAnalys(tempApp_UnionStruct, TermAnaResult);

	return retCode;
}

unsigned char jcb_TransProcess(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char retCode;
	unsigned char CryptInfo;
	unsigned char TransactionMode;
	unsigned char respCrpt = 0;


	TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;

	CryptInfo = emvbase_avl_gettagvalue(EMVTAG_CryptInfo);
	EMVBase_Trace( "Paywaveb_TransProcess CryptInfo=%02x\r\n", CryptInfo);

	if((CryptInfo&0xC0)==0x40 || (CryptInfo&0xC0)==0x80 || TransactionMode == Magstripe_Mode)
	{
		EMVBase_Trace( "operation start!\r\n");
	}

	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Magstripe_Mode)
	{
		tempApp_UnionStruct->EMVTradeParam->TransResult=RESULT_ONLINE_WAIT;   //sxl?ÊòØÂê¶Ë¶ÅÂÅöCDA
		tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 2;
		EMVBase_Trace( "Jcb_TransProcess Magstripe_Mode");
		return RLT_EMV_OK;
	}

	if((CryptInfo&0xC0)==0x00)
	{
		tempApp_UnionStruct->EMVTradeParam->TransResult=RESULT_OFFLINE_DECLINE;
		tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 1;
		//jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
		return RLT_EMV_OFFLINE_DECLINE;
	}
    else if((CryptInfo&0xC0)==0x40)
    {
    	//return RLT_ERR_EMV_IccDataFormat;
    	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode)
    	{
			tempApp_UnionStruct->EMVTradeParam->TransResult=RESULT_OFFLINE_APPROVE;
			tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 0;
			//jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_APPROVED);
    	}
		else
		{
			return RLT_ERR_EMV_IccDataFormat;
		}
    }
	else if((CryptInfo&0xC0)==0x80)
	{
		tempApp_UnionStruct->EMVTradeParam->TransResult=RESULT_ONLINE_WAIT;   //sxl?ÊòØÂê¶Ë¶ÅÂÅöCDA
		tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 2;
		//retCode = Emvb_ArQcOnline(tempApp_UnionStruct);
	}
	else
    {
        return RLT_EMV_ERR;
    }

	return RLT_EMV_OK;
}


unsigned char jcb_TcOffLine(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char respCrpt, retCode;
    unsigned char bCombineDDAFail = 0;

    tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_APPROVE;

    respCrpt = emvbase_avl_gettagvalue(EMVTAG_ReferenceControlParameter);

    if((respCrpt & 0x10) != 0)
    {
        emvbase_avl_setvalue_or(EMVTAG_TSI, 0, 0x80);

        retCode = jcb_CombineDDA(GenerateAC1, tempApp_UnionStruct);

        if(retCode != RLT_EMV_OK)
        {
            EMVBase_Trace( "JCB-info: CDA failed\r\n");

            emvbase_avl_setvalue_or(EMVTAG_TVR, 0, 0x04);
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
		}
    }
    return RLT_EMV_OK;
}



unsigned char jcb_ArQcOnline(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char respCrpt, retCode = RLT_EMV_OK;

    respCrpt = emvbase_avl_gettagvalue(EMVTAG_ReferenceControlParameter);
    tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_ONLINE_WAIT;

    if((respCrpt & 0x10) != 0)
    {
        emvbase_avl_setvalue_or(EMVTAG_TSI, 0, 0x80);                              //Offline Data Authentication was performed
        retCode = jcb_CombineDDA(GenerateAC1, tempApp_UnionStruct);

        if(retCode != RLT_EMV_OK)
        {
            EMVBase_Trace( "JCB-info: CDA failed\r\n");

            emvbase_avl_setvalue_or(EMVTAG_TVR, 0, 0x04);
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
        }
        retCode = RLT_EMV_OK;
    }
    return retCode;
}

unsigned char jcb_CDAafterGAC1(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char retCode;
	EMVBASE_APDU_SEND apdu_s;
	EMVBASE_APDU_RESP apdu_r;
	unsigned char CryptInfo;

	EMVBase_Trace( "JCB-info: GAC with CDA\r\n");

	CryptInfo = emvbase_avl_gettagvalue(EMVTAG_CryptInfo);
	EMVBase_Trace( "JCB-info: CID(9F27): %02x\r\n", CryptInfo);

	if((CryptInfo & 0xC0) == 0x00)
	{
		EMVBase_Trace( "JCB-info: AAC\r\n");
		retCode = jcb_AacRefuse(tempApp_UnionStruct);
	}
	else if((CryptInfo & 0xC0) == 0x40)
	{
		EMVBase_Trace( "JCB-info: TC\r\n");
		//retCode = Emvb_TcOffLine(tempApp_UnionStruct);
		retCode = jcb_TcOffLine(tempApp_UnionStruct);
	}
	else if((CryptInfo & 0xC0) == 0x80)
	{
		EMVBase_Trace( "JCB-info: ARQC\r\n");
		//retCode = Emvb_ArQcOnline(tempApp_UnionStruct);

		retCode = jcb_ArQcOnline(tempApp_UnionStruct);
	}
	//EMVBase_Trace( "Jcb_CDAafterGAC1 retcode=%d\r\n", retCode);

	return retCode;
}

/*******************************************************************
   ◊˜    ’ﬂ: sxl
   ∫Ø ˝√˚≥∆: unsigned char Paywave_DataAuth(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: Õ—ª˙ ˝æ›»œ÷§
   »Îø⁄≤Œ ˝:
   ∑µ ªÿ ÷µ:
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:  sxl0221
 ********************************************************************/
unsigned char jcb_DataAuth(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char retCode = RLT_EMV_OK;
	unsigned char AIP[2];
	//unsigned char TermCapab[3];
	//EMVBASE_APDU_SEND apdu_s;
	//EMVBASE_APDU_RESP apdu_r;
	unsigned char ODAFail = 0;
	unsigned char ImpleOption,CombOptions;
	unsigned char TransactionMode;
	unsigned char TVR[5] = {0};


	ImpleOption = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, &CombOptions, 0, 1);
	TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;

	memset(AIP, 0, sizeof(AIP));
	//memset(TermCapab, 0, sizeof(TermCapab));

	emvbase_avl_gettagvalue_spec(EMVTAG_AIP, AIP, 0, 2);
	//emvbase_avl_gettagvalue_spec(EMVTAG_TermCapab, TermCapab, 0, 3);
	//jcb_CheckIfdoCDA(tempApp_UnionStruct);
	EMVBase_Trace( "JCB-info: AIP: %02x %02x\r\n", AIP[0], AIP[1]);
	EMVBase_Trace( "JCB-info: Implementation Option: %02x\r\n", ImpleOption);
	EMVBase_Trace( "JCB-info: Combination Option: %02x\r\n", CombOptions);

	if((TransactionMode==EMV_Mode) && (AIP[0] & 0x01) && ((ImpleOption & 0xa0) == 0xa0) && ((CombOptions & 0x22) == 0x22))//ICC and terminal support Combined DDA/AC
	{
		EMVBase_Trace( "JCB-info: support ODA-CDA\r\n");

		if(tempApp_UnionStruct->EMVTradeParam->JcbTornTransactionFlag) // normal flow has recovered before GAC Command 20210420
		{
			if(tempApp_UnionStruct->EMVTradeParam->bErrSDATL || tempApp_UnionStruct->EMVTradeParam->bErrAuthData)		   //SDA_TL.tag not EMV; represents data not available at the moment; refers to constructed Data Object;//processing of SDA Tag List failed
			{
				EMVBase_Trace("JCB-info: Tag9F4A error(len is not equal to 1 or value is not equal to '82')\r\n");
				EMVBase_Trace("or data need to be authenticated returned by read app data is not format '70'\r\n");
				ODAFail = 1;
			}
			else
			{
				retCode = jcb_CombineDDA_FirstHalf(tempApp_UnionStruct);//Get CAPK,IPK,ICCPK
				//if(Emvb_CombineDDA_FirstHalf(tempApp_UnionStruct) != RLT_EMV_OK)			   //Get CAPK,IPK,ICCPK //∏¥∫œ∂ØÃ¨»œ÷§œ»»°µ√CAPK£¨IPK£¨ICCPK.
				if(retCode != RLT_EMV_OK)
				{
					ODAFail = 1;

					tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
					jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
					retCode = RLT_EMV_OFFLINE_DECLINE;
				}
			}

			if(ODAFail)
			{
				emvbase_avl_setvalue_or(EMVTAG_TVR, 0, 0x04);		//Offline Combined DDA/AC Generation failed
				//emvbase_avl_setvalue_or(EMVTAG_TSI, 0, 0x80);		//set bit 'Offline Data Authentication was performed' bit 1
			}
		}
		else // normal flow 20210420
		{
			memset(TVR, 0, sizeof(TVR));
			emvbase_avl_gettagvalue_spec(EMVTAG_TVR, TVR, 0, 5);

	        if((TVR[0] & 0x04))
			{
				tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
				retCode = RLT_EMV_OFFLINE_DECLINE;
			}
		}
		//return RLT_EMV_OK;
	}
	else	//all dynamic and static authentication and DDA/AC are not supported.
	{
		EMVBase_Trace( "JCB-info: ODA is not performed\r\n");
		//The bit should be set to 1 according to test script 2CI.023.00
		emvbase_avl_setvalue_or(EMVTAG_TVR, 0, 0x80);								//Offline Data Authentication not performed
		retCode = RLT_EMV_OK;
	}

	if(retCode == RLT_EMV_OK)
	{
		retCode = jcb_CheckIfdoCDA(tempApp_UnionStruct);
		if(retCode)
		{
			retCode = jcb_CDAafterGAC1(tempApp_UnionStruct);
			emvbase_avl_printtagallvalue("JCB-info: After GAC TVR: ",EMVTAG_TVR);
		}
	}

	return retCode;
}

unsigned char jcb_CardHolderVerf_onlinepin(JCBTradeUnionStruct *tempApp_UnionStruct,unsigned char pintype)
{
	unsigned char tempCreditPwd[20];
	unsigned char retCode;

	memset(tempCreditPwd,0,sizeof(tempCreditPwd));

    retCode = tempApp_UnionStruct->EMVB_InputCreditPwd(pintype,tempCreditPwd);
    //tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 2;
	if(retCode == RLT_ERR_EMV_InputBYPASS)
    {
        //emvbase_avl_setvalue_or(EMVTAG_TVR,2,0x08);
		//emvbase_avl_setvalue_and(EMVTAG_TVR,2,0xDF);

		return JCB_ERR_CVMFail;
    }
	else if(retCode == RLT_ERR_EMV_CancelTrans)
    {
    	return RLT_ERR_EMV_CancelTrans;
    }
    else if(retCode == RLT_EMV_OK)
    {
        if(strlen(tempCreditPwd))
        {
	        emvbase_avl_createsettagvalue(EMVTAG_PIN,tempCreditPwd,8);
	        //emvbase_avl_setvalue_or(EMVTAG_TVR,2,0x04); //Online PIN entered
        }
		else
		{
			emvbase_avl_createsettagvalue(EMVTAG_PIN,NULL,0);
		}
    }
    else// if(retCode == RLT_EMV_ERR)
    {
    	//emvbase_avl_setvalue_or(EMVTAG_TVR,2,0x10);
		return JCB_ERR_CVMFail;
    }

	return retCode;


}



unsigned char jcbLegacyInitCardHolderVerify(JCBTradeUnionStruct *tempApp_UnionStruct,CVMSTRCUT *CVMdata)
{
	unsigned char i;
	unsigned char AIP[2],KernelConfigure;
	unsigned char CVMResult[3];
    unsigned char CVMListbExist;
	EMVBASETAGCVLITEM *CVMListitem = NULL;


	//get cvm list in the card, if not available, return
	CVMListitem = emvbase_avl_gettagitemandstatus(EMVTAG_CVMList,&CVMListbExist);
	if(CVMListbExist==0x00)//CVM List is absent.
	{
		emvbase_avl_setvalue_or(EMVTAG_TVR,0,0x20);
//		tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
		return RLT_EMV_OFFLINE_DECLINE;	//for JCB 1.6

		//CVMResult[0]=JCB_CVMRESULT_NOCVMPERFORMED;
		//CVMResult[0]=0x3F;
		CVMResult[0] = JCB_CVMRESULT_DEFAULT;
		CVMResult[1] = 0x00;
		CVMResult[2] = JCB_CVMRESULT_UNKNOWN;
		emvbase_avl_createsettagvalue(EMVTAG_CVMResult,CVMResult,3);

		EMVBase_Trace( "JcbLegacyInitCardHolderVerify  CVM List is absent\r\n");

		return RLT_EMV_OK;
	}

	if(CVMListitem->len<10||(CVMListitem->len%2))
	{
		emvbase_avl_setvalue_or(EMVTAG_TVR,2,0x80);  //CVM FIAL
		CVMResult[0]=JCB_CVMRESULT_NOCVMPERFORMED;
		CVMResult[1]=0x00;
		CVMResult[2]=JCB_CVMRESULT_UNKNOWN;
		emvbase_avl_createsettagvalue(EMVTAG_CVMResult,CVMResult,3);
		EMVBase_Trace( "JcbLegacyInitCardHolderVerify  CVM data format error\r\n");

		return RLT_EMV_OK;
	}

	//to compatibal with PC and 51MCU and MCVZ328 MCU,make the long integer independent of byte order.
	memcpy(&CVMdata->CVM_X[2],CVMListitem->data,4);
	memcpy(&CVMdata->CVM_Y[2],&CVMListitem->data[4],4);


	CVMdata->CVRListLen=(CVMListitem->len-8)/2;
	for(i=0;i<CVMdata->CVRListLen;i++)
	{
		CVMdata->CVRList[i].method = CVMListitem->data[i*2+8];
		CVMdata->CVRList[i].condition = CVMListitem->data[i*2+9];
	}

	return RLT_EMV_OTHER;
}


unsigned short jcbLegacyAnalyCvmlist(unsigned char method,unsigned char condition,unsigned char TransType,unsigned char TermType,unsigned char *CVM_X,unsigned char *CVM_Y)
{

	unsigned char needcvm = 0,nextcvm = 0;
	unsigned short returnvalue;


	needcvm = 1;

    returnvalue = nextcvm;
    returnvalue = (returnvalue<<8)|needcvm;

	return returnvalue;
}



unsigned char jcbLegacy_PerformCVM(JCBTradeUnionStruct *tempApp_UnionStruct,unsigned char method,unsigned char condition,unsigned char *DTIP)
{
	unsigned char retCode = 0;
	unsigned char temp;
	unsigned char CVMResult[3];


	temp = method&0x3F;

    EMVBase_Trace( "JcbLegacy_PerformCVM method=%02x temp=%02x DTIP=%02x\r\n", method, temp,*DTIP);

	switch(temp)		//
	{
		case 0x00:	   //cvm fail
			retCode = JCB_ERR_CVMFailALWAYS;
			break;
		case 0x01:    //PLAINTEXT PIN VERIFICATION PERFORMED BY ICC
		    retCode = JCB_ERR_CVMNoSupport;
			break;

		case 0x02:   //enciphered PIN verification online
			EMVBase_Trace("online pin\r\n");
			CVMResult[2]=JCB_CVMRESULT_UNKNOWN;
			if((*DTIP & 0x20) == 0x20) //online pin
			{
				tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_ONLINEPIN;
				CVMResult[0] = JCB_CVMRESULT_ONLINEPIN;
				CVMResult[1] = 0x00;
				retCode = jcb_CardHolderVerf_onlinepin(tempApp_UnionStruct, EMVB_DATAAUTH_ONLINEPIN);
				if(retCode == RLT_EMV_OK)
				{
					//CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
				}
				else if(retCode == JCB_ERR_CVMFail)
				{
					//CVMResult[2] = JCB_CVMRESULT_UNKNOWN;
				}
				emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);
				return RLT_EMV_OK;
			}
			else
			{
				EMVBase_Trace( "2222222222\r\n");
				return JCB_ERR_CVMNoSupport;
				//return JCB_OPS_STATUS_DECLINED;
			}
			//emvbase_avl_setvalue_or(EMVTAG_TVR,2,0x04);  //ÂØÜÁ†ÅÂú®ÊúÄÂêéËæìÂÖ•Ôºå???ÂÆöË¶ÅÂÖàÁΩÆËµ∑Êù•Ê†áÂøó
			retCode = RLT_EMV_OK;
			return retCode;
			break;
		case 0x03:  //Plaintext PIN verification performed by ICC and signature(paper)
		    return JCB_ERR_CVMNoSupport;
			break;
		case 0x04:  //enciphered PIN verification performed by ICC
			return JCB_ERR_CVMNoSupport;
			break;
		case 0x05: //enciphered PIN verification performed by ICC and signature(paper)
			return JCB_ERR_CVMNoSupport;
			break;
		case 0x1E://signature (paper)
			CVMResult[2]=JCB_CVMRESULT_UNKNOWN;
			if((*DTIP & 0xc0) == 0xc0)
			{
				tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_OBTAINSIGNATURE;
				tempApp_UnionStruct->EMVTradeParam->bPrintReceipt=1;

				CVMResult[0] = JCB_CVMRESULT_SIGBATURE;
				CVMResult[1] = 0x00;
				//CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
				emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);
				return RLT_EMV_OK;
			}
			else
			{
				EMVBase_Trace( "11111111\r\n");
				//return JCB_OPS_STATUS_DECLINED;
				return JCB_ERR_CVMNoSupport;
			}
			retCode = RLT_EMV_OK;
			return retCode;
			break;
		case 0x1F://no CVM required
			return JCB_ERR_CVMNoSupport;
			break;
		default:
			emvbase_avl_setvalue_or(EMVTAG_TVR,2,0x40);
			//CVMResult[2]=CVR_FAIL;//sxl
			//return JCB_ERR_CVMFail;
			break;

	}
	CVMResult[0]=method;
	CVMResult[1]=condition;

	emvbase_avl_createsettagvalue(EMVTAG_CVMResult,CVMResult,3);

	return retCode;


}

unsigned char jcb_Legacy_CardHolderVerf(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char retCode;
    CVMSTRCUT CVMdata;
	unsigned char NowCVRListLen = 0;
	unsigned char needcvm,nextcvm;
	unsigned short needcvmnextcvm;
	unsigned char TransType,TermType;
	unsigned char TermCapab[3];
	unsigned char CVMResult[3];
	unsigned char DTIP;


	emvbase_avl_gettagvalue_spec(EMVTAG_JCBDynamicTIP, &DTIP, 0, 1);


	memset(&CVMdata,0,sizeof(CVMSTRCUT));
	retCode = jcbLegacyInitCardHolderVerify(tempApp_UnionStruct,&CVMdata);
	EMVBase_Trace( "JcbLegacyInitCardHolderVerify retCode=%d\r\n", retCode);
    if(retCode == RLT_EMV_OK || retCode == RLT_EMV_OFFLINE_DECLINE)	//for jcb 1.6
    {
    	return retCode;
    }


	EMVBase_Trace( "Jcb_Legacy_CardHolderVerf begin to compare\r\n");
	while(NowCVRListLen < CVMdata.CVRListLen)
	{
		EMVBase_Trace( "NowCVRListLen=%d CVMdata.CVRListLen=%d\r\n", NowCVRListLen, CVMdata.CVRListLen);
		needcvmnextcvm = 0;
		needcvmnextcvm = jcbLegacyAnalyCvmlist(CVMdata.CVRList[NowCVRListLen].method,CVMdata.CVRList[NowCVRListLen].condition,TransType,TermType,CVMdata.CVM_X,CVMdata.CVM_Y);
        EMVBase_Trace( "JcbLegacyAnalyCvmlist needcvmnextcvm=%d\r\n", needcvmnextcvm);

        needcvm = needcvmnextcvm&0xff;
        nextcvm = (needcvmnextcvm>>8)&0xff;
		EMVBase_Trace( "needcvm=%d nextcvm=%d\r\n", needcvm, nextcvm);

		if(needcvm == 1)
		{
			retCode=jcbLegacy_PerformCVM(tempApp_UnionStruct,CVMdata.CVRList[NowCVRListLen].method,CVMdata.CVRList[NowCVRListLen].condition, &DTIP);
			EMVBase_Trace( "JcbLegacy_PerformCVM retCode = %d\r\n",retCode);

			if(retCode==RLT_EMV_OK)
			{
				EMVBase_Trace( "111111111\r\n");
				return retCode;
			}
			else if(retCode == JCB_OPS_STATUS_DECLINED || retCode == JCB_ERR_CVMFailALWAYS)
			{
				EMVBase_Trace( "22222222222\r\n");
				jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
					tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
				return RLT_EMV_OFFLINE_DECLINE;
			}

            EMVBase_Trace( "tempterminfo->CVRList[tempterminfo->NowCVRListLen].method =%02x retCode=%d\r\n",CVMdata.CVRList[NowCVRListLen].method,retCode);

			/*if(retCode == JCB_ERR_CVMFailALWAYS)
			{
				emvbase_avl_setvalue_or(EMVTAG_TVR,2,0x80);

				if((CVMdata.CVRList[NowCVRListLen].method&0x3f) == 0)
				{
					CVMResult[0]=CVMdata.CVRList[NowCVRListLen].method;
					CVMResult[1]=CVMdata.CVRList[NowCVRListLen].condition;
					CVMResult[2] = 0x01;

					emvbase_avl_createsettagvalue(EMVTAG_CVMResult,CVMResult,3);
				}
				return RLT_EMV_OK;
			}*/

			if((NowCVRListLen==CVMdata.CVRListLen-1))
			{
				EMVBase_Trace( "method=%02x\r\n", CVMdata.CVRList[NowCVRListLen].method);
				tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
				jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
				tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 1;
//				emvbase_avl_setvalue_or(EMVTAG_TVR,2,0x80);
				return RLT_EMV_OK;
			}
			else nextcvm = 1;

		}
		if(nextcvm == 1)
		{
			(NowCVRListLen)++;
		}

	}
	EMVBase_Trace( "Jcb_Legacy_CardHolderVerf fail!!!--->declined\r\n");
	tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
	jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
	tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 1;
	emvbase_avl_setvalue_or(EMVTAG_TVR,2,0x80);			//set "Cardholder verification was not successful"
	return RLT_EMV_OK;


}


/*******************************************************************
???    ???: sxl
ÂáΩÊï∞ÂêçÁß∞: unsigned char Paywaveb_CardHolderVerf(AppUnionStruct *tempApp_UnionStruct)
ÂáΩÊï∞ÂäüËÉΩ: ÊåÅÂç°‰∫∫ËÆ§???
ÂÖ•Âè£ÂèÇÊï∞:
??? ??? ???:
Áõ∏ÂÖ≥Ë∞ÉÁî®:
???    ???:
‰øÆÊîπ‰ø°ÊÅØ: sxl0221
********************************************************************/
unsigned char jcb_CardHolderVerf_Inline(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char retCode = RLT_EMV_OK;
	unsigned char termCVMFlag1,termCVMFlag2;
	//unsigned char ifcardneedcvm = 0;
	unsigned char TermTransPredicable[4],CardTransPredicable[2],CardTransPredicableexist;
    unsigned char qPBOCUndfinedParamexist;
	EMVBASETAGCVLITEM *item;
	unsigned char TVR[5];

	unsigned char TransactionMode;
	unsigned char CardholderVerifStatus, TIP[3], tempCVS = 0;
	unsigned char MagCVMDecisionDigit, Track2Buf[19];
	unsigned char tempbuf[128];
	unsigned short templen;
	unsigned char CVMResult[3];

	memset(TIP, 0x00, 3);
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBDynamicTIP, TIP, 0, 3);
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCardholderVerifyStatus, &CardholderVerifStatus, 0, 1);
	//EMVTAG_Track2Equ
	EMVBase_Trace( "Jcb_CardHolderVerf CardholderVerifStatus=%02x DTIP=%02x %02x %02x\r\n", CardholderVerifStatus, TIP[0], TIP[1], TIP[2]);

	EMVBase_Trace( "Jcb_CardHolderVerf JcbDRRI=%d\r\n",tempApp_UnionStruct->EMVTradeParam->JcbDRRI);

	//ËÑ±Êú∫‰∫§ÊòìÂíåÊãíÁªù‰∫§Êòì‰∏çÂÅöCVM Â§ÑÁêÜ
    if(tempApp_UnionStruct->EMVTradeParam->JcbDRRI == 1)
    {
        emvbase_avl_createsettagvalue(EMVTAG_CVMResult,"\x1F\x00\x00",3);

    	return RLT_EMV_OK;
    }


	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Undefined_Mode)
	{
		return RLT_EMV_OK;
	}

	memset(CVMResult, 0x00, 3);
	memcpy(CVMResult, "\x3F\x00\x00", 3);//sjz 2020.6.19//1F0000->3F0000
	emvbase_avl_createsettagvalue(EMVTAG_CVMResult,CVMResult,3);

	EMVBase_Trace( "Jcb_CardHolderVerf transmode=%d\r\n", tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode);
	if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Magstripe_Mode)
	{
	#if 0
		memset(Track2Buf, 0x00, 19);
		emvbase_avl_gettagvalue_spec(EMVTAG_Track2Equ, Track2Buf, 0, 19);
		EMVBase_TraceHex( "Track2Buf ", Track2Buf, 19);
		MagCVMDecisionDigit = Track2Buf[18] & 0xF0;
		MagCVMDecisionDigit >>= 4;
	#endif
		memset(tempbuf, 0x00, 128);
		emvbase_avl_gettagvalue_all(EMVTAG_Track2Equ, tempbuf,&templen);
		EMVBase_TraceHex( "EMVTAG_Track2Equ ",tempbuf,templen);
		MagCVMDecisionDigit = tempbuf[templen-1];
		//EMVBase_Trace( "MagCVMDecisionDigit111=%02x\r\n", MagCVMDecisionDigit);
		MagCVMDecisionDigit &= 0xF0;
		//EMVBase_Trace( "MagCVMDecisionDigit222=%02x\r\n", MagCVMDecisionDigit);
		MagCVMDecisionDigit >>= 4;
		EMVBase_Trace( "MagCVMDecisionDigit333=%d\r\n", MagCVMDecisionDigit);
		if(MagCVMDecisionDigit == 0 || (MagCVMDecisionDigit >= 5 && MagCVMDecisionDigit <= 9))//Not Applicable (no CVM preference)
		{
			return RLT_EMV_OK;
		}

		if((TIP[0] & 0x80) == 0x80 && MagCVMDecisionDigit == 0x01)//kernel request && card not support
		{
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
		}

		if((TIP[0]& 0x40) == 0x40 && (MagCVMDecisionDigit == 0x02)) //signature
		{
			tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_OBTAINSIGNATURE;
			tempApp_UnionStruct->EMVTradeParam->bPrintReceipt=1;

			CVMResult[0] = JCB_CVMRESULT_SIGBATURE;
			CVMResult[1] = 0x00;
			//CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
			CVMResult[2] = JCB_CVMRESULT_UNKNOWN;
			emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);
			return RLT_EMV_OK;
		}
		else if((TIP[0]& 0x20) == 0x20 && (MagCVMDecisionDigit == 0x03)) //online pin
		{
			tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_ONLINEPIN;
			CVMResult[0] = JCB_CVMRESULT_ONLINEPIN;
			CVMResult[1] = 0x00;
			//CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
			CVMResult[2] = JCB_CVMRESULT_UNKNOWN;
			emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);
			return jcb_CardHolderVerf_onlinepin(tempApp_UnionStruct,EMVB_DATAAUTH_ONLINEPIN);
		}
		else if((TIP[0]& 0x10) == 0x10 && (MagCVMDecisionDigit == 0x04))//Confirmation Code Verified
		{
			tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_CONFVERIFIED;
			CVMResult[0] = JCB_CVMRESULT_PLAINTEXTPINVERIFICATION;
			CVMResult[1] = 0x00;
			CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
			emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);
			return RLT_EMV_OK;
		}
		else
		{
			if((TIP[0] & 0x80) != 0x80 && MagCVMDecisionDigit == 0x01)// NO CVM
			{
				tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_NOCVMREQ;
				CVMResult[0] = JCB_CVMRESULT_NOCVMPERFORMED;
				CVMResult[1] = 0x00;
				CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
				emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);
				return RLT_EMV_OK;
			}
			EMVBase_Trace( "fail !!!\r\n");
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
		}

	}
	else if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == Legacy_Mode)
	{
		emvbase_avl_createsettagvalue(EMVTAG_CVMResult,"\x3F\x00\x00",3);
		if((TIP[0] & 0x80) == 0x80)
		{
			retCode = jcb_Legacy_CardHolderVerf(tempApp_UnionStruct);

			//retCode = Jcb_Legacy_CardHolderVerf1(tempApp_UnionStruct);

			emvbase_avl_gettagvalue_spec(EMVTAG_TVR,TVR,0,5);
			//emvbase_avl_createsettagvalue(EMVTAG_TVR,"\x00\x00\x00\x00\x00",5);

			EMVBase_Trace( "TVR[2] = %02x \r\n",TVR[2]);

			if(TVR[2]&0x80)
			{
				retCode = RLT_EMV_OK;
				tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 1;
				return retCode;
			}
			else
			{
				//retCode = RLT_EMV_OK;
				//if(retCode != RLT_EMV_OK)
					return retCode;
			}

			/*if(tempApp_UnionStruct->EMVTradeParam->paypassonlinepinreq)
			{
				tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_ONLINEPIN;
				retCode = Jcb_CardHolderVerf_onlinepin(tempApp_UnionStruct,EMVB_DATAAUTH_ONLINEPIN);
				if(retCode != RLT_EMV_OK)
				{
					EMVBase_Trace( "Legacy_Mode onlinepin error\r\n");
					return retCode;
				}
			}*/
			return retCode;
		}
		else
		{
			EMVBase_Trace( "Legacy_Mode JCB_OPS_CVM_NOCVMREQ\r\n");
			tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_NOCVMREQ;
			CVMResult[0] = JCB_CVMRESULT_NOCVMPERFORMED;
			CVMResult[1] = 0x00;
			CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
			emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);

			return RLT_EMV_OK;
		}
	}
	else if(tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode == EMV_Mode)
	{
#if 0//sjz 2020.6.23
		if(CardholderVerifStatus != 0x00 && CardholderVerifStatus != 0x10 && CardholderVerifStatus != 0x20  \
			&& (CardholderVerifStatus&0xF0) != 0x30)//Not Applicable (no CVM preference)
		{
			return RLT_EMV_OK;
		}
#endif
		if(TIP[0] & 0x04) // JCB Specification Bulletin No.007   // zzq 20200715
		{
			tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_NOCVMREQ;
			CVMResult[0] = JCB_CVMRESULT_NOCVMPERFORMED;
			CVMResult[1] = 0x00;
			CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
			emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);
			return RLT_EMV_OK;
		}

		if((TIP[0] & 0x80) && !(TIP[0] & 0x04) && (CardholderVerifStatus != 0x10 && CardholderVerifStatus != 0x20  \
			&& (CardholderVerifStatus&0xF0) != 0x30))//sjz 2020.6.19
		{
			EMVBase_Trace( "terminal request CVM && no transit Reader && CVSI != 10,20,3X\r\n");
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
		}
		if(((CardholderVerifStatus==0x10) && ((TIP[0]&0x40) != 0x40))
			|| ((CardholderVerifStatus==0x20) && ((TIP[0]&0x20) != 0x20))
			|| (((CardholderVerifStatus&0xF0)==0x30) && ((TIP[0]& 0x10) != 0x10)))//sjz 2020.6.19
		{
			EMVBase_Trace( "terminal request CVM && no transit Reader && CVSI != 10,20,3X\r\n");
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
		}

#if 0//sjz 2020.6.23
		if((TIP[0] & 0x80) == 0x80 && CardholderVerifStatus == 0x00) // request CVM but card return no cvm
		{
			EMVBase_Trace( "terminal request CVM but card return no cvm!\r\n");
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
		}
#endif
		//tempCVS = CardholderVerifStatus & 0xF0;
		EMVBase_Trace( "EMV_Mode tempCVS=%02x CardholderVerifStatus=%02x\r\n", tempCVS,CardholderVerifStatus);
		if((TIP[0]& 0x40) == 0x40 && (CardholderVerifStatus == 0x10)) //signature
		{
			tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_OBTAINSIGNATURE;
			tempApp_UnionStruct->EMVTradeParam->bPrintReceipt=1;
			CVMResult[0] = JCB_CVMRESULT_SIGBATURE;
			CVMResult[1] = 0x00;
			//CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
			CVMResult[2] = JCB_CVMRESULT_UNKNOWN;
			emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);
			return RLT_EMV_OK;
		}
		else if((TIP[0]& 0x20) == 0x20 && (CardholderVerifStatus == 0x20)) //online pin
		{
			tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_ONLINEPIN;
			CVMResult[0] = JCB_CVMRESULT_ONLINEPIN;
			CVMResult[1] = 0x00;
			//CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
			CVMResult[2] = JCB_CVMRESULT_UNKNOWN;
			emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);

			return jcb_CardHolderVerf_onlinepin(tempApp_UnionStruct,EMVB_DATAAUTH_ONLINEPIN);
		}
		else if((TIP[0]& 0x10) == 0x10 && (CardholderVerifStatus & 0xF0) == 0x30)//Confirmation Code Verified
		{
			tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_CONFVERIFIED;
			CVMResult[0] = JCB_CVMRESULT_PLAINTEXTPINVERIFICATION;
			CVMResult[1] = 0x00;
			CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
			emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);
			return RLT_EMV_OK;
		}
		else
		{
			//emvbase_avl_createsettagvalue(EMVTAG_CVMResult,"\x1F\x00\x00",3);
			if((TIP[0] & 0x80) != 0x80 && CardholderVerifStatus == 0x00) // NO CVM
			{
				tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_NOCVMREQ;
				CVMResult[0] = JCB_CVMRESULT_NOCVMPERFORMED;
				CVMResult[1] = 0x00;
				CVMResult[2] = JCB_CVMRESULT_SUCCESSFUL;
				emvbase_avl_createsettagvalue(EMVTAG_CVMResult, CVMResult, 3);
				return RLT_EMV_OK;
			}
#if 0//sjz 2020.6.23
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct,JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_OFFLINE_DECLINE;
#else
			return RLT_EMV_OK;
#endif
		}
	}

	//return retCode;

}

unsigned char jcb_CardHolderVerf(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char retCode = RLT_EMV_ERR, k;
	unsigned char IUParameter=0;

	retCode = jcb_CardHolderVerf_Inline(tempApp_UnionStruct);
	if(retCode == RLT_EMV_OK)
    {
        if(tempApp_UnionStruct->EMVTradeParam->JcbDRRI == 2
			|| tempApp_UnionStruct->EMVTradeParam->JcbDRRI == 0x12) //online
        {
            k=emvbase_avl_gettagvalue(EMVTAG_TermType)&0x0F;

            if((k==1||k==2||k==4||k==5)) //online capable //online capable
            {
                tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_ONLINE_WAIT;
                retCode = RLT_EMV_ONLINE_WAIT;
            }
            else
            {
                tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 1;
                tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
            }

        }
        else if(tempApp_UnionStruct->EMVTradeParam->JcbDRRI == 1)
        {
            tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
        }
        else if(tempApp_UnionStruct->EMVTradeParam->JcbDRRI == 0)
        {
            tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_APPROVE;
        }
        else
        {
            retCode = RLT_EMV_ERR;
        }
        EMVBase_Trace("TransResult=%d\r\n",tempApp_UnionStruct->EMVTradeParam->TransResult);
    }

	return retCode;
}
unsigned char jcb_UnableGoOnlineDeal(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	unsigned char TransactionMode;
	JCBUSERINTERFACEREQUESTDATA *tempuserinterfacerequestdata;


    tempuserinterfacerequestdata = tempApp_UnionStruct->EMVTradeParam->pJcbUserInterfaceRequestData;
	TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;

	if(TransactionMode == Legacy_Mode || TransactionMode == Magstripe_Mode)
	{
		 tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
		 return RLT_EMV_OK;
	}

	if(tempuserinterfacerequestdata->sendMsgFlag)//lsy 2021.4.8 The case requires that the background does not respond within the timeout period when online, and after the timeout, the UI of 17 should be printed once more and displayed on the screen.
	{
		jcb_SendUIRequest(tempApp_UnionStruct, tempuserinterfacerequestdata->MessageID, tempuserinterfacerequestdata->Status, NULL);
		jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);//lsy 2021.4.12 The specification states that when the background timeout does not respond, it should be regarded as Decline instead of End
	}

    if(tempApp_UnionStruct->EMVTradeParam->JcbDRRI  == 0)
    {
        tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_APPROVE;
    }
    else
    {
        tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;

    }
    return RLT_EMV_OK;
}

unsigned char jcb_CommandIssuerAuthen(EMVBASE_APDU_SEND *apdu_s, unsigned char *IssuAuthenData, unsigned char IssuAuthenDataLen)
{
    memcpy(apdu_s->Command, "\x00\x82\x00\x00", 4);

    memcpy(apdu_s->DataIn, IssuAuthenData, IssuAuthenDataLen);
    apdu_s->Lc = IssuAuthenDataLen;
    apdu_s->Le = 0;  //no Le


    apdu_s->EnableCancel = 0;
    return RLT_EMV_OK;
}

unsigned char jcb_IssuerAuthen(EMVBASE_APDU_RESP *apdu_r)
{
    if(apdu_r->SW1 == 0x90 && apdu_r->SW2 == 0x00)
    {
        return RLT_EMV_OK;
    }
    else
    {
        return RLT_ERR_EMV_IccReturn;
    }
}

unsigned char jcb_DealIssuerAuthenData(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char retCode = RLT_EMV_OK;
    EMVBASE_APDU_SEND apdu_s;
    EMVBASE_APDU_RESP apdu_r;
    EMVBASETAGCVLITEM *item;

	EMVBase_Trace("Emvb_DealpaywaveIssuerAuthenData begin");

    item = emvbase_avl_gettagitempointer(EMVTAG_IssuerAuthenData);

    if(item == NULL)
    {
        return RLT_EMV_OK;
    }

    if(item->len == 0)
    {
        return RLT_EMV_OK;
    }
   // emvbase_avl_setvalue_or(EMVTAG_TSI, 0, 0x10);  //Issuer authentication was performed
    jcb_CommandIssuerAuthen(&apdu_s, item->data, item->len);

    tempApp_UnionStruct->IsoCommand(&apdu_s, &apdu_r);

    if(apdu_r.ReadCardDataOk == RLT_EMV_OK)
    {
        retCode = jcb_IssuerAuthen(&apdu_r);
    }
    else
    {
        retCode = RLT_ERR_EMV_APDUTIMEOUT;
		emvbase_avl_setvalue_or(EMVTAG_TVR, 4, 0x40);
		return retCode;
    }

    if(retCode != RLT_EMV_OK)
    {
        emvbase_avl_setvalue_or(EMVTAG_TVR, 4, 0x40);
    }

	EMVBase_Trace("Emvb_DealpaywaveIssuerAuthenData end");
    return RLT_EMV_OK;
}

void jcb_SetIsserStatus(JCBTradeUnionStruct *tempApp_UnionStruct)
{
	tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate = 0;
	tempApp_UnionStruct->EMVTradeParam->startdealiup = 0;
	//if(tempApp_UnionStruct->EMVTradeParam->TransResult != RESULT_OFFLINE_APPROVE)
	{
		tempApp_UnionStruct->EMVTradeParam->TransResult = tempApp_UnionStruct->EMVTradeParam->TransResultBak;
	}
}





unsigned char jcb_DealGAC2Rsp(JCBTradeUnionStruct * tempApp_UnionStruct, EMVBASE_APDU_RESP *apdu_r)
{
		unsigned char ret;
		unsigned char k,j,t,indexRet = 0;
		unsigned short index,indexTemp,len,lenTemp,orgindex,tmplen,startpos;
		unsigned char Primitivetype = 0;
		unsigned char countpadding = 0;
		unsigned char bIntable;
		unsigned char GenerateACRetDataflag;
		unsigned char CryptInfo,reqCrpt;
		unsigned char VLPAvailableFund[6];
		EMVBASETAGCVLITEM *item;
		unsigned char AppCryptExist, CryptInfoExist, ATCExist,CardholderVerifStatus,SDADExist;
		unsigned char TransactionMode, ImpleOption, AcquiOption, AIP[2];

		tempApp_UnionStruct->EMVTradeParam->GenerateACRetDataLen = 0;
		memset(tempApp_UnionStruct->EMVTradeParam->GenerateACRetData,0,255);

		TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;
		if(TransactionMode != EMV_Mode)
		{
			return RLT_EMV_ERR;
		}

		//if(TransactionMode == EMV_Mode)
		{
			if(!(apdu_r->SW1==0x90 && apdu_r->SW2==0x00))
			{
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
				return RLT_EMV_TERMINATE_TRANSERR;
				//return RLT_ERR_EMV_IccDataFormat;
			}
		}

		if(apdu_r->DataOut[0] != 0x77)
		{
			return RLT_ERR_EMV_IccDataFormat;
		}

		emvbase_avl_deletetag(EMVTAG_CryptInfo);
		emvbase_avl_deletetag(EMVTAG_ATC);
		emvbase_avl_deletetag(EMVTAG_AppCrypt);

		index=0;
		if(apdu_r->DataOut[index]==0x77)//TLV coded data
		{
			Primitivetype = 1;
			index++;
			if(EMVBase_ParseExtLen(apdu_r->DataOut,&index,&lenTemp))
			{
				return RLT_ERR_EMV_IccDataFormat;
			}
			indexTemp=index;
			indexRet=0;
			while(index<indexTemp+lenTemp)
			{
				if(index>=apdu_r->LenOut)
				{
					EMVBase_Trace( "EMVBase_ParseExtLen 77 error\r\n");
					return RLT_ERR_EMV_IccDataFormat;
				}
				if(apdu_r->DataOut[index]==0xFF || apdu_r->DataOut[index]==0x00)
				{
					index++;
					continue;
				}

				orgindex = index;

				bIntable = 0;
				//GenerateACRetDataflag = 1;

				/*if(!memcmp((unsigned char*)&apdu_r->DataOut[index],"\x9F\x4B",2))
				{
					GenerateACRetDataflag = 0;
				}*/

				ret = jcb_ParseAndStoreCardResponse(tempApp_UnionStruct,apdu_r->DataOut,&index,&bIntable,0);
				if(ret != RLT_EMV_OK)
				{
					return ret;
				}

				//sxl20110406
				/*if(GenerateACRetDataflag&&(indexRet+index-orgindex <=255))
				{
					memcpy(tempApp_UnionStruct->EMVTradeParam->GenerateACRetData+indexRet,(unsigned char*)&apdu_r->DataOut[orgindex],index-orgindex);
					indexRet+=index-orgindex;
				}*/

			}	//endwhile
			if(index!=indexTemp+lenTemp)
			{
				EMVBase_Trace( "\r\nerr117\r\n");
				return RLT_ERR_EMV_IccDataFormat;
			}
		}

		//unsigned char AppCryptExist, CryptInfoExist, ATCExist,CardholderVerifStatus,SDADExist;
		/*AppCryptExist = emvbase_avl_checkiftagexist(TAG_AppCrypt);//9F26
		CryptInfoExist = emvbase_avl_checkiftagexist(TAG_CryptInfo);//9F27
		ATCExist = emvbase_avl_checkiftagexist(TAG_ATC);//9F36
		SDADExist = emvbase_avl_checkiftagexist(TAG_SignDynAppData);//9F4B
		CardholderVerifStatus = emvbase_avl_checkiftagexist(TAG_JCBCardholderVerificationStatus);//9F50
		*/

		if((emvbase_avl_checkiftagexist(EMVTAG_CryptInfo)==0)||
				(emvbase_avl_checkiftagexist(EMVTAG_ATC)==0) || (emvbase_avl_checkiftagexist(EMVTAG_AppCrypt)==0))
		{
			EMVBase_Trace( "jcb_dealgac2rsp err111\r\n");

			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
			return RLT_EMV_TERMINATE_TRANSERR;
			//return JCB_OPS_STATUS_DECLINED;

		}

		CryptInfo = emvbase_avl_gettagvalue(EMVTAG_CryptInfo);
		reqCrpt = emvbase_avl_gettagvalue(EMVTAG_ReferenceControlParameter);
		emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, &AcquiOption, 0, 1);
		ImpleOption = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;

		//if(TransactionMode == EMV_Mode)
		//tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = 0xFF;
		if((CryptInfo&0xc0) == 0x00)//AAC
		{
			if(emvbase_avl_checkiftagexist(EMVTAG_SCRIPT2))
			{
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
				tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
				EMVBase_Trace("TAG_SCRIPT2 exist, return emv_ok");
				return RLT_EMV_OK;
			}
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
				tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			return RLT_EMV_OFFLINE_DECLINE;
		}
		if((CryptInfo&0xc0) == 0x40 && (reqCrpt&TAA_OFFLINE))//TC
		{
			if(tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter == JCB_OPS_CVM_ONLINEPIN)
			{
				tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_NA;
				emvbase_avl_setvalue_and(EMVTAG_TVR, 2, 0xFB);
			}
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_APPROVED);
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_ONLINE_APPROVE;
			tempApp_UnionStruct->EMVTradeParam->JcbDRRI = 0;
		}
		else
		{
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_OFFLINE_DECLINE;
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_DECLINED);
			return RLT_EMV_ONLINE_DECLINE;
		}

		ret = RLT_EMV_OK;

		return ret;

}



unsigned char jcb_ReadScriptNum(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned short index;       // 1106
    unsigned short len;
    unsigned char j, k, retCode = RLT_EMV_OK;
    unsigned char IssuScriptNum;
    unsigned char i;
    unsigned char tag[4];
    unsigned char IssuScript[300];
    unsigned short IssuScriptLen;



    IssuScriptNum = 0;
    memset(tag, 0, sizeof(tag));

    for(i = 0; i < 2; i++)
    {
        if(i == 0)
        {
            tag[0] = 0x71;

            if(emvbase_avl_checkiftagexist(tag) == 0)
            {
                continue;
            }
        }
        else
        {
            tag[0] = 0x72;

            if(emvbase_avl_checkiftagexist(tag) == 0)
            {
                continue;
            }
        }
        retCode = RLT_EMV_OK;
        memset(IssuScript, 0, sizeof(IssuScript));
        emvbase_avl_gettagvalue_all(tag, IssuScript, &IssuScriptLen);
        index = 0;

        while(index < IssuScriptLen)
        {
            if(IssuScript[index] == 0x71 || IssuScript[index] == 0x72)
            {
                index++;

                if(IssuScript[index] <= 127)
                {
                    len = IssuScript[index];
                    len += 2;
                    k = 0;
                }
                else
                {
                    len = 0;
                    k = IssuScript[index] & 0x7F;

                    for(j = 1; j <= k; j++)
                    {
                        len = len * 256 + IssuScript[index + j];
                    }

                    len += k + 2;
                }
                index--;

                if(index + len > IssuScriptLen)
                {
                    //EMVBase_Trace("index = %d,len = %d,IssuScriptLen =%d \r\n",tempterminfo->IssuScriptLen);
                    retCode = JCB_ERR_EMV_ScriptFormat;
                    break;
                }
                IssuScriptNum++;
                index += len;
            }
            else
            {
                retCode = JCB_ERR_EMV_ScriptFormat;
                break;
            }
        }


        if(retCode != RLT_EMV_OK)
        {
            IssuScriptNum++;
        }
    }



    return IssuScriptNum;
}

unsigned char jcb_SendScriptCommand(unsigned char* Script, unsigned char ScriptLen, EMVBASE_APDU_SEND *apdu_s)
{
    memcpy(apdu_s->Command, Script, 4);
    memcpy(apdu_s->DataIn, (unsigned char*)&Script[5], Script[4]);
    apdu_s->Lc = Script[4];
    apdu_s->Le = 0;  //no Le
    apdu_s->EnableCancel = 0;

    return RLT_EMV_OK;
}


#define MAXSCRIPTLEN 128
/*******************************************************************
   ◊˜    ’ﬂ: ÀŒ–¢¡¡
   ∫Ø ˝√˚≥∆: unsigned char Paywave_ScriptProcess(unsigned char tag,AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: Paywave Ω≈±æ¥¶¿Ì
   »Îø⁄≤Œ ˝:
   ∑µ ªÿ ÷µ:
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:  20130731  ∑≈µΩcom
 ********************************************************************/
unsigned char jcb_ScriptProcess(unsigned char tag, JCBTradeUnionStruct *tempApp_UnionStruct)
{
    //unsigned char m;   //j,
    unsigned short lenScript, index, Scriptindex;
    //unsigned char script_amount[6];
    EMVBASE_APDU_SEND apdu_s;
    EMVBASE_APDU_RESP apdu_r;
    EMVBASETAGCVLITEM *item;
    unsigned char *IssuScript;       //[300]
    unsigned short lenScriptPerformed, IssuScriptlen;
    unsigned char *ScriptResult;    //[300]
    unsigned char ScriptResultlen;
    unsigned char curScriptID[4];
    unsigned char ScriptK;
    unsigned char ifscriptlenoverfloor;
    unsigned char numScriptCom;
    unsigned char retCode = RLT_EMV_OK;
    unsigned char dealscripterr;
    unsigned char script_amount[6];
    unsigned char VLPAvailableFund[6] = {0};
    unsigned char VLPTransLimit[6] = {0};
	unsigned short templen86, tempindex86;

	EMVBase_Trace("start jcb_ScriptProcess\r\n");

    if(tag == 0x71)
    {
        item = emvbase_avl_gettagitempointer(EMVTAG_SCRIPT1);
    }
    else if(tag == 0x72)
    {
        item = emvbase_avl_gettagitempointer(EMVTAG_SCRIPT2);
    }
    else
    {
        return RLT_EMV_OK;
    }

    if(item == NULL)
    {
        return RLT_EMV_OK;
    }

    if(item->len == 0)
    {
        return RLT_EMV_OK;
    }
    IssuScript = (unsigned char *)emvbase_malloc(512);
    ScriptResult = (unsigned char *)emvbase_malloc(300);

    index = 0;
    memset(IssuScript, 0, 512);

    if(item->len > 512)
    {
        IssuScriptlen = 512;
    }
    else
    {
        IssuScriptlen = item->len;
    }
    memcpy(IssuScript, item->data, IssuScriptlen);
    EMVBase_TraceHex("Script content", IssuScript, IssuScriptlen);

    ScriptResultlen = 0;
    memset(ScriptResult, 0, 300);


    lenScriptPerformed = 0; //zcl add 20160513
    while(index < IssuScriptlen)
    {
        EMVBase_Trace( "\r\nEmvb_Paywave_ScriptProcess index = %d IssuScriptlen = %d\r\n", index, IssuScriptlen);

        if(IssuScript[index] == tag)
        {
            index++;

            if(EMVBase_ParseExtLen(IssuScript, &index, &lenScript))
            {
                retCode = JCB_ERR_EMV_ScriptFormat;
                break;
            }
            Scriptindex = index;

            memset(curScriptID, 0, sizeof(curScriptID));

            ifscriptlenoverfloor = 1;
            numScriptCom = 0;
            dealscripterr = 0;
            //lenScriptPerformed = 0;   //zcl add 20160513

            while(index < (Scriptindex + lenScript))
            {
		       EMVBase_Trace( "\r\n1111index = %d %02x\r\n", index, IssuScript[index]);

                if(ifscriptlenoverfloor)
                {
                    ifscriptlenoverfloor = 0;
                    lenScriptPerformed += (lenScript + 2);

					EMVBase_Trace( "\r\nScript len = %d\r\n", lenScriptPerformed);

                    if(lenScriptPerformed > MAXSCRIPTLEN)
                    {
                        dealscripterr = 1;
                        break;
                    }
                }

                if(!memcmp(&IssuScript[index], "\x9F\x18\x04", 3))
                {
                    memcpy(curScriptID, &IssuScript[index + 3], 4);
                    index += 7;
                }
                else if(!memcmp(&IssuScript[index], "\x9F\x18\x00", 3))
                {
                    index += 3;
                }
                else if(IssuScript[index] == 0x86)
                {
#if 0

                    if(lenScriptPerformed + lenScript > MAXSCRIPTLEN)
                    {
                        dealscripterr = 1;
                        break;
                    }
#endif
					tempindex86 = index + 1;	//2021.8.18 lishiyao should parse len rather than tag
					templen86 = 0;
					if(EMVBase_ParseExtLen(IssuScript, &tempindex86, &templen86))
					{
						EMVBase_Trace( "JCB-info:Parse Tag86 data len error\r\n");
						retCode = JCB_ERR_EMV_ScriptFormat;
						dealscripterr = 1;
                		break;
					}
                    ScriptK = IssuScript[index + 1];                    // script len //  0D

					#if 0
                    if((ScriptK < 5) || (IssuScript[index + 3] & 0x01) == 0x01
                       || (IssuScript[index + 3] & 0xF0) == 0x60 || (IssuScript[index + 3] & 0xF0) == 0x90 ||
                       (((ScriptK != IssuScript[index + 6]+5) && (ScriptK != IssuScript[index + 6]+6)) || ( (index+ScriptK+2) > (Scriptindex + lenScript))))
					#endif
					if((ScriptK < 5) || (IssuScript[index + 3] & 0x01) == 0x01
                       || (IssuScript[index + 3] & 0xF0) == 0x60 || (IssuScript[index + 3] & 0xF0) == 0x90 ||
                       (((ScriptK != IssuScript[index + 6]+5) && (ScriptK != IssuScript[index + 6]+6)) || ( (index+ScriptK+2) > (Scriptindex + lenScript))))
                    {
						EMVBase_Trace( "IssuScript Error\r\n");
                        dealscripterr = 1;
                        break;
                    }

					EMVBase_Trace( "Start jcb_SendScriptCommand\r\n");
                    jcb_SendScriptCommand((unsigned char*)&IssuScript[index + 2], ScriptK, &apdu_s);

                    tempApp_UnionStruct->IsoCommand(&apdu_s, &apdu_r);

					EMVBase_Trace( "apdu_r.ReadCardDataOk=%d apdu_r.SW1=%02x apdu_r.SW2=%02x\r\n", apdu_r.ReadCardDataOk, apdu_r.SW1, apdu_r.SW2);
					if(apdu_r.ReadCardDataOk != RLT_EMV_OK)
					{
						retCode = JCB_OPS_STATUS_ENDAPPLICATION;
						if(tag == 0x71)//ISSUER_SCRIPT_080	72 fail don't send end application when timeout happens
						{
							jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
						}
						//break;
						return retCode;
					}

                    if((apdu_r.SW1 == 0x90 && apdu_r.SW2 == 0x00) || apdu_r.SW1 == 0x62 || apdu_r.SW1 == 0x63)
                    {
                        index += (ScriptK + 2);
                        numScriptCom++;
                    }
                    else
                    {
                    	EMVBase_Trace( "sw is not 9000,62,63! break\r\n");
                        numScriptCom++;
                        dealscripterr = 1;
                        break;
                    }
                }
                else
                {
					EMVBase_Trace( "jcb_ScriptProcess error flag2\r\n");
                    dealscripterr = 1;
                    break;
                }
#if 0

                if(ifscriptlenoverfloor)
                {
                    ifscriptlenoverfloor = 0;
                    lenScriptPerformed += index;

                    if(lenScriptPerformed > MAXSCRIPTLEN)
                    {
                        dealscripterr = 1;
                        break;
                    }
                }
#endif
            }

            EMVBase_Trace( "\r\ndeal script err = %d %d %02x\r\n", dealscripterr, index, IssuScript[index]);

            if(dealscripterr)
            {
            	EMVBase_Trace( "dealscript error! tag=%02x\r\n", tag);
                //emvbase_avl_setvalue_or(EMVTAG_TSI, 0, 0x04);

                if(tag == 0x71)
                {
                    emvbase_avl_setvalue_or(EMVTAG_TVR, 4, 0x20);                  //Script processing failed before final GENERATE AC
                }
                else                //tag=0x72
                {
                    emvbase_avl_setvalue_or(EMVTAG_TVR, 4, 0x10);                  //Script processing failed after final GENERATE AC
                }

                if(numScriptCom)
                {
                    ScriptResult[ScriptResultlen] = 0x10;

                    if(numScriptCom < 15)
                    {
                        ScriptResult[ScriptResultlen] |= numScriptCom;
                    }
                    else
                    {
                        ScriptResult[ScriptResultlen] |= 0x0F;
                    }
                }
                memcpy(&ScriptResult[ScriptResultlen + 1], curScriptID, 4);
                ScriptResultlen += 5;

                index = Scriptindex + lenScript;
                /*=======BEGIN: sujianzhong 2016.11.04  15:46 modify===========*/
                //»Áπ˚Ω≈±æ÷¥–– ß∞‹£¨ªÚ’ﬂΩ≈±æƒ⁄»›≤ª∫œ∑®£¨ƒ«√¥∫Û–¯À˘”–µƒ(∂¯∑«÷∏∏√Ω≈±æ¿Ô√Êµƒ÷∏¡Ó)Ω≈±æ∂º≤ª÷¥––
                //¿˝»Á:∫ÛÃ®œ¬∑¢71°¢72°¢71°¢72Àƒ∏ˆΩ≈±æ£¨ºŸ»Áµ⁄“ª∏ˆ71÷¥–– ß∞‹£¨ƒ«√¥∫Û√Êµƒ72°¢71°¢72∂º≤ª÷¥––
                retCode = JCB_ERR_EMV_ScriptFormat;
                //break;  //zwb for jcb 171108
                /*====================== END======================== */
            }
            else
            {
                //emvbase_avl_setvalue_or(EMVTAG_TSI, 0, 0x04);

                if(numScriptCom)
                {
                    ScriptResult[ScriptResultlen] = 0x20;

                    memcpy(&ScriptResult[ScriptResultlen + 1], curScriptID, 4);
                    ScriptResultlen += 5;
                }
            }
        }
        else
        {
        	EMVBase_Trace( "else JCB_ERR_EMV_ScriptFormat\r\n");
            retCode = JCB_ERR_EMV_ScriptFormat;
			break;
        }
    }
	// ∂˛¥Œª”ø®ΩÁ√Ê∞¥»°œ˚£¨Ω≈±æΩ·π˚Œ™00 zwb for visa
	if(tempApp_UnionStruct->EMVTradeParam->canceldealiup)
	{
		ScriptResult[0] = 0x00;
		tempApp_UnionStruct->EMVTradeParam->canceldealiup = 0;
	}

    if(ScriptResultlen)
    {
        if(tag == 0x71)
        {
            emvbase_avl_createsettagvalue(EMVTAG_SCRIPT71RESULT, ScriptResult, ScriptResultlen);
        }
        else
        {
            emvbase_avl_createsettagvalue(EMVTAG_SCRIPT72RESULT, ScriptResult, ScriptResultlen);
        }
    }
    emvbase_free(IssuScript);
    emvbase_free(ScriptResult);
	EMVBase_Trace("Emvb_Paywave_ScriptProcess end");

    return retCode;
}


unsigned char jcb_DealIssuScript(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    //unsigned char retCode;


#if 1 //ƒø«∞ «Ω≈±æ¥¶¿Ì ß∞‹‘Ú∫Û–¯µƒΩ≈±æ≤ª‘Ÿ¥¶¿Ì£¨÷±Ω”ÕÀ≥ˆ
    jcb_ScriptProcess(0x71, tempApp_UnionStruct);
    //Emvb_jcb_ScriptProcess(0x72, tempApp_UnionStruct);
#else
    if(RLT_EMV_OK == Emvb_Paywave_ScriptProcess(0x71, tempApp_UnionStruct))
    {
        Emvb_Paywave_ScriptProcess(0x72, tempApp_UnionStruct);
    }
#endif
    return RLT_EMV_OK;
}


/*******************************************************************
   ◊˜    ’ﬂ: ÀŒ–¢¡¡
   ∫Ø ˝√˚≥∆: unsigned char Paywave_OnLineDeal(AppUnionStruct *tempApp_UnionStruct)
   ∫Ø ˝π¶ƒ‹: ¡™ª˙≥…π¶∫Û¥¶¿Ì
   »Îø⁄≤Œ ˝:
   ∑µ ªÿ ÷µ:
   œ‡πÿµ˜”√:
   ±∏    ◊¢:
   –ﬁ∏ƒ–≈œ¢:
 ********************************************************************/
unsigned char jcb_OnLineDeal(JCBTradeUnionStruct *tempApp_UnionStruct)  //
{
    unsigned char retCode;
    EMVBASE_APDU_SEND apdu_s;
	EMVBASE_APDU_RESP apdu_r;
    unsigned char issuerauthendataexist, issuerscript1dataexist, issuerscript2dataexist;
    unsigned char TermTransPredicable[4], CardTransPredicable[2], AuthRespCode[2];
	//unsigned char AuthRespCode[2];
	unsigned char CDOL2bExist;
    EMVBASETAGCVLITEM *item;
	unsigned short len;
	unsigned char TransactionMode,IUParamter, IUParamterExist = 0;
	unsigned char ImpleOption,CombOptions;
	//unsigned char CommandP1;
	unsigned char CryptInfo;
	unsigned char DTIP[3];


    CryptInfo = emvbase_avl_gettagvalue(EMVTAG_CryptInfo);
	memset(DTIP, 0x00, 3);
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBDynamicTIP, DTIP, 0, 3);

	ImpleOption = tempApp_UnionStruct->EMVTradeParam->JcbImplementationOption;
	emvbase_avl_gettagvalue_spec(EMVTAG_JCBCombinationOptions, &CombOptions, 0, 1);
	TransactionMode = tempApp_UnionStruct->EMVTradeParam->JcbTransactionMode;

	IUParamterExist = !(emvbase_avl_gettagvalue_spec(EMVTAG_JCBIssuerUpdateParameter, &IUParamter, 0, 1));

	EMVBase_Trace("IUParamter=%02x ImpleOption=%02x CombOptions=%02x CryptInfo=%02x DTIP[1]=%02x\r\n", IUParamter, ImpleOption, CombOptions, CryptInfo, DTIP[1]);
	EMVBase_Trace("EMVTradeParam->JcbIssuerUpdate = %d\r\n", tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate);

    if(tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate == 0)
    {
        //check if need do issuer authen or issuer script
        issuerauthendataexist = emvbase_avl_checkiftagexist(EMVTAG_IssuerAuthenData);
        issuerscript1dataexist = emvbase_avl_checkiftagexist(EMVTAG_SCRIPT1);
        issuerscript2dataexist = emvbase_avl_checkiftagexist(EMVTAG_SCRIPT2);
		//emvbase_avl_gettagvalue_spec(EMVTAG_JCBIssuerUpdateParameter, &IUParamter, 0, 1);

        if(issuerauthendataexist || issuerscript1dataexist || issuerscript2dataexist)
        {
        	EMVBase_Trace( "jcb_OnLineDeal start issuerscript\r\n");
			//if((IUParamter == 0x01) /*|| (IUParamter == 0x02)*/)
			{
				if((IUParamterExist == 1) && (TransactionMode==EMV_Mode) && ((ImpleOption & 0x88) == 0x88) && ((CombOptions & 0x02) == 0x02) && ((DTIP[1]&0x80) == 0x80) &&((CryptInfo&0x80) == 0x80))
				{
					EMVBase_Trace( "4444444446666\r\n");
					tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate = 1;
					tempApp_UnionStruct->EMVTradeParam->secondtap = 1;
					tempApp_UnionStruct->EMVTradeParam->JcbIUPperformed = 1;
					EMVBase_Trace( "IUParamter = %x\r\n",IUParamter);
					if(IUParamter != 0x01 && IUParamter != 0x02)
					{
						tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate = 0;
						tempApp_UnionStruct->EMVTradeParam->secondtap = 0;
						tempApp_UnionStruct->EMVTradeParam->JcbIUPperformed = 0;
					}
				}
			}

        }
		else
		{
			if((IUParamterExist == 1) && (TransactionMode==EMV_Mode) && ((ImpleOption & 0x88) == 0x88) && ((CombOptions & 0x02) == 0x02) && ((DTIP[1]&0x80) == 0x80) &&((CryptInfo&0x80) == 0x80))
			{
				if(IUParamter == 0x02 || IUParamter == 0x01)
				{
					retCode = JCB_OPS_STATUS_ENDAPPLICATION;
					jcb_SendTransactionOutCome(tempApp_UnionStruct, retCode);
					return RLT_EMV_TERMINATE_TRANSERR;
				}
			}
		}

        memset(AuthRespCode, 0, sizeof(AuthRespCode));
        emvbase_avl_gettagvalue_spec(EMVTAG_AuthRespCode, AuthRespCode, 0, 2);
        EMVBase_Trace( "\r\nAuthRespCode = %02x %02x \r\n", AuthRespCode[0], AuthRespCode[1]);
		EMVBase_Trace("tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate = %d\n",tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate);

		//sxl?  Issuer Update Processing “™◊Óœ»≤‚ ‘
        if(!memcmp(AuthRespCode, "00", 2) ||
           !memcmp(AuthRespCode, "10", 2) ||
           !memcmp(AuthRespCode, "11", 2) )   //Issuer approval,according to p172 of VIS140 terminal specification.
        {
            tempApp_UnionStruct->EMVTradeParam->TransResultBak = RESULT_ONLINE_APPROVE;
        }
        else
        {
            tempApp_UnionStruct->EMVTradeParam->TransResultBak = RESULT_ONLINE_DECLINE;
        }

        if(tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate == 0)
        {
            tempApp_UnionStruct->EMVTradeParam->TransResult = tempApp_UnionStruct->EMVTradeParam->TransResultBak;
			EMVBase_Trace( "xxxxx111111\r\n");
			return RLT_EMV_OK;
		}
		if(IUParamter == 0x01) //prement card and hold
		{
		}
		else if(IUParamter == 0x02) // two presentment
		{
			EMVBase_Trace( "xxxxx22222\r\n");
			return RLT_EMV_OK;
		}
    }
    //else
    {
		if(DTIP[1] & 0x80 == 0)	//for jcb 1.6
		{
			return RLT_EMV_TERMINATE_TRANSERR;
		}

        tempApp_UnionStruct->EMVTradeParam->startdealiup = 1;
		if(tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate == 1)
		{
			EMVBase_Trace( "goto deal script71!\r\n");
			retCode = jcb_ScriptProcess(0x71, tempApp_UnionStruct);
			EMVBase_Trace( "jcb_ScriptProcess retcode=%d\r\n", retCode);
				if(retCode == JCB_ERR_NEEDREVERSAL || retCode == JCB_ERR_EC_QUANCUNERR||retCode == JCB_OPS_STATUS_ENDAPPLICATION)
        	{
            	return retCode;
        	}

			EMVBase_Trace( "IssuerAuthenDat exit = %d\r\n",emvbase_avl_checkiftagexist(EMVTAG_IssuerAuthenData));
			EMVBase_Trace( "EMVTAG_SCRIPT2 exit = %d\r\n",emvbase_avl_checkiftagexist(EMVTAG_SCRIPT2) == 0);
			//5.11.2.2
			if(emvbase_avl_checkiftagexist(EMVTAG_IssuerAuthenData) == 0 && emvbase_avl_checkiftagexist(EMVTAG_SCRIPT2) == 0)
			{
				retCode = JCB_OPS_STATUS_ENDAPPLICATION;
				jcb_SendTransactionOutCome(tempApp_UnionStruct, retCode);
				return RLT_EMV_TERMINATE_TRANSERR;
			}
		}

		memset(AuthRespCode, 0, sizeof(AuthRespCode));
		emvbase_avl_gettagvalue_spec(EMVTAG_AuthRespCode, AuthRespCode, 0, 2);
		EMVBase_Trace( "jcb_OnLineDeal issuerupdate111111\r\n");

		memset(&apdu_s, 0, sizeof(EMVBASE_APDU_SEND));
		memcpy(apdu_s.Command, "\x80\xAE", 2);

		if(!memcmp(AuthRespCode, "00", 2) ||
		   !memcmp(AuthRespCode, "10", 2) ||
		   !memcmp(AuthRespCode, "11", 2) ) //Issuer approval,according to p172 of VIS140 terminal specification.
		{
			memcpy(&apdu_s.Command[2], "\x40\x00", 2);
		}
		else if(!memcmp(AuthRespCode, "01", 2) ||
				!memcmp(AuthRespCode, "02", 2))
		{
			retCode = tempApp_UnionStruct->EMVB_InputCreditPwd(EMVB_NEEDREFER, NULL);
			EMVBase_Trace( "EMVB_InputCreditPwd retCode=%d\r\n", retCode);

			if(retCode == RLT_EMV_OK)
			{
				memcpy(&apdu_s.Command[2], "\x40\x00", 2);
			}
			else
			{
				memcpy(&apdu_s.Command[2], "\x00\x00", 2);
			}
		}
		else
		{
			memcpy(&apdu_s.Command[2], "\x00\x00", 2);
		}

		item = emvbase_avl_gettagitemandstatus(EMVTAG_CDOL2, &CDOL2bExist);
		if(CDOL2bExist == 0)
		{
			EMVBase_Trace( "jcb_OnLineDeal EMVTAG_CDOL2 not exist!\r\n");
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
			return RLT_EMV_TERMINATE_TRANSERR;
		}
		EMVBase_Trace( "jcb_OnLineDeal issuerupdate2222\r\n");

#if 0
		if(tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter == JCB_OPS_CVM_ONLINEPIN)
			{
				EMVBase_Trace( "reset online pin in TVR");
				tempApp_UnionStruct->EMVTradeParam->JcbCVMParameter = JCB_OPS_CVM_NA;
				emvbase_avl_setvalue_and(EMVTAG_TVR, 2, 0xFB);
			}

#endif

		EMVBase_TraceHex( "EMVTAG_CDOL2 ",item->data,item->len);
		if(jcb_GACDOLProcess(typeCDOL2, item->data, item->len, apdu_s.DataIn, &len, tempApp_UnionStruct) != RLT_EMV_OK)
		{
			/*if(apdu_s.Command[2] & 0x10)	   //CDA
			{
				apdu_s.Command[2] &= 0xEF;

				memcpy(&apdu_s.Command[2], "\x00\x00", 2);			//get AAC

				emvbase_avl_setvalue_or(EMVTAG_TVR, 0, 0x04);
				//emvbase_avl_setvalue_or(EMVTAG_TSI, 0, 0x80);

				Emvb_DOLProcess(typeCDOL2, item->data, item->len, apdu_s.DataIn, &len, tempApp_UnionStruct);
			}*/
			EMVBase_Trace( "CDOL2 process error!\r\n");
			return RLT_EMV_TERMINATE_TRANSERR;
		}
		apdu_s.Lc = len;

		emvbase_avl_createsettagvalue(EMVTAG_CDOL2RelatedData, apdu_s.DataIn, apdu_s.Lc);
    	apdu_s.Le = 256;
    	apdu_s.EnableCancel = 1;

    	emvbase_avl_settag(EMVTAG_ReferenceControlParameter, apdu_s.Command[2]);
		tempApp_UnionStruct->IsoCommand(&apdu_s, &apdu_r);
	    if(apdu_r.ReadCardDataOk != RLT_EMV_OK)
    	{
        	retCode = RLT_ERR_EMV_APDUTIMEOUT;
			jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
    	}
    	else
    	{
        	retCode = RLT_EMV_OK;
    	}
		EMVBase_Trace( "jcb_OnLineDeal issuerupdate33 retcode=%d\r\n", retCode);

    	if(retCode == RLT_EMV_OK)
    	{
        	retCode = jcb_DealGAC2Rsp(tempApp_UnionStruct, &apdu_r);
    	}
		EMVBase_Trace( "jcb_OnLineDeal issuerupdate44 retcode=%d\r\n", retCode);

		if(retCode != RLT_EMV_OK)
		{
			if(retCode == RLT_ERR_EMV_IccDataFormat)
			{
				jcb_SendTransactionOutCome(tempApp_UnionStruct, JCB_OPS_STATUS_ENDAPPLICATION);
				return RLT_EMV_TERMINATE_TRANSERR;
			}
			jcb_SetIsserStatus(tempApp_UnionStruct);
			tempApp_UnionStruct->EMVTradeParam->TransResult = RESULT_ONLINE_DECLINE;
			return retCode;
		}

		EMVBase_Trace( "jcb_OnLineDeal issuerupdate55\r\n");

		jcb_SetIsserStatus(tempApp_UnionStruct);
	#if 0
        tempApp_UnionStruct->EMVTradeParam->PaywaveIUP = 0;
		tempApp_UnionStruct->EMVTradeParam->JcbIssuerUpdate = 0;
        tempApp_UnionStruct->EMVTradeParam->startdealiup = 0;
        tempApp_UnionStruct->EMVTradeParam->TransResult = tempApp_UnionStruct->EMVTradeParam->TransResultBak;
	#endif
		return RLT_EMV_OK;
    }
}

unsigned char jcb_PreTransComplete(JCBTradeUnionStruct *tempApp_UnionStruct)
{
    unsigned char retCode;
    unsigned char issuerscript2dataexist;
    unsigned char AuthRespCode[2];
    unsigned char bReserval = 0;

    issuerscript2dataexist = emvbase_avl_checkiftagexist(EMVTAG_SCRIPT2);

    	EMVBase_Trace( "JcbIUPperformed=%d\r\n", tempApp_UnionStruct->EMVTradeParam->JcbIUPperformed);
    if(issuerscript2dataexist && tempApp_UnionStruct->EMVTradeParam->JcbIUPperformed)
    {
        jcb_ScriptProcess(0x72, tempApp_UnionStruct);  //sxl20131024 pinpadµƒ‘› ±≤ª◊ˆ¥¶¿Ì  EMV_EC_QCACCNO’‚–©Ω≈±æ“™≤ª“™«¯∑÷ø™¿¥
    }
    memset(AuthRespCode, 0, sizeof(AuthRespCode));
    emvbase_avl_gettagvalue_spec(EMVTAG_AuthRespCode, AuthRespCode, 0, 2);

    //ºÏ≤È «∑Ò–Ë“™∑¢ÀÕ≥Â’˝
    if(tempApp_UnionStruct->EMVTradeParam->onlinetradestatus != RLT_EMV_OK)
    {
        bReserval = 1;
    }
    else
    {
        if((!memcmp(AuthRespCode, "00", 2) ||
            !memcmp(AuthRespCode, "10", 2) ||
            !memcmp(AuthRespCode, "11", 2)) && (tempApp_UnionStruct->EMVTradeParam->TransResult != RESULT_ONLINE_APPROVE) )
        {
            bReserval = 1;
        }
    }

    if(bReserval)
    {
        return JCB_ERR_NEEDREVERSAL;
    }
    return RLT_EMV_OK;
}



