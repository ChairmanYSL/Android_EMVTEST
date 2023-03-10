package com.szzt.emvtest;

import android.app.Application;

import java.io.UnsupportedEncodingException;

public class EMVApp extends Application
{
    private static EMVApp signleton = new EMVApp();
    private PosLcd g_lcd;
    private PosKeyboard g_keyboard;

    public MainActivity mainActivity;

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

    public void StartTransMenu() throws UnsupportedEncodingException
    {
        g_lcd.Lcd_Cls();
        g_lcd.Lcd_PrintfXY(0, 0, "JCB L2 TEST", 8);
        g_lcd.Lcd_PrintfXY(1, 0, "1.SALE 2.Other Trans", 4);
        g_lcd.Lcd_PrintfXY(2, 0, "3.Jcb Param 4.AutoTest", 4);
        g_lcd.Lcd_PrintfXY(3, 0, "5.BCTC Param", 4);
    }

}
