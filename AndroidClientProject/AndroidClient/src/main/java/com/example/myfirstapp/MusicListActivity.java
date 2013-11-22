package com.example.myfirstapp;

import android.content.Intent;
import android.os.Bundle;

public class MusicListActivity extends BaseActivity {
    @Override
    protected void onStart() {
        super.onStart();
        Intent serviceIntent = new Intent(this, SocketService.class);
        doBindService(serviceIntent);
    }

    public int getLayoutResourceId() {
        return R.layout.activity_music_list;
    }
}
