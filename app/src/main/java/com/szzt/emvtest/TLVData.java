package com.szzt.emvtest;

public class TLVData {

    byte [] TLVSeek(byte [] TLVDataGroup, short TlvDataLen, short Tag) {
        byte[] TlvData = new byte [256+3+1];


        if(TLVDataGroup.length <= 0)
        {
            return null;
        }
        return TlvData;
    }
}
