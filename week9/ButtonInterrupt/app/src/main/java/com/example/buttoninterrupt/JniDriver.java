package com.example.buttoninterrupt;
import android.util.Log;

public class JniDriver implements IJniListener{
    private boolean mConnectFlag;

    public TranseThread mTranseThread;
    private IJniListener mMainActivity;

    static{System.loadLibrary("JniDriver");}
    private native static int openDriver(String path);
    private native static void closeDriver( );
    private native char readDriver( );
    private native int getInterrupt();

    public JniDriver() {
        mConnectFlag=false;
    }

    public String getThreadState(){
        return mTranseThread.getState()+"";
    }
    @Override
    public void onReceive(int val){
        Log.e("test", "4");
        if(mMainActivity !=null){
            mMainActivity.onReceive(val);
            Log.e("test","2");
        }
    }

    public void setListener(IJniListener a){
        mMainActivity =a;
    }

    public int open(String driver){
        if(mConnectFlag) return -1;
        if(openDriver(driver)>0){
            mConnectFlag=true;
            mTranseThread = new TranseThread();
            mTranseThread.start();
            return 1;
        }
        else{
            return -1   ;
        }
    }

    public void close(){
        if(!mConnectFlag) return;
        mConnectFlag=false;
        closeDriver();
    }

    protected void finalize() throws Throwable{
        close();
        super.finalize();
    }

    public char read(){
        return readDriver();
    }

    private class TranseThread extends Thread{
        @Override
        public void run(){
            super.run();
            try{
                while(mConnectFlag){
                    try{
                        onReceive(getInterrupt());
                        Thread.sleep(100);
                    }catch(InterruptedException e){
                        e.printStackTrace();
                    }
                }
            } catch(Exception e){}
        }
    }


}
