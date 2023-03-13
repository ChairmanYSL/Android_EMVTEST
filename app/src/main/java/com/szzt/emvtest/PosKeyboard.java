package com.szzt.emvtest;

import android.content.Context;
import android.graphics.Paint;
import android.inputmethodservice.Keyboard;
import android.os.Handler;
import android.os.Message;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import android.util.Log;

/**
 * add by zhoulingling
 */
public class PosKeyboard extends Keyboard {

	static public final char[][] keyCharTable = { { '1', 'q', 'z', '.', 'Q', 'Z' }, { '2', 'a', 'b', 'c', 'A', 'B', 'C' },
			{ '3', 'd', 'e', 'f', 'D', 'E', 'F' }, { '4', 'g', 'h', 'i', 'G', 'H', 'I' }, { '5', 'j', 'k', 'l', 'J', 'K', 'L' },
			{ '6', 'm', 'n', 'o', 'M', 'N', 'O' }, { '7', 'p', 'r', 's', 'P', 'R', 'S' }, { '8', 't', 'u', 'v', 'T', 'U', 'V' },
			{ '9', 'w', 'x', 'y', 'W', 'X', 'Y' }, { '0', ',', '*', '#', '@', '%', '$' }, };

	static public final char NOKEY = 0XFF;

	static public final char KEY1 = '1';
	static public final char KEY2 = '2';
	static public final char KEY3 = '3';
	static public final char KEY4 = '4';
	static public final char KEY5 = '5';
	static public final char KEY6 = '6';
	static public final char KEY7 = '7';
	static public final char KEY8 = '8';
	static public final char KEY9 = '9';
	static public final char KEY0 = '0';

	static public final char KEYENTER = 0x01;
	static public final char KEYCANCEL = 0x02;
	static public final char KEYCLEAR = 0x0A;
	static public final char KEYBCKSPACE = 0x11;
	static public final char KEYUP = 0x04;
	static public final char KEYDOWN = 0x05;
	static public final char KEYMENU = 0x22;
	static public final char KEYEXIT = 0x55;
	static public final char KEYALPHA = 0x0C;
	static public final char KEYFN = 0x0F;

	static public final char MAXKEYBUFF = 16;

	static public final char INPUTMODE_NUMERIC = 0x00;
	static public final char INPUTMODE_STRING = 0x01;
	static public final char INPUTMODE_PWD = 0x02;
	static public final char INPUTMODE_AMT = 0x03;

	private List<Integer> keyList;
	private Integer m_keyCode;
	private PosLcd lcd;
	private Context context;
	public int keyValue = 0;
	Handler handler;

	public PosKeyboard(Context context, Handler handler, int xmlLayoutResId, PosLcd m_lcd) {
		super(context, xmlLayoutResId);
		// TODO Auto-generated constructor stub
		m_keyCode = new Integer(0);
		keyList = new ArrayList<Integer>();
		lcd = m_lcd;
		this.context = context;
		this.handler = handler;
	}

	public void Kb_Flush() {
		keyList.clear();
	}

	public void KbStdin(int keyCode) {
		if (keyList.size() >= MAXKEYBUFF) {
			return;
		}
		m_keyCode = keyCode;
		keyList.add(m_keyCode);
	}

	public int Kb_Hit() {
//		Log.d("lishiyao", "in Kb_Hit KeyList size: "+keyList.toString());
		if (keyList.size() > 0) {
			return 1;
		} else {
			return 0;
		}
	}

	public int Kb_GetKey() {
		// Integer keyValue;
		int value;
		while (true) {
			if (Kb_Hit() == 1) {
				break;
			}

			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		m_keyCode = keyList.get(0);
		keyList.remove(0);
		value = m_keyCode.intValue();
		Log.d("lishiyao", "Kb_GetKey value = " + value);

		if(value == 0x05){
			Message msg = new Message();
			msg.what = 2;
			handler.sendMessage(msg);
		}

		switch (value){
			case 0x01:
				value = KEYUP;
				break;
			case 0x02:
				value = KEYDOWN;
				break;
			case 0x03:
				value = KEYMENU;
				break;
			case 0x43:
				value = KEYCLEAR;
				break;
			case 0x3E:
				value = KEYENTER;
				break;
			case 0x3F:
				value = KEYCANCEL;
				break;
			case 0x40:
				value = KEYBCKSPACE;
				break;
			case 0x15:
				value = KEYFN;
				break;
			case 0x05:
				value = KEYEXIT;
		}

		return value;
	}
}
