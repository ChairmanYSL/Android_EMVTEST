package com.szzt.emvtest.EMVSDK;

import com.ochafik.lang.jnaerator.runtime.Structure;
import java.util.Arrays;
import java.util.List;
/**
 * This file was autogenerated by <a href="http://jnaerator.googlecode.com/">JNAerator</a>,<br>
 * a tool written by <a href="http://ochafik.com/">Olivier Chafik</a> that <a href="http://code.google.com/p/jnaerator/wiki/CreditsAndLicense">uses a few opensource projects.</a>.<br>
 * For help, please visit <a href="http://nativelibs4java.googlecode.com/">NativeLibs4Java</a> , <a href="http://rococoa.dev.java.net/">Rococoa</a>, or <a href="http://jna.dev.java.net/">JNA</a>.
 */
public class SDK_EMVBASE_CL_HIGHESTAID extends Structure<SDK_EMVBASE_CL_HIGHESTAID, SDK_EMVBASE_CL_HIGHESTAID.ByValue, SDK_EMVBASE_CL_HIGHESTAID.ByReference > {
    public byte aidlen;
    /** C type : unsigned char[16] */
    public byte[] aid = new byte[16];
    public byte kernelidlen;
    /** C type : unsigned char[8] */
    public byte[] kernelid = new byte[8];
    public SDK_EMVBASE_CL_HIGHESTAID() {
        super();
    }
    protected List<String > getFieldOrder() {
        return Arrays.asList("aidlen", "aid", "kernelidlen", "kernelid");
    }
    /**
     * @param aid C type : unsigned char[16]<br>
     * @param kernelid C type : unsigned char[8]
     */
    public SDK_EMVBASE_CL_HIGHESTAID(byte aidlen, byte aid[], byte kernelidlen, byte kernelid[]) {
        super();
        this.aidlen = aidlen;
        if ((aid.length != this.aid.length))
            throw new IllegalArgumentException("Wrong array size !");
        this.aid = aid;
        this.kernelidlen = kernelidlen;
        if ((kernelid.length != this.kernelid.length))
            throw new IllegalArgumentException("Wrong array size !");
        this.kernelid = kernelid;
    }
    protected ByReference newByReference() { return new ByReference(); }
    protected ByValue newByValue() { return new ByValue(); }
    protected SDK_EMVBASE_CL_HIGHESTAID newInstance() { return new SDK_EMVBASE_CL_HIGHESTAID(); }
    public static SDK_EMVBASE_CL_HIGHESTAID[] newArray(int arrayLength) {
        return Structure.newArray(SDK_EMVBASE_CL_HIGHESTAID.class, arrayLength);
    }
    public static class ByReference extends SDK_EMVBASE_CL_HIGHESTAID implements Structure.ByReference {

    };
    public static class ByValue extends SDK_EMVBASE_CL_HIGHESTAID implements Structure.ByValue {

    };
}

