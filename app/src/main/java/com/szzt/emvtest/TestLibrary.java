package com.szzt.emvtest;

import com.ochafik.lang.jnaerator.runtime.LibraryExtractor;
import com.ochafik.lang.jnaerator.runtime.MangledFunctionMapper;
import com.sun.jna.Callback;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.NativeLibrary;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;


import com.szzt.emvtest.EMVSDK.SDK_EMVBASE_CL_AIDLIST;
import com.szzt.emvtest.EMVSDK.SDK_EMVBASE_CL_HIGHESTAID;
import com.szzt.emvtest.EMVSDK.SDK_EMVBASE_AID_STRUCT;
import com.szzt.emvtest.EMVSDK.SDK_EMVBASE_CAPK_STRUCT;
import com.szzt.emvtest.EMVSDK.SDK_RANDOM_STRUCT;
import com.szzt.emvtest.EMVSDK.SDK_RSA_PRIVATE_KEY;
import com.szzt.emvtest.EMVSDK.SDK_RSA_PUBLIC_KEY;

public interface TestLibrary extends Library {
    public static final String JNA_LIBRARY_NAME = LibraryExtractor.getLibraryPath("libszzt_sdkjcbjni", true, TestLibrary.class);
    public static final NativeLibrary JNA_NATIVE_LIB = NativeLibrary.getInstance(TestLibrary.JNA_LIBRARY_NAME, (ClassLoader) MangledFunctionMapper.DEFAULT_OPTIONS);
    public static final TestLibrary INSTANCE = (TestLibrary)Native.loadLibrary(TestLibrary.JNA_LIBRARY_NAME, TestLibrary.class, null);



    /** enum values */
    public static interface SDK_EMVBASE_CVM_RESULT {
        /** <i>native declaration : line 99</i> */
        public static final int SDKEMVBASE_CVM_NA = 0;
        /** <i>native declaration : line 100</i> */
        public static final int SDKEMVBASE_CVM_NOCVMREQ = 1;
        /** <i>native declaration : line 101</i> */
        public static final int SDKEMVBASE_CVM_OBTAINSIGNATURE = 2;
        /** <i>native declaration : line 102</i> */
        public static final int SDKEMVBASE_CVM_ONLINEPIN = 3;
        /** <i>native declaration : line 103</i> */
        public static final int SDKEMVBASE_CVM_CONFVERIFIED = 4;
        /** <i>native declaration : line 104</i> */
        public static final int SDKEMVBASE_CVM_MOBILE = 5;
        /** <i>native declaration : line 105</i> */
        public static final int SDKEMVBASE_CVM_PLAINTEXTPIN = 6;
        /** <i>native declaration : line 106</i> */
        public static final int SDKEMVBASE_CVM_ENCIPHEREDPIN = 7;
        /** <i>native declaration : line 107</i> */
        public static final int SDKEMVBASE_CVM_PLAINTEXTPIN_AND_SIGN = 8;
        /** <i>native declaration : line 108</i> */
        public static final int SDKEMVBASE_CVM_ENCIPHEREDPIN_AND_SIGN = 9;
        /** <i>native declaration : line 109</i> */
        public static final int SDKEMVBASE_CVM_SKIPCVM = 10;
    };
    /** <i>native declaration : line 146</i> */
    public interface sdkEMVBaseSetInputPINFun_fun_inputpin_callback extends Callback {
        int apply(byte ucIccEncryptWay, byte ucPINTryCount, byte [] pheCreditPwd);
    };
    /** <i>native declaration : line 153</i> */
    public interface sdkEMVBaseSetRevocationKey_fun_cmprevocation_callback extends Callback {
        int apply(Pointer key);
    };
    /**
     * Original signature : <code>int sdkemvbaseInitDDI()</code><br>
     * <i>native declaration : line 112</i>
     */
    int sdkemvbaseInitDDI();
    /**
     * Original signature : <code>void sdkEMVBase_SetEMVLogOutput(bool)</code><br>
     * <i>native declaration : line 113</i>
     */
    void sdkEMVBase_SetEMVLogOutput(byte enable);
    /**
     * Original signature : <code>int sdkEMVBasePowerStartInit()</code><br>
     * <i>native declaration : line 115</i>
     */
    int sdkEMVBasePowerStartInit();
    /**
     * Original signature : <code>int sdkEMVBaseDelOneAID(const unsigned char*, int)</code><br>
     * <i>native declaration : line 116</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseDelOneAID(byte[], int)} and {@link #sdkEMVBaseDelOneAID(com.sun.jna.Pointer, int)} instead
     */
    @Deprecated
    int sdkEMVBaseDelOneAID(Pointer pheInAID, int siAIDLen);
    /**
     * Original signature : <code>int sdkEMVBaseDelOneAID(const unsigned char*, int)</code><br>
     * <i>native declaration : line 116</i>
     */
    int sdkEMVBaseDelOneAID(byte pheInAID[], int siAIDLen);
    /**
     * Original signature : <code>void sdkEMVBaseDelAllAIDLists()</code><br>
     * <i>native declaration : line 117</i>
     */
    void sdkEMVBaseDelAllAIDLists();
    /**
     * Original signature : <code>int sdkEMVBaseAddAnyAIDList(const SDK_EMVBASE_AID_STRUCT*, int)</code><br>
     * <i>native declaration : line 118</i>
     */
    int sdkEMVBaseAddAnyAIDList(SDK_EMVBASE_AID_STRUCT pstInAIDList, int siAIDNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetAnyAIDList(int, int, SDK_EMVBASE_AID_STRUCT*, int*)</code><br>
     * <i>native declaration : line 119</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseGetAnyAIDList(int, int, test.SDK_EMVBASE_AID_STRUCT, java.nio.IntBuffer)} and {@link #sdkEMVBaseGetAnyAIDList(int, int, test.SDK_EMVBASE_AID_STRUCT, com.sun.jna.ptr.IntByReference)} instead
     */
    @Deprecated
    int sdkEMVBaseGetAnyAIDList(int siStartIndex, int siAIDNum, SDK_EMVBASE_AID_STRUCT pstOutAIDList, IntByReference psiOutAIDNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetAnyAIDList(int, int, SDK_EMVBASE_AID_STRUCT*, int*)</code><br>
     * <i>native declaration : line 119</i>
     */
    int sdkEMVBaseGetAnyAIDList(int siStartIndex, int siAIDNum, SDK_EMVBASE_AID_STRUCT pstOutAIDList, IntBuffer psiOutAIDNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetAIDListNum(int*)</code><br>
     * <i>native declaration : line 120</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseGetAIDListNum(java.nio.IntBuffer)} and {@link #sdkEMVBaseGetAIDListNum(com.sun.jna.ptr.IntByReference)} instead
     */
    @Deprecated
    int sdkEMVBaseGetAIDListNum(IntByReference psiInAIDListNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetAIDListNum(int*)</code><br>
     * <i>native declaration : line 120</i>
     */
    int sdkEMVBaseGetAIDListNum(IntBuffer psiInAIDListNum);
    /**
     * Original signature : <code>int sdkEMVbaseGetMatchAID(const unsigned char*, int, SDK_EMVBASE_AID_STRUCT*, int, int*)</code><br>
     * <i>native declaration : line 121</i><br>
     * @deprecated use the safer methods {@link #sdkEMVbaseGetMatchAID(byte[], int, test.SDK_EMVBASE_AID_STRUCT, int, java.nio.IntBuffer)} and {@link #sdkEMVbaseGetMatchAID(com.sun.jna.Pointer, int, test.SDK_EMVBASE_AID_STRUCT, int, com.sun.jna.ptr.IntByReference)} instead
     */
    @Deprecated
    int sdkEMVbaseGetMatchAID(Pointer pheInAID, int siAIDLen, SDK_EMVBASE_AID_STRUCT pstOutAID, int MaxOutAIDNum, IntByReference psiOutAIDNum);
    /**
     * Original signature : <code>int sdkEMVbaseGetMatchAID(const unsigned char*, int, SDK_EMVBASE_AID_STRUCT*, int, int*)</code><br>
     * <i>native declaration : line 121</i>
     */
    int sdkEMVbaseGetMatchAID(byte pheInAID[], int siAIDLen, SDK_EMVBASE_AID_STRUCT pstOutAID, int MaxOutAIDNum, IntBuffer psiOutAIDNum);
    /**
     * Original signature : <code>int sdkEMVBaseDelOneCAPK(const unsigned char*, int)</code><br>
     * <i>native declaration : line 123</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseDelOneCAPK(byte[], int)} and {@link #sdkEMVBaseDelOneCAPK(com.sun.jna.Pointer, int)} instead
     */
    @Deprecated
    int sdkEMVBaseDelOneCAPK(Pointer pheInRID, int siCAPKIndex);
    /**
     * Original signature : <code>int sdkEMVBaseDelOneCAPK(const unsigned char*, int)</code><br>
     * <i>native declaration : line 123</i>
     */
    int sdkEMVBaseDelOneCAPK(byte pheInRID[], int siCAPKIndex);
    /**
     * Original signature : <code>void sdkEMVBaseDelAllCAPKLists()</code><br>
     * <i>native declaration : line 124</i>
     */
    void sdkEMVBaseDelAllCAPKLists();
    /**
     * Original signature : <code>int sdkEMVBaseAddAnyCAPKList(const SDK_EMVBASE_CAPK_STRUCT*, int)</code><br>
     * <i>native declaration : line 125</i>
     */
    int sdkEMVBaseAddAnyCAPKList(SDK_EMVBASE_CAPK_STRUCT pstInCAPKList, int siCAPKNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetAnyCAPKList(int, int, SDK_EMVBASE_CAPK_STRUCT*, int*)</code><br>
     * <i>native declaration : line 126</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseGetAnyCAPKList(int, int, test.SDK_EMVBASE_CAPK_STRUCT, java.nio.IntBuffer)} and {@link #sdkEMVBaseGetAnyCAPKList(int, int, test.SDK_EMVBASE_CAPK_STRUCT, com.sun.jna.ptr.IntByReference)} instead
     */
    @Deprecated
    int sdkEMVBaseGetAnyCAPKList(int siStartIndex, int siCAPKNum, SDK_EMVBASE_CAPK_STRUCT pstOutCAPKList, IntByReference psiOutCAPKNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetAnyCAPKList(int, int, SDK_EMVBASE_CAPK_STRUCT*, int*)</code><br>
     * <i>native declaration : line 126</i>
     */
    int sdkEMVBaseGetAnyCAPKList(int siStartIndex, int siCAPKNum, SDK_EMVBASE_CAPK_STRUCT pstOutCAPKList, IntBuffer psiOutCAPKNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetCAPKListNum(int*)</code><br>
     * <i>native declaration : line 127</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseGetCAPKListNum(java.nio.IntBuffer)} and {@link #sdkEMVBaseGetCAPKListNum(com.sun.jna.ptr.IntByReference)} instead
     */
    @Deprecated
    int sdkEMVBaseGetCAPKListNum(IntByReference psiInCAPKListNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetCAPKListNum(int*)</code><br>
     * <i>native declaration : line 127</i>
     */
    int sdkEMVBaseGetCAPKListNum(IntBuffer psiInCAPKListNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetMatchCAPK(const unsigned char*, int, SDK_EMVBASE_CAPK_STRUCT*)</code><br>
     * <i>native declaration : line 128</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseGetMatchCAPK(byte[], int, test.SDK_EMVBASE_CAPK_STRUCT)} and {@link #sdkEMVBaseGetMatchCAPK(com.sun.jna.Pointer, int, test.SDK_EMVBASE_CAPK_STRUCT)} instead
     */
    @Deprecated
    int sdkEMVBaseGetMatchCAPK(Pointer pheInRID, int uiCAPKIndex, SDK_EMVBASE_CAPK_STRUCT pstOutCAPK);
    /**
     * Original signature : <code>int sdkEMVBaseGetMatchCAPK(const unsigned char*, int, SDK_EMVBASE_CAPK_STRUCT*)</code><br>
     * <i>native declaration : line 128</i>
     */
    int sdkEMVBaseGetMatchCAPK(byte pheInRID[], int uiCAPKIndex, SDK_EMVBASE_CAPK_STRUCT pstOutCAPK);
    /**
     * Original signature : <code>int sdkEMVBaseDelOneGMCAPK(const unsigned char*, int)</code><br>
     * <i>native declaration : line 130</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseDelOneGMCAPK(byte[], int)} and {@link #sdkEMVBaseDelOneGMCAPK(com.sun.jna.Pointer, int)} instead
     */
    @Deprecated
    int sdkEMVBaseDelOneGMCAPK(Pointer pheInRID, int siCAPKIndex);
    /**
     * Original signature : <code>int sdkEMVBaseDelOneGMCAPK(const unsigned char*, int)</code><br>
     * <i>native declaration : line 130</i>
     */
    int sdkEMVBaseDelOneGMCAPK(byte pheInRID[], int siCAPKIndex);
    /**
     * Original signature : <code>void sdkEMVBaseDelAllGMCAPKLists()</code><br>
     * <i>native declaration : line 131</i>
     */
    void sdkEMVBaseDelAllGMCAPKLists();
    /**
     * Original signature : <code>int sdkEMVBaseAddAnyGMCAPKList(const SDK_EMVBASE_CAPK_STRUCT*, int)</code><br>
     * <i>native declaration : line 132</i>
     */
    int sdkEMVBaseAddAnyGMCAPKList(SDK_EMVBASE_CAPK_STRUCT pstInCAPKList, int siCAPKNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetAnyGMCAPKList(int, int, SDK_EMVBASE_CAPK_STRUCT*, int*)</code><br>
     * <i>native declaration : line 133</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseGetAnyGMCAPKList(int, int, test.SDK_EMVBASE_CAPK_STRUCT, java.nio.IntBuffer)} and {@link #sdkEMVBaseGetAnyGMCAPKList(int, int, test.SDK_EMVBASE_CAPK_STRUCT, com.sun.jna.ptr.IntByReference)} instead
     */
    @Deprecated
    int sdkEMVBaseGetAnyGMCAPKList(int siStartIndex, int siCAPKNum, SDK_EMVBASE_CAPK_STRUCT pstOutCAPKList, IntByReference psiOutCAPKNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetAnyGMCAPKList(int, int, SDK_EMVBASE_CAPK_STRUCT*, int*)</code><br>
     * <i>native declaration : line 133</i>
     */
    int sdkEMVBaseGetAnyGMCAPKList(int siStartIndex, int siCAPKNum, SDK_EMVBASE_CAPK_STRUCT pstOutCAPKList, IntBuffer psiOutCAPKNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetGMCAPKListNum(int*)</code><br>
     * <i>native declaration : line 134</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseGetGMCAPKListNum(java.nio.IntBuffer)} and {@link #sdkEMVBaseGetGMCAPKListNum(com.sun.jna.ptr.IntByReference)} instead
     */
    @Deprecated
    int sdkEMVBaseGetGMCAPKListNum(IntByReference psiInCAPKListNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetGMCAPKListNum(int*)</code><br>
     * <i>native declaration : line 134</i>
     */
    int sdkEMVBaseGetGMCAPKListNum(IntBuffer psiInCAPKListNum);
    /**
     * Original signature : <code>int sdkEMVBaseGetMatchGMCAPK(const unsigned char*, int, SDK_EMVBASE_CAPK_STRUCT*)</code><br>
     * <i>native declaration : line 135</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseGetMatchGMCAPK(byte[], int, test.SDK_EMVBASE_CAPK_STRUCT)} and {@link #sdkEMVBaseGetMatchGMCAPK(com.sun.jna.Pointer, int, test.SDK_EMVBASE_CAPK_STRUCT)} instead
     */
    @Deprecated
    int sdkEMVBaseGetMatchGMCAPK(Pointer pheInRID, int uiCAPKIndex, SDK_EMVBASE_CAPK_STRUCT pstOutCAPK);
    /**
     * Original signature : <code>int sdkEMVBaseGetMatchGMCAPK(const unsigned char*, int, SDK_EMVBASE_CAPK_STRUCT*)</code><br>
     * <i>native declaration : line 135</i>
     */
    int sdkEMVBaseGetMatchGMCAPK(byte pheInRID[], int uiCAPKIndex, SDK_EMVBASE_CAPK_STRUCT pstOutCAPK);
    /**
     * Original signature : <code>bool sdkEMVBaseCheckCAPKExit(unsigned char*, unsigned char)</code><br>
     * <i>native declaration : line 136</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseCheckCAPKExit(java.nio.ByteBuffer, byte)} and {@link #sdkEMVBaseCheckCAPKExit(com.sun.jna.Pointer, byte)} instead
     */
    @Deprecated
    byte sdkEMVBaseCheckCAPKExit(Pointer RID, byte capki);
    /**
     * Original signature : <code>bool sdkEMVBaseCheckCAPKExit(unsigned char*, unsigned char)</code><br>
     * <i>native declaration : line 136</i>
     */
    byte sdkEMVBaseCheckCAPKExit(ByteBuffer RID, byte capki);
    /**
     * Original signature : <code>int sdkEMVBaseTransInit()</code><br>
     * <i>native declaration : line 138</i>
     */
    int sdkEMVBaseTransInit();
    /**
     * Original signature : <code>int sdkEMVBaseEntryPoint(SDK_EMVBASE_CL_HIGHESTAID*)</code><br>
     * <i>native declaration : line 139</i>
     */
    int sdkEMVBaseEntryPoint(SDK_EMVBASE_CL_HIGHESTAID tempHighestAID);
    /**
     * Original signature : <code>int sdkEMVBaseConfigTLV(const unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 141</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseConfigTLV(byte[], byte[], int)} and {@link #sdkEMVBaseConfigTLV(com.sun.jna.Pointer, com.sun.jna.Pointer, int)} instead
     */
    @Deprecated
    int sdkEMVBaseConfigTLV(Pointer pheTag, Pointer pheInData, int siInLen);
    /**
     * Original signature : <code>int sdkEMVBaseConfigTLV(const unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 141</i>
     */
    int sdkEMVBaseConfigTLV(byte pheTag[], byte pheInData[], int siInLen);
    /**
     * Original signature : <code>int sdkEMVBaseReadTLV(const unsigned char*, unsigned char*, int*)</code><br>
     * <i>native declaration : line 142</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseReadTLV(byte[], java.nio.ByteBuffer, java.nio.IntBuffer)} and {@link #sdkEMVBaseReadTLV(com.sun.jna.Pointer, com.sun.jna.Pointer, com.sun.jna.ptr.IntByReference)} instead
     */
    @Deprecated
    int sdkEMVBaseReadTLV(Pointer pheTag, Pointer pheOutData, IntByReference psiOutLen);
    /**
     * Original signature : <code>int sdkEMVBaseReadTLV(const unsigned char*, unsigned char*, int*)</code><br>
     * <i>native declaration : line 142</i>
     */
//    int sdkEMVBaseReadTLV(byte pheTag[], ByteBuffer pheOutData, IntBuffer psiOutLen);
    int sdkEMVBaseReadTLV(byte pheTag[], byte [] pheOutData, IntBuffer psiOutLen);
    /**
     * Original signature : <code>bool sdkEMVBaseCheckTagExit(unsigned char*)</code><br>
     * <i>native declaration : line 143</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseCheckTagExit(java.nio.ByteBuffer)} and {@link #sdkEMVBaseCheckTagExit(com.sun.jna.Pointer)} instead
     */
    @Deprecated
    byte sdkEMVBaseCheckTagExit(Pointer tag);
    /**
     * Original signature : <code>bool sdkEMVBaseCheckTagExit(unsigned char*)</code><br>
     * <i>native declaration : line 143</i>
     */
    byte sdkEMVBaseCheckTagExit(ByteBuffer tag);
    /**
     * Original signature : <code>int sdkEMVBaseDelTLV(const unsigned char*)</code><br>
     * <i>native declaration : line 144</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseDelTLV(byte[])} and {@link #sdkEMVBaseDelTLV(com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkEMVBaseDelTLV(Pointer pheTag);
    /**
     * Original signature : <code>int sdkEMVBaseDelTLV(const unsigned char*)</code><br>
     * <i>native declaration : line 144</i>
     */
    int sdkEMVBaseDelTLV(byte pheTag[]);
    /**
     * Original signature : <code>int sdkEMVBaseSetInputPINFun(sdkEMVBaseSetInputPINFun_fun_inputpin_callback*)</code><br>
     * <i>native declaration : line 146</i>
     */
    int sdkEMVBaseSetInputPINFun(TestLibrary.sdkEMVBaseSetInputPINFun_fun_inputpin_callback fun_inputpin);
    /**
     * default is false<br>
     * Original signature : <code>int sdkEMVBaseSetSpTermCheck(bool)</code><br>
     * <i>native declaration : line 147</i>
     */
    int sdkEMVBaseSetSpTermCheck(byte enable);
    /**
     * Original signature : <code>int sdkEMVBaseGetScriptResult(int*, unsigned char*)</code><br>
     * <i>native declaration : line 148</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseGetScriptResult(java.nio.IntBuffer, java.nio.ByteBuffer)} and {@link #sdkEMVBaseGetScriptResult(com.sun.jna.ptr.IntByReference, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkEMVBaseGetScriptResult(IntByReference psiScriptResultLen, Pointer pheScriptResult);
    /**
     * Original signature : <code>int sdkEMVBaseGetScriptResult(int*, unsigned char*)</code><br>
     * <i>native declaration : line 148</i>
     */
    int sdkEMVBaseGetScriptResult(IntBuffer psiScriptResultLen, ByteBuffer pheScriptResult);
    /**
     * Original signature : <code>bool sdkEMVBaseNeedSignature()</code><br>
     * <i>native declaration : line 149</i>
     */
    byte sdkEMVBaseNeedSignature();
    /**
     * must call after sdkEMVBaseEntryPoint<br>
     * Original signature : <code>int sdkEMVBaseGetEntryPointAIDList(SDK_EMVBASE_CL_AIDLIST*, int, int*)</code><br>
     * <i>native declaration : line 150</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseGetEntryPointAIDList(test.SDK_EMVBASE_CL_AIDLIST, int, java.nio.IntBuffer)} and {@link #sdkEMVBaseGetEntryPointAIDList(test.SDK_EMVBASE_CL_AIDLIST, int, com.sun.jna.ptr.IntByReference)} instead
     */
    @Deprecated
    int sdkEMVBaseGetEntryPointAIDList(SDK_EMVBASE_CL_AIDLIST entrypointlist, int silistnum, IntByReference psiOutlistnum);
    /**
     * must call after sdkEMVBaseEntryPoint<br>
     * Original signature : <code>int sdkEMVBaseGetEntryPointAIDList(SDK_EMVBASE_CL_AIDLIST*, int, int*)</code><br>
     * <i>native declaration : line 150</i>
     */
    int sdkEMVBaseGetEntryPointAIDList(SDK_EMVBASE_CL_AIDLIST entrypointlist, int silistnum, IntBuffer psiOutlistnum);
    /**
     * must call after sdkEMVBaseEntryPoint<br>
     * Original signature : <code>int sdkEMVBaseSetUserSelcetFixedAid(unsigned char*, unsigned char)</code><br>
     * <i>native declaration : line 151</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseSetUserSelcetFixedAid(java.nio.ByteBuffer, byte)} and {@link #sdkEMVBaseSetUserSelcetFixedAid(com.sun.jna.Pointer, byte)} instead
     */
    @Deprecated
    int sdkEMVBaseSetUserSelcetFixedAid(Pointer aidincard, byte aidlen);
    /**
     * must call after sdkEMVBaseEntryPoint<br>
     * Original signature : <code>int sdkEMVBaseSetUserSelcetFixedAid(unsigned char*, unsigned char)</code><br>
     * <i>native declaration : line 151</i>
     */
    int sdkEMVBaseSetUserSelcetFixedAid(ByteBuffer aidincard, byte aidlen);
    /**
     * Original signature : <code>int sdkEMVBaseSetRevocationKey(sdkEMVBaseSetRevocationKey_fun_cmprevocation_callback*)</code><br>
     * <i>native declaration : line 153</i>
     */
    int sdkEMVBaseSetRevocationKey(TestLibrary.sdkEMVBaseSetRevocationKey_fun_cmprevocation_callback fun_cmprevocation);
    /**
     * Original signature : <code>int sdkEMVBaseReSelectApp(SDK_EMVBASE_CL_HIGHESTAID*)</code><br>
     * <i>native declaration : line 155</i>
     */
    int sdkEMVBaseReSelectApp(SDK_EMVBASE_CL_HIGHESTAID tempHighestAID);
    /**
     * Original signature : <code>int sdkEMVBaseSetTransAmount(unsigned char*)</code><br>
     * <i>native declaration : line 157</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseSetTransAmount(java.nio.ByteBuffer)} and {@link #sdkEMVBaseSetTransAmount(com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkEMVBaseSetTransAmount(Pointer bcdAmount);
    /**
     * Original signature : <code>int sdkEMVBaseSetTransAmount(unsigned char*)</code><br>
     * <i>native declaration : line 157</i>
     */
    int sdkEMVBaseSetTransAmount(ByteBuffer bcdAmount);
    /**
     * Original signature : <code>int sdkEMVBaseSetTwoTransAmount(unsigned char*, unsigned char*)</code><br>
     * <i>native declaration : line 158</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseSetTwoTransAmount(java.nio.ByteBuffer, java.nio.ByteBuffer)} and {@link #sdkEMVBaseSetTwoTransAmount(com.sun.jna.Pointer, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkEMVBaseSetTwoTransAmount(Pointer bcdAmount, Pointer bcdOtherAmount);
    /**
     * Original signature : <code>int sdkEMVBaseSetTwoTransAmount(unsigned char*, unsigned char*)</code><br>
     * <i>native declaration : line 158</i>
     */
    int sdkEMVBaseSetTwoTransAmount(ByteBuffer bcdAmount, ByteBuffer bcdOtherAmount);
    /**
     * Original signature : <code>int sdkEMVBasePreTrans()</code><br>
     * <i>native declaration : line 159</i>
     */
    int sdkEMVBasePreTrans();
    /**
     * Original signature : <code>int sdkEMVBasePreTransGetTransType()</code><br>
     * <i>native declaration : line 160</i>
     */
    int sdkEMVBasePreTransGetTransType();
    /**
     * Original signature : <code>int sdkEMVBaseSelectFromTermSelect(unsigned char*)</code><br>
     * <i>native declaration : line 162</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseSelectFromTermSelect(java.nio.ByteBuffer)} and {@link #sdkEMVBaseSelectFromTermSelect(com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkEMVBaseSelectFromTermSelect(Pointer aidnum);
    /**
     * Original signature : <code>int sdkEMVBaseSelectFromTermSelect(unsigned char*)</code><br>
     * <i>native declaration : line 162</i>
     */
    int sdkEMVBaseSelectFromTermSelect(ByteBuffer aidnum);
    /**
     * Original signature : <code>int sdkEMVBaseGetFromTermSelectAIDList(SDK_EMVBASE_CL_AIDLIST*, int, int*)</code><br>
     * <i>native declaration : line 163</i><br>
     * @deprecated use the safer methods {@link #sdkEMVBaseGetFromTermSelectAIDList(test.SDK_EMVBASE_CL_AIDLIST, int, java.nio.IntBuffer)} and {@link #sdkEMVBaseGetFromTermSelectAIDList(test.SDK_EMVBASE_CL_AIDLIST, int, com.sun.jna.ptr.IntByReference)} instead
     */
    @Deprecated
    int sdkEMVBaseGetFromTermSelectAIDList(SDK_EMVBASE_CL_AIDLIST entrypointlist, int silistnum, IntByReference psiOutlistnum);
    /**
     * Original signature : <code>int sdkEMVBaseGetFromTermSelectAIDList(SDK_EMVBASE_CL_AIDLIST*, int, int*)</code><br>
     * <i>native declaration : line 163</i>
     */
    int sdkEMVBaseGetFromTermSelectAIDList(SDK_EMVBASE_CL_AIDLIST entrypointlist, int silistnum, IntBuffer psiOutlistnum);
    /**
     * Original signature : <code>SDK_EMVBASE_CVM_RESULT sdkEMVBaseGetCVMresult()</code><br>
     * <i>native declaration : line 165</i>
     */
    int sdkEMVBaseGetCVMresult();
    /**
     * Original signature : <code>int sdkEMVBaseSetSupportExternSelect(unsigned char)</code><br>
     * <i>native declaration : line 167</i>
     */
    int sdkEMVBaseSetSupportExternSelect(byte flag);
    /**
     * Original signature : <code>int sdkEMVBaseSetAppListCandicateMaxNum(unsigned char)</code><br>
     * <i>native declaration : line 170</i>
     */
    int sdkEMVBaseSetAppListCandicateMaxNum(byte maxnum);

    public static interface SDK_ENCRYPT_MODE {
        /**
         * DES<br>
         * <i>native declaration : line 10</i>
         */
        public static final int SDK_DES = 0x01;
        /**
         * 3DES<br>
         * <i>native declaration : line 11</i>
         */
        public static final int SDK_3DES = 0x02;
    };
    /** enum values */
    public static interface SDK_ENC_DES_MODE {
        /**
         * \ufffd\ufffd\ufffd\ufffd//decrypt<br>
         * <i>native declaration : line 16</i>
         */
        public static final int SDK_DECRYPT = 0;
        /**
         * \ufffd\ufffd\ufffd\ufffd//encrypt<br>
         * <i>native declaration : line 17</i>
         */
        public static final int SDK_ENCRYPT = 1;
    };
    public static final int SDK_MAX_RSA_MODULUS_LEN = (int)256;
    public static final int SDK_MAX_RSA_PRIME_LEN = (int)128;
    /**
     * Original signature : <code>int sdkDesS(bool, unsigned char*, const unsigned char*)</code><br>
     * <i>native declaration : line 51</i><br>
     * @deprecated use the safer methods {@link #sdkDesS(byte, java.nio.ByteBuffer, byte[])} and {@link #sdkDesS(byte, com.sun.jna.Pointer, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkDesS(byte bEncrypt, Pointer pheDatat, Pointer phekeyt);
    /**
     * Original signature : <code>int sdkDesS(bool, unsigned char*, const unsigned char*)</code><br>
     * <i>native declaration : line 51</i>
     */
    int sdkDesS(byte bEncrypt, ByteBuffer pheDatat, byte phekeyt[]);
    /**
     * Original signature : <code>int sdkDes3S(bool, unsigned char*, const unsigned char*)</code><br>
     * <i>native declaration : line 52</i><br>
     * @deprecated use the safer methods {@link #sdkDes3S(byte, java.nio.ByteBuffer, byte[])} and {@link #sdkDes3S(byte, com.sun.jna.Pointer, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkDes3S(byte bEncrypt, Pointer pheDatat, Pointer phekeyt);
    /**
     * Original signature : <code>int sdkDes3S(bool, unsigned char*, const unsigned char*)</code><br>
     * <i>native declaration : line 52</i>
     */
    int sdkDes3S(byte bEncrypt, ByteBuffer pheDatat, byte phekeyt[]);
    /**
     * Original signature : <code>int sdkMD5(unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 53</i><br>
     * @deprecated use the safer methods {@link #sdkMD5(java.nio.ByteBuffer, byte[], int)} and {@link #sdkMD5(com.sun.jna.Pointer, com.sun.jna.Pointer, int)} instead
     */
    @Deprecated
    int sdkMD5(Pointer pheDest, Pointer hepSrc, int silen);
    /**
     * Original signature : <code>int sdkMD5(unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 53</i>
     */
    int sdkMD5(ByteBuffer pheDest, byte hepSrc[], int silen);
    /**
     * Original signature : <code>int sdkSHA1(const unsigned char*, int, unsigned char*)</code><br>
     * <i>native declaration : line 54</i><br>
     * @deprecated use the safer methods {@link #sdkSHA1(byte[], int, java.nio.ByteBuffer)} and {@link #sdkSHA1(com.sun.jna.Pointer, int, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkSHA1(Pointer pheSrc, int siLen, Pointer pheDest);
    /**
     * Original signature : <code>int sdkSHA1(const unsigned char*, int, unsigned char*)</code><br>
     * <i>native declaration : line 54</i>
     */
    int sdkSHA1(byte pheSrc[], int siLen, ByteBuffer pheDest);
    /**
     * Original signature : <code>int sdkSM2InitElliptic(const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*)</code><br>
     * <i>native declaration : line 55</i><br>
     * @deprecated use the safer methods {@link #sdkSM2InitElliptic(byte[], byte[], byte[], byte[], byte[], byte[])} and {@link #sdkSM2InitElliptic(com.sun.jna.Pointer, com.sun.jna.Pointer, com.sun.jna.Pointer, com.sun.jna.Pointer, com.sun.jna.Pointer, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkSM2InitElliptic(Pointer pGroupP, Pointer pGroupA, Pointer pGroupB, Pointer pGroupX, Pointer pGroupY, Pointer pGroupN);
    /**
     * Original signature : <code>int sdkSM2InitElliptic(const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*)</code><br>
     * <i>native declaration : line 55</i>
     */
    int sdkSM2InitElliptic(byte pGroupP[], byte pGroupA[], byte pGroupB[], byte pGroupX[], byte pGroupY[], byte pGroupN[]);
    /**
     * shijianglong 2013.05.30 16:15<br>
     * Original signature : <code>int sdkSM3Encrypt(unsigned char*, int, unsigned char*)</code><br>
     * <i>native declaration : line 56</i><br>
     * @deprecated use the safer methods {@link #sdkSM3Encrypt(java.nio.ByteBuffer, int, java.nio.ByteBuffer)} and {@link #sdkSM3Encrypt(com.sun.jna.Pointer, int, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkSM3Encrypt(Pointer pSrcData, int siLen, Pointer pDestData);
    /**
     * shijianglong 2013.05.30 16:15<br>
     * Original signature : <code>int sdkSM3Encrypt(unsigned char*, int, unsigned char*)</code><br>
     * <i>native declaration : line 56</i>
     */
    int sdkSM3Encrypt(ByteBuffer pSrcData, int siLen, ByteBuffer pDestData);
    /**
     * Original signature : <code>int sdkSM2CheckSign(const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*)</code><br>
     * <i>native declaration : line 57</i><br>
     * @deprecated use the safer methods {@link #sdkSM2CheckSign(byte[], byte[], byte[], byte[], byte[], byte[])} and {@link #sdkSM2CheckSign(com.sun.jna.Pointer, com.sun.jna.Pointer, com.sun.jna.Pointer, com.sun.jna.Pointer, com.sun.jna.Pointer, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkSM2CheckSign(Pointer pPkeyX, Pointer pPkeyY, Pointer pSignR, Pointer pSignS, Pointer pSrcData, Pointer pIDA);
    /**
     * Original signature : <code>int sdkSM2CheckSign(const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*, const unsigned char*)</code><br>
     * <i>native declaration : line 57</i>
     */
    int sdkSM2CheckSign(byte pPkeyX[], byte pPkeyY[], byte pSignR[], byte pSignS[], byte pSrcData[], byte pIDA[]);
    /**
     * Original signature : <code>int sdkSM2ClearAll()</code><br>
     * <i>native declaration : line 58</i>
     */
    int sdkSM2ClearAll();
    /**
     * Original signature : <code>int sdkRSAPublicEncrypt(unsigned char*, int*, const unsigned char*, int, const SDK_RSA_PUBLIC_KEY*, const SDK_RANDOM_STRUCT*)</code><br>
     * <i>native declaration : line 59</i><br>
     * @deprecated use the safer methods {@link #sdkRSAPublicEncrypt(java.nio.ByteBuffer, java.nio.IntBuffer, byte[], int, test.SDK_RSA_PUBLIC_KEY, test.SDK_RANDOM_STRUCT)} and {@link #sdkRSAPublicEncrypt(com.sun.jna.Pointer, com.sun.jna.ptr.IntByReference, com.sun.jna.Pointer, int, test.SDK_RSA_PUBLIC_KEY, test.SDK_RANDOM_STRUCT)} instead
     */
    @Deprecated
    int sdkRSAPublicEncrypt(Pointer pheDest, IntByReference psiDestlen, Pointer pheSrc, int siSrclen, SDK_RSA_PUBLIC_KEY pstPublicKey, SDK_RANDOM_STRUCT pstRandomStruct);
    /**
     * Original signature : <code>int sdkRSAPublicEncrypt(unsigned char*, int*, const unsigned char*, int, const SDK_RSA_PUBLIC_KEY*, const SDK_RANDOM_STRUCT*)</code><br>
     * <i>native declaration : line 59</i>
     */
    int sdkRSAPublicEncrypt(ByteBuffer pheDest, IntBuffer psiDestlen, byte pheSrc[], int siSrclen, SDK_RSA_PUBLIC_KEY pstPublicKey, SDK_RANDOM_STRUCT pstRandomStruct);
    /**
     * Original signature : <code>int sdkRSAPrivateEncrypt(unsigned char*, int*, const unsigned char*, int, const SDK_RSA_PRIVATE_KEY*)</code><br>
     * <i>native declaration : line 60</i><br>
     * @deprecated use the safer methods {@link #sdkRSAPrivateEncrypt(java.nio.ByteBuffer, java.nio.IntBuffer, byte[], int, test.SDK_RSA_PRIVATE_KEY)} and {@link #sdkRSAPrivateEncrypt(com.sun.jna.Pointer, com.sun.jna.ptr.IntByReference, com.sun.jna.Pointer, int, test.SDK_RSA_PRIVATE_KEY)} instead
     */
    @Deprecated
    int sdkRSAPrivateEncrypt(Pointer pheDest, IntByReference psiDestlen, Pointer pucSrc, int siSrclen, SDK_RSA_PRIVATE_KEY pstPrivateKey);
    /**
     * Original signature : <code>int sdkRSAPrivateEncrypt(unsigned char*, int*, const unsigned char*, int, const SDK_RSA_PRIVATE_KEY*)</code><br>
     * <i>native declaration : line 60</i>
     */
    int sdkRSAPrivateEncrypt(ByteBuffer pheDest, IntBuffer psiDestlen, byte pucSrc[], int siSrclen, SDK_RSA_PRIVATE_KEY pstPrivateKey);
    /**
     * Original signature : <code>int sdkRSAPublicDecrypt(unsigned char*, int*, const unsigned char*, int, const SDK_RSA_PUBLIC_KEY*)</code><br>
     * <i>native declaration : line 61</i><br>
     * @deprecated use the safer methods {@link #sdkRSAPublicDecrypt(java.nio.ByteBuffer, java.nio.IntBuffer, byte[], int, test.SDK_RSA_PUBLIC_KEY)} and {@link #sdkRSAPublicDecrypt(com.sun.jna.Pointer, com.sun.jna.ptr.IntByReference, com.sun.jna.Pointer, int, test.SDK_RSA_PUBLIC_KEY)} instead
     */
    @Deprecated
    int sdkRSAPublicDecrypt(Pointer pucDest, IntByReference psiDestlen, Pointer pheSrc, int siSrclen, SDK_RSA_PUBLIC_KEY pstPublicKey);
    /**
     * Original signature : <code>int sdkRSAPublicDecrypt(unsigned char*, int*, const unsigned char*, int, const SDK_RSA_PUBLIC_KEY*)</code><br>
     * <i>native declaration : line 61</i>
     */
    int sdkRSAPublicDecrypt(ByteBuffer pucDest, IntBuffer psiDestlen, byte pheSrc[], int siSrclen, SDK_RSA_PUBLIC_KEY pstPublicKey);
    /**
     * Original signature : <code>int sdkRSAPrivateDecrypt(unsigned char*, int*, const unsigned char*, int, const SDK_RSA_PRIVATE_KEY*)</code><br>
     * <i>native declaration : line 62</i><br>
     * @deprecated use the safer methods {@link #sdkRSAPrivateDecrypt(java.nio.ByteBuffer, java.nio.IntBuffer, byte[], int, test.SDK_RSA_PRIVATE_KEY)} and {@link #sdkRSAPrivateDecrypt(com.sun.jna.Pointer, com.sun.jna.ptr.IntByReference, com.sun.jna.Pointer, int, test.SDK_RSA_PRIVATE_KEY)} instead
     */
    @Deprecated
    int sdkRSAPrivateDecrypt(Pointer pheDest, IntByReference psiDestlen, Pointer pucSrc, int siSrclen, SDK_RSA_PRIVATE_KEY pstPrivateKey);
    /**
     * Original signature : <code>int sdkRSAPrivateDecrypt(unsigned char*, int*, const unsigned char*, int, const SDK_RSA_PRIVATE_KEY*)</code><br>
     * <i>native declaration : line 62</i>
     */
    int sdkRSAPrivateDecrypt(ByteBuffer pheDest, IntBuffer psiDestlen, byte pucSrc[], int siSrclen, SDK_RSA_PRIVATE_KEY pstPrivateKey);
    /**
     * Original signature : <code>int sdkAscToBcd(unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 64</i><br>
     * @deprecated use the safer methods {@link #sdkAscToBcd(java.nio.ByteBuffer, byte[], int)} and {@link #sdkAscToBcd(com.sun.jna.Pointer, com.sun.jna.Pointer, int)} instead
     */
    @Deprecated
    int sdkAscToBcd(Pointer pbcDest, Pointer pasSrc, int siSrclen);
    /**
     * Original signature : <code>int sdkAscToBcd(unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 64</i>
     */
    int sdkAscToBcd(ByteBuffer pbcDest, byte pasSrc[], int siSrclen);
    /**
     * Original signature : <code>bool sdkIsAscii(const unsigned char*)</code><br>
     * <i>native declaration : line 65</i><br>
     * @deprecated use the safer methods {@link #sdkIsAscii(byte[])} and {@link #sdkIsAscii(com.sun.jna.Pointer)} instead
     */
    @Deprecated
    byte sdkIsAscii(Pointer pasSrc);
    /**
     * Original signature : <code>bool sdkIsAscii(const unsigned char*)</code><br>
     * <i>native declaration : line 65</i>
     */
    byte sdkIsAscii(byte pasSrc[]);
    /**
     * Original signature : <code>int sdkU8ToBcd(unsigned char*, unsigned const char, int)</code><br>
     * <i>native declaration : line 66</i><br>
     * @deprecated use the safer methods {@link #sdkU8ToBcd(java.nio.ByteBuffer, byte, int)} and {@link #sdkU8ToBcd(com.sun.jna.Pointer, byte, int)} instead
     */
    @Deprecated
    int sdkU8ToBcd(Pointer pbcDest, byte ucSrc, int siDestlen);
    /**
     * Original signature : <code>int sdkU8ToBcd(unsigned char*, unsigned const char, int)</code><br>
     * <i>native declaration : line 66</i>
     */
    int sdkU8ToBcd(ByteBuffer pbcDest, byte ucSrc, int siDestlen);
    /**
     * Original signature : <code>int sdkBcdToAsc(unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 67</i><br>
     * @deprecated use the safer methods {@link #sdkBcdToAsc(java.nio.ByteBuffer, byte[], int)} and {@link #sdkBcdToAsc(com.sun.jna.Pointer, com.sun.jna.Pointer, int)} instead
     */
    @Deprecated
    int sdkBcdToAsc(Pointer pasDest, Pointer pbcSrc, int siBcdSrclen);
    /**
     * Original signature : <code>int sdkBcdToAsc(unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 67</i>
     */
    int sdkBcdToAsc(ByteBuffer pasDest, byte pbcSrc[], int siBcdSrclen);
    /**
     * Original signature : <code>int sdkU16ToAsc(unsigned short const, unsigned char*)</code><br>
     * <i>native declaration : line 68</i><br>
     * @deprecated use the safer methods {@link #sdkU16ToAsc(short, java.nio.ByteBuffer)} and {@link #sdkU16ToAsc(short, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkU16ToAsc(short usSrc, Pointer pasDest);
    /**
     * Original signature : <code>int sdkU16ToAsc(unsigned short const, unsigned char*)</code><br>
     * <i>native declaration : line 68</i>
     */
    int sdkU16ToAsc(short usSrc, ByteBuffer pasDest);
    /**
     * Original signature : <code>int sdkU32ToHex(unsigned char*, unsigned const int, int)</code><br>
     * <i>native declaration : line 69</i><br>
     * @deprecated use the safer methods {@link #sdkU32ToHex(java.nio.ByteBuffer, int, int)} and {@link #sdkU32ToHex(com.sun.jna.Pointer, int, int)} instead
     */
    @Deprecated
    int sdkU32ToHex(Pointer pheDest, int uiSrc, int siHexlen);
    /**
     * Original signature : <code>int sdkU32ToHex(unsigned char*, unsigned const int, int)</code><br>
     * <i>native declaration : line 69</i>
     */
    int sdkU32ToHex(ByteBuffer pheDest, int uiSrc, int siHexlen);
    /**
     * Original signature : <code>int sdkU32ToAsc(unsigned const int, unsigned char*)</code><br>
     * <i>native declaration : line 70</i><br>
     * @deprecated use the safer methods {@link #sdkU32ToAsc(int, java.nio.ByteBuffer)} and {@link #sdkU32ToAsc(int, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkU32ToAsc(int uiSrc, Pointer pasDest);
    /**
     * Original signature : <code>int sdkU32ToAsc(unsigned const int, unsigned char*)</code><br>
     * <i>native declaration : line 70</i>
     */
    int sdkU32ToAsc(int uiSrc, ByteBuffer pasDest);
    /**
     * Original signature : <code>int sdku8ToAsc(unsigned const char, unsigned char*)</code><br>
     * <i>native declaration : line 71</i><br>
     * @deprecated use the safer methods {@link #sdku8ToAsc(byte, java.nio.ByteBuffer)} and {@link #sdku8ToAsc(byte, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdku8ToAsc(byte ucSrc, Pointer pasDest);
    /**
     * Original signature : <code>int sdku8ToAsc(unsigned const char, unsigned char*)</code><br>
     * <i>native declaration : line 71</i>
     */
    int sdku8ToAsc(byte ucSrc, ByteBuffer pasDest);
    /**
     * Original signature : <code>int sdkAscToBcdR(unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 72</i><br>
     * @deprecated use the safer methods {@link #sdkAscToBcdR(java.nio.ByteBuffer, byte[], int)} and {@link #sdkAscToBcdR(com.sun.jna.Pointer, com.sun.jna.Pointer, int)} instead
     */
    @Deprecated
    int sdkAscToBcdR(Pointer pbcDest, Pointer pasSrc, int siBcdlen);
    /**
     * Original signature : <code>int sdkAscToBcdR(unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 72</i>
     */
    int sdkAscToBcdR(ByteBuffer pbcDest, byte pasSrc[], int siBcdlen);
    /**
     * Original signature : <code>int sdkBcdToU16(unsigned short*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 73</i><br>
     * @deprecated use the safer methods {@link #sdkBcdToU16(java.nio.ShortBuffer, byte[], int)} and {@link #sdkBcdToU16(com.sun.jna.ptr.ShortByReference, com.sun.jna.Pointer, int)} instead
     */
    @Deprecated
    int sdkBcdToU16(short pusDest, Pointer pbcSrc, int siSrclen);
    /**
     * Original signature : <code>int sdkBcdToU16(unsigned short*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 73</i>
     */
    int sdkBcdToU16(short pusDest, byte pbcSrc[], int siSrclen);
    /**
     * Original signature : <code>int sdkU16ToBcd(unsigned char*, unsigned short const, int)</code><br>
     * <i>native declaration : line 74</i><br>
     * @deprecated use the safer methods {@link #sdkU16ToBcd(java.nio.ByteBuffer, short, int)} and {@link #sdkU16ToBcd(com.sun.jna.Pointer, short, int)} instead
     */
    @Deprecated
    int sdkU16ToBcd(Pointer pbcDest, short usSrc, int siDestlen);
    /**
     * Original signature : <code>int sdkU16ToBcd(unsigned char*, unsigned short const, int)</code><br>
     * <i>native declaration : line 74</i>
     */
    int sdkU16ToBcd(ByteBuffer pbcDest, short usSrc, int siDestlen);
    /**
     * Original signature : <code>int sdkXOR8(unsigned char*, const unsigned char*, const unsigned char*)</code><br>
     * <i>native declaration : line 75</i><br>
     * @deprecated use the safer methods {@link #sdkXOR8(java.nio.ByteBuffer, byte[], byte[])} and {@link #sdkXOR8(com.sun.jna.Pointer, com.sun.jna.Pointer, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkXOR8(Pointer pheDest, Pointer pheSrc1, Pointer pheSrc2);
    /**
     * Original signature : <code>int sdkXOR8(unsigned char*, const unsigned char*, const unsigned char*)</code><br>
     * <i>native declaration : line 75</i>
     */
    int sdkXOR8(ByteBuffer pheDest, byte pheSrc1[], byte pheSrc2[]);
    /**
     * Original signature : <code>int sdkEcb(unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 76</i><br>
     * @deprecated use the safer methods {@link #sdkEcb(java.nio.ByteBuffer, byte[], int)} and {@link #sdkEcb(com.sun.jna.Pointer, com.sun.jna.Pointer, int)} instead
     */
    @Deprecated
    int sdkEcb(Pointer pheDest, Pointer pheSrc, int siLen);
    /**
     * Original signature : <code>int sdkEcb(unsigned char*, const unsigned char*, int)</code><br>
     * <i>native declaration : line 76</i>
     */
    int sdkEcb(ByteBuffer pheDest, byte pheSrc[], int siLen);

    int sdkBcdAdd(byte [] pbcDest, byte pbcSrc1[], int siBcdSrc1len, byte [] pbcSrc2, int siBcdSrc2len);
    public interface sdkJcbSetDispRemoveCard_fun_setdispremovecard_callback extends Callback {
        int apply();
    };
    /** <i>native declaration : line 6</i> */
    public interface sdkJcbSetBeforeGPO_fun_setbeforegpo_callback extends Callback {
        int apply();
    };
    /** <i>native declaration : line 7</i> */
    public interface sdkJcbSetVerifyCardNo_fun_setVerifyCardNo_callback extends Callback {
        int apply(Pointer asPAN);
    };
    /** <i>native declaration : line 9</i> */
    public interface sdkJcbSetRevocationKey_fun_cmprevocation_callback extends Callback {
        int apply(Pointer key);
    };
    /** <i>native declaration : line 12</i> */
    public interface sdkJcbSetDispTapCardAgain_fun_setDispTapCardAgain_callback extends Callback {
        int apply();
    };
    /**
     * Original signature : <code>int sdkJcbTransInit()</code><br>
     * <i>native declaration : line 2</i>
     */
    int sdkJcbTransInit();
    /**
     * Original signature : <code>int sdkJcbTransFlow1()</code><br>
     * <i>native declaration : line 3</i>
     */
    int sdkJcbTransFlow1();
    /**
     * Original signature : <code>int sdkJcbTransFlow2()</code><br>
     * <i>native declaration : line 4</i>
     */
    int sdkJcbTransFlow2();
    /**
     * fun_setdispremovecard\ufffd\ufffd\ufffd\ufffd\u05b5:SDK_OK:\ufffd\u0279\ufffd; SDK_ERR:\u02a7\ufffd\ufffd<br>
     * Original signature : <code>int sdkJcbSetDispRemoveCard(sdkJcbSetDispRemoveCard_fun_setdispremovecard_callback*)</code><br>
     * @param fun_setdispremovecard fun_setdispremovecard\ufffd\ufffd\ufffd\ufffd\u05b5:SDK_OK:\ufffd\u0279\ufffd; SDK_ERR:\u02a7\ufffd\ufffd<br>
     * <i>native declaration : line 5</i>
     */
    int sdkJcbSetDispRemoveCard(TestLibrary.sdkJcbSetDispRemoveCard_fun_setdispremovecard_callback fun_setdispremovecard);
    /**
     * fun_setbeforegpo\ufffd\ufffd\ufffd\ufffd\u05b5:SDK_OK:\ufffd\u0279\ufffd; SDK_ERR:\u02a7\ufffd\ufffd<br>
     * Original signature : <code>int sdkJcbSetBeforeGPO(sdkJcbSetBeforeGPO_fun_setbeforegpo_callback*)</code><br>
     * @param fun_setbeforegpo fun_setbeforegpo\ufffd\ufffd\ufffd\ufffd\u05b5:SDK_OK:\ufffd\u0279\ufffd; SDK_ERR:\u02a7\ufffd\ufffd<br>
     * <i>native declaration : line 6</i>
     */
    int sdkJcbSetBeforeGPO(TestLibrary.sdkJcbSetBeforeGPO_fun_setbeforegpo_callback fun_setbeforegpo);
    /**
     * fun_setVerifyCardNo\ufffd\ufffd\ufffd\ufffd\u05b5:SDK_OK:\ufffd\u0279\ufffd; SDK_ERR:\u02a7\ufffd\ufffd<br>
     * Original signature : <code>int sdkJcbSetVerifyCardNo(sdkJcbSetVerifyCardNo_fun_setVerifyCardNo_callback*)</code><br>
     * @param fun_setVerifyCardNo fun_setVerifyCardNo\ufffd\ufffd\ufffd\ufffd\u05b5:SDK_OK:\ufffd\u0279\ufffd; SDK_ERR:\u02a7\ufffd\ufffd<br>
     * <i>native declaration : line 7</i>
     */
    int sdkJcbSetVerifyCardNo(TestLibrary.sdkJcbSetVerifyCardNo_fun_setVerifyCardNo_callback fun_setVerifyCardNo);
    /**
     * Original signature : <code>int sdkJcbGetTransMode()</code><br>
     * <i>native declaration : line 8</i>
     */
    int sdkJcbGetTransMode();
    /**
     * Original signature : <code>int sdkJcbSetRevocationKey(sdkJcbSetRevocationKey_fun_cmprevocation_callback*)</code><br>
     * <i>native declaration : line 9</i>
     */
    int sdkJcbSetRevocationKey(TestLibrary.sdkJcbSetRevocationKey_fun_cmprevocation_callback fun_cmprevocation);
    /**
     * Original signature : <code>int sdkJcbSetForceOnline(bool)</code><br>
     * <i>native declaration : line 10</i>
     */
    int sdkJcbSetForceOnline(byte bIsForceOnline);
    /**
     * Original signature : <code>int sdkJcbImportOnlineResult(int, const unsigned char*)</code><br>
     * <i>native declaration : line 11</i><br>
     * @deprecated use the safer methods {@link #sdkJcbImportOnlineResult(int, byte[])} and {@link #sdkJcbImportOnlineResult(int, com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkJcbImportOnlineResult(int ucOnlineResult, Pointer pheRspCode);
    /**
     * Original signature : <code>int sdkJcbImportOnlineResult(int, const unsigned char*)</code><br>
     * <i>native declaration : line 11</i>
     */
    int sdkJcbImportOnlineResult(int ucOnlineResult, byte pheRspCode[]);
    /**
     * fun_setDispTapCardAgain\ufffd\ufffd\u02be\ufffd\ufffd\ufffd\ufffd\ufffd,\u023b\ufffd\ufffd\u03bb: SDK_OK:\ufffd\u0279\ufffd; SDK_ERR:\u02a7\ufffd\ufffd<br>
     * Original signature : <code>int sdkJcbSetDispTapCardAgain(sdkJcbSetDispTapCardAgain_fun_setDispTapCardAgain_callback*)</code><br>
     * @param fun_setDispTapCardAgain fun_setDispTapCardAgain\ufffd\ufffd\u02be\ufffd\ufffd\ufffd\ufffd\ufffd,\u023b\ufffd\ufffd\u03bb: SDK_OK:\ufffd\u0279\ufffd; SDK_ERR:\u02a7\ufffd\ufffd<br>
     * <i>native declaration : line 12</i>
     */
    int sdkJcbSetDispTapCardAgain(TestLibrary.sdkJcbSetDispTapCardAgain_fun_setDispTapCardAgain_callback fun_setDispTapCardAgain);
    /**
     * Original signature : <code>int sdkJcbSetOption(unsigned char)</code><br>
     * <i>native declaration : line 13</i>
     */
    int sdkJcbSetOption(byte option);
    /**
     * Original signature : <code>int sdkJcbSetStatic9F53(unsigned char*)</code><br>
     * <i>native declaration : line 14</i><br>
     * @deprecated use the safer methods {@link #sdkJcbSetStatic9F53(java.nio.ByteBuffer)} and {@link #sdkJcbSetStatic9F53(com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkJcbSetStatic9F53(Pointer data);
    /**
     * Original signature : <code>int sdkJcbSetStatic9F53(unsigned char*)</code><br>
     * <i>native declaration : line 14</i>
     */
    int sdkJcbSetStatic9F53(ByteBuffer data);
    /**
     * Original signature : <code>int sdkJcbGetStatic9F53(unsigned char*)</code><br>
     * <i>native declaration : line 15</i><br>
     * @deprecated use the safer methods {@link #sdkJcbGetStatic9F53(java.nio.ByteBuffer)} and {@link #sdkJcbGetStatic9F53(com.sun.jna.Pointer)} instead
     */
    @Deprecated
    int sdkJcbGetStatic9F53(Pointer data);
    /**
     * Original signature : <code>int sdkJcbGetStatic9F53(unsigned char*)</code><br>
     * <i>native declaration : line 15</i>
     */
    int sdkJcbGetStatic9F53(ByteBuffer data);
    /**
     * Original signature : <code>int sdkJcbTransReadAppData()</code><br>
     * <i>native declaration : line 16</i>
     */
    int sdkJcbTransReadAppData();
}



