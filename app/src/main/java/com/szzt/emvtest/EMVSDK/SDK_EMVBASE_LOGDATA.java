package com.szzt.emvtest.EMVSDK;

import com.ochafik.lang.jnaerator.runtime.Structure;
import java.util.Arrays;
import java.util.List;
/**
 * This file was autogenerated by <a href="http://jnaerator.googlecode.com/">JNAerator</a>,<br>
 * a tool written by <a href="http://ochafik.com/">Olivier Chafik</a> that <a href="http://code.google.com/p/jnaerator/wiki/CreditsAndLicense">uses a few opensource projects.</a>.<br>
 * For help, please visit <a href="http://nativelibs4java.googlecode.com/">NativeLibs4Java</a> , <a href="http://rococoa.dev.java.net/">Rococoa</a>, or <a href="http://jna.dev.java.net/">JNA</a>.
 */
public class SDK_EMVBASE_LOGDATA extends Structure<SDK_EMVBASE_LOGDATA, SDK_EMVBASE_LOGDATA.ByValue, SDK_EMVBASE_LOGDATA.ByReference > {
    public short LenOut;
    /** C type : unsigned char[256] */
    public byte[] DataOut = new byte[256];
    public byte SW1;
    public byte SW2;
    public SDK_EMVBASE_LOGDATA() {
        super();
    }
    protected List<String> getFieldOrder() {
        return Arrays.asList("LenOut", "DataOut", "SW1", "SW2");
    }
    /** @param DataOut C type : unsigned char[256] */
    public SDK_EMVBASE_LOGDATA(short LenOut, byte DataOut[], byte SW1, byte SW2) {
        super();
        this.LenOut = LenOut;
        if ((DataOut.length != this.DataOut.length))
            throw new IllegalArgumentException("Wrong array size !");
        this.DataOut = DataOut;
        this.SW1 = SW1;
        this.SW2 = SW2;
    }
    protected ByReference newByReference() { return new ByReference(); }
    protected ByValue newByValue() { return new ByValue(); }
    protected SDK_EMVBASE_LOGDATA newInstance() { return new SDK_EMVBASE_LOGDATA(); }
    public static SDK_EMVBASE_LOGDATA[] newArray(int arrayLength) {
        return Structure.newArray(SDK_EMVBASE_LOGDATA.class, arrayLength);
    }
    public static class ByReference extends SDK_EMVBASE_LOGDATA implements Structure.ByReference {

    };
    public static class ByValue extends SDK_EMVBASE_LOGDATA implements Structure.ByValue {

    };
}
