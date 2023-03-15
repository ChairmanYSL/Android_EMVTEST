package com.szzt.emvtest;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.inputmethodservice.Keyboard;
import android.inputmethodservice.KeyboardView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.LinearLayout;
import android.widget.Toast;
import android.util.Log;

import androidx.constraintlayout.solver.PriorityGoalRow;

import com.szzt.emvtest.EMVSDK.SDK_RETURN_CODE;
import com.szzt.emvtest.PosKeyboardView;
import com.szzt.emvtest.EMVApp;
import com.szzt.emvtest.PosLcd;
import com.szzt.sdk.device.port.SerialPort;
import com.szzt.emvtest.PosKeyboard;

import java.io.UnsupportedEncodingException;
import com.szzt.emvtest.TestLibrary;

public class MainActivity extends Activity
{
    private  PosKeyboardView kb_view;
    private PosLcd p_lcd;
    private AlertDialog mAlertDialog;
    private MainApplication mApplicattion;
//    private EMVApp p_app;
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.d("lishiyao", "The onCreate() event");
        mApplicattion=(MainApplication) getApplication();

        //获取屏幕显示信息
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);

        p_lcd = new PosLcd(this, dm.widthPixels, (int)(dm.heightPixels * 0.4), m_handler);

        LinearLayout p_llayout = (LinearLayout) findViewById(R.id.lcdView);
        p_llayout.addView(p_lcd);

        kb_view = new PosKeyboardView(this.getBaseContext(), m_handler, (KeyboardView) findViewById(R.id.keyboard_view), R.layout.poskeyboard, p_lcd);
        EMVApp.getInstance().mainActivity = this;
        EMVApp.getInstance().setPosLcd(p_lcd);
        Log.d("lishiyao", "onCreate setPosKeyboard: ");
        EMVApp.getInstance().setPosKeyboard(kb_view.m_PosKb);
        EMVApp.getInstance().setMainApplication(mApplicattion);
        EMVApp.getInstance().InitData();
    }

    @Override
    protected void onStart() {
        Log.d("lishiyao", "onStart: ");
        super.onStart();
        AppSysmain.start();
        AppSysmain.setPriority(10);
    }

    @Override
    protected void onStop() {
        SerialPort port = mApplicattion.getSerialPortWrapperImpl();
        port.close();
        Log.d("lishiyao", "onStop: close Serial");
        super.onStop();
    }

    Thread AppSysmain = new Thread(new Runnable() {
        @Override
        public void run() {
            int rslt;
            int key, page = 0;
            boolean bisBrushScrean = true;
            Log.d("lishiyao", "run: before TestLibrary.INSTANCE.sdkemvbaseInitDDI");
            TestLibrary.INSTANCE.sdkemvbaseInitDDI();
            Log.d("lishiyao", "run: after TestLibrary.INSTANCE.sdkemvbaseInitDDI");
            while(true)
            {
                switch(page)
                {
                    case 1:
                            key = EMVApp.getInstance().Menu1();
                        break;

                    default:
                            key = EMVApp.getInstance().Menu0();
                        bisBrushScrean = false;
                        break;
                }

                switch ( key )
                {
                    case PosKeyboard.KEYENTER:
                    case PosKeyboard.KEYDOWN:
                        bisBrushScrean = true;
                        page++;

                        if(page > 2)
                        {
                            page = 0;
                        }
                        break;

                    case PosKeyboard.KEYUP:
                        bisBrushScrean = true;
                        page--;

                        if(page < 0)
                        {
                            page = 2;
                        }
                        break;

                    case PosKeyboard.KEYCANCEL:

                        if(page != 0)
                        {
                            bisBrushScrean = true;
                        }
                        page = 0;
                        break;

                    default:
                        break;
                }
            }

        }

    });

    private  Handler m_handler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case 1:
                    p_lcd.invalidate();
                    break;
                case 2:
                    Log.d("lishiyao", "dialog");
                    if (mAlertDialog != null) {
                        mAlertDialog.show();
                    }
                    break;
            }
            super.handleMessage(msg);
        }
    };

    private void dialogInit()
    {
        Log.d("lishiyao", "dialogInit");
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setTitle("Warning");
        builder.setMessage("Exit EmvL2 test?");

        builder.setNegativeButton("Canel", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
//                JniNative.appoutprocess();
//                System.exit(0);
            }
        });

        mAlertDialog = builder.create();
    }

    protected void dialog()
    {
        m_handler.sendEmptyMessage(2);
    }

}
