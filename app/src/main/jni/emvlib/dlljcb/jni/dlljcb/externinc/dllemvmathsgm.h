#ifndef EMVMATHGM_H
#define  EMVMATHGM_H

extern void EMVBaseMathGM_GetLibVersion(unsigned char *version);
extern unsigned char EMVBase_SM_SignatureVerf(unsigned char* dgtsgnt, unsigned char* Datatbsign, unsigned short DatatbsignLen, unsigned char *pkdata, unsigned short pkdatalen);

#endif

