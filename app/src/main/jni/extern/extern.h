#ifndef EXTERN_H
#define EXTERN_H

#include "sdktypedef.h"
#include "sdkDebug.h"
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>

#define DDI_OK 0

typedef struct
{
	int   Year; 	//!< The number of years						range 1900 to xxxx
	int   Month;	//!< The number of months,						range 1 to 12
	int   Day;		//!< The day of the month,						range 1 to 31
	int   Hour; 	//!< The number of hours past midnight, 		range 0 to 23
	int   Min;		//!< The number of minutes after the hour,		range 0 to 59
	int   Sec;		//!< The number of seconds after the minute,	range 0 to 59, (60 to allow for leap seconds)
	int   MSec; 	//!< The number of MSec after the minute,		range
	int   WeekDay;	//!< The number of days since Sunday,			range 0 to 6
	int   YearDay;	//!< The number of days since January 1,		range 0 to 365
}DDI_DATE_TIME;


typedef struct
{
	int (*ddi_apdu_exchange)(s32 tCardIndex,const u8* pCmdBuf,u32 nCmdLen,u8* pOutBuf,u32 nOutSize, u32* pOutLen, u32* pSW);
	int (*ddi_get_random)(u8 *buf, int len);
	int (*ddi_get_timerId)(DDI_DATE_TIME* pTime);
	int (*ddi_encry)(s32 type, const u8* key, u32 keylen, const u8* iv, const u8* input, u32 inlen, u8* output, u32 outsize, u32* outlen, s32 mode);	//used for encrypt online pin
	int (*ddi_hash)(s32 type, const u8* input, u32 inlen, u8* output, u32 outsize, u32* outlen, s32 flag);
}DDI_API;



#endif
