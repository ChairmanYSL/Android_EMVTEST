package com.szzt.emvtest.EMVSDK;

import com.ochafik.lang.jnaerator.runtime.Structure;
import java.util.Arrays;
import java.util.List;

public class SDK_EMVBASE_CAPK_STRUCT extends Structure<SDK_EMVBASE_CAPK_STRUCT, SDK_EMVBASE_CAPK_STRUCT.ByValue, SDK_EMVBASE_CAPK_STRUCT.ByReference > {
    /** C type : unsigned char[5] */
    public byte[] RID = new byte[5];
    public byte CAPKI;
    public byte HashInd;
    public byte ArithInd;
    public byte ModulLen;
    /** C type : unsigned char[248] */
    public byte[] Modul = new byte[248];
    public byte ExponentLen;
    /** C type : unsigned char[3] */
    public byte[] Exponent = new byte[3];
    public byte CheckSumLen;
    /** C type : unsigned char[20] */
    public byte[] CheckSum = new byte[20];
    /** C type : unsigned char[4] */
    public byte[] ExpireDate = new byte[4];
    public SDK_EMVBASE_CAPK_STRUCT() {
        super();
    }
    protected List<String > getFieldOrder() {
        return Arrays.asList("RID", "CAPKI", "HashInd", "ArithInd", "ModulLen", "Modul", "ExponentLen", "Exponent", "CheckSumLen", "CheckSum", "ExpireDate");
    }
    protected ByReference newByReference() { return new ByReference(); }
    protected ByValue newByValue() { return new ByValue(); }
    protected SDK_EMVBASE_CAPK_STRUCT newInstance() { return new SDK_EMVBASE_CAPK_STRUCT(); }
    public static SDK_EMVBASE_CAPK_STRUCT[] newArray(int arrayLength) {
        return Structure.newArray(SDK_EMVBASE_CAPK_STRUCT.class, arrayLength);
    }
    public static class ByReference extends SDK_EMVBASE_CAPK_STRUCT implements Structure.ByReference {

    };
    public static class ByValue extends SDK_EMVBASE_CAPK_STRUCT implements Structure.ByValue {

    };
}

