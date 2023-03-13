package com.szzt.emvtest;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.os.Environment;
import android.util.Log;


import java.io.File;
import java.nio.ByteBuffer;


/**
 * Created by 28070 on 2016/5/19.
 */
public class DeviceOpera {


    public int testCallJava(int i) {

        Log.e("CallJava", "" + i);
        return i;
    }


    public void Lcd_Cls() {
        EMVApp.getInstance().getPosLcd().Lcd_Cls();
    }

    public void Lcd_Printf(String dispText) {

        //Log.e("CallJava", dispText);

        try {
            EMVApp.getInstance().getPosLcd().Lcd_Printf(dispText);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void Lcd_ClearLine(int startLine, int endLine) {
        EMVApp.getInstance().getPosLcd().Lcd_ClearLine(startLine, endLine);
    }

    public void Lcd_GotoXY(int line, int col) {
        EMVApp.getInstance().getPosLcd().Lcd_GotoXY(line, col);
    }

    public void Lcd_PrintfXY(int line, int col, String dispText, int attr) {
        try {
            EMVApp.getInstance().getPosLcd().Lcd_PrintfXY(line, col, dispText, attr);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void Lcd_SetFont(int FontType) {
        EMVApp.getInstance().getPosLcd().Lcd_SetFont(FontType);
    }

    //按键处理
    public void Kb_Flush() {
        EMVApp.getInstance().getPosKeyboard().Kb_Flush();
    }

    public void KbStdin(int keyCode) {
        EMVApp.getInstance().getPosKeyboard().KbStdin(keyCode);
    }

    public int Kb_Hit() {
        return EMVApp.getInstance().getPosKeyboard().Kb_Hit();
    }

    public int Kb_GetKey() {
        if (EMVApp.getInstance().getPosKeyboard().Kb_Hit() == 0)
            return 0;
        return EMVApp.getInstance().getPosKeyboard().Kb_GetKey();
    }

}
