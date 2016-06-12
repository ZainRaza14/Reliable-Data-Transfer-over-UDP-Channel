________________________________________________________________________
		RELIABLE DATA TRANSFER PROTOCOL Over UDP Channel
________________________________________________________________________

It is a protocol which runs on UDP (which is an unreliable best effort datagram communication service).

This protocol provides reliable data transfer, and it's design include functionalities such as
 - Sliding Window Protocol (GBN) GO Back N
 - ACKS 
 - Error control using CRC-16 and 2D Parity

 
CONTENTS
1.	How to run program
2.	Required Files for the Project
3.	Project Working
4.	Design Decisions & Issues Faced

____________________________
	>>How to run program
____________________________
	Program can be executed using makefile
	command >> make
	command >> ./server
	command >> ./client

	Program can be executed manually
	command >> g++ server.cpp -o server
	command >> ./server
	command >> g++ client.cpp -o client
	command >> ./client

__________________________________________
	>>Required Files for the Project
__________________________________________

->Server and Client both require configuration files.

	Client configuration file contains following information
	> Server IP
	> Server port
	> Client Port
	> File name
	> Packet size
	> ServerTimeout
	> ACK number to drop
	> ACK number to corrupt

	Server configuration file contains following information
	> Server port
	> File name 
	> Packet size
	> Packet number to drop
	> Packet number to corrupt

__________________________
	>>Project Working
__________________________

SERVER:
	The server executes and retrieves configuration from con-fig file. Creates and bind socket, creates new threads and waits
	for incoming connections.
	Connection is created. 
	Then it receives packets from client and stores them in linked list.
	It checks the packets for errors.
	It corrects errors and writes the packet to file(Dropped,Discard and Received Packets and ACKS info to the File).
	When it receives Last Packets error free then it will start waiting for the other client.

CLIENT:
	The client executes and retrieves configuration from con-fig file.
	Creates packets of file data and appends header to each of them.
	Creates ServerAddr and creates connection to Server.
	It then sends packets using Go Back N protocol, and when it has received ACKS for all the packets, it sends
	finish signal to server and exits.
	
__________________________________________	
	>>Design Decisions & Issues Faced
__________________________________________	
Threads:
	Threads are used both at the client and the server for synchronization. 
	They will also help to implement the whole procedure in a more quick way as compared to a program which don’t have threads.
	
	Server side Threads
		1-	Recieving_Thread.
		2-	Manage_Thread.
	
	Client Side Threads	
		1-	Sending_Thread.
		2-	Receiving_ACK_Thread.

Alarms
	Alarms are also used at some points so that we can encounter some problems as faced during the implementation of the project.

Mutex
	We also tried to implement mutexes to further synchronize the threads.
	So that we can get more good results but we faced many problems while implementing it.
	So, we decided to ignore it and came up with using only threads.

CRC 16 and 2-D Parity used.
	2-D Parity is used because 1-D parity cannot detect the 2-bit error. 

__________________________________________________________________________________END_____________________________________________________________________________	
	
	
