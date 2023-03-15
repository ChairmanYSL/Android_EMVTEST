package com.szzt.emvtest;

import android.app.Application;
import android.content.Context;
import android.hardware.usb.UsbManager;
import android.text.method.KeyListener;
import android.util.Log;

import java.io.UnsupportedEncodingException;
import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.security.Key;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Timer;

import com.sun.jna.Pointer;
import com.szzt.android.util.HexDump;
import com.szzt.emvtest.EMVSDK.SDK_EMVBASE_AID_STRUCT;
import com.szzt.emvtest.EMVSDK.SDK_EMVBASE_CAPK_STRUCT;
import com.szzt.emvtest.EMVSDK.SDK_EMVBASE_CL_HIGHESTAID;
import com.szzt.emvtest.EMVSDK.SDK_RETURN_CODE;
import com.szzt.emvtest.EMVSDK.SDK_DISP;
import com.szzt.sdk.device.beep.Beep;
import com.szzt.sdk.device.card.ContactlessCardReader;
import com.szzt.sdk.device.port.SerialPort;
import com.szzt.emvtest.PosTimer;
import com.szzt.emvtest.SimData;
import com.szzt.emvtest.TestLibrary;
import com.szzt.sdk.system.HwSecurityManagerV2;

public class EMVApp extends Application
{
    private static EMVApp signleton = new EMVApp();
    private  PosLcd g_lcd;
    public  PosKeyboard g_keyboard;
    private MainApplication g_Application;
    public MainActivity mainActivity;
    //trans var
    public static boolean AutoTestFlag = false;
    public int gAutoTestTimeout = 5;

    public byte [] gstAmount;
    public byte [] gstAmountOther;
    public byte gstTransType;
    public boolean receiveSerialFlag = false;
    public static SimData g_simdata;

    public static int curExternAIDIndex;
    public static ExternAIDParam [] g_externAIDParams;
    public int BCTCret;

    public static EMVApp getInstance() {return signleton;}
    public static  interface  KeyCodeListener{
        public void sendKeyCode(int keyCode);
    }

    public static interface SerialDataListener{
        public void SendSerialFlag(boolean flag);
    }



    public PosLcd getPosLcd() {return g_lcd;}
    public void setPosLcd(PosLcd posLcd) {this.g_lcd = posLcd;}
    public PosKeyboard getPosKeyboard() {return g_keyboard;}
    public void setPosKeyboard(PosKeyboard posKeyboard) {
        Log.d("lishiyao", "setPosKeyboard: ");
        this.g_keyboard = posKeyboard;
        Log.d("lishiyao", "setPosKeyboard: "+this.g_keyboard.toString());
    }
    public void setMainApplication(MainApplication app){
        this.g_Application = app;
    }
    public MainApplication getMainApplication(){
     return g_Application;
    }

    public void InitData()
    {
        gstAmount = new byte[6];
        gstAmountOther = new byte[6];
        g_simdata = new SimData();
        g_externAIDParams = ExternAIDParam.Create(40);
        curExternAIDIndex = 0;
    }

    public void showTransMenu() {
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "JCB L2 TEST", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "1.SALE 2.Other Trans", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE3, 0, "3.Jcb Param 4.AutoTest", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE4, 0, "5.BCTC Param", SDK_DISP.SDK_DISP_LEFT);
    }

    public  int Menu0() {
        DeviceOpera deviceOpera = new DeviceOpera();
        int ret = 0,key = 0;
        boolean breakFlag = false;

        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "JCB L2 TEST", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "1.SALE 2.Other Trans", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE3, 0, "3.Jcb Param 4.AutoTest", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE4, 0, "5.BCTC Param", SDK_DISP.SDK_DISP_LEFT);

        deviceOpera.Kb_Flush();
        while (true)
        {
            key = deviceOpera.Kb_GetKey();
            if(AutoTestFlag)
            {
                ret =  BCTCStartTrade();
                if(ret == SDK_RETURN_CODE.SDK_OK)
                {
                    key = PosKeyboard.KEYCANCEL;
                }
            }
            else
            {
                ret = BCTCSignleTrade();
                if(ret == SDK_RETURN_CODE.SDK_OK)
                {
                    key = PosKeyboard.KEYFN;
                }
            }
            switch (key)
            {
                case PosKeyboard.KEYCANCEL:
                    AutoTestFlag = false;
                    break;
                case PosKeyboard.KEYFN:
                    DealTrade();
                    breakFlag = true;
                    break;
                case PosKeyboard.KEY4:
                    posSetAutoTest();
                    breakFlag = true;
                    break;
                case PosKeyboard.KEY5:
                    posSetBCTCParam();
                    breakFlag = true;
                    break;
                case PosKeyboard.KEY3:
                    posSetJCBParam();
                    breakFlag = true;
                    break;
                default:
                    break;
            }
            if(breakFlag)
            {
                break;
            }
        }

        return key;
    }

    public int Menu1() {
        DeviceOpera deviceOpera = new DeviceOpera();
        int ret = 0,key = 0;
        boolean breakFlag = false;

        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "Menu1", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "1.Date       2.Terminal", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE3, 0, "3.Settle     4.RF Para", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE4, 0, "5.Batch mode 6.AID Param", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE5, 0, "7.Sys Init   8.Printer", SDK_DISP.SDK_DISP_LEFT);

        return key;
    }

    public void showBCTCSettingMenu() {
        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "BCTC Settings", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "1.AID     2.CAPK", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE3, 0, "3.Term Parm   4.ExceptionFile", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE4, 0, "5.Revocation PK", SDK_DISP.SDK_DISP_LEFT);
    }

    public void posSetBCTCParam() {
        PosTimer timer = new PosTimer(1000*20);
        int key;
        DeviceOpera deviceOpera = new DeviceOpera();

        showBCTCSettingMenu();
        timer.timerStart();
        while (timer.checkTimerExpired() == false){
            key = deviceOpera.Kb_GetKey();
            switch (key){
                case PosKeyboard.KEY1:
                    BCTCDownloadAID();
                    showBCTCSettingMenu();
                    break;
                case PosKeyboard.KEY2:
                    BCTCDownloadCAPK();
                    showBCTCSettingMenu();
                    break;
                case PosKeyboard.KEY3:
                    BCTCDownloadTermParam();
                    showBCTCSettingMenu();
                    break;
                case PosKeyboard.KEY4:
                    BCTCDownloadExceptionFile();
                    showBCTCSettingMenu();
                    break;
                case PosKeyboard.KEY5:
                    BCTCDownloadRevocationPK();
                    showBCTCSettingMenu();
                    break;
                case PosKeyboard.KEYCANCEL:
                    return;
            }
        }
        return;
    }
    public void BCTCDownloadAID() {
        int ret;

        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "Download AID", SDK_DISP.SDK_DISP_CENTER);
        TestLibrary.INSTANCE.sdkEMVBaseDelAllAIDLists();
        if(g_externAIDParams == null)
        {
            g_externAIDParams = ExternAIDParam.Create(40);
        }
        SerialPort port = g_Application.getSerialPortWrapperImpl();
        int portID = port.open("/dev/ttyUSB0", 115200);
        if (portID > 0)
        {
            byte [] sendData = new byte[4];
            byte [] recvData = new byte[1024];
            byte [] finishCode = new byte[7];
            sendData[0] = (byte)0x02;
            sendData[1] = (byte)0xC3;
            sendData[2] = 0;
            sendData[3] = 0;

            finishCode[0] = (byte)0x02;
            finishCode[1] = (byte)0x83;
            finishCode[2] = (byte)0x00;
            finishCode[3] = (byte)0x03;
            finishCode[4] = (byte)0x03;
            finishCode[5] = (byte)0x01;
            finishCode[6] = 0;

            do{
                ret = port.send(portID, sendData, 4);
                if(ret > 0)
                {
                    ret = port.receive(portID, recvData, 1024, 60);
                    if(ret > 0)
                    {
                        if(ret == 7)
                        {
                            break;
                        }
                        else
                        {
                            ret = tlvToAID(subByte(recvData, 4, recvData.length-4), recvData[2]*256+recvData[3]);
                        }
                    }
                }
            }while (ret > 0 && strstr(recvData, finishCode, 0, finishCode.length) < 0);
        }
        else
        {
            Log.e("lishiyao", "BCTCDownloadAID: open Comm Error!");
        }
    }
    public void BCTCDownloadCAPK() {
        int ret;

        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "Download CAPK", SDK_DISP.SDK_DISP_CENTER);
        TestLibrary.INSTANCE.sdkEMVBaseDelAllCAPKLists();

        SerialPort port = g_Application.getSerialPortWrapperImpl();
        int portID = port.open("/dev/ttyUSB0", 115200);
        if (portID > 0)
        {
            byte [] sendData = new byte[4];
            byte [] recvData = new byte[1024];
            byte [] finishCode = new byte[7];
            sendData[0] = (byte)0x02;
            sendData[1] = (byte)0xC2;
            sendData[2] = 0;
            sendData[3] = 0;

            finishCode[0] = (byte)0x02;
            finishCode[1] = (byte)0x82;
            finishCode[2] = (byte)0x00;
            finishCode[3] = (byte)0x03;
            finishCode[4] = (byte)0x03;
            finishCode[5] = (byte)0x01;
            finishCode[6] = 0;

            do{
                ret = port.send(portID, sendData, 4);
                if(ret > 0)
                {
                    ret = port.receive(portID, recvData, 1024, 60);
                    if(ret > 0)
                    {
                        if(ret == 7)
                        {
                            break;
                        }
                        else
                        {
                            ret = tlvToAID(subByte(recvData, 4, recvData.length-4), recvData[2]*256+recvData[3]);
                        }
                    }
                }
            }while (ret > 0 && strstr(recvData, finishCode, 0, finishCode.length) < 0);
        }
        else
        {
            Log.e("lishiyao", "BCTCDownloadCAPK: open Comm Error!");
        }
    }
    public void BCTCDownloadTermParam(){

    }
    public void BCTCDownloadExceptionFile(){

    }
    public void BCTCDownloadRevocationPK(){

    }

    public void posSetJCBParam() {
        int key;
        DeviceOpera deviceOpera = new DeviceOpera();
        PosTimer timer = new PosTimer(1000*10);

        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "TRANSACTION MODE", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "Support emv mode?", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE3, 0, "0-NO     1-YES", SDK_DISP.SDK_DISP_CENTER);


    }
    public void posSetAutoTest() {
        PosTimer timer = new PosTimer(1000*10);
        DeviceOpera deviceOpera = new DeviceOpera();
        int key;

        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "Whether Enable AutoTest", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "0-Disable     1-Enable", SDK_DISP.SDK_DISP_CENTER);

        timer.timerStart();
        while (!timer.checkTimerExpired())
        {
            key = deviceOpera.Kb_GetKey();
            Log.d("lishiyao", "posSetAutoTest: Kb_GetKey:"+key);
            if(key == PosKeyboard.KEY1)
            {
                AutoTestFlag = true;
                break;
            }
            else if(key == PosKeyboard.KEY0)
            {
                AutoTestFlag = false;
                break;
            }
        }
        Log.d("lishiyao", "posSetAutoTest: AutoTestFlag:"+AutoTestFlag);

        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "AutoTest Interval", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, String.valueOf(gAutoTestTimeout), SDK_DISP.SDK_DISP_CENTER);
        List<Integer> keyList = new ArrayList<>();
        keyList = getInputKeyList(10, SDK_DISP.SDK_DISP_LINE2, SDK_DISP.SDK_DISP_RIGHT);
        if(keyList == null)
        {
            return;
        }
        else
        {
            Log.d("lishiyao", "posSetAutoTest: keyList:"+ keyList);
            gAutoTestTimeout = keyList.get(0) - 0x30;
            for (int i = 1; i < keyList.size(); i++)
            {
                gAutoTestTimeout += ((keyList.get(i) - 0x30) * 10 * i);
            }
        }

        return;
    }
    public int InputTradeAmount(byte [] Amount) {
        int p_key = 0;
        int i = 0;
        List< Key > p_keyList;
        byte[] AmountBCD = new byte[6];

        if(Amount == null)
        {
            return SDK_RETURN_CODE.SDK_OK;
        }
        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "Purchase", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "Please Input Amount:", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE3, 0, "0.00", SDK_DISP.SDK_DISP_RIGHT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE5, 0,"press [CLEAR] if err", SDK_DISP.SDK_DISP_RIGHT);

        g_keyboard.Kb_Flush();
        while (true)
        {
            p_key = g_keyboard.Kb_GetKey();
            switch (p_key)
            {
                case '1':

                    break;
                case 0xFF:
                    break;
                default:
                    return 1;
            }
            if(p_key == g_keyboard.KEYENTER)
            {
                break;
            }
        }


        return SDK_RETURN_CODE.SDK_OK;
    }

    public List<Integer> getInputKeyList(int timeout, int dispLine, int attr){
        PosTimer timer = new PosTimer(timeout*1000);
        List<Integer> keyList = null;
        keyList = new ArrayList<>();
        int key;
        DeviceOpera deviceOpera = new DeviceOpera();

        timer.timerStart();
        while (timer.checkTimerExpired() == false)
        {
            key = deviceOpera.Kb_GetKey();
            switch (key)
            {
                case 0:
                case PosKeyboard.KEYUP:
                case PosKeyboard.KEYDOWN:
                case PosKeyboard.KEYMENU:
                case PosKeyboard.KEYEXIT:
                case PosKeyboard.KEYFN:
                    break;
                case PosKeyboard.KEYCANCEL:
                    return null;
                case PosKeyboard.KEYCLEAR:
                    keyList.remove(keyList.size()-1);
                    break;
                case PosKeyboard.KEYENTER:
                    break;
                default:
                    keyList.add(key);
            }
            if(key == PosKeyboard.KEYENTER)
            {
                return keyList;
            }
            if(key >= PosKeyboard.KEY0 && key <= PosKeyboard.KEY9)
            {
                StringBuilder disp = new StringBuilder();
                for(int i = 0; i < keyList.size();i++)
                {
                    char c = (char)key;
                    disp.append(c);
                }
                g_lcd.Lcd_PrintfXY(dispLine, 0, disp.toString(), attr);
            }
        }
        return null;
    }

    public void ReadCardDisp(){
        byte [] temp = new byte [30];
        byte [] text = new byte [30];
        byte [] amount = new byte[6];

        Arrays.fill(temp, (byte) 0);
        Arrays.fill(amount, (byte) 0);
        TestLibrary.INSTANCE.sdkBcdAdd(amount,gstAmount, gstAmount.length, gstAmountOther, gstAmountOther.length);
        Arrays.fill(text, (byte) 0);
        RegulateAmount(text, temp);

        DeviceOpera deviceOpera = new DeviceOpera();
        deviceOpera.Lcd_Cls();
        deviceOpera.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "Present Card", SDK_DISP.SDK_DISP_CENTER);
        deviceOpera.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE5, 0, text.toString(), SDK_DISP.SDK_DISP_CENTER);
    }
    public int DealTrade(){
        PosTimer timer = new PosTimer(1000*6);
        int key;
        int ret = 0;
        final int[] Collisionret = new int[1];
        byte option = (byte)0xFC;
        DeviceOpera deviceOpera = new DeviceOpera();

        Log.d("lishiyao", "DealTrade: 9F02"+HexDump.toHexStringValue(gstAmount));
        Log.d("lishiyao", "DealTrade: 9F03"+HexDump.toHexStringValue(gstAmountOther));
        Log.d("lishiyao", "DealTrade: 9C"+HexDump.toHexStringX(gstTransType));

        TestLibrary.INSTANCE.sdkEMVBaseTransInit();
        TestLibrary.sdkEMVBaseSetInputPINFun_fun_inputpin_callback inputPINCallback = new TestLibrary.sdkEMVBaseSetInputPINFun_fun_inputpin_callback() {
            @Override
            public int apply(byte ucIccEncryptWay, byte ucPINTryCount, byte [] pheCreditPwd) {
                int key, KeyNum;
                byte [] tempkey = new byte[13];
                byte [] KeyBuf = new byte[13];
                byte ucMin = 4, ucMax = 12;
                DeviceOpera deviceOpera = new DeviceOpera();
                Beep beeper = g_Application.getBeep();

                if(AutoTestFlag)
                {
                    KeyNum = 4;
                    KeyBuf[0] = '1';
                    KeyBuf[1] = '2';
                    KeyBuf[2] = '3';
                    KeyBuf[3] = '4';
                    pheCreditPwd[0] = (byte) KeyNum;

                    if(ucIccEncryptWay == 0)
                    {
                        pheCreditPwd[0]  = 8;
                        //TODO:add online enc pin
//                        OnlineEncPIN(KeyBuf);
                        KeyBuf = subByte(pheCreditPwd, 1, 8);
                    }
                    else
                    {
                        KeyBuf = subByte(pheCreditPwd, 1, KeyNum);
                    }
                    return SDK_RETURN_CODE.SDK_OK;
                }

                g_lcd.Lcd_Cls();
                byte [] tag = new byte[2];
                tag[0] = (byte)0x9F;
                tag[1] = (byte)0x02;

                IntBuffer lenBuffer = IntBuffer.allocate(1);

                byte [] asc = new byte[12];
                byte [] temp = new byte[6];

                TestLibrary.INSTANCE.sdkEMVBaseReadTLV(tag, temp, lenBuffer);
                RegulateAmount(asc, temp);
                StringBuilder sb = new StringBuilder("Amout:");
                sb.append(asc);
                g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, sb.toString(), SDK_DISP.SDK_DISP_CENTER);
                g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "Please Input PIN:", SDK_DISP.SDK_DISP_LEFT);
                g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE4, 0, "[ENTER]-BYPASS PIN", SDK_DISP.SDK_DISP_RIGHT);

                KeyNum = 0;
                Arrays.fill(tempkey,(byte)0);
                Arrays.fill(KeyBuf, (byte)0);
                deviceOpera.Kb_Flush();

                while (true)
                {
                    key = deviceOpera.Kb_GetKey();

                    if(key != 0)
                    {
                        switch(key)
                        {
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                            case '5':
                            case '6':
                            case '7':
                            case '8':
                            case '9':
                            case '0':

                                if(KeyNum < ucMax)
                                {
                                    tempkey[KeyNum] = '*';
                                    tempkey[KeyNum + 1] = 0;
                                    KeyBuf[KeyNum++] = (byte) key;

                                    deviceOpera.Lcd_ClearLine(SDK_DISP.SDK_DISP_LINE4,SDK_DISP.SDK_DISP_LINE4);
                                    deviceOpera.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE4, 0, tempkey.toString(), SDK_DISP.SDK_DISP_RIGHT);
                                    beeper.beep(3000);
                                }
                                else
                                {
                                    beeper.beep(3000);
                                }
                                break;

                            case PosKeyboard.KEYBCKSPACE:

                                if(KeyNum != 0)
                                {
                                    tempkey[KeyNum - 1] = 0;
                                    KeyBuf[KeyNum--] = 0;
                                    deviceOpera.Lcd_ClearLine(SDK_DISP.SDK_DISP_LINE4,SDK_DISP.SDK_DISP_LINE4);
                                    deviceOpera.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE4, 0, tempkey.toString(), SDK_DISP.SDK_DISP_RIGHT);
                                    beeper.beep(3000);
                                }
                                else
                                {
                                    deviceOpera.Lcd_ClearLine(SDK_DISP.SDK_DISP_LINE4,SDK_DISP.SDK_DISP_LINE4);
                                    deviceOpera.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE4, 0, " ", SDK_DISP.SDK_DISP_RIGHT);
                                    beeper.beep(3000);
                                }
                                break;

                            case PosKeyboard.KEYENTER:

                                if(KeyNum == 0)
                                {
                                    pheCreditPwd[0] = 0;
                                    return SDK_RETURN_CODE.SDK_OK;
                                }
                                else if (KeyNum < ucMin)
                                {
                                    beeper.beep(3000);
                                }
                                else
                                {
                                    pheCreditPwd[0] = (byte)KeyNum;

                                    if(ucIccEncryptWay == 0)
                                    {
                                        pheCreditPwd[0]  = 8;
                                        //TODO:add online enc pin
//                                        OnlineEncPIN(KeyBuf);
                                        KeyBuf = subByte(pheCreditPwd, 1, 8);
                                    }
                                    else
                                    {
                                        KeyBuf = subByte(pheCreditPwd, 1, KeyNum);
                                    }
                                    return SDK_RETURN_CODE.SDK_OK;
                                }
                                break;

                            case PosKeyboard.KEYCANCEL:
                                beeper.beep(3000);
                                return SDK_RETURN_CODE.SDK_ERR;

                            default:
                                beeper.beep(3000);
                                break;
                        }
                    }
                }
            }
        };
        TestLibrary.INSTANCE.sdkEMVBaseSetInputPINFun(inputPINCallback);
        TestLibrary.INSTANCE.sdkJcbTransInit();
        TestLibrary.INSTANCE.sdkJcbSetOption(option);
        TestLibrary.sdkJcbSetBeforeGPO_fun_setbeforegpo_callback BeforeGPOcallback = new TestLibrary.sdkJcbSetBeforeGPO_fun_setbeforegpo_callback() {
            @Override
            public int apply() {
                byte [] CVMRes = {(byte)0x9F,(byte)0x34};
                byte [] CVMRes_Value = {(byte)0x1F,(byte)0x00, (byte) 0x00};
                byte [] CountryCode = {(byte)0x9F,(byte)0x1A};
                byte [] CountryCode_Value = {(byte) 0x03, (byte) 0x92};
                byte [] JCBCombinationOptions = {(byte)0xFF,(byte)0x81,(byte)0x78};
                byte [] JCBRemovalTimeout = {(byte)0xFF,(byte)0x81,(byte)0x79};
                byte [] JCBStaticTIP = {(byte) 0xFF,(byte) 0x81, (byte) 0x80};
                byte [] MerchantName = {(byte) 0x9F,(byte) 0x4E};
                byte [] AcquireID = {(byte) 0x9F,(byte) 0x01};
                byte [] MerchCateCode = {(byte) 0x9F,(byte) 0x15};
                byte [] TransCurcyCode = {(byte) 0x5F,(byte) 0x2A};
                byte [] TransCurcyExp = {(byte) 0x5F,(byte) 0x36};
                byte [] AIDName = {(byte) 0x9F,(byte) 0x06};
                byte [] TermType = {(byte) 0x9F, (byte) 0x35};
                byte [] buf = new byte [16];
                IntBuffer len = IntBuffer.allocate(1);


                TestLibrary.INSTANCE.sdkEMVBaseConfigTLV(CVMRes, CVMRes_Value, 3);
                TestLibrary.INSTANCE.sdkEMVBaseConfigTLV(CountryCode, CountryCode_Value, 2);

                TestLibrary.INSTANCE.sdkEMVBaseReadTLV(AIDName, buf, len);
                len.flip();
                if(len.get(0) < 5)
                {
                    return  SDK_RETURN_CODE.SDK_OK;
                }
                for(int i = 0; i < curExternAIDIndex; i++)
                {
                    if(g_externAIDParams[i].Aid.equals(buf) && g_externAIDParams[i].AidLen == len.get(0))
                    {
                        TestLibrary.INSTANCE.sdkEMVBaseConfigTLV(JCBCombinationOptions, g_externAIDParams[i].CombOptions, 2);
                        TestLibrary.INSTANCE.sdkEMVBaseConfigTLV(JCBRemovalTimeout, g_externAIDParams[i].RemovalTimeout, 2);
                        TestLibrary.INSTANCE.sdkEMVBaseConfigTLV(JCBStaticTIP, g_externAIDParams[i].TIPStatic, 3);
                        byte [] TermType_Value = new byte[2];
                        Arrays.fill(TermType_Value, (byte) 0);
                        TermType_Value[0] = g_externAIDParams[i].terminaltype;
                    }
                }
                return SDK_RETURN_CODE.SDK_OK;
            }
        };
        TestLibrary.INSTANCE.sdkJcbSetBeforeGPO(BeforeGPOcallback);
        TestLibrary.INSTANCE.sdkEMVBaseSetSupportExternSelect((byte)0x01);

        ReadCardDisp();
        //read and reset contactless card
        ContactlessCardReader mCard=g_Application.getContactlessCardReader();
        if(mCard!=null){
            ret=mCard.open();
            if(ret>=0){
                timer.timerStart();
                mCard.listenForCard(60 * 1000, new ContactlessCardReader.onContactlessListener() {
                    @Override
                    public void onNotify(int ret, int event) {
                        if(ret==0){
                            Collisionret[0] =mCard.powerOn(new byte[256]);
                        }
                    }
                });
                while (!timer.checkTimerExpired())
                {
                    key = deviceOpera.Kb_GetKey();
                    if(key == PosKeyboard.KEYCANCEL)
                    {
                        mCard.cancel();
                        ret = -1;
                        break;
                    }
                }
            }
        }

        if(ret != -1 && Collisionret[0] != 2)
        {
            SDK_EMVBASE_CL_HIGHESTAID tempHighAid = new SDK_EMVBASE_CL_HIGHESTAID();
            ret = TestLibrary.INSTANCE.sdkEMVBaseEntryPoint(tempHighAid);
            if(ret != SDK_RETURN_CODE.SDK_OK)
            {
                return ret;
            }
            else
            {
                while (true)
                {
                    ret = TestLibrary.INSTANCE.sdkJcbTransFlow1();
                    if(ret ==  -317)
                    {
                        byte [] randomValue = new byte[4];
                        byte [] random = {(byte)0x9F, (byte) 0x37};
                        random = g_Application.getHwSecurityManager().getRandom(4);
                        TestLibrary.INSTANCE.sdkEMVBaseConfigTLV(random, randomValue, 4);
                        tempHighAid = new SDK_EMVBASE_CL_HIGHESTAID();
                        ret = TestLibrary.INSTANCE.sdkEMVBaseReSelectApp(tempHighAid);
                        if(ret == SDK_RETURN_CODE.SDK_OK)
                        {
                            continue;
                        }
                        else
                        {
                            ret = SDK_RETURN_CODE.SDK_ERR;
                            break;
                        }
                    }
                }
                switch (ret)
                {
                    case 303:
                    //TODO:add end trans online baowen
//                        ret = SendOnlineBag();
                        if(ret != SDK_RETURN_CODE.SDK_OK)
                        {
                            TestLibrary.INSTANCE.sdkJcbImportOnlineResult(SDK_RETURN_CODE.SDK_ERR, (Pointer) null);
                            break;
                        }
                        else
                        {
                            byte [] ResponseCode = {(byte) 0x30, (byte) 0x30};
                            TestLibrary.INSTANCE.sdkJcbImportOnlineResult(SDK_RETURN_CODE.SDK_OK, ResponseCode);
                            TestLibrary.INSTANCE.sdkJcbTransFlow2();
                        }
                        break;
                    default:
                        //TODO: add send trans
//                        ret = SendTransResult();
                        break;
                }
            }
        }

        return SDK_RETURN_CODE.SDK_OK;
    }

    public int BCTCSignleTrade()
    {
        SerialPort port;
        byte recvData [] = new byte[128];
        int ret;
        int tlvDataLen;

//        Log.d("lishiyao", "Start BCTCSignleTrade");
        port = g_Application.getSerialPortWrapperImpl();
        int portId=port.open("/dev/ttyUSB0",115200);
//        Log.d("lishiyao", "BCTCSignleTrade: portId"+portId);
        if(portId>0)
        {
            ret = port.receive(portId, recvData, 128, 100);
            if(ret > 0)
            {
                Log.d("lishiyao", "in BCTCSignleTrade receive pack len:"+ret);
            }

            if( ret >= 8)
            {
                Log.d("lishiyao", "in BCTCStartTrade recvData:" + HexDump.toHexStringValue(recvData));
                tlvDataLen = recvData[2] * 256 + recvData[3];
                if (tlvDataLen >= 3)
                {
                    byte [] tempbt = new byte[2];
                    int index;
                    tempbt[0] = (byte)0x9F;
                    tempbt[1] = (byte)0x02;
                    index = strstr(recvData, tempbt, 4, recvData.length);
                    if(index > 0)
                    {
                        gstAmount = subByte(recvData, index, (int)recvData[index+2]);
                    }
                    tempbt[0] = (byte) 0x9F;
                    tempbt[1] = (byte) 0x03;
                    index = strstr(recvData, tempbt, 4, recvData.length);
                    if(index > 0)
                    {
                        gstAmountOther = subByte(recvData, index, (int)recvData[index+2]);
                    }
                    byte transType;
                    transType = (byte)0x9C;

                    index = strchar(recvData,transType, 4, recvData.length);
                    if(index > 0)
                    {
                        gstTransType = recvData[index+2];
                    }
                }
                port.close(portId);
                return SDK_RETURN_CODE.SDK_OK;
            }
            else if(ret < 0)
            {
//                Log.e("lishiyao", "BCTCSignleTrade: receive comm error!");
                port.close(portId);
            }
        }
        else
        {
            Log.e("lishiyao", "BCTCSignleTrade: Open comm error!");
            port.close(portId);
        }
        return SDK_RETURN_CODE.SDK_ERR;
    }

    public int BCTCStartTrade() {
        SerialPort port;
        byte recvData [] = new byte[64];
        byte sendData [] = new byte[4];
        int ret;
        PosTimer timer = new PosTimer(1000*gAutoTestTimeout);
        int tlvDataLen;

        port = g_Application.getSerialPortWrapperImpl();
        int portId=port.open("/dev/ttyUSB0",115200);
        if(portId>0)
        {
            sendData[0] = (byte) 0x02;
            sendData[1] = (byte) 0xC0;
            sendData[2] = 0;
            sendData[3] = 0;

            ret = port.send(portId, sendData, 4);
            if(ret < 0)
            {
                return SDK_RETURN_CODE.SDK_ERR;
            }

            timer.timerStart();
            while (true)
            {
                ret = port.receive(portId, recvData, 64, 1);
                if(ret >= 4)
                {
                    Log.d("lishiyao", "in BCTCStartTrade receive pack len:"+ret);
                    break;
                }
                if(timer.checkTimerExpired())
                {
                    port.close(portId);
                    break;
                }
            }
            if( ret >= 4)
            {
                Log.d("lishiyao", "in BCTCStartTrade recvData:" + recvData.toString());
                tlvDataLen = recvData[2] * 256 + recvData[3];
                if (tlvDataLen >= 3)
                {
                    byte [] tempbt = new byte[2];
                    int index;
                    tempbt[0] = (byte)0x9F;
                    tempbt[1] = (byte)0x02;
                    index = strstr(recvData, tempbt, 4, recvData.length);
                    if(index > 0)
                    {
                        gstAmount = subByte(recvData, index, (int)recvData[index+2]);
                        Log.d("lishiyao", "BCTCStartTrade: gstAmount:"+HexDump.toHexStringValue(gstAmount));
                    }
                    tempbt[0] = (byte) 0x9F;
                    tempbt[1] = (byte) 0x03;
                    index = strstr(recvData, tempbt, 4, recvData.length);
                    if(index > 0)
                    {
                        gstAmountOther = subByte(recvData, index, (int)recvData[index+2]);
                        Log.d("lishiyao", "BCTCStartTrade: gstAmountOther:"+HexDump.toHexStringValue(gstAmountOther));
                    }
                    byte transType;
                    transType = (byte)0x9C;

                    index = strchar(recvData,transType, 4, recvData.length);
                    if(index > 0)
                    {
                        gstTransType = recvData[index+2];
                        Log.d("lishiyao", "BCTCStartTrade: gstTransType:"+HexDump.toHexStringX(gstTransType));
                    }
                }
                port.close(portId);
                return SDK_RETURN_CODE.SDK_OK;
            }
        }
        if(timer.checkTimerExpired())
        {
            port.close(portId);
            return SDK_RETURN_CODE.SDK_TIMEOUT;
        }
        else
        {
            port.close(portId);
            return SDK_RETURN_CODE.SDK_ERR;
        }
    }

    public int RegulateAmount(byte [] pasDest, byte [] pbcAmount)
    {
        int i, j = 0;
        ByteBuffer temp = ByteBuffer.allocate(32);

        if(pbcAmount == null || pasDest == null)
        {
            return SDK_RETURN_CODE.SDK_PARAM_ERR;
        }
        TestLibrary.INSTANCE.sdkBcdToAsc(temp, pbcAmount, 6);
        temp.flip();
        i = temp.toString().indexOf("0");

        if(i > 9)                                                                                   //ֻ�нǷ�
        {
            pasDest[0] = '0';
            pasDest[1] = '.';
            pasDest[2] = temp.get(11);
            pasDest[3] = temp.get(12);
            pasDest[4] = 0;
        }
        else
        {
            for(j = 0; j < (10 - i); j++)                                   //���ڽǷ�
            {
                pasDest[j] = temp.get(i+j+1);
            }

            pasDest[j++] = '.';
            pasDest[j++] = temp.get(11);                                                //��
            pasDest[j++] = temp.get(12);                                                //��
            pasDest[j++] = 0;
        }
        return SDK_RETURN_CODE.SDK_OK;
    }
    public static int strstr(byte[] str1, byte[] str2, int start, int end)
    {
        int index1 = start;
        int index2 = 0;
        if(str2!=null)
        {
            while(index1<str1.length && index1<end)
            {
                int dsite = 0;
                while(str1[index1+dsite]==str2[index2+dsite]) {
                    if(index2+dsite+1>=str2.length)
                        return index1;
                    dsite++;
                    if(index1+dsite>=str1.length || index2+dsite>=str2.length)
                        break;
                }
                index1++;
            }
            return -1;
        }
        else
            return index1;
    }

    public static int strchar(byte [] str, byte b, int star, int end){
        int i = 0;
        while (i<str.length && i < end){
            if(b == str[i])
            {
                return i;
            }
            else
            {
                i++;
            }
        }
        return -1;
    }

    public byte[] byteMerger(byte[] byte_1, byte[] byte_2){
        byte[] byte_3 = new byte[byte_1.length+byte_2.length];
        System.arraycopy(byte_1, 0, byte_3, 0, byte_1.length);
        System.arraycopy(byte_2, 0, byte_3, byte_1.length, byte_2.length);
        return byte_3;
    }
    public byte[] subByte(byte[] b,int off,int length){
        byte[] b1 = new byte[length];
        System.arraycopy(b, off, b1, 0, length);
        return b1;
    }

    public static boolean compareByteArrays(byte[] array1, byte[] array2) {
        if (array1.length != array2.length) {
            return false;
        }
        for (int i = 0; i < array1.length; i++) {
            if (array1[i] != array2[i]) {
                return false;
            }
        }
        return true;
    }

    public static byte[] BCDToHex(byte[] bcd) {
        byte[] hex = new byte[bcd.length * 2];
        for (int i = 0; i < bcd.length; i++) {
            byte b = bcd[i];
            hex[2 * i] = (byte) ((b & 0xF0) >> 4);
            hex[2 * i + 1] = (byte) (b & 0x0F);
        }
        return hex;
    }

    public static int byteToInt(byte b) {
        return (int) b & 0xFF;
    }

    public int tlvToAID(byte [] tlvData, int dataLen){
        byte [] TagDouble = new byte[2];
        byte [] TagTriple = new byte[3];
        int ret;
        SDK_EMVBASE_AID_STRUCT tempAid = new SDK_EMVBASE_AID_STRUCT();
        ExternAIDParam externAIDParam = new ExternAIDParam();

        TagDouble[0] = (byte)0x9F;
        TagDouble[1] = (byte)0x06;
        int tagDataLen = 0;

        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret == -1)
        {
            return SDK_RETURN_CODE.SDK_ERR;
        }
        else
        {
            tempAid.transvaule = (byte) 0xFF;
            tempAid.contactorcontactless = 0;
            tempAid.AidLen = tlvData[ret+2];
            tempAid.Aid = subByte(tlvData, ret+3, tempAid.AidLen);
            externAIDParam.Aid = tempAid.Aid;
            externAIDParam.AidLen = tempAid.AidLen;
        }

        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x01;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            if(tlvData[ret+3] == 0)
            {
                tempAid.Asi = 0;
            }
            else if(tlvData[ret+3] == 1)
            {
                tempAid.Asi = 1;
            }
        }
        //TacDefault
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x11;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.TacDefault = subByte(tlvData, ret+3, tlvData[ret+2]);
        }
        //TacOnline
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x12;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.TacOnline = subByte(tlvData, ret+3, tlvData[ret+2]);
        }
        //TacDecline
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x13;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.TacDecline = subByte(tlvData, ret+3, tlvData[ret+2]);
        }
        //DEFAULT DDOL
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x14;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.TermDDOLLen = tlvData[ret+2];
            tempAid.TermDDOL = subByte(tlvData, ret+3, tempAid.TermDDOLLen);
        }
        //偏置随机选择的阈值
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x15;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.Threshold = BCDToHex(subByte(tlvData, ret+3, tlvData[2]));
        }
        //偏置随机选择的最大目标百分数
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x16;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.MaxTargetPercent = tlvData[ret+3];
        }
        //随机选择的目标百分数
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x17;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.TargetPercent = tlvData[ret+3];
        }
        //终端最低限额（4 字节，16 进制）
        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x1B;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.FloorLimit = subByte(tlvData,ret+3, tlvData[ret+2]);
        }
        //应用版本
        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x09;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.AppVerNum = subByte(tlvData,ret+3, tlvData[ret+2]);
        }
        //Merchant Category Code
        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x15;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.MerchCateCode = subByte(tlvData,ret+3, tlvData[ret+2]);
        }
        //
        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x16;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.MerchID = subByte(tlvData,ret+3, tlvData[ret+2]);
        }
        //
        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x4E;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.MerchantNameLen = tlvData[ret+2];
            externAIDParam.MerchantName = subByte(tlvData,ret+3, externAIDParam.MerchantNameLen);
        }
        //
        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x1C;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.TermID = subByte(tlvData,ret+3, tlvData[ret+2]);
        }
        //
        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x1D;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.TRMDataLen = tlvData[ret+2];
            externAIDParam.TRMData = subByte(tlvData,ret+3, externAIDParam.TRMDataLen);
        }
        //Transaction Currency Exponent
        TagDouble[0] = (byte) 0x5F;
        TagDouble[1] = (byte) 0x36;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.TransCurcyExp = tlvData[ret+3];
        }
        //Transaction Reference Currency Cod
        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x3C;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.TransReferCurcyCode = subByte(tlvData,ret+3, tlvData[ret+2]);
        }
        //Transaction Reference Currency Exponent (n1)
        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x3D;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.TransReferCurcyExp = tlvData[ret+3];
        }
        //Transaction Reference Currency Cod
        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x7B;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.ectranslimit = subByte(tlvData,ret+3, tlvData[ret+2]);
        }

        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x01;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            if(tlvData[ret+2] > 6)
            {
                tlvData[ret+2] = 6;
            }
            externAIDParam.AcquireID = subByte(tlvData,ret+3, tlvData[ret+2]);
        }

        TagTriple[0] = (byte) 0xDF;
        TagTriple[1] = (byte) 0x81;
        TagTriple[2] = (byte) 0x02;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.TermTDOLLen = tlvData[ret+2];
            externAIDParam.TermTDOL = subByte(tlvData,ret+3, externAIDParam.TermTDOLLen);
        }

        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x19;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.cl_offlinelimit = subByte(tlvData,ret+3, tlvData[ret+2]);
        }

        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x20;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.cl_translimit = subByte(tlvData,ret+3, tlvData[ret+2]);
        }

        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x21;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempAid.cl_cvmlimit = subByte(tlvData,ret+3, tlvData[ret+2]);
        }

        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x40;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.CombOptions = subByte(tlvData,ret+3, tlvData[ret+2]);
        }

        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x41;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.RemovalTimeout = subByte(tlvData,ret+3, tlvData[ret+2]);
        }

        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x42;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            externAIDParam.TIPStatic = subByte(tlvData,ret+3, tlvData[ret+2]);
        }

        ret = TestLibrary.INSTANCE.sdkEMVBaseAddAnyAIDList(tempAid, 1);
        if(ret == 1)
        {
            ExternAIDParam.Update(g_externAIDParams, curExternAIDIndex, externAIDParam);
            curExternAIDIndex++;
            g_lcd.Lcd_Cls();
            g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, tempAid.Aid.toString(), SDK_DISP.SDK_DISP_CENTER);
            g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "Download OK", SDK_DISP.SDK_DISP_CENTER);
        }
        return SDK_RETURN_CODE.SDK_OK;
    }

    public int tlvToCAPK(byte [] tlvData, int dataLen) {
        byte [] TagDouble = new byte[2];
        byte [] TagTriple = new byte[3];
        int ret;
        SDK_EMVBASE_CAPK_STRUCT tempCAPK = new SDK_EMVBASE_CAPK_STRUCT();

        TagDouble[0] = (byte)0x9F;
        TagDouble[1] = (byte)0x06;
        int tagDataLen = 0;

        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret == -1)
        {
            return SDK_RETURN_CODE.SDK_ERR;
        }
        else
        {
            tempCAPK.RID = subByte(tlvData, ret+3, tlvData[ret+2]);
        }

        TagDouble[0] = (byte) 0x9F;
        TagDouble[1] = (byte) 0x22;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempCAPK.CAPKI = tlvData[ret+2];
        }
        //TacDefault
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x06;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempCAPK.ArithInd = tlvData[ret+2];
        }
        //TacOnline
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x07;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempCAPK.HashInd = tlvData[ret+2];
        }
        //TacDecline
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x02;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempCAPK.Modul = subByte(tlvData, ret+3, tlvData[ret+2]);
        }
        //DEFAULT DDOL
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x04;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempCAPK.ExponentLen = tlvData[ret+2];
            tempCAPK.Exponent = subByte(tlvData, ret+3, tempCAPK.ExponentLen);
        }
        //偏置随机选择的阈值
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x03;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempCAPK.CheckSumLen = tlvData[ret+2];
            tempCAPK.CheckSum = subByte(tlvData, ret+3, tempCAPK.CheckSumLen);
        }
        else
        {
            tempCAPK.CheckSumLen = 0;
        }
        //偏置随机选择的最大目标百分数
        TagDouble[0] = (byte) 0xDF;
        TagDouble[1] = (byte) 0x05;
        ret = strstr(tlvData, TagDouble, 0, dataLen);
        if(ret != -1)
        {
            tempCAPK.ExpireDate = subByte(tlvData, ret+3, tlvData[ret+2]);
        }

        ret = TestLibrary.INSTANCE.sdkEMVBaseAddAnyCAPKList(tempCAPK, 1);
        if(ret == 1)
        {
            StringBuilder sb = new StringBuilder();
            sb.append(String.format("%02X", tempCAPK.CAPKI));
            g_lcd.Lcd_Cls();
            g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "RID:"+tempCAPK.RID.toString(), SDK_DISP.SDK_DISP_CENTER);
            g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE3, 0, "Index:"+sb.toString()+"is OK", SDK_DISP.SDK_DISP_CENTER);
            return SDK_RETURN_CODE.SDK_OK;
        }
        else
        {
            StringBuilder sb = new StringBuilder();
            sb.append(String.format("%02X", tempCAPK.CAPKI));
            g_lcd.Lcd_Cls();
            g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "RID:"+tempCAPK.RID.toString(), SDK_DISP.SDK_DISP_CENTER);
            g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE3, 0, "Index:"+sb.toString()+"is ERR", SDK_DISP.SDK_DISP_CENTER);
            return SDK_RETURN_CODE.SDK_ERR;
        }
    }
}
