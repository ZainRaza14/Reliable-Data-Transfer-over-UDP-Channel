#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include<string.h>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fstream>
#include <pthread.h>

#include "CRC.h"

#pragma hdrstop

#define CRC16_INIT_VALUE	0xffff
#define CRC16_XOR_VALUE		0x0000
#define strSize 100
#define headerSize 40


#ifdef CREATE_CRC_TABLE

static unsigned short crctable[256]; //this is CRC table of Size 256
void make_crc_table( void )  //initiating the table
{
	int i, j;
	unsigned long poly, c;
	
	static const byte p[] = {0,5,12};								
	
	poly = 0L;
	for (i=0; i < sizeof(p) / sizeof(byte); i++) {
		poly |= 1L << p[i];
	}

	for(i=0; i<256; i++) {
		c = i << 8;
		for ( j = 0; j < 8; j++ ) {
			c = ( c & 0x8000 ) ? poly ^ ( c << 1 ) : ( c << 1 );
		}
		crctable[i] = (unsigned short) c;
	}
}
#else

bool isnottxt=false; //ye flag txt aur .pdf ko check karnay k liye ha

//this is table
static unsigned short crctable[256] = {
	0x0000,	0x1021,	0x2042,	0x3063,	0x4084,	0x50a5,	0x60c6,	0x70e7,
	0x8108,	0x9129,	0xa14a,	0xb16b,	0xc18c,	0xd1ad,	0xe1ce,	0xf1ef,
	0x1231,	0x0210,	0x3273,	0x2252,	0x52b5,	0x4294,	0x72f7,	0x62d6,
	0x9339,	0x8318,	0xb37b,	0xa35a,	0xd3bd,	0xc39c,	0xf3ff,	0xe3de,
	0x2462,	0x3443,	0x0420,	0x1401,	0x64e6,	0x74c7,	0x44a4,	0x5485,
	0xa56a,	0xb54b,	0x8528,	0x9509,	0xe5ee,	0xf5cf,	0xc5ac,	0xd58d,
	0x3653,	0x2672,	0x1611,	0x0630,	0x76d7,	0x66f6,	0x5695,	0x46b4,
	0xb75b,	0xa77a,	0x9719,	0x8738,	0xf7df,	0xe7fe,	0xd79d,	0xc7bc,
	0x48c4,	0x58e5,	0x6886,	0x78a7,	0x0840,	0x1861,	0x2802,	0x3823,
	0xc9cc,	0xd9ed,	0xe98e,	0xf9af,	0x8948,	0x9969,	0xa90a,	0xb92b,
	0x5af5,	0x4ad4,	0x7ab7,	0x6a96,	0x1a71,	0x0a50,	0x3a33,	0x2a12,
	0xdbfd,	0xcbdc,	0xfbbf,	0xeb9e,	0x9b79,	0x8b58,	0xbb3b,	0xab1a,
	0x6ca6,	0x7c87,	0x4ce4,	0x5cc5,	0x2c22,	0x3c03,	0x0c60,	0x1c41,
	0xedae,	0xfd8f,	0xcdec,	0xddcd,	0xad2a,	0xbd0b,	0x8d68,	0x9d49,
	0x7e97,	0x6eb6,	0x5ed5,	0x4ef4,	0x3e13,	0x2e32,	0x1e51,	0x0e70,
	0xff9f,	0xefbe,	0xdfdd,	0xcffc,	0xbf1b,	0xaf3a,	0x9f59,	0x8f78,
	0x9188,	0x81a9,	0xb1ca,	0xa1eb,	0xd10c,	0xc12d,	0xf14e,	0xe16f,
	0x1080,	0x00a1,	0x30c2,	0x20e3,	0x5004,	0x4025,	0x7046,	0x6067,
	0x83b9,	0x9398,	0xa3fb,	0xb3da,	0xc33d,	0xd31c,	0xe37f,	0xf35e,
	0x02b1,	0x1290,	0x22f3,	0x32d2,	0x4235,	0x5214,	0x6277,	0x7256,
	0xb5ea,	0xa5cb,	0x95a8,	0x8589,	0xf56e,	0xe54f,	0xd52c,	0xc50d,
	0x34e2,	0x24c3,	0x14a0,	0x0481,	0x7466,	0x6447,	0x5424,	0x4405,
	0xa7db,	0xb7fa,	0x8799,	0x97b8,	0xe75f,	0xf77e,	0xc71d,	0xd73c,
	0x26d3,	0x36f2,	0x0691,	0x16b0,	0x6657,	0x7676,	0x4615,	0x5634,
	0xd94c,	0xc96d,	0xf90e,	0xe92f,	0x99c8,	0x89e9,	0xb98a,	0xa9ab,
	0x5844,	0x4865,	0x7806,	0x6827,	0x18c0,	0x08e1,	0x3882,	0x28a3,
	0xcb7d,	0xdb5c,	0xeb3f,	0xfb1e,	0x8bf9,	0x9bd8,	0xabbb,	0xbb9a,
	0x4a75,	0x5a54,	0x6a37,	0x7a16,	0x0af1,	0x1ad0,	0x2ab3,	0x3a92,
	0xfd2e,	0xed0f,	0xdd6c,	0xcd4d,	0xbdaa,	0xad8b,	0x9de8,	0x8dc9,
	0x7c26,	0x6c07,	0x5c64,	0x4c45,	0x3ca2,	0x2c83,	0x1ce0,	0x0cc1,
	0xef1f,	0xff3e,	0xcf5d,	0xdf7c,	0xaf9b,	0xbfba,	0x8fd9,	0x9ff8,
	0x6e17,	0x7e36,	0x4e55,	0x5e74,	0x2e93,	0x3eb2,	0x0ed1,	0x1ef0
};
#endif

void CRC16_InitChecksum( unsigned short &crcvalue )  //initializing the checksum
{
	crcvalue = CRC16_INIT_VALUE;
}

void CRC16_Update( unsigned short &crcvalue, const char data )  //creating new wala CRC16 ki value
{
	crcvalue = ( crcvalue << 8 ) ^ crctable[ ( crcvalue >> 8 ) ^ data ];
}

void CRC16_UpdateChecksum( unsigned short &crcvalue, const void *data, int length ) //ab hum save kar lein ge in crcValue passing it as ref.
{
	unsigned short crc;
	const unsigned char *buf = (const unsigned char *) data;

	crc = crcvalue;
	while( length-- ) 		
	{
		crc = ( crc << 8 ) ^ crctable[ ( crc >> 8 ) ^ *buf++ ];
	}
	crcvalue = crc;
}


void CRC16_FinishChecksum( unsigned short &crcvalue ) //XOR the Value stored in the the Final CheckSum 
{
	crcvalue ^= CRC16_XOR_VALUE;
}

unsigned short CRC16_BlockChecksum( const void *data, int length )
 {
	unsigned short crc;

	CRC16_InitChecksum( crc );
	CRC16_UpdateChecksum( crc, data, length );
	CRC16_FinishChecksum( crc );
	return crc;
}




unsigned short calculateCRC(struct Packet * PKT)
{
	return CRC16_BlockChecksum((void*)PKT, sizeof(PKT));
} 



int serverPort,clientPort,packetSize; //aik ha server ki port, aik client port and packet ka Size from the config file
int packetToDrop,packetToCorrupt,serverTimeOut; //packettodrop,packettocorrup and Server ka timeout
char fileName[50],serverIP[25];
int increment; //this is value for the Drop packet.
int incrementCorr; //this is value for the Corrupt . send NAk
char *parity; //for the Parity value

void paritymaker(char *data) //caluculation of 2d parity
{     
	int size;    
	size = packetSize;
	int i=0;
	int j=1;
	int intResult=0;
	int finResult=0;
	int check = 0;
	while(check != 1)
	{                    
		intResult = i*i;              
		finResult = j*j;              
		if ((intResult <= size) && (finResult >= size))
		{  
			check = 1;                               
			break;
		}
		i++; //incrementing for the rows
		j++; //colums
	}

	int arraysize = j;   //this is array size. 2d. same as jxj
	char array[j][j]; //making array of jxj
	parity = new char[j*j];
	int zerochecker = (j*j) - size;   //appending extra zeros in the start of array
	int newsize = size + zerochecker;
	char* newdata;
	newdata = new char[newsize];
	i = 0;
	while (zerochecker!=0) //loop of adding up the 0's
	{     
		newdata[i] = '0';
		zerochecker--;
		i++;
	}

	j = 0;
	while (size!=0) //populating data
	{    
		newdata[i] = data[j];
		size--;
		i++;
		j++;
	}
	i = 0;
	for (int outerloop = 0; outerloop < arraysize; outerloop++)
	{    
		for (int innerloop = 0; innerloop < arraysize; innerloop++)
		{
			array[outerloop][innerloop] = newdata[i];
			i++;
		}
	}
	int ones = 0;
	i=0;
	for (int outerloop = 0; outerloop < arraysize; outerloop++)
	{   //coloum k lehaaz se calculting Parity
		for (int innerloop = 0; innerloop < arraysize; innerloop++)
		{
			if ((array[outerloop][innerloop])== '1')
				ones++;
		}
		if ((ones%2)==0)
			parity[i]='0';
		else if ((ones%2)==1)
			parity[i]='1';
		i++;
		ones =0;
	}
	for (int outerloop = 0; outerloop < arraysize; outerloop++)
	{  //row k lehaaz se parity calculating
		for (int innerloop = 0; innerloop < arraysize; innerloop++)
		{
			if ((array[innerloop][outerloop])== '1')
				ones++;
		}
		if ((ones%2)==0)
			parity[i]='0';
		else if ((ones%2)==1)
			parity[i]='1';
		i++;
		ones =0;
	}
}





bool getnottxt() //ye wohi .pdf and .txt distinguish karnay k liye ha
{
	return isnottxt;
}

void setisnottxt() //true if its .pdf
{
	isnottxt=true;

}


using namespace std;

int checkLast=0;
const int size_of_pkts=100;  //this is packet Size
struct Packet //structure for Packet which is to be recieved
{
	  
	int source_port_number;
	int destination_port_number;  
	char payload[size_of_pkts];
	int sequence_number;
	unsigned short checksum;
	int number_of_packets;	  
};
     struct TempoPacket //structure same like Packet but used to check checksum and parity after that made into Packet
  {
	  
	int source_port_number;
	
	int destination_port_number;  
	char payload[size_of_pkts];
	int sequence_number;
	int number_of_packets;  
  };
  int sockfd; //global socket descriptor
 class dump_linklist
 {
	struct IgnorePacket
		{
	  
			int source_port_number;
			int destination_port_number;  
			char payload[size_of_pkts];
			int sequence_number;
			unsigned short checksum;
			int number_of_packets;
			IgnorePacket *next;	  
		}; 
	public:
	IgnorePacket * LINK_HEADER;
	 
	 	 public:
	  dump_linklist ()
	 {
		 LINK_HEADER=NULL;
	 } 
	 void InsertIntoList(Packet obj) //organizing the packets into a link list
		{
			IgnorePacket  * newptr=new IgnorePacket , *ptrtemp=LINK_HEADER;
			newptr->source_port_number=obj.source_port_number;
			newptr->destination_port_number=obj.destination_port_number;
			strcpy(newptr->payload,obj.payload);
			newptr->sequence_number=obj.sequence_number;
			newptr->checksum=obj.checksum;
			newptr->number_of_packets=obj. number_of_packets;
			newptr->next=NULL;
			if(LINK_HEADER==NULL)
				{
					LINK_HEADER=newptr;
					return;
				}
			while(ptrtemp->next!=NULL)
				ptrtemp=ptrtemp->next;
				
				ptrtemp->next=newptr;
			
			if(ptrtemp->next->sequence_number==ptrtemp->next->number_of_packets) //this is to check if it was needed seq or not
			{
				
				int size=size_of_pkts;			
				int check_null=0;
				IgnorePacket* ptrcurrent=LINK_HEADER;


			

if(getnottxt()) //binaray file writing as it was need for .pdf format
{
	 char *binaryData;
        ofstream file;
        file.open(fileName,ios::out | ios::binary);
        if(file.is_open()){
            while(ptrcurrent!=NULL)
		{
                
                binaryData = new char[size+2];             

                binaryData[size+1] = '\0';
                file.write(binaryData,strlen(binaryData));
                ptrcurrent = ptrcurrent->next;
            	}
            file.close();
        }

}				
			while((ptrcurrent!=NULL))
			{
						char  *buffer;
						buffer=new char[size];
						strcpy(buffer,ptrcurrent->payload);
						ptrcurrent=ptrcurrent->next;
			
			}
			

			}	
		
		}
			
	
	
		void display_list_2()
		{
			int count=0;
			IgnorePacket* ptrcurrent=LINK_HEADER;
		 while(ptrcurrent!=NULL)
			{
				for(int i=0;i<size_of_pkts;i++)
					{
						cout<<ptrcurrent->payload[i];
						if(i/10==0)
						cout<<"\n";
					}
				ptrcurrent=ptrcurrent->next;	
			}
		}
	
		
	Packet Pull_packet()
		{
			Packet obj;
				if(LINK_HEADER!=NULL)	
				{
				obj.source_port_number=LINK_HEADER->source_port_number;
				obj.destination_port_number=LINK_HEADER->destination_port_number;
				strcpy(obj.payload,LINK_HEADER->payload);
				obj.sequence_number=LINK_HEADER->sequence_number;
				obj.checksum=LINK_HEADER->checksum;
				obj.number_of_packets=LINK_HEADER->number_of_packets;
				return obj;
				}
				else
				{
					//refreshing the OBJ sources.
					obj.source_port_number=0;
					obj.destination_port_number=0;
					obj.sequence_number=0;
					obj.checksum=0;
					obj.number_of_packets=0;
					return obj;
				
				}
		
				
		}
		void move_header()
		{
			LINK_HEADER=LINK_HEADER->next;
			
		}
 };  
  
  class linklist_clients_data
{
	struct Client_info
     {
		struct sockaddr_in cli_info;
		 dump_linklist packets_of_file;
		 int expected_sequence_number;
		 Client_info *next;
		 
	 };
	 Client_info * LINK_HEADER;
	 
	 	 public:
	  linklist_clients_data ()
	 {
		 LINK_HEADER=NULL;
	 }
	 
	 void InsertIntoList_client(struct sockaddr_in temp)
	 {
		 Client_info * newptr=new Client_info, *ptrtemp=LINK_HEADER;
		 newptr->cli_info=temp;
		 newptr->expected_sequence_number=1;
		 newptr->next=NULL;
		 if(LINK_HEADER==NULL)
			{
				LINK_HEADER=newptr;
				return;
			}
			
			while(ptrtemp->next!=NULL)
				ptrtemp=ptrtemp->next;
				
				
				ptrtemp->next=newptr;
	 }
	 void Addpacketnode(int port,Packet temp)
	 {
		 Client_info* ptrcurrent=LINK_HEADER;
			while((ptrcurrent!=NULL) && (ptrcurrent->cli_info.sin_port!=port))
				ptrcurrent=ptrcurrent->next;
				if(ptrcurrent==NULL)
				{
				cout<<"\n\n\n\t\t\tClient with this port doesnt exist\n\n\n"<<endl;
				return;
				}
				else
				{
					ptrcurrent->packets_of_file.InsertIntoList(temp);
					
				}
				
	 }
	
	 bool search_specific_node(int port) //searching a specific packet in the List
	 {
		 Client_info* ptrcurrent=LINK_HEADER;
			while((ptrcurrent!=NULL) && (ptrcurrent->cli_info.sin_port!=port))
				ptrcurrent=ptrcurrent->next;
				if(ptrcurrent==NULL)
					return true;
				else 
					return false;
		 
		 
	 }
	 bool search_for_specific_node_to_check_expected_seq_num(Packet temp) //searching and checking for a packet with seq Number.
	 {
		 		 Client_info* ptrcurrent=LINK_HEADER;
			while((ptrcurrent!=NULL) && (ptrcurrent->cli_info.sin_port!=temp.source_port_number))
				ptrcurrent=ptrcurrent->next;
				
				cout << "to send" << endl;
			


			if(temp.sequence_number==packetToCorrupt) //not to send ack
			{

				int prev_ack=ptrcurrent->expected_sequence_number-1;
				char *file_name;
				file_name=new char[strlen("logFileServer.txt")];
				strcpy(file_name,"logFileServer.txt"); //maintainlog at Server Side
				ofstream outfile;
				outfile.open(file_name,fstream::app);
				outfile<<"Packet Number # : ";
				outfile<<temp.sequence_number;
				outfile<<" | Ack not Send \n";
				//outfile.close;
				cout<<"Last SuccessFully Obtained Ack -> "<<prev_ack<<endl;
				packetToCorrupt+=incrementCorr;
				
				int numberofbytes=sendto(sockfd,&prev_ack,sizeof(int),0,(struct sockaddr*)&ptrcurrent->cli_info,sizeof(ptrcurrent->cli_info));
				 if(numberofbytes == -1)
					{
						cout<<"Error in sending ack"<<endl;
					}
				cout<<"Ack was SuccessFully Send"<<endl;
			return false;	

			}

			
			if(temp.sequence_number==packetToDrop) //not to send ack
			{

				int prev_ack=ptrcurrent->expected_sequence_number-1;
				char *file_name;
				file_name=new char[strlen("logFileServer.txt")];
				strcpy(file_name,"logFileServer.txt"); //maintainlog at Server Side
				ofstream outfile;
				outfile.open(file_name,fstream::app);
				outfile<<"Packet Number # : ";
				outfile<<temp.sequence_number;
				outfile<<" | was Dropped at Server \n";
				//outfile.close;
				cout<<"Last SuccessFully Obtained Ack -> "<<prev_ack<<endl;
				packetToDrop+=increment;
				
				int numberofbytes=sendto(sockfd,&prev_ack,sizeof(int),0,(struct sockaddr*)&ptrcurrent->cli_info,sizeof(ptrcurrent->cli_info));
				 if(numberofbytes == -1)
					{
						cout<<"Error in sending ack"<<endl;
					}
				cout<<"Ack Was SuccessFully Sent"<<endl;
			return false;	

			}



			if(ptrcurrent->expected_sequence_number==temp.sequence_number) 
			{


				cout<<"Sending Ack From Server"<<endl;
				char *file_name;
				file_name=new char[strlen("logFileServer.txt")];
				strcpy(file_name,"logFileServer.txt"); //maintainlog at Server Side
				ofstream outfile;
				outfile.open(file_name,fstream::app);
				outfile<<"Packet Number # : ";
				outfile<<temp.sequence_number;
				outfile<<"| was Acked Now \n";
				
				if(temp.sequence_number<=temp.number_of_packets)
				{
					int size=size_of_pkts;			

					if((checkLast==0)&&(temp.sequence_number==temp.number_of_packets-1))
					{
					
					  	ofstream writi;
						writi.open(fileName,fstream::app);
						char  *buffer;
						buffer=new char[size];
						strcpy(buffer,temp.payload);
						writi.write(buffer,size);
						checkLast++;
					 }

					else if((checkLast==0)&&(temp.sequence_number<temp.number_of_packets-1))
					{
						ofstream writi;
						writi.open(fileName,fstream::app);
						char  *buffer;
						buffer=new char[size];
						strcpy(buffer,temp.payload);
						writi.write(buffer,size);
						

					}

				}
				
				
				int numberofbytes=sendto(sockfd,&ptrcurrent->expected_sequence_number,sizeof(int),0,(struct sockaddr*)&ptrcurrent->cli_info,sizeof(ptrcurrent->cli_info));
				
				 if(numberofbytes == -1)
					{
						cout<<"Error in sending ack"<<endl;
					}
				cout<<"Sent Ack From Server"<<ptrcurrent->expected_sequence_number<<endl;	
				if(ptrcurrent->expected_sequence_number==temp.number_of_packets)
					return true;
	
						ptrcurrent->expected_sequence_number++;
						
				return true;
			}
			else 
			{
				char *file_name;
				file_name=new char[strlen("logFileServer.txt")];
				strcpy(file_name,"logFileServer.txt"); //maintainlog at Server Side
				ofstream outfile;
				outfile.open(file_name,fstream::app);
				outfile<<"Packet Number # : ";
				outfile<<temp.sequence_number;
				outfile<<" was discard \n";
				cout<<"Packet "<<temp.sequence_number<<" has been recieved  but was not expected"<<endl;
				int prev_ack=ptrcurrent->expected_sequence_number-1;
				cout<<"Last SuccessFully Obtained Ack -> "<<prev_ack<<endl;
				int numberofbytes=sendto(sockfd,&prev_ack,sizeof(int),0,(struct sockaddr*)&ptrcurrent->cli_info,sizeof(ptrcurrent->cli_info));
				 if(numberofbytes == -1)
					{
						cout<<"Error in sending ack"<<endl;
					}
				cout<<"Ack SuccessFully Sent"<<endl;
			return false;	
			}
		 
	 }
	
 	
};
linklist_clients_data client_address;

 
 dump_linklist dumping_list;
 
	void * downloadAndWrite(void*)
	{
		struct sockaddr_in my_address;
		my_address.sin_family = AF_INET;
		my_address.sin_port   = htons(serverPort);
		my_address.sin_addr.s_addr   = htonl(INADDR_ANY);
		struct sockaddr_in cli_address; 
		sockfd = socket(AF_INET,SOCK_DGRAM,0);
		if(sockfd < 0)
			{
				perror("Error in socket creation");
				exit(1);
			}
		else
				cout<<"\n\tSocket was made SuccessFully"<<endl;
		int res = bind(sockfd,(struct sockaddr *)&my_address,sizeof(my_address));
  
		if(res < 0)
			{
				perror("Error in binding");
				exit(1);
			}
		else
				cout<<"\n\tBinding was SuccessFully Done"<<endl;
		
		socklen_t len = sizeof(cli_address);
						
				while(1)
				{
					
					Packet *file_chunks;
					cout<<"Recieving  thread"<<endl;
					file_chunks=new Packet[1];
					int numberofbytes = recvfrom(sockfd, &file_chunks[0],sizeof(Packet),0, (struct sockaddr*)&cli_address,&len);
					
					
					file_chunks[0].source_port_number=cli_address.sin_port;
					if(numberofbytes < 0)
						{
							perror("Data is not recieved");
							exit(1);
						}
				
					if(client_address.search_specific_node(cli_address.sin_port))
						{
							client_address.InsertIntoList_client(cli_address);
						}		
					
					dumping_list.InsertIntoList(file_chunks[0]);	
				
					
				}
		close(sockfd);
	}
TempoPacket storeTempo(Packet obj)
  {
	TempoPacket *temp;
	temp=new  TempoPacket[1];
	temp[0].source_port_number=obj.source_port_number;
	temp[0].destination_port_number=obj.destination_port_number;
	strcpy(temp[0].payload,obj.payload);
	temp[0].sequence_number=obj.sequence_number;
	temp[0].number_of_packets=obj.number_of_packets;  
	 return  temp[0];
  }
	void * AckManagment(void*)
	{
		while(1)
		{
			
			Packet temp=dumping_list.Pull_packet();
			unsigned short temp_check_sum=temp.checksum;
			temp.checksum=0;
			int temp_port_number=temp.source_port_number;
			
			
				
			if(temp.source_port_number==0)
				{
					continue;
				}
				temp.source_port_number=0;
	unsigned short temp_check_sum_2=calculateCRC(&temp);
		
			if(temp_check_sum!=temp_check_sum_2)
			{
				cout<<"CORRUPTED DATA"<<endl;
				dumping_list.move_header();
				continue;
			}			
			temp.source_port_number=temp_port_number;
			if(client_address.search_for_specific_node_to_check_expected_seq_num(temp))
				{
					client_address.Addpacketnode(temp.source_port_number,temp);
					dumping_list.move_header();
					cout<<"Sequence number\t\t"<<temp.sequence_number<<endl;
					cout<<"Data:\n\n\t\t"<<temp.payload<<endl;
					cout<<"Source Port:\t\t"<<temp.source_port_number<<endl;
					cout<<"Destination Port:\t\t"<<temp.destination_port_number<<endl;
					
				
				}
			else
				{
					dumping_list.move_header();
				}
			
		}
	}


bool ConfigExists(){             // Checking whether the configuration file of the client exits or not.
    struct dirent *dir;              // if yes then it return true else return false
    DIR *dir_obj;
    dir_obj = opendir(".");
    while (dir = readdir(dir_obj)){
        if (strcmp(dir->d_name,"server_config.txt") == 0 && dir->d_type == DT_REG) //dir prespective
            return true;
    }
    return false;
}



//CHECKS ERROR
void checkError(int value, std::string error){
    if(value<0){
        cerr<<error<<endl;
        exit(0);
    }
}



char tempStr[strSize]; 


void strTok(){
    char newTempStr[strSize];
    strcpy(newTempStr,tempStr);
    memset(tempStr,0,strSize);
    int found,index = 0;
    string strToSearch = newTempStr;
    string strToFind = (":");
    found = strToSearch.find(strToFind);
    for(int i=(found+1);i<strlen(newTempStr);i++){
      tempStr[index] = newTempStr[i];
      index++;
    }
}


void retriveInfo(){
    fstream file;
    file.open("server_config.txt");
      if(file.is_open()){
        while(!file.eof()){
            memset(tempStr,0,100);
            file.getline(tempStr,100,'\n');
            if(strncmp(tempStr,"Server IP",9) == 0){
                strTok();
                strcpy(serverIP,tempStr);
            }
            else if(strncmp(tempStr,"Server Port",11) == 0){
                strTok();
                serverPort = atoi(tempStr);
               
            }
            else if(strncmp(tempStr,"File Name",9) == 0){
                strTok();
                strcpy(fileName,tempStr);
            }
            else if(strncmp(tempStr,"Packet Size",11) == 0){
                strTok();
                packetSize = atoi(tempStr);
                if(packetSize > 32768){
                    packetSize = 32760 - headerSize;
                    cout<<"Packet Size changed to "<<packetSize<<" bytes\n";
                }
            }
            else if(strncmp(tempStr,"Packet number to drop",21) == 0){
                strTok();
                packetToDrop = atoi(tempStr);
		increment=packetToDrop;
            }
            else if(strncmp(tempStr,"Packet number to corrupt",24) == 0){
                strTok();
                packetToCorrupt = atoi(tempStr);
		incrementCorr=packetToCorrupt;
		
            }
        }
      }
      else
          checkError(-1,"Cannot Open Configuration File");
      file.close();

      cout<<"serverIP: "<<serverIP<<endl;
      cout<<"serverPort: "<<serverPort<<endl;
      cout<<"fileName: "<<fileName<<endl;
      cout<<"packetSize: "<<packetSize<<endl;

	int found = -1;
		
		string str = fileName;
		
		found = str.find(".txt");
		if(found==-1)
		{
			setisnottxt();
			

		}   


      cout<<"packetToDrop: "<<packetToDrop<<endl;
      cout<<"packetToCorrupt:"<<packetToCorrupt<<endl;
}




	
int main()
{
	
	
	 if(ConfigExists() == true)                  // checking whether the confi file exists or not
                    retriveInfo();
                    else
                    checkError(-1,"Configuration File Not Found");             // if file doesnot find then it will display the error




	pthread_t RecvThread;
	pthread_t AckManThread;
	int error_check;
	error_check=pthread_create(&RecvThread,NULL,downloadAndWrite,NULL);
	if(error_check!=0)
	{
		cout<<" Thread Not CREATED"<<endl;
	}
	
	sleep(1);
	error_check=pthread_create(&AckManThread,NULL,AckManagment,NULL);
	if(error_check!=0)
	{
		cout<<" Thread Not CREATED"<<endl;
	}
	pthread_join(RecvThread,NULL);
	pthread_join(AckManThread,NULL);
    
  
	return 0;
}
  
	
	
