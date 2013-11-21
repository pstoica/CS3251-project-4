package com.example.myfirstapp;

import android.app.Activity;
import android.app.ActionBar;
import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.content.Intent;
import android.os.Build;
import android.app.ActionBar;

public abstract class BaseActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(getLayoutResourceId());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        switch (item.getItemId()) {
            case R.id.command_list:
                startList();
                return true;
            case R.id.command_diff:
                return true;
            case R.id.command_pull:
                return true;
            case R.id.command_leave:
                return true;
            case R.id.command_cap:
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    protected abstract int getLayoutResourceId();

    public void startList() {
        startActivity(new Intent (this, MusicListActivity.class));
    }
}
