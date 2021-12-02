package com.mp.jaesun_final;

import android.content.Context;
import android.media.AudioManager;
import android.media.SoundPool;

public class SoundManager {
    SoundPool sound;
    AudioManager audiMana;
    static final int GOOD=0, BAD=1, FINISH=2, NEXT=3;
    int[] id = new int[5];
    int[] streamId = new int[5];

    public SoundManager(Context context){
        sound = new SoundPool(1, AudioManager.STREAM_MUSIC,0);
        audiMana = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
        id[GOOD] = sound.load(context,R.raw.good,1);
        id[BAD] = sound.load(context,R.raw.bad,1);
        id[FINISH] = sound.load(context,R.raw.tadah,1);
        id[NEXT] = sound.load(context,R.raw.next,1);
    }

    public void play(int code){
        float volume = audiMana.getStreamVolume(AudioManager.STREAM_MUSIC);
        streamId[code] = sound.play(id[code],volume,volume,1,0,1.0f);
        try{
            while(streamId[code]==0){
                Thread.sleep(200);
                streamId[code] = sound.play(id[code],volume,volume,1,0,1.0f);
            }
        }catch (InterruptedException e){}
    }
    public void stop(int code){
        sound.stop(streamId[code]);
    }
}
