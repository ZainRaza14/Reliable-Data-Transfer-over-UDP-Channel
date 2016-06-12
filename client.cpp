#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include  <string.h>
#include  <iostream>

#include  <sys/types.h>
#include  <sys/socket.h>

#include  <netinet/in.h>
#include  <arpa/inet.h>

#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "CRC.h"

#pragma hdrstop
#define strSize 100
#define CRC16_INIT_VALUE	0xffff
#define CRC16_XOR_VALUE		0x0000
#define headerSize 40 


#ifdef CREATE_CRC_TABLE
static unsigned short crctable[256];
void make_crc_table( void ) {
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

//Global Variable
int serverPort,clientPort,packetSize;
int ACKToDrop,ACKToCorrupt,serverTimeOut;
char *parity;
char fileName[50],serverIP[25];
bool isnottxt=false;


bool getnottxt()
{
	return isnottxt;
}

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



void CRC16_InitChecksum( unsigned short &crcvalue ) 
{
	crcvalue = CRC16_INIT_VALUE;
}

void CRC16_Update( unsigned short &crcvalue, const char data ) 
{
	crcvalue = ( crcvalue << 8 ) ^ crctable[ ( crcvalue >> 8 ) ^ data ];
}

void CRC16_UpdateChecksum( unsigned short &crcvalue, const void *data, int length )
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


void calculateParity(char *data) //calculation of 2d parity
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
	int extrazeros = (j*j) - size;   //appending extra zeros in the start of array
	int newsize = size + extrazeros;
	char* newdata;
	newdata = new char[newsize];
	i = 0;
	while (extrazeros!=0) //loop of adding up the 0's
	{     
		newdata[i] = '0';
		extrazeros--;
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



void CRC16_FinishChecksum( unsigned short &crcvalue ) {
	crcvalue ^= CRC16_XOR_VALUE;
}

unsigned short CRC16_BlockChecksum( const void *data, int length ) {
	unsigned short crc;

	CRC16_InitChecksum( crc );
	CRC16_UpdateChecksum( crc, data, length );
	CRC16_FinishChecksum( crc );
	return crc;
}

using namespace std;
const int size_of_pkts=100;



void setisnottxt()
{
	isnottxt=true;

}

 struct Packet_to_send // data for the packet to send header + Payload
  {
	  
	int source_port_number;
	
	int destination_port_number;  
	char payload[size_of_pkts];
	int sequence_number;
	unsigned short checksum;
	int number_of_packets;  
  };
   struct temp_pckt// data for the packet to check CHECKSUM header + Payload
  {
	  
	int source_port_number;
	
	int destination_port_number;  
	char payload[size_of_pkts];
	int sequence_number;
	int number_of_packets;  
  };
  


unsigned short calculateCRC(struct Packet_to_send * PKT) //calculating crc of the recieved pkt
{
	return CRC16_BlockChecksum((void*)PKT, sizeof(PKT));
} 

int srv_port=9898;
int count_for_seq_no=0;
int base=1;
int next_seq_number=1;
int window_size=10;



class Packet_list // Link list of the packetsss to send.
{
	public:
  struct Packet
  {
	  
	int source_port_number;
	int destination_port_number;  
	char *payload;
	int sequence_number;
	unsigned short checksum;
	bool isbase;
	bool is_sent;
	bool is_acknoldged; // info of ack recived or not
	bool is_lost;
	bool is_check_sum_corrupt;
	Packet *next;  
	  
  };
  
	Packet *Packet_header; 
		 public:
	  Packet_list ()
	 {
		Packet_header=NULL;
		
	 }
	   
	void add_packet_to_tail(int source_port_num,int dest_port_num,int seq_num,int checksum_rcv,char payload_recvd[])
	{
	
		 Packet* newptr=new Packet, *ptrtemp=Packet_header;
		 Packet_to_send *obj;
		 obj=new Packet_to_send[1];
		 newptr->source_port_number=source_port_num;
		
		 newptr->destination_port_number=dest_port_num;
		 newptr->payload=new char[size_of_pkts];
		 strcpy(newptr->payload,payload_recvd);
		 
		 newptr->sequence_number=seq_num;
		 
		 newptr->is_sent=false;
		 newptr->is_acknoldged=false;
		 newptr->isbase=false;
		 newptr->is_lost=false;
		 newptr->is_check_sum_corrupt=false;
		 newptr->checksum=0;// call chek sum here and pass 
		 newptr->next=NULL;
		 if(Packet_header==NULL)
			{
				Packet_header=newptr;
				return;
			}
		 while(ptrtemp->next!=NULL)
			ptrtemp=ptrtemp->next;
					
					
		 ptrtemp->next=newptr;
	}  
	
	void display_packet_sent() //display Function
	{
			Packet* ptrcurrent=Packet_header;
			while((ptrcurrent!=NULL) && (ptrcurrent->is_sent==true))
				{		
					cout<<"Sequence number\t\t"<<ptrcurrent->sequence_number<<endl;
					cout<<"Data:\n\n\t\t"<<ptrcurrent->payload<<endl;
					cout<<"Destination Port:\t\t"<<ptrcurrent->destination_port_number<<endl;
					cout<<"ACked\t\t";
					if(ptrcurrent->is_acknoldged)
						cout<<1<<endl;
					else
						cout<<0<<endl;
						cout<<"Is sent\t\t";
					if(ptrcurrent->is_sent)
						cout<<1<<endl;
					else
						cout<<0<<endl;
				 
					ptrcurrent=ptrcurrent->next;
				}
	}
int calculate_checksum(char payload_from_add[])
	{	
		return 0;	
	}
	
int count_clients()
	{
		int count=0;
		Packet* ptrcurrent=Packet_header;
		while(ptrcurrent!=NULL)
			{
				ptrcurrent=ptrcurrent->next;
				count++;			
			}
		return count;
	}	
Packet_to_send pull_packet() // pulling pkt...
		{
			Packet_to_send obj;
			Packet* ptrcurrent=Packet_header;
			while((ptrcurrent!=NULL) && (ptrcurrent->is_sent==true))
			ptrcurrent=ptrcurrent->next;
			
			if(ptrcurrent==NULL)
			cout<<"Packets end"<<endl;
			if(ptrcurrent!=NULL)
			{
			ptrcurrent->is_sent=true;
			
			
			//cout << "here" << endl;
			obj.source_port_number=ptrcurrent->source_port_number;
						
			obj.destination_port_number=ptrcurrent->destination_port_number;
		//	obj.payload=new char[50];
			strcpy(obj.payload,ptrcurrent->payload);
			obj.sequence_number=ptrcurrent->sequence_number;
			obj.checksum=ptrcurrent->checksum;
			obj.number_of_packets=this->count_clients();
			//cout << "here1" << endl;
		}
			return obj;
			
		}
	void move_header()
		{
			Packet_header=Packet_header->next;
			
		}	

void setting_acks(int ack) // seting the ack if right pkt recieved in order
{
	Packet* ptrcurrent=Packet_header;
	while( (ptrcurrent!=NULL) && ((ptrcurrent->sequence_number)<=ack))
	  {
			if(!(ptrcurrent->is_acknoldged))
				ptrcurrent->is_acknoldged=true;
				
				ptrcurrent=ptrcurrent->next;
	  }
}

void making_Headers_is_base_true() // header and base both point same the return true
{
	Packet_header->isbase=true;
}	

void setting_base(int ack_plus_one) //moving base of the window.
{
	Packet* ptrcurrent=Packet_header;
	while(ptrcurrent!=NULL)
	{
		
		if(ptrcurrent->sequence_number== ack_plus_one)
			{
				ptrcurrent->isbase=true;
				return;
			}
		ptrcurrent->isbase=false;
			ptrcurrent=ptrcurrent->next;
	}
	
}

	Packet_to_send pull_packet_for_repeat_send(int seq_num_to_trace)// pulling pkt for repeat
	{
		Packet_to_send obj;
			
		Packet* ptrcurrent=Packet_header;
		while((ptrcurrent!=NULL) && (ptrcurrent->sequence_number!=seq_num_to_trace))
		ptrcurrent=ptrcurrent->next;
		
		if(ptrcurrent==NULL)
		{
			cout<<"Packets end at Pull_packet_for_repeat"<<endl;
			obj.source_port_number=0;
			return obj;
		}
		if(ptrcurrent!=NULL)
		{	
			obj.source_port_number=ptrcurrent->source_port_number;
						
			obj.destination_port_number=ptrcurrent->destination_port_number;
		//	obj.payload=new char[50];
			strcpy(obj.payload,ptrcurrent->payload);
			obj.sequence_number=ptrcurrent->sequence_number;
			obj.checksum=ptrcurrent->checksum;
			obj.number_of_packets=this->count_clients();
						return obj;
		}

	}
	void setting_lost_packets(int seq_num_to_trace)// setting the pkt which is lost
		{
			Packet* ptrcurrent=Packet_header;
			while((ptrcurrent!=NULL) && (ptrcurrent->sequence_number!=seq_num_to_trace))
			ptrcurrent=ptrcurrent->next;
			if(ptrcurrent==NULL)
				cout<<"Packets end at Pull_packet_for_repeat"<<endl;
			
			ptrcurrent->is_lost=true;	

			
		}
		
		void setting_corrupt_packets(int seq_num_to_trace)
		{
			Packet* ptrcurrent=Packet_header;
			while((ptrcurrent!=NULL) && (ptrcurrent->sequence_number!=seq_num_to_trace))
			ptrcurrent=ptrcurrent->next;
			if(ptrcurrent==NULL)
				cout<<"Packets end at Pull_packet_for_repeat"<<endl;
			
			ptrcurrent->is_check_sum_corrupt=true;	

			
		}
	bool check_is_lost_or_not(int seq_num_to_trace)
	{
			Packet* ptrcurrent=Packet_header;
			while((ptrcurrent!=NULL) && (ptrcurrent->sequence_number!=seq_num_to_trace))
			ptrcurrent=ptrcurrent->next;
			if(ptrcurrent==NULL)
				cout<<"Packets end at Pull_packet_for_repeat"<<endl;
			if(ptrcurrent->is_lost)
			{
			 ptrcurrent->is_lost = false;
			 return true;
			}
			else
			 return false;
	}	
	
		bool check_is_crpt_or_not(int seq_num_to_trace)
	{
			Packet* ptrcurrent=Packet_header;
			while((ptrcurrent!=NULL) && (ptrcurrent->sequence_number!=seq_num_to_trace))
			ptrcurrent=ptrcurrent->next;
			if(ptrcurrent==NULL)
				cout<<"Packets end at Pull_packet_for_repeat"<<endl;
			if(ptrcurrent->is_check_sum_corrupt)
			{
			 ptrcurrent->is_check_sum_corrupt = false;
			 return true;
			}
			else
			 return false;
	}
};
 class Ack_list
 {
	 
		struct Ack_packet
			{
				int Ack;
				Ack_packet *next;
			}; 
		Ack_packet * LINK_HEADER;
		public:
		 Ack_list()
			{
				LINK_HEADER=NULL;
		
			}	
		void add_ack_packet_to_tail(int ack)
			{
				Ack_packet * newptr=new Ack_packet, *ptrtemp=LINK_HEADER;
				newptr->Ack=ack;
				newptr->next=NULL;
				cout<<"*****************ACk being added"<<ack<<"******************************88"<<endl;
				if(LINK_HEADER==NULL)
				{
					LINK_HEADER=newptr;
					return;
				}
				
				while(ptrtemp->next!=NULL)
					ptrtemp=ptrtemp->next;
		
			}
		int pull_ack_from_ack_list()
		{
			if(LINK_HEADER==NULL)
			return -1;
			else
			return LINK_HEADER->Ack;
		}
		void move_header()
		{
			LINK_HEADER=LINK_HEADER->next;
			
		}		
 };
 Ack_list acknolwgdement;
Packet_list All_packets;

  int sockfd;	
  struct sockaddr_in srv;
  
void Repeat_sent(int sig_no)
{
	
	for(int i=base;i<next_seq_number;i++)
	{
	    if(base== next_seq_number)
			exit(0);
		Packet_to_send *obj;
			if(i==All_packets.count_clients())
			break;
		obj=new Packet_to_send[1];
		
		obj[0]=All_packets.pull_packet_for_repeat_send(i);
		obj[0].checksum = 0;
		obj[0].checksum=calculateCRC(&obj[0]);
		cout<<"Sending "<<obj[0].sequence_number<<" # Packet"<<endl;
		cout<<"****************************************************"<<endl;
		if(obj[0].source_port_number==0)
			{
				
			}
		int numberofbytes=sendto(sockfd,&obj[0],sizeof(Packet_to_send),0,(struct sockaddr*)&srv,sizeof(srv));
		  if(numberofbytes == -1)
			{
				perror("Error");
				exit(1);
			}
		cout<<"Sent Repeated"<<endl;	
	}

			
}


void Putting_file_in_link_list()
	{
		
		//char *file_name;
		//int size_of_file_name=strlen("Checksum.txt");
		//file_name=new char[size_of_file_name];
		//strcpy(file_name,"Checksum.txt");
		//testing ends here
		
		

		
		if(getnottxt()==true) //have to use binary scene
		{
		
			string fileData;
			ifstream file;
			
			int fileSize,iteration,block = 1;
			char *binaryData;
			               // clear the fileData object of string class
			file.open(fileName, ios::in | ios::binary| ios::ate);    // open the pdf file in in, binary and ate mode. in for reading

		
			if(file.is_open())
			{                                      // binary for reading bit by bit and ate, so that the cursor will come to the start of file
				file.seekg (0,ios::end);                          // seekg will take the cursor to the end of file
				fileSize = (int) file.tellg();                  // it will calculate the size of the file
				binaryData = new char[fileSize];              // make new array of the size of file
				file.seekg(0,ios::beg); 

	                       // bring the cursor to the start of file again
		                // at filesize time, the iteration will run
				while(iteration > 0)
				{
					   // 
				memset(binaryData,0,strlen(binaryData));   // 
				file.read(binaryData,block);               // read a block and set rest index of array with null
				binaryData[block] = '\0';    
					            // read a block and set rest index of array with null                // to avoid the garbage value

	                  // appending the block into the fileData string, since this 											would be our final
					iteration--;                               // string in which the whole file is kept
				} //end while


			
				//now entire data is in file data. ab hum us ko packets m convert kar lein ge. binary scene ha sara .
				int k=0;		
				while((k!=fileSize)||(k<fileSize))
				{
					char *to_deliver;
					int temp=packetSize+2;
					to_deliver=new char[temp];
					for(int i=0;i<packetSize;i++)
					{
						to_deliver[i]=fileData[k];
						k++;
					}					
					
					count_for_seq_no++;
					
				All_packets.add_packet_to_tail(0,serverPort,count_for_seq_no,0,to_deliver);
				}
		

		}

		
		}
			
		else
		{
		ifstream fin;
		fin.open(fileName);
			while(!(fin.eof())) //making of the linklist from the File till its end
			{
				char *to_deliver;
				to_deliver=new char[packetSize];
				for(int i=0;i<packetSize;i++)
				to_deliver[i]=fin.get();
				count_for_seq_no++;
				All_packets.add_packet_to_tail(0,serverPort,count_for_seq_no,0,to_deliver);
			}
		fin.close();
		}
		All_packets.making_Headers_is_base_true(); //all are set and Ready to be Send

	}
  temp_pckt make_temp_for_check(Packet_to_send obj)
  {
	temp_pckt *temp;
	temp=new  temp_pckt[1];
	temp[0].source_port_number=obj.source_port_number;
	temp[0].destination_port_number=obj.destination_port_number;
	strcpy(temp[0].payload,obj.payload);
	temp[0].sequence_number=obj.sequence_number;
	temp[0].number_of_packets=obj.number_of_packets;  
	 return  temp[0];
  }
  void * sending_packets(void*)
	{

		int Number_of_packets=All_packets.count_clients();
		 sockfd = socket(AF_INET,SOCK_DGRAM,0); //UDP Socket being used
  
		if(sockfd < 0)
		 {
			perror("Error in socket creation");
			exit(1);
		 }
		else
			cout<<"\n\t** Socket creation successfull."<<endl;

		

		srv.sin_family = AF_INET;
		srv.sin_port = htons(serverPort);
		srv.sin_addr.s_addr = inet_addr(serverIP);

		socklen_t len = sizeof(srv);
		char *file_name;
		int size=100;
		file_name=new char[strlen("logFileClient.txt")];
		strcpy(file_name,"logFileClient.txt");
		ofstream outfile;
		outfile.open(file_name,fstream::app);
		
		int i=0;
		while(i<=Number_of_packets)
		{
			signal(SIGALRM,Repeat_sent);
				
			

			if(next_seq_number< (base+window_size))
			{
					
					Packet_to_send *obj;
				
					obj=new Packet_to_send[1];
					obj[0]=All_packets.pull_packet();
					obj[0].checksum = 0;
					obj[0].checksum=calculateCRC(&obj[0]);
					if((All_packets.check_is_lost_or_not(obj[0].sequence_number))) //checking if that specific packet is to be Lost or Not
							{
								cout<<"\nPacket with Seq number "<<obj[0].sequence_number<<"will be lost"<<endl;
								outfile<<"Packet Number # : ";
								outfile<<obj[0].sequence_number;
								outfile<<" | was Lost \n";

								next_seq_number++;
								continue;
;
							}
							
					else if((All_packets.check_is_crpt_or_not(obj[0].sequence_number)))//checking if that specific packet is to be made Corrupted or Not
					{
						cout<<"Packet with Sequence number "<<obj[0].sequence_number<<"was Corrupted"<<endl;
						obj[0].checksum++; //changed its checksum, with 1;
						
						
								outfile<<"Packet Number # : ";
								outfile<<obj[0].sequence_number;
								outfile<<" | was made Corrupted \n";
								next_seq_number++;
								continue;

					//lets make a file where we can make sure that the Packet is Actually being Courrpted
					
						
					}		
					else
					{	
									outfile<<"Packet Number # : ";
									outfile<<obj[0].sequence_number;
									outfile<<" | was Sent Successfully \n";
									next_seq_number++;
									i++;
								if(base==next_seq_number)
								{	
									sleep(serverTimeOut);			
									alarm(2);
								}
					}				
					int numberofbytes=sendto(sockfd,&obj[0],sizeof(Packet_to_send),0,(struct sockaddr*)&srv,sizeof(srv));
			}
		}
		cout << "sending ended: " << endl;
		outfile.close();
	}

void * recieving_ack(void *)
{
	
			
	socklen_t len = sizeof(srv);
	
	while(1)
		{

			
			cout<<"Recieving Ack Thread working"<<endl;
			int ack;
			cout<<"Recievning Ack"<<endl;
			int numberofbytes = recvfrom(sockfd,&ack,sizeof(int),0, (struct sockaddr*)&srv,&len);
			cout<<"Recieved Acked"<<endl;
			cout<<"Acknowledgement\t\t"<<ack<<endl;
			
			int should_break=0;
			
	
			if(ack==-1) //it was a Nack
			{
					continue;
			}
			else
			{
				base=ack+1;
				All_packets.setting_acks(ack);
				All_packets.setting_base(base);
				
				if(base==next_seq_number) //generate alarm if base becomes equal to the NextSequence Number
				{
					alarm(0);
					
				}

				else
				{
					alarm (2);
					
				}
				
				cout << "Packet# " << ack << " Acked Still now " << All_packets.count_clients()-1<< endl;
				if (ack == All_packets.count_clients())
				{
					cout << "all ack received" << endl;
					pthread_exit(0);
				}

			}				
			
		}
}
void * Manage_ack(void*)
	{
		while(1)
		{
			
		}
	}
	void making_packet_lost()
	{
		int number_of_pkts=All_packets.count_clients();
		cout<<"Total number of packets are\t"<<number_of_pkts-1<<endl;
		int checker=ACKToDrop;
		int packet_to_loose=ACKToDrop;
			
			while(1)
			{
				if((packet_to_loose==number_of_pkts) ||(packet_to_loose>number_of_pkts)) //setting which packets are not to be sent
				{	
					break;

				}	
				else
				{
					int packetLost;
					packetLost=(packet_to_loose);
					All_packets.setting_lost_packets(packetLost);
					packet_to_loose=packet_to_loose+checker; //incrementing which ones are to be lost.
				}
			}
	}
	void making_packets_is_corrupt()
	{
		int number_of_pkts=All_packets.count_clients();
		cout<<"Total number of packets are\t"<<number_of_pkts-1<<endl;
		int packet_to_corrupt=ACKToCorrupt;
		int checker2=ACKToCorrupt;			
		while(1)
		{	
			
				if((packet_to_corrupt==number_of_pkts)||(packet_to_corrupt>number_of_pkts)) //setting the Corrupt Packets
				{

					break;
				}
				 
				else
				{
					int packet_to_corrupt_for_link_list;
					packet_to_corrupt_for_link_list=(packet_to_corrupt);
					All_packets.setting_corrupt_packets(packet_to_corrupt_for_link_list);
					packet_to_corrupt=packet_to_corrupt+checker2;
				}
		
		}


	}

bool searchConfigFile() // config file chk hori hy k woh hay yaa nahi
{             
    struct dirent *dir;              
    DIR *dir_obj;
    dir_obj = opendir(".");
    while (dir = readdir(dir_obj)){
        if (strcmp(dir->d_name,"client_config.txt") == 0 && dir->d_type == DT_REG)
            return true;
    }
    return false;
}



//error deakh rha hy generally
void checkError(int value,string error)
{
    if(value<0){
        cerr<<error<<endl;
        exit(0);
    }
}



char tempStr[strSize]; 


void strTok() //string ko tokens myn kr rha hy divide
{
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



void retriveInfo()
{ 
	ifstream file;
	                
    file.open("client_config.txt");
      if(file.is_open()){
        while(!file.eof()){                      //read file
            memset(tempStr,0,strSize);
            file.getline(tempStr,strSize,'\n');            
            if(strncmp(tempStr,"Server IP",9) == 0){
                strTok();
                strcpy(serverIP,tempStr);                  
            }
            else if(strncmp(tempStr,"Server Port",11) == 0)
            {  
                strTok();
                serverPort = atoi(tempStr);
                //now converting the serverPort into int
            }
            else if(strncmp(tempStr,"Client Port",11) == 0){ 
                strTok();
                clientPort = atoi(tempStr);
                                      // now converting the clientPort into int
            }
            else if(strncmp(tempStr,"File Name",9) == 0)
	    {  
                strTok();
                strcpy(fileName,tempStr); 
		
		 
		                // now copying the File Name into fileName string
            }
            else if(strncmp(tempStr,"Packet Size",11) == 0)
	    {   // reading Packet Size and tokenized it from the string using strtok function
                strTok();                                      
                packetSize = atoi(tempStr);                  
                if(packetSize > 30000){                      
                    packetSize = 30000 - headerSize;
                    cout<<"Packet Size changed to "<<packetSize<<" bytes\n";
                }
            }
            else if(strncmp(tempStr,"ServerTimeout",13) == 0)
	    {    
                strTok();
                serverTimeOut = atoi(tempStr);                     
            }
            else if(strncmp(tempStr,"ACK number to drop",18) == 0){    // reading ACK number to drop and tokenized it from the string using strtok function
                strTok();
                ACKToDrop = atoi(tempStr);                             
            }
            else if(strncmp(
tempStr,"ACK number to corrupt",21) == 0){    // reading ACK number to corrupt and tokenized it from the string using strtok function
                strTok();
                ACKToCorrupt = atoi(tempStr);                         
            }
        }
      }
      else
          checkError(-1,"Cannot Open Configuration File");           
      
	file.close();           


      cout<<"The Configuration was SuccessFully Made !! ";
      cout<<"serverIP: "<<serverIP<<endl;
      cout<<"serverPort: "<<serverPort<<endl;
      cout<<"clientPort: "<<clientPort<<endl;
      cout<<"fileName: "<<fileName<<endl;
      cout<<"packetSize: "<<packetSize<<endl;
		int found = 0;
		/*string str = fileName;
		
		found = str.find(".txt");
		if(found==-1)
		{
			setisnottxt();
			

		} */  
		
		
     cout<<"serverTimeOut: "<<serverTimeOut<<endl;
     cout<<"ACKToDrop: "<<ACKToDrop<<endl;     
	 cout<<"ACKToCorrupt: "<<ACKToCorrupt<<endl;
      
     
}


int main()
{
			if(searchConfigFile() == true)                  // checking whether the confi file exists or not
				retriveInfo();
			else
				checkError(-1,"Configuration File Not Found");             // agr file nah mili to error
			//cout<<"here";

			srand(time(0));
		
			Putting_file_in_link_list();
			making_packet_lost();
			making_packets_is_corrupt();	
			pthread_t sending_thread;
			pthread_t recieving_ack_thread;
			pthread_t Managing_ack_thread;
			int error_check;
			error_check=pthread_create(&sending_thread,NULL,sending_packets,NULL);

			if(error_check!=0)
					{
						cout<<" Thread Not CREATED"<<endl;
					}

			error_check=pthread_create(&recieving_ack_thread,NULL,recieving_ack,NULL);
			if(error_check!=0)
				{
					cout<<" Thread Not CREATED"<<endl;
				}
					
			pthread_join(sending_thread,NULL);
			pthread_join(recieving_ack_thread,NULL);
		
			close(sockfd);
			return 0;
}



