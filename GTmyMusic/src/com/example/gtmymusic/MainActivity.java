package com.example.gtmymusic;

import java.io.BufferedInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import com.example.gtmymusic.SocketService.connectSocket;
import com.squareup.wire.Wire;

import android.os.AsyncTask;
import android.os.Bundle;
import android.os.IBinder;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
	
	public static final String SERVERIP = "130.207.114.21";
    public static final int SERVERPORT = 8254;
    OutputStream out;
    DataOutputStream dataOutputStream;
    PrintWriter output;
    BufferedInputStream input;
    Socket socket;
    InetAddress serverAddress;
    Spinner spinner;
    TextView app_out;
    Wire wire;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		app_out = (TextView) findViewById(R.id.outputText);
		
		spinner = (Spinner) findViewById(R.id.command_list);
		ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this, R.array.commands_array, android.R.layout.simple_spinner_dropdown_item);
		adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		spinner.setAdapter(adapter);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	@Override
    protected void onStart() {
        super.onStart();
	}
	 
	public void sendCommand(View view){
		// Send selected command to server
		String cmd = spinner.getSelectedItem().toString();
		if(cmd.equals("LIST")){
			// do list logic
		} else if(cmd.equals("DIFF")){
			// do diff logic
		} else if(cmd.equals("PULL")){
			// do pull logic
		} else if(cmd.equals("CAP")){
			// do cap logic
		} else if(cmd.equals("LEAVE")){
			// do list logic
		}
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
		
		try {
            socket.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        socket = null;
	}
	
	 private class LISTTask extends AsyncTask<Void, Void, String> {
		 
		 String ret;
		 
		 @Override
		 protected String doInBackground(Void...params){
			 try {
				 serverAddress = InetAddress.getByName(SERVERIP);
				 socket = new Socket(serverAddress, SERVERPORT);
				 ret = "Connected: "+ socket.toString();
				 out = socket.getOutputStream();
                 output = new PrintWriter(out, true);
                 dataOutputStream = new DataOutputStream(out);
                 input = new BufferedInputStream(socket.getInputStream());
                 wire = new Wire();
                 
                 // do list stuff
                 
                 out.close();
         		 output.close();
         		 dataOutputStream.close();
         		 input.close();
         		 socket.close();
			} catch (UnknownHostException e) {
				 // TODO Auto-generated catch block
				 e.printStackTrace();
			} catch (IOException e) {
				 // TODO Auto-generated catch block
				 e.printStackTrace();
			} 
			 
		 	 ret += System.getProperty("line.separator");
		 	 return ret;
		 }
		 
		 protected void onPostExecute(String msg) {
    		app_out.setText(msg);
    	}
	 }
	
	/*class connectSocket implements Runnable {
        @Override
        public void run() {
            try {
                //here you must put your computer's IP address.
                serverAddress = InetAddress.getByName(SERVERIP);
                Log.d("TCP Client", "Client: Connecting...");

                //create a socket to make the connection with the server
                socket = new Socket(serverAddress, SERVERPORT);
                Log.d("TCP Client", "Socket connected: " + socket.isConnected());
                
                try {
                    out = socket.getOutputStream();
                    output = new PrintWriter(out, true);
                    dataOutputStream = new DataOutputStream(out);
                    input = new BufferedInputStream(socket.getInputStream());
                } catch (Exception e) {
                    Log.e("TCP", "Server: Error", e);
                }
            } catch (Exception e) {
                Log.e("TCP", "Client: Error", e);
            }
        }
    }*/
	
	public int getLayoutResourceId() {
        return R.layout.activity_main;
    }

}
