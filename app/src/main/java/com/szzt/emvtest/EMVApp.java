package com.szzt.emvtest;

import android.app.Application;
import android.util.Log;

import java.io.UnsupportedEncodingException;
import java.security.Key;
import java.util.List;

import com.szzt.emvtest.EMVSDK.SDK_RETURN_CODE;
import com.szzt.emvtest.TestLibrary;
import com.szzt.emvtest.EMVSDK.SDK_DISP;

public class EMVApp extends Application
{
    private static EMVApp signleton = new EMVApp();
    private PosLcd g_lcd;
    private PosKeyboard g_keyboard;

    public MainActivity mainActivity;
    //trans var
    private boolean AutoTestFlag;
    public byte [] gstAmount;
    public byte [] gstAmountOther;
    public byte [] gstTransType;

    public static EMVApp getInstance() {return signleton;}

    public PosLcd getPosLcd() {return g_lcd;}
    public void setPosLcd(PosLcd posLcd) {this.g_lcd = posLcd;}
    public PosKeyboard getPosKeyboard() {return g_keyboard;}
    public void setPosKeyboard(PosKeyboard posKeyboard) {this.g_keyboard = posKeyboard;}
    public void show_rf_logo() {EMVApp.getInstance().mainActivity.dialog();}

    public void InitData()
    {
        gstAmount = new byte[12];
        gstAmountOther = new byte[12];
        gstTransType = new byte[1];
    }

    public void StartTransMenu() throws UnsupportedEncodingException {
        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "JCB L2 TEST", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "1.SALE 2.Other Trans", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE3, 0, "3.Jcb Param 4.AutoTest", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE4, 0, "5.BCTC Param", SDK_DISP.SDK_DISP_LEFT);
    }

    public int InputTradeAmount(byte [] Amount) throws UnsupportedEncodingException {
        int p_key = 0;
        int i = 0;
        List< Key > p_keyList;

        if(Amount == null)
        {
            return SDK_RETURN_CODE.SDK_PARAM_ERR;
        }
        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE1, 0, "Purchase", SDK_DISP.SDK_DISP_CENTER);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE2, 0, "Please Input Amount:", SDK_DISP.SDK_DISP_LEFT);
        g_lcd.Lcd_PrintfXY(SDK_DISP.SDK_DISP_LINE5, 0,"press [CLEAR] if err", SDK_DISP.SDK_DISP_RIGHT);

        g_keyboard.Kb_Flush();
//        while (true)
//        {
//            p_key = g_keyboard.Kb_GetKey();
//            if(p_key == )
//        }



        if(p_key == g_keyboard.KEYENTER)
        {
            Log.d("lishiyao", "get key code: "+p_key);

        }



        return SDK_RETURN_CODE.SDK_OK;
    }
    public int DealTrade(){
        return SDK_RETURN_CODE.SDK_OK;
    }


}
