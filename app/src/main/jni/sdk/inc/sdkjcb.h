#ifndef SDKJCB_H
#define SDKJCB_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


extern s32 sdkJcbTransInit();
extern s32 sdkJcbTransFlow1();
extern s32 sdkJcbTransFlow2();
extern s32 sdkJcbSetDispRemoveCard(s32(*fun_setdispremovecard)());//fun_setdispremovecard返回值:SDK_OK:成功; SDK_ERR:失败
extern s32 sdkJcbSetBeforeGPO(s32(*fun_setbeforegpo)());//fun_setbeforegpo返回值:SDK_OK:成功; SDK_ERR:失败
extern s32 sdkJcbSetVerifyCardNo(s32(*fun_setVerifyCardNo)(u8* asPAN));//fun_setVerifyCardNo返回值:SDK_OK:成功; SDK_ERR:失败
extern s32 sdkJcbGetTransMode(void);
extern s32 sdkJcbSetRevocationKey(s32(*fun_cmprevocation)(u8 *key));
extern s32 sdkJcbSetForceOnline(bool bIsForceOnline);
extern s32 sdkJcbImportOnlineResult(s32 ucOnlineResult, const u8 *pheRspCode);
extern s32 sdkJcbSetDispTapCardAgain(s32(*fun_setDispTapCardAgain)());//fun_setDispTapCardAgain显示请读卡,然后复位: SDK_OK:成功; SDK_ERR:失败
extern s32 sdkJcbSetOption(u8 option);
extern s32 sdkJcbSetStatic9F53(u8 *data);
extern s32 sdkJcbGetStatic9F53(u8 *data);
extern s32 sdkJcbTransReadAppData();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif



