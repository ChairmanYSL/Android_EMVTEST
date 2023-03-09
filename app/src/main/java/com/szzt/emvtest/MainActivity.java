package com.szzt.emvtest;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.inputmethodservice.KeyboardView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.DisplayMetrics;
import android.widget.LinearLayout;
import android.widget.Toast;
import android.util.Log;


public class MainActivity extends Activity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.d("lishiyao", "The onCreate() event");

    }

}
