package com.szzt.emvtest;

import android.app.Application;
import android.content.Context;
import android.hardware.usb.UsbManager;
import android.util.Log;

import java.io.UnsupportedEncodingException;
import java.security.Key;
import java.util.List;

import com.szzt.emvtest.EMVSDK.SDK_RETURN_CODE;
import com.szzt.emvtest.EMVSDK.SDK_DISP;
import com.szzt.sdk.device.port.SerialPort;
import com.szzt.emvtest.PosTimer;

public class EMVApp extends Application
{
    private static EMVApp signleton = new EMVApp();
    private PosLcd g_lcd;
    private PosKeyboard g_keyboard;
    private MainApplication g_Application;
    public MainActivity mainActivity;
    //trans var
    private boolean AutoTestFlag = true;
    private int gAutoTestTimeout = 5;

    public byte [] gstAmount;
    public byte [] gstAmountOther;
    public byte [] gstTransType;
    public boolean receiveSerialFlag = false;
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

    private void InitData()
    {
        gstAmount = new byte[12];
        gstAmountOther = new byte[12];
        gstTransType = new byte[1];
    }

    public void StartTransMenu(KeyCodeListener keylistener, SerialDataListener commlistener) throws UnsupportedEncodingException, InterruptedException {
        int key,ret;

        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "JCB L2 TEST", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "1.SALE 2.Other Trans", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE3, 0, "3.Jcb Param 4.AutoTest", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE4, 0, "5.BCTC Param", SDK_DISP.SDK_DISP_LEFT);

        DeviceOpera deviceOpera = new DeviceOpera();
        deviceOpera.Kb_Flush();

        InitData();
        Log.d("lishiyao", "StartTransMenu: InitData");

        Thread ListenKeyboardThread = new Thread(new Runnable() {
            @Override
            public void run() {
                int key;
                while (true)
                {
                    key = deviceOpera.Kb_GetKey();
                    try {
                        Thread.sleep(10);
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                    if(key > 0)
                    {
                        Log.d("lishiyao", "in start trans menu key: " + key);
                        if(keylistener!=null){
                            keylistener.sendKeyCode(key);
                            Log.d("lishiyao", "after call back key: "+g_keyboard.keyValue);
                        }
                        else{
                            Log.e("lishiyao", "listener is null" );
                        }
                    }
                }
            }
        });
        ListenKeyboardThread.setPriority(10);
        ListenKeyboardThread.start();

        Thread LisenSerialThread = new Thread(new Runnable() {
            @Override
            public void run() {
                PosTimer timer = new PosTimer(1000*10);
                timer.timerStart();
                while (true)
                {
                    if(AutoTestFlag)
                    {
                        BCTCret =  BCTCStartTrade();
                        if(BCTCret == SDK_RETURN_CODE.SDK_OK)
                        {
                            if(commlistener != null)
                            {
                                commlistener.SendSerialFlag(true);
                            }
                            else
                            {
                                Log.e("lishiyao", "commlistener is null! ");
                            }
                            break;
                        }

                    }
                    else
                    {
                        BCTCret =
                    }
                }

            }
        });

        while (true)
        {
            if(AutoTestFlag)
            {
                ret = BCTCStartTrade();
                if(ret != SDK_RETURN_CODE.SDK_OK)
                {
                    key = g_keyboard.KEYCANCEL;
                }
                else
                {

                }
                if(g_keyboard.keyValue == g_keyboard.KEY1)
                {
                    ListenKeyboardThread.stop();
                    InputTradeAmount(gstAmount);
                }
            }
        }

        return ;
    }

    public int InputTradeAmount(byte [] Amount) throws UnsupportedEncodingException {
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
    public int DealTrade(){

        return SDK_RETURN_CODE.SDK_OK;
    }

    private int BCTCSignleTrade()
    {
        SerialPort port;
        byte recvData [] = new byte[128];
        int ret;
        PosTimer timer = new PosTimer(1000*gAutoTestTimeout);
        int tlvDataLen;

        port = g_Application.getSerialPortWrapperImpl();
        int portId=port.open("/dev/ttyUSB0",115200);
        if(portId>0)
        {
            timer.timerStart();
            while (true)
            {
                ret = port.receive(portId, recvData, 128, 1);
                if(ret >= 8)
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
            if( ret >= 8)
            {
                Log.d("lishiyao", "in BCTCStartTrade recvData:" + recvData.toString());
                tlvDataLen = recvData[2] * 256 + recvData[3];
                if (tlvDataLen >= 3) {

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

    private int BCTCStartTrade() {
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
                if (tlvDataLen >= 3) {

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
}
