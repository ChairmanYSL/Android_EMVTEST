#include "extern.h"

DDI_API gstddiapi = {0};

//FIXME: consider make init ddi to a callback,set from app
s32 sdkemvbaseInitDDI(void)
{
	void* pHandler;

	pHandler = dlopen("libAsdkClient_8623.so", RTLD_LAZY);
	if(pHandler == NULL)
	{
		Trace("lishiyao", "ddi-fatal:open libAsdkClient_8623.so fail\n");
		pHandler = dlopen("/system/lib/libAsdkClient.so", RTLD_LAZY);
		if(pHandler == NULL)
		{
			Trace("lishiyao", "ddi-fatal:open /system/lib/libAsdkClient.so fail\n");
			return;
		}
	}

	gstddiapi.ddi_apdu_exchange = dlsym(pHandler,"API_ICC_Apdu");
	gstddiapi.ddi_get_random= dlsym(pHandler,"API_Rng");
	gstddiapi.ddi_get_timerId= dlsym(pHandler,"API_TimeGet");
	gstddiapi.ddi_encry = dlsym(pHandler,"API_Crypt");
	gstddiapi.ddi_hash = dlsym(pHandler,"API_Hash");
}

s32 sdkDevContactlessSendAPDU(const u8 *pheInBuf, s32 siInLen, u8 *pheOutBuf, s32 *psiOutLen)
{
    s32 ret = 0;
	u32 size = 512;
	u32 sw = 0;

    if((NULL == pheInBuf) || (NULL == pheOutBuf) || (NULL == psiOutLen) || siInLen < 0)
    {
        return SDK_PARA_ERR;
    }

	ret = gstddiapi.ddi_apdu_exchange(1, pheInBuf, (u32)siInLen,pheOutBuf, size, (u32 *)psiOutLen, &sw);

	Trace("extern", "ddi_apdu_exchange ret = %d\r\n", ret);

    if(ret == DDI_OK)
    {
    	TraceHex("emv", "contactless r-apdu:", pheOutBuf, *psiOutLen);
        return SDK_OK;
    }

    return SDK_ERR;
}

void *sdkGetMem(u32 size_t)
{
	return (void *)malloc(size_t);
}

void sdkFreeMem(void *block)
{
	free(block);
}

s32 sdkInsertFile(const u8 *pasFile, const u8 *pheSrc, s32 siStart, s32 siSrclen)
{
	s32 fp,i,ret;

    if (NULL == pasFile || NULL == pheSrc || siStart < 0 || siSrclen < 0) {
        return SDK_PARA_ERR;
    }

    if (siStart != 0)
	{
        i = sdkGetFileSize(pasFile);
        if (siStart > i)
        {
            return SDK_PARA_ERR;
   		}
    }

	fp = fopen(pasFile, "rb+");
	if(NULL == fp)
	{
		Trace("ddi", "open file error\r\n");
		return SDK_ERR;
	}

	fp = fseek(fp, siStart, SEEK_SET);

	ret = fwrite(pheSrc, sizeof(u8), siSrclen, fp);
	if(ret == siSrclen)
	{
		fclose(fp);
		return SDK_OK;
	}
	else
	{
		fclose(fp);
		return SDK_ERR;
	}
}

s32 sdkReadFile(const u8 *pasFile, u8 *pheDest, s32 siOffset, s32 *psiDestlen)
{
	s32 fp;
	u32 i;

    if (NULL == pasFile || NULL == pheDest || NULL == psiDestlen || siOffset < 0)
	{
        return SDK_PARA_ERR;
    }

	fp = fopen(pasFile, "r");
	if(fp == NULL)
	{
		Trace("lishiyao", "open file error!\r\n");
		return SDK_FUN_NULL;																						//�ļ���ʧ��
	}

	fseek(fp, siOffset, SEEK_SET);

	i = *psiDestlen;
	*psiDestlen = fread(pheDest, sizeof(u8), i, fp);
	if(*psiDestlen != i) //consider it as get end of file
	{
		fclose(fp);
		return SDK_ERR;
	}
	else
	{
		fclose(fp);
		return SDK_OK;
	}
}

s32 sdkGetFileSize(const u8 *pasFile)
{
	s32 fileSize;
	s32 fp;

	if (NULL == pasFile)
	{
		return SDK_PARA_ERR;
	}

	fp = fopen(pasFile, "rb");
	if(NULL == fp)
	{
		return SDK_FUN_NULL;																						//�ļ���ʧ��
	}

	fseek(fp, 0, SEEK_END);

	fileSize = ftell(fp);

	fclose(fp);

	return fileSize;
}

s32 sdkDelFile(const u8 *pasFile)
{
	s32 ret;

	if(pasFile == NULL)
	{
		return SDK_PARA_ERR;
	}

	ret = remove(pasFile);

	if(ret == 0)
	{
		return SDK_OK;
	}
	else
	{
		return SDK_ERR;
	}
}

s32 sdkSysGetCurAppDir(u8 *FileDir)
{
	if(FileDir == NULL)
	{
		return SDK_PARA_ERR;
	}

    strcpy(FileDir, "/emv/discover/");
    return SDK_OK;
}

s32 sdkGetRtc(u8 *pbuf)
{
	s32 ret;
	DDI_DATE_TIME timer = {0};
	u32 decNum = 0;
	u8 temp;

	if(pbuf == NULL)
	{
		return SDK_PARA_ERR;
	}

	ret = gstddiapi.ddi_get_timerId(&timer);
	Trace("lishiyao", "ddi_get_timerId ret = %d\r\n", ret);

	if(timer.Year >= 1950 && timer.Year < 2000)
	{
		decNum = (unsigned int)((unsigned int)(timer.Year - 1950) + 50);
		sdkU32ToBcd(&pbuf[0], decNum, 1);
	}
	else if(timer.Year >= 2000 && timer.Year < 2049)
	{
		decNum = (unsigned int)(timer.Year - 2000) ;
		sdkU32ToBcd(&pbuf[0], decNum, 1);
	}
	else
	{
		Trace("lishiyao", "DDI-FATAL:time format error!");
		return SDK_ERR;
	}

	sdkU32ToBcd(&pbuf[1], timer.Month, 1);
	sdkU32ToBcd(&pbuf[2], timer.Day, 1);


	TraceHex("lishiyao", "DDI-INFO:", pbuf, 3);
	return SDK_OK;
}

void sdkGetRandom(u8 *pbuf, u32 buflen)
{
	s32 ret;
	ret = gstddiapi.ddi_get_random(pbuf, (s32)buflen);
	Trace("lishiyao", "ddi_get_random ret = %d\r\n", ret);
	TraceHex("lishiyao", "randnum", pbuf, buflen);
}

void sdkReadPosSn(u8 *pbuf)
{
	return;
}

s32 sdkDevContactSendAPDU(const u8 *pheInBuf, s32 siInLen, u8 *pheOutBuf, s32 *psiOutLen)
{
	return 0;
}

void sdkDev_Printf(char *fmt, ...)
{
	va_list args;
	unsigned char printbuffer[256] = {0};

	va_start ( args, fmt );
	vsprintf ( printbuffer, fmt, args );
	va_end ( args );
	printf("%s",printbuffer);
}

