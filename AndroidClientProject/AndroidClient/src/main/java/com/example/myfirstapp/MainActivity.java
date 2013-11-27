package com.example.myfirstapp;

import android.app.Activity;
import android.app.ActionBar;
import android.app.Fragment;
import android.content.ComponentName;
import android.content.Context;
import android.content.ServiceConnection;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.IBinder;
import android.os.Message;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.content.Intent;
import android.os.Build;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import java.io.BufferedInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.ByteBuffer;

import com.squareup.wire.Wire;

public class MainActivity extends Activity {
    public static final String SERVERIP = "192.168.56.101"; //your computer IP address should be written here
    public static final int SERVERPORT = 2001;
    private TextView textView;
    private boolean isConnected = false;

    private OutputStream out;
    private DataOutputStream dataOutputStream;
    private PrintWriter output;
    private BufferedInputStream input;
    private Socket socket;
    private InetAddress serverAddress;
    private Wire wire;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(getLayoutResourceId());
        textView = (TextView) findViewById(R.id.textView);
    }

    public void doList(View view) {
        Header header = new Header.Builder()
                .method(Header.MethodType.LIST)
                .build();

        new NetworkingTask().execute(header);
    }

    public void doDiff(View view) {
        Header header = new Header.Builder()
                .method(Header.MethodType.DIFF)
                .build();

        new NetworkingTask().execute(header);
    }

    public void doPull(View view) {

    }

    public void doLeave(View view) {
        Header header = new Header.Builder()
                .method(Header.MethodType.LEAVE)
                .build();

        new NetworkingTask().execute(header);

        MainActivity.this.finish();
    }

    public void doCap(View view) {
        // use .limit to add the cap limit to the header
        Header header = new Header.Builder()
                .method(Header.MethodType.CAP)
                .limit(5)
                .build();

        new NetworkingTask().execute(header);
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    public int getLayoutResourceId() {
        return R.layout.activity_main;
    }

    private class NetworkingTask extends AsyncTask<Header, Void, String> {
        public void connectToServer() {
            if (!isConnected) {
                try {
                    Log.d("TCP", "Client: Connecting...");
                    serverAddress = InetAddress.getByName(SERVERIP);

                    //create a socket to make the connection with the server
                    socket = new Socket(serverAddress, SERVERPORT);
                    out = socket.getOutputStream();
                    output = new PrintWriter(out, true);
                    dataOutputStream = new DataOutputStream(out);
                    input = new BufferedInputStream(socket.getInputStream());
                    wire = new Wire();

                    isConnected = true;
                } catch (Exception e) {
                    Log.e("TCP", "Server: Error", e);
                }
            }
        }

        public void sendPrefixLength(int length) {
            Log.d("TCP", "Prefix: " + length);
            if (out != null) {
                try {
                    dataOutputStream.writeInt(length);
                    dataOutputStream.flush();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        public int receivePrefixLength() {
            int bytes = 4;
            byte[] receivedMessage = new byte[bytes];

            try {
                input.read(receivedMessage, 0, bytes);
            } catch (IOException e) {
                e.printStackTrace();
            }

            return ByteBuffer.wrap(receivedMessage).getInt();
        }

        public void sendMessage(byte[] message) {
            Log.d("TCP", "Sending message...");
            if (out != null) {
                // Toast.makeText(this,"Sending message of length: " + message.length, Toast.LENGTH_LONG).show();
                try {
                    out.write(message);
                    out.flush();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        public void sendHeader(Header header) {
            Log.d("TCP", "Sending header...");
            byte[] data = header.toByteArray();
            int length = data.length;
            sendPrefixLength(length);
            sendMessage(data);
        }

        public Header receiveHeader() {
            int bytes = receivePrefixLength();
            Log.d("TCP", "Byte length: " + bytes);
            byte[] receivedMessage = new byte[bytes];
            Header header = null;

            try {
                input.read(receivedMessage, 0, bytes);
                header = wire.parseFrom(receivedMessage, Header.class);
            } catch (IOException e) {
                e.printStackTrace();
            }

            return header;
        }

        protected String doList(Header header) {
            sendHeader(header);

            Header response = receiveHeader();

            String result = "LIST result:\n";

            System.out.println(response.songs);

            if (response.songs != null) {
                for (Song song: response.songs) {
                    result += song.title + " -- " + song.checksum + " -- " + song.lenofsong + "\n";
                }
            } else {
                result += "No songs found.\n";
            }

            return result;
        }

        protected String doDiff(Header header) {
            //return "diff\n";
            sendHeader(header);

            Header response = receiveHeader();

            String path = Environment.getExternalStorageDirectory().toString() + "/Download/";
            File dir = new File(path);
            String file[] = dir.list();
            String songs[] = new String[file.length];
            int song_cnt = 0;
            if(file != null){
                for(int i = 0; i < file.length; i++){
                    if(file[i].endsWith(".mp3")){
                        songs[song_cnt] = file[i];
                        Log.d("DIFF (local)", file[i]);
                        song_cnt++;
                    }
                }
            }

            String result = "DIFF result (no checksum check):\n";
            System.out.println(response.songs);
            Log.d("DIFF (remote)", response.songs + "");

            if (response.songs != null) {
                int num_matched = 0;
                for (Song song: response.songs) {
                    boolean found = false;
                    for (int i = 0; i < song_cnt && !found; i++){
                        if(song.title.equals(songs[i])){
                            found = true;
                        }
                    }
                    if(!found){
                        result += song.title + "\n";
                    }
                }
                if(num_matched == 0){
                    result += "No songs found.\n";
                }
            } else {
                result += "No songs found.\n";
            }

            return result;
        }

        protected String doPull(Header header) {
            return "pull\n";
        }

        protected String doLeave(Header header) {
            sendHeader(header);
            return "LEAVE\n";
        }

        protected String doCap(Header header) {
            return "cap\n";
        }

        protected String doInBackground(Header... headers) {
            connectToServer();

            Header header = headers[0];
            switch(header.method) {
                case LIST:
                    return doList(header);
                case DIFF:
                    return doDiff(header);
                case PULL:
                    return doPull(header);
                case LEAVE:
                    return doLeave(header);
                case CAP:
                    return doCap(header);
            }

            return "unrecognized command\n";
        }

        protected void onPostExecute(String result) {
            // Add to the textView
            textView.append(result);
        }
    }
}
