package android_serialport_api;

import android.util.Log;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class SerialPort {
	private static final String TAG = SerialPort.class.getSimpleName() ;
	private ExecutorService executor = Executors.newSingleThreadExecutor();
	private FileDescriptor mFd;
	private FileInputStream mFileInputStream;
	private FileOutputStream mFileOutputStream;
	private File device;
	private int baudrate;
	private int inx;
	private byte[] scanBuffer = new byte[2048];
	public SerialPort(File device, int baudrate) throws SecurityException, IOException {
		this.device = device;
		this.baudrate = baudrate;
		mFd = open(device.getAbsolutePath(), baudrate, 2);
		if (mFd == null) {
			Log.e(TAG, "native open returns null");
			throw new IOException();
		}
		mFileInputStream = new FileInputStream(mFd);
		mFileOutputStream = new FileOutputStream(mFd);
	}


	public InputStream getInputStream() {
		return mFileInputStream;
	}

	public OutputStream getOutputStream() {
		return mFileOutputStream;
	}

	private native static FileDescriptor open(String path, int baudrate, int port);

	public native void close();

	static {
		System.loadLibrary("jni_serial_port");
		//System.load("/sdcard/Download/libserial_port.so");
	}

	public int setOptions(int inx, File device, int tBoud) {
		this.inx = inx;
		this.device = device;
		this.baudrate = tBoud;
		mFd = open(device.getAbsolutePath(), baudrate, inx);
		if (mFd != null) {
			mFileInputStream = new FileInputStream(mFd);
			mFileOutputStream = new FileOutputStream(mFd);
			return 0;
		}
		return -1;
	}

	public byte[] readData(int timeout) {
		try {
			if(mFileInputStream!=null)
			{
				byte[] data=new byte[2048];
				long start=System.currentTimeMillis();
				while (true){
					int len=mFileInputStream.read(data);
					if(len>0){
						byte[] ret=new byte[len];
						System.arraycopy(data,0,ret,0,len);
						return ret;
					}
					if(System.currentTimeMillis()-start>timeout){
						return null;
					}
				}
			}
		}catch (Exception e){}

		return null;
	}

	public void send(byte[] data) {
		try {
			if(mFileOutputStream!=null)
			{
				mFileOutputStream.write(data);
				mFileOutputStream.flush();
			}
		}catch (Exception e){}
	}
}
