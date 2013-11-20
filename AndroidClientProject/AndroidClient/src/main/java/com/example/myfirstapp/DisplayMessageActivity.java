package com.example.myfirstapp;

import android.app.Activity;
import android.app.ActionBar;
import android.app.Fragment;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.os.Build;
import android.widget.TextView;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.math.BigInteger;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;

public class DisplayMessageActivity extends Activity {
    private TextView textView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_message);

        // Get the message from the intent
        Intent intent = getIntent();
        String message = intent.getStringExtra(MainActivity.EXTRA_MESSAGE);

        textView = new TextView(this);
        textView.setTextSize(40);

        new NetworkingTask().execute(message);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        switch (item.getItemId()) {
            case R.id.action_settings:
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private class NetworkingTask extends AsyncTask<String, Void, String> {
        protected String doInBackground(String... messages) {
            byte[] receivedMessage = new byte[32];

            // Send to the echo server
            try {
                Socket s = new Socket("10.0.2.2", 12345);

                //outgoing stream redirect to socket
                OutputStream out = s.getOutputStream();

                PrintWriter output = new PrintWriter(out, true);
                output.print(messages[0]);
                output.flush();
                BufferedInputStream input = new BufferedInputStream(s.getInputStream());

                //read line(s)
                input.read(receivedMessage, 0, 32);

                //Close connection
                s.close();
            } catch (UnknownHostException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

            StringBuilder builder = new StringBuilder();
            for (byte b : receivedMessage) {
                builder.append(String.format("%02x", b));
            }

            return builder.toString();
        }

        protected void onPostExecute(String result) {
            // Create the text view
            textView.setText(result);

            // Set the text view as the activity layout
            setContentView(textView);
        }
    }
}