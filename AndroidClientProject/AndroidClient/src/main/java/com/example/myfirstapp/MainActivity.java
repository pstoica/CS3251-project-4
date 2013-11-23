package com.example.myfirstapp;

import android.app.Activity;
import android.app.ActionBar;
import android.app.Fragment;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.content.Intent;
import android.os.Build;
import android.widget.EditText;

public class MainActivity extends BaseActivity {
    public final static String EXTRA_MESSAGE = "com.example.myfirstapp.MESSAGE";

    public ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceDisconnected(ComponentName name) {
            mBoundService = null;
            mIsBound = false;
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mBoundService = ((SocketService.LocalBinder) service).getService();
            mIsBound = true;
        }
    };

    @Override
    protected void onStart() {
        super.onStart();
        Intent serviceIntent = new Intent(this, SocketService.class);
        startService(serviceIntent);
        doBindService(serviceIntent, mConnection);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        doUnbindService(mConnection);
    }

    /*@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (savedInstanceState == null) {
            getFragmentManager().beginTransaction()
                    .add(R.id.container, new PlaceholderFragment())
                    .commit();
        }
    }

    public void sendMessage(View view) {
        Intent intent = new Intent(this, DisplayMessageActivity.class);
        EditText editText = (EditText) findViewById(R.id.edit_message);
        String message = editText.getText().toString();
        intent.removeExtra(EXTRA_MESSAGE);
        System.out.println("Message: " + message);
        System.out.println("Message Length: " + message.length());
        intent.putExtra(EXTRA_MESSAGE, message);
        startActivity(intent);
    }*/

    public int getLayoutResourceId() {
        return R.layout.activity_main;
    }
}
