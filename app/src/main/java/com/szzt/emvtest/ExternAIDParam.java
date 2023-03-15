package com.szzt.emvtest;

public class ExternAIDParam {
    public byte AidLen;
    public byte[] Aid = new byte[16];
    public byte[] TransCurcyCode = new byte[2];
    public byte TransCurcyExp;
    public byte[] TransReferCurcyCode = new byte[2];
    public byte TransReferCurcyExp;
    public byte[] AcquireID = new byte[6];
    public byte[] TermID = new byte[8];
    public byte[] MerchCateCode = new byte[2];
    public byte[] MerchID = new byte[15];
    public byte MerchantNameLen;
    public byte[] MerchantName = new byte[20];
    public byte TRMDataLen;
    public byte[] TRMData = new byte[8];
    public byte TermTDOLLen;
    public byte[] TermTDOL = new byte[64];
    public byte[] TermTransPredicable = new byte[4];
    public byte[] terminalcapability = new byte[3];
    public byte[] terminaladdcapability = new byte[5];
    public byte terminaltype;
    public byte[] CombOptions = new byte[2];
    public byte[] RemovalTimeout = new byte[2];
    public byte[] TIPStatic = new byte[3];


    public static int size(){
        return 166;
    }
    public ExternAIDParam()
    {
        AidLen = 0;
        TransCurcyExp = 0;
        TransReferCurcyExp = 0;
        terminaltype = 0;
        TermTDOLLen = 0;
        TRMDataLen = 0;
        MerchantNameLen = 0;
    }
    public static ExternAIDParam [] Create(int nums){
        ExternAIDParam [] externAIDParams = new ExternAIDParam[nums];
        return externAIDParams;
    }
    public static ExternAIDParam [] Update( ExternAIDParam [] src, int index, ExternAIDParam update){
        int i = index;
        src[i] = update;
        return src;
    }
}
