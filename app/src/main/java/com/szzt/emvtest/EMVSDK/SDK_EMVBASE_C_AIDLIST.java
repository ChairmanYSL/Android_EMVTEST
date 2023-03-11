package com.szzt.emvtest.EMVSDK;

import com.ochafik.lang.jnaerator.runtime.Structure;
import java.util.Arrays;
import java.util.List;
/**
 * This file was autogenerated by <a href="http://jnaerator.googlecode.com/">JNAerator</a>,<br>
 * a tool written by <a href="http://ochafik.com/">Olivier Chafik</a> that <a href="http://code.google.com/p/jnaerator/wiki/CreditsAndLicense">uses a few opensource projects.</a>.<br>
 * For help, please visit <a href="http://nativelibs4java.googlecode.com/">NativeLibs4Java</a> , <a href="http://rococoa.dev.java.net/">Rococoa</a>, or <a href="http://jna.dev.java.net/">JNA</a>.
 */
public class SDK_EMVBASE_C_AIDLIST extends Structure<SDK_EMVBASE_C_AIDLIST, SDK_EMVBASE_C_AIDLIST.ByValue, SDK_EMVBASE_C_AIDLIST.ByReference > {
    public byte ucAidLen;
    /**
     * 5-16<br>
     * C type : unsigned char[32]
     */
    public byte[] asAid = new byte[32];
    public byte ucAppLabelLen;
    /** C type : unsigned char[32] */
    public byte[] asAppLabel = new byte[32];
    public byte ucPreferNameLen;
    /** C type : unsigned char[32] */
    public byte[] asPreferName = new byte[32];
    /** tag'87' */
    public byte ucPriority;
    public byte ucLangPreferLen;
    /**
     * 2-8<br>
     * C type : unsigned char[16]
     */
    public byte[] asLangPrefer = new byte[16];
    /** Issuer Code Table Index.lang used for display app list according to ISO8859.but not include Chinese,Korea,etc. */
    public byte ucIcti;
    /** If display app list using local language considerless of info in card.0-use card info;1-use local language. */
    public byte ucLocalName;
    public byte ucAppLocalNameLen;
    /** C type : unsigned char[32] */
    public byte[] asAppLocalName = new byte[32];
    public SDK_EMVBASE_C_AIDLIST() {
        super();
    }
    protected List<String > getFieldOrder() {
        return Arrays.asList("ucAidLen", "asAid", "ucAppLabelLen", "asAppLabel", "ucPreferNameLen", "asPreferName", "ucPriority", "ucLangPreferLen", "asLangPrefer", "ucIcti", "ucLocalName", "ucAppLocalNameLen", "asAppLocalName");
    }
    protected ByReference newByReference() { return new ByReference(); }
    protected ByValue newByValue() { return new ByValue(); }
    protected SDK_EMVBASE_C_AIDLIST newInstance() { return new SDK_EMVBASE_C_AIDLIST(); }
    public static SDK_EMVBASE_C_AIDLIST[] newArray(int arrayLength) {
        return Structure.newArray(SDK_EMVBASE_C_AIDLIST.class, arrayLength);
    }
    public static class ByReference extends SDK_EMVBASE_C_AIDLIST implements Structure.ByReference {

    };
    public static class ByValue extends SDK_EMVBASE_C_AIDLIST implements Structure.ByValue {

    };
}
