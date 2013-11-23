package com.example.myfirstapp;

import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import java.io.ByteArrayOutputStream;

public class MusicListActivity extends BaseActivity {
    public ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mBoundService = ((SocketService.LocalBinder) service).getService();
            mIsBound = true;

            Header header = new Header.Builder()
                    .method(Header.MethodType.LIST)
                    .length(0)
                    .indexes(0)
                    .build();

            byte[] data = header.toByteArray();
            int length = data.length;

            mBoundService.sendPrefixLength(length);
            mBoundService.sendMessage(data);
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            mBoundService = null;
            mIsBound = false;
        }
    };

    @Override
    protected void onStart() {
        super.onStart();
        Intent serviceIntent = new Intent(this, SocketService.class);
        doBindService(serviceIntent, mConnection);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        doUnbindService(mConnection);
    }

    public int getLayoutResourceId() {
        return R.layout.activity_music_list;
    }
}
