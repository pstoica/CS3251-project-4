Project 2 for CS 3251 by Marissa Wall and Audrey Meinders

Here are the steps to compile and run our code:

Place server code on shuttle2:
Our client code will be sending its data to the IP address of shuttle2; therefore, the server code must be placed on shuttle2 and run from there. The client code can be run from any of the shuttles.

Store mp3s with the code:
Client folder: Contains the code to be run on the client side. All the mp3s for the client need to be either placed in this folder, or all the client code needs to be moved to the location where the client mp3s are stored.
Server folder: Contains the code to be run on the server side. The same steps that were done with the Client folder need to be taken for the server so that the mp3s are in the same location as the server code.

Run make:
The client and server code each have their own makefiles since they are in separate locations. Because of this, the makefiles for the server and client must be run individually.

Start:
Server: type "./musicServer" to run the server on shuttle2
Client: type "./musicClient" to run the client
Follow the commands, typing LIST, DIFF, PULL, or LEAVE in all CAPS to execute the methods.



Here is a close look at our code:

We have defined three structures that we use in our code. I will go through and explain how each one is used.

typedef struct{
	int method;
	int length;
	int indexes;
} header;

The header struct is sent before any data needs to be sent. It allows the receiver to know what to expect. 
    - The variable 'method' is used to specify which of the commands is being called (i.e. LIST=0, DIFF=1, PULL=2, LEAVE=3). 
    - ‘length’ is the number of bytes of the next thing to expect. In practice the next thing to expect is either an array of song structs or nothing. If nothing is to follow for whatever reason, length is set to zero. ‘length’ could be extended for project 4 if you feel it is necessary. 
    - ‘indexes’ is the number of indexes of the next thing to be sent. Again in practice the next thing to expect is either nothing or an array of song structs.


typedef struct {
	char title[TITLELEN]; 
	unsigned char checksum[MD5_DIGEST_LENGTH]; 
	int lenOfSong;		
} song;

The song struct contains info about a song file. It is normally created by calling
    -The ‘title’ is the title of the song. The max length of the title is a #define. 
    -The ‘checksum’ is what we use to compare songs to see if they are the same. 
    -The ‘lenOfSong’ is the length of the song in bytes.


typedef struct {
	int clientSock;	/*Socket descripter for client*/
	char *ip;
}ThreadArgs;
	
This struct is used to help the multithreaded server handoff info about the client socket and the client ip address. Unless you are doing something crazy you shouldn’t need to modify this for Project 4.



We use a client function and a server function for each of the commands (LIST, DIFF, PULL, LEAVE). I will walk you through what our code does for those functions.

List:

-The clientList method in networking.c sends a header with method=0, length=0, indexes=0 that tells the server that he wants to have the client LIST the songs on the server.
-The server looks at the header, sees that it is a LIST command and calls serverList in networking.c
-If the server has 0 .mp3 files in their current directory it informs the client by sending back a header with method=0, length=0, indexes=0 that tells the client that it is responding to a LIST command and that nothing will follow because there are no files on the server. The client prints out a message that the server has no files for them.
-If the server has .mp3 files in its current directory it sends header back with method =0, length= numSongs*sizeof(song), indexes= numSongs. This header tells the client that it is responding to a LIST command and to expect to receive a song array of ‘length’ number of bytes. This song array has ‘indexes’ number of song structs. 
-The client receives the header and prepares to receive the song struct array.
-The server sends an array of song structs to the client. The client receives the array. The client then loops through the array and prints out the titles of the songs for the user.


Diff:

-The clientDiff method in networking.c sends a header with method=1, length=0, indexes=0 that tells the server that he wants to have the client DIFF the songs on the server.
-The server looks at the header, sees that it is a DIFF command and calls serverDiff in networking.c.
-If the server has 0 .mp3 files in their current directory it informs the client by sending back a header with method=1, length=0, indexes=0 that tells the client that it is responding to a DIFF command and that nothing will follow because there are no files on the server. It prints out a message to the client that the server has no files for them.
-If the server has .mp3 files in its current directory it sends header back with method =1, length= numSongs*sizeof(song), indexes= numSongs. This header tells the client that it is responding to a DIFF command and to expect to receive a song array of ‘length’ number of bytes. This song array has ‘indexes’ number of song structs. 
-The client receives the header and prepares to receive the song struct array.
-The client receives the song array. 
-If the client has 0 .mp3 files in its current directory it prints out the titles of the songs that the server sent.
-The client compares the .mp3 files. 
-If the client and server have the same songs it informs the user. If the sever has a different songs than the client the songs are printed out to the user.


Pull: (NOTE! The Diff command compares the songs on the client side. The client receives the song array, creates its own array and compares them. The Pull command compares the songs on the server side. The server receives the song array from the client, creates its own array and compares them.)

-The clientPull method in networking.c sends a header with method=2, length=numSongs*sizeof(song), indexes=numSongs that tells the server that it wants to have the client PULL the songs on the server, and that an array of song structs will follow.
-The server looks at the header, sees that it is a PULL command and calls serverPull in networking.c.
-The client sends an array of song structs to the server.
-The server receives the array of song structs.
-The server creates an array of song structs from the .mp3 files in its current directory.
-The server compares the client song array and its own song array. It creates an array of song structs that contains all the songs that the server has that the client does not have.
-The server sends a header back indicating that an array will follow and its length in bytes is the length of the newly created song array. In some cases no song array is created and the length and indexes are set to 0 to inform the client that it already has all the songs that the server has.
-The client receives the header. If the length and indexes are not 0 it prepares to receive the song array.
-If there are songs that the client does not have, the server sends the song array.
-If there are songs to receive the client receives the array of song structs. 
-The server begins sending the first song in the array of song structs.
-The client receives the first song. It knows how many bytes the song is because it is contained in the first song struct in the song struct array.
-As soon as the first song finishes the server begins to send the next song. The client knows when the first song is finished receiving and begins receiving the second song. This continues until the all the songs in the song array have been sent and received.


Leave:
-The clientLeave method in networking.c sends a header with method 3=0, length=0, indexes=0 that tells the server that it wishes to LEAVE the connection.
-The server looks at the header, sees that it is a LEAVE command and calls serverLeave in networking.c.
-The serverLeave function sends a header acknowledging the clients desire to leave.  
-The client receives the acknowledgement. The connection is over.



Things to note:
-The endianness of the linux and the android phone might be different. When sending ints back and forth between the project 4 end hosts they might get miss-interrupted. That is neither our fault nor our problem. You will need to handle it yourself.
-If you have difficulty understanding our code you can email me, Marissa Wall, at mwall6@gatech.edu. I will not help you with project 4 or help you debug your code.





