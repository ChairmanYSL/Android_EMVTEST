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
        int apply(byte ucIccEncryptWay, byte ucPINTryCount, Pointer pheCreditPwd);
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
    int sdkEMVBaseReadTLV(byte pheTag[], ByteBuffer pheOutData, IntBuffer psiOutLen);
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
}



