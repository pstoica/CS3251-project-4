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
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.security.DigestInputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import com.squareup.wire.ByteString;
import com.squareup.wire.Wire;

public class MainActivity extends Activity {
    //public static final String SERVERIP = "192.168.56.101"; //your computer IP address should be written here
    public static final String SERVERIP = "10.0.2.2";
    public static final int SERVERPORT = 2001;
    public static final String path = Environment.getExternalStorageDirectory().toString() + "/Download/";
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

    protected ByteString getChecksum(File f) {
        MessageDigest md = null;
        DigestInputStream dis;
        byte[] buffer = new byte[2048];

        try {
            md = MessageDigest.getInstance("SHA-256");
            dis = new DigestInputStream(new FileInputStream(f), md);
            while(dis.read(buffer) != -1);
            dis.close();
            return ByteString.of(md.digest());
        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }

    protected String byteArrayToHex(byte[] a) {
        StringBuilder sb = new StringBuilder();
        for(byte b: a)
            sb.append(String.format("%02x", b&0xff));
        return sb.toString();
    }

    protected List<Song> getSongs() {
        String path = Environment.getExternalStorageDirectory().toString() + "/Download/";
        File dir = new File(path);
        String file[] = dir.list();

        if (file != null) {
            List<Song> songs = new ArrayList<Song>(file.length);
            if(file != null){
                for(int i = 0; i < file.length; i++){
                    if(file[i].endsWith(".mp3")){
                        File f = new File(path + file[i]);

                        Song song = new Song.Builder()
                            .title(file[i])
                            .checksum(getChecksum(f))
                            .lenofsong((int) f.length())
                            .build();

                        songs.add(song);

                        Log.d("DIFF (local)", file[i]);
                    }
                }
            }

            return songs;
        }

        return null;
    }

    public List<Song> compareSongs(List<Song> serverSongs) {
        String result = "";
        List<Song> clientSongs = getSongs();
        List<Song> diffSongs = new ArrayList<Song>();

        if (serverSongs != null) {
            int numMatched = 0;
            for (Song serverSong: serverSongs) {
                boolean found = false;

                for (Song clientSong: clientSongs) {
                    if (clientSong.checksum.equals(serverSong.checksum)) {
                        found = true;
                    }
                }

                if (!found) {
                    diffSongs.add(serverSong);
                }
            }
        }

        return diffSongs;
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
        Header header = new Header.Builder()
                .method(Header.MethodType.PULL)
                .songs(getSongs())
                .build();

        System.out.println(header.songs);

        new NetworkingTask().execute(header);
    }

    public void doLeave(View view) {
        Header header = new Header.Builder()
                .method(Header.MethodType.LEAVE)
                .build();

        new NetworkingTask().execute(header);
    }

    public void doCap(View view) {
        EditText box = (EditText) findViewById(R.id.cap_value);
        String cap_str = box.getText().toString();
        int cap_val;

        try {
            cap_val = Integer.parseInt(cap_str);
        } catch (NumberFormatException e){
            e.printStackTrace();
            Toast.makeText(getApplicationContext(), "Enter numbers only!", Toast.LENGTH_LONG);
            box.setText("");
            return;
        }

        // use .limit to add the cap limit to the header
        Header header = new Header.Builder()
                .method(Header.MethodType.CAP)
                .limit(cap_val)
                .build();

        new NetworkingTask().execute(header);

        box.setText("");
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
                try {
                    out.write(message);
                    out.flush();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        public void receiveFile(Song song){
            Log.d("RECV", "Starting receiveFile()\n");
            int bufferLength = 512;
            int numBytesToRecv = song.lenofsong;
            byte[] buffer = new byte[bufferLength];
            Log.d("RECV", "Bytes to receive: "+numBytesToRecv+"\n");

            try {
                File file = new File(path + song.title);
                file.setWritable(true);
                FileOutputStream stream = new FileOutputStream(path + song.title);

                int totalRead = 0;
                int bytesRead = 0;
                int bytesToRead = 0;

                while (totalRead < numBytesToRecv) {
                    bytesToRead = Math.min(bufferLength, numBytesToRecv - bufferLength);
                    bytesRead = input.read(buffer, 0, bytesToRead);
                    stream.write(buffer, 0, bytesRead);
                    totalRead += bytesRead;
                }

                stream.close();
            } catch (Exception e) {
                e.printStackTrace();
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
                    result += song.title + "\n";
                }
            } else {
                result += "No songs found.\n";
            }

            return result;
        }

        protected String doDiff(Header header) {
            sendHeader(header);

            Header response = receiveHeader();

            String result = "DIFF result:\n";

            List<Song> diffSongs = compareSongs(response.songs);

            for (Song song: diffSongs) {
                result += song.title + "\n";
            }

            return result;
        }

        protected String doPull(Header header) {
            sendHeader(header);

            Header response = receiveHeader();

            String result = "PULL result:\n";

            for (Song song : response.songs) {
                receiveFile(song);
                System.out.println("finished download: " + song.title);
                result += "Received: " + song.title + "\n";
            }

            return result;
        }

        protected String doLeave(Header header) {
            sendHeader(header);

            MainActivity.this.finish();
            System.exit(0);
            return "LEAVE\n";
        }

        protected String doCap(Header header) {
            sendHeader(header);
            Header response = receiveHeader();
            String result = "CAP " + header.limit + " MB\n";
            return result;
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
