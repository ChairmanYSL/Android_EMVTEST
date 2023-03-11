package com.szzt.emvtest.EMVSDK;

import com.ochafik.lang.jnaerator.runtime.Structure;
import java.util.Arrays;
import java.util.List;
/**
 * This file was autogenerated by <a href="http://jnaerator.googlecode.com/">JNAerator</a>,<br>
 * a tool written by <a href="http://ochafik.com/">Olivier Chafik</a> that <a href="http://code.google.com/p/jnaerator/wiki/CreditsAndLicense">uses a few opensource projects.</a>.<br>
 * For help, please visit <a href="http://nativelibs4java.googlecode.com/">NativeLibs4Java</a> , <a href="http://rococoa.dev.java.net/">Rococoa</a>, or <a href="http://jna.dev.java.net/">JNA</a>.
 */
public class SDK_EMVBASE_CL_AIDLIST extends Structure<SDK_EMVBASE_CL_AIDLIST, SDK_EMVBASE_CL_AIDLIST.ByValue, SDK_EMVBASE_CL_AIDLIST.ByReference > {
    public byte AIDLen;
    /** C type : unsigned char[16] */
    public byte[] AID = new byte[16];
    public byte AppLabelLen;
    /** C type : unsigned char[16] */
    public byte[] AppLabel = new byte[16];
    public byte PreferNameLen;
    /** C type : unsigned char[16] */
    public byte[] PreferName = new byte[16];
    public byte Priority;
    public byte LangPreferLen;
    /** C type : unsigned char[8] */
    public byte[] LangPrefer = new byte[8];
    public byte AidInTermLen;
    /** C type : unsigned char[16] */
    public byte[] AidInTerm = new byte[16];
    public byte KernelIdentifierLen;
    /** C type : unsigned char[8] */
    public byte[] KernelIdentifier = new byte[8];
    public SDK_EMVBASE_CL_AIDLIST() {
        super();
    }
    protected List<String > getFieldOrder() {
        return Arrays.asList("AIDLen", "AID", "AppLabelLen", "AppLabel", "PreferNameLen", "PreferName", "Priority", "LangPreferLen", "LangPrefer", "AidInTermLen", "AidInTerm", "KernelIdentifierLen", "KernelIdentifier");
    }
    protected ByReference newByReference() { return new ByReference(); }
    protected ByValue newByValue() { return new ByValue(); }
    protected SDK_EMVBASE_CL_AIDLIST newInstance() { return new SDK_EMVBASE_CL_AIDLIST(); }
    public static SDK_EMVBASE_CL_AIDLIST[] newArray(int arrayLength) {
        return Structure.newArray(SDK_EMVBASE_CL_AIDLIST.class, arrayLength);
    }
    public static class ByReference extends SDK_EMVBASE_CL_AIDLIST implements Structure.ByReference {

    };
    public static class ByValue extends SDK_EMVBASE_CL_AIDLIST implements Structure.ByValue {

    };
}
