package com.example.gtmymusic;

import android.app.IntentService;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import com.squareup.wire.Wire;

import java.io.BufferedInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.ByteBuffer;

public class SocketService extends IntentService {

	protected SocketService(String name) {
		super(name);
		// TODO Auto-generated constructor stub
	}

	public static final String SERVERIP = "10.0.2.2"; //your computer IP address should be written here
    public static final int SERVERPORT = 2001;
    OutputStream out;
    DataOutputStream dataOutputStream;
    PrintWriter output;
    BufferedInputStream input;
    Socket socket;
    InetAddress serverAddress;
    Wire wire;

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

    public void sendPrefixLength(int length) {
        if (out != null) {
            try {
                dataOutputStream.write(length);
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
        if (out != null) {
            Toast.makeText(this,"Sending message of length: " + message.length, Toast.LENGTH_LONG).show();
            try {
                out.write(message);
                out.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public Header receiveHeader(int bytes) {
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
                    out = socket.getOutputStream();
                    output = new PrintWriter(out, true);
                    dataOutputStream = new DataOutputStream(out);
                    input = new BufferedInputStream(socket.getInputStream());
                    wire = new Wire();
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

	@Override
	protected void onHandleIntent(Intent intent) {
		// TODO Auto-generated method stub
		
	}
}
