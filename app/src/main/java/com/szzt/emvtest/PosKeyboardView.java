package com.szzt.emvtest;

import android.content.Context;
import android.inputmethodservice.KeyboardView;
import android.inputmethodservice.KeyboardView.OnKeyboardActionListener;
import android.os.Handler;
import android.view.View;

import com.szzt.emvtest.PosLcd;

/**
 * add by zhoulinglong
 */
public class PosKeyboardView extends View{

	public PosKeyboard m_PosKb;
	private KeyboardView m_PosKbView;
	
	public PosKeyboardView(Context context, Handler handler, KeyboardView kbView, int xmlLayoutResId, PosLcd m_lcd) {
		super(context);
		// TODO Auto-generated constructor stub
		m_PosKb = new PosKeyboard(context, handler, xmlLayoutResId, m_lcd);
		m_PosKbView = kbView;
		m_PosKbView.setKeyboard(m_PosKb);
		m_PosKbView.setOnKeyboardActionListener(kbListener);
		m_PosKbView.setVisibility(View.VISIBLE);
	}
	
	private OnKeyboardActionListener kbListener = new OnKeyboardActionListener() {
		
		public void swipeUp() {
			// TODO Auto-generated method stub
			
		}
		
		public void swipeRight() {
			// TODO Auto-generated method stub
			
		}
		
		public void swipeLeft() {
			// TODO Auto-generated method stub
			
		}
		
		public void swipeDown() {
			// TODO Auto-generated method stub
			
		}
		
		public void onText(CharSequence text) {
			// TODO Auto-generated method stub
			
		}
		
		public void onRelease(int primaryCode) {
			// TODO Auto-generated method stub
			
		}
		
		public void onPress(int primaryCode) {
			// TODO Auto-generated method stub
//			Log.i("code", Integer.toString(primaryCode));
			m_PosKb.KbStdin(primaryCode);
		}
		
		public void onKey(int primaryCode, int[] keyCodes) {
			// TODO Auto-generated method stub
			
		}
	};
}
