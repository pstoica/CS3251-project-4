package com.example.myfirstapp;

import android.app.IntentService;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import java.io.BufferedInputStream;
import java.io.BufferedWriter;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Random;

public class SocketService extends Service {
    public static final String SERVERIP = "10.0.2.2"; //your computer IP address should be written here
    public static final int SERVERPORT = 2001;
    PrintWriter output;
    BufferedInputStream input;
    Socket socket;
    InetAddress serverAddress;

    @Override
    public IBinder onBind(Intent intent) {
        return myBinder;
    }

    private final IBinder myBinder = new LocalBinder();

    public class LocalBinder extends Binder {
        public SocketService getService() {
            return SocketService.this;
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }

    public void sendMessage(String message){
        if (output != null && !output.checkError()) {
            Toast.makeText(this,"Sending message: " + message, Toast.LENGTH_LONG).show();
            output.println(message);
            output.flush();
        }
    }

    @Override
    public int onStartCommand(Intent intent,int flags, int startId) {
        super.onStartCommand(intent, flags, startId);
        Toast.makeText(this,"Service Connected", Toast.LENGTH_LONG).show();
        Runnable connect = new connectSocket();
        new Thread(connect).start();
        return START_STICKY;
    }

    class connectSocket implements Runnable {
        @Override
        public void run() {
            try {
                //here you must put your computer's IP address.
                serverAddress = InetAddress.getByName(SERVERIP);
                Log.d("TCP Client", "Client: Connecting...");

                //create a socket to make the connection with the server
                socket = new Socket(serverAddress, SERVERPORT);

                try {
                    OutputStream out = socket.getOutputStream();
                    output = new PrintWriter(out, true);
                    input = new BufferedInputStream(socket.getInputStream());
                } catch (Exception e) {
                    Log.e("TCP", "Server: Error", e);
                }
            } catch (Exception e) {
                Log.e("TCP", "Client: Error", e);
            }
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        try {
            socket.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        socket = null;
    }
}
