package com.szzt.emvtest;

public class SimData {
    byte[] AcquirerID;
    byte[] IFDSN;
    byte[] MerchantCategoryCode;
    byte[] MerchantID;
    byte[] MerchantNameLocation;
    byte[] TerminalCountryCode;
    byte[] TerminalID;
    byte TerminalType;
    byte[] TransCurrencyCode;
    byte TransCurrencyExponent;
    byte JcbImplementationOption;//kernel option for jcb emv mode/oda/issuer/except file

    public SimData(){
        AcquirerID = new byte[6];
        IFDSN = new byte[8];
        MerchantCategoryCode = new byte[2];
        MerchantID = new byte[15];
        MerchantNameLocation = new byte[23];
        TerminalCountryCode = new byte[2];
        TerminalID = new byte[8];
        TransCurrencyCode = new byte[2];
        TransCurrencyExponent = 0;
        JcbImplementationOption = (byte)0xFC;
        TerminalType = (byte) 0x22;
    }
}
