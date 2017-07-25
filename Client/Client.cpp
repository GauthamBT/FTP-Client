#include<iostream>
#include<unistd.h> //To use uSleep()
#include<netdb.h> //To use Hostent
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h> //To use strerror()
#include<errno.h> //To get errno
#include<stdlib.h>
#include<stdio.h>
#include<fstream>

using namespace std;

class Client
{
public:
	struct sockaddr_in serverAddress;

	int clientSocket;

	//Starting client socket. 
	Client(string IPAddress, int portnumber, string protocol)
	{
		serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(8888);

		if(protocol == "TCP")
		{
			clientSocket = socket(AF_INET, SOCK_STREAM, 0);

			if(clientSocket < 0)
			{
				cerr << "Error in socket: " << strerror(errno) << endl;
				exit(0);
			}

			cout << "Client Socket created" << endl;
		}
		else if(protocol == "UDP")
		{
			clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

			if(clientSocket < 0)
			{
				cerr << "Error in socket: " << strerror(errno) << endl;
				exit(0);
			}

			cout << "Client Socket created" << endl;
		}
	}
};

int main()
{

	//Initiating client socket. 
	Client c("127.0.0.1", 8888, "TCP");

	int i = 0;
	int fileSize;
	int readSize;
	fstream file;
	char buffer[1024];
	char filename[20];

	//Connecting to the server. 
	if(connect(c.clientSocket,(struct sockaddr *) &c.serverAddress, sizeof(c.serverAddress)) < 0)
	{
		cerr << "Error in Connect: " << strerror(errno) << endl;
		exit(0);
	}

	cout << "Client connected" << endl;

	//Identifying to the server. 
	cout << "Identify to the server" << endl;

	//Geting input from the user. 
	cin.getline(buffer, 1024);

	//Sending identification to the server. 
	if(write(c.clientSocket, buffer, 1024) < 0)
	{
		cerr << "Error in write: " << strerror(errno) << endl;
		exit(0);
	}

	//To read the file list sent by the server. 
	if(read(c.clientSocket, buffer, 1024) < 0)
	{
		cerr << "Error in Read: " << strerror(errno) << endl;
		exit(0);
	}

	cout << "Received message: " << endl << buffer << endl;

	//To send the file name. After getting the file list. 
	cout << "Send file name" << endl;

	//Getting the file name from the user. 
	cin.getline(filename, 20);

	//Sending the file name to the user. 
	if(write(c.clientSocket, filename, sizeof(filename)) < 0)
	{
		cerr << "Error in write: " << strerror(errno) << endl;
		exit(0);
	}

	cout << "File name sent" << endl;

	//To read file size
	if(read(c.clientSocket, buffer, 1024) < 0)
	{
		cerr << "Error in Read: " << strerror(errno) << endl;
		exit(0);
	}

	//converting the filesize to int.
	sscanf(buffer, "%d", &fileSize);

	cout << "File size is: " << fileSize << endl;

	//Creating the file received from the server. 
	file.open(filename, ios::out | ios::app | ios::binary);

	//Received the file till the end of the file. 
	while(fileSize > 0)
	{
		readSize = read(c.clientSocket, buffer, 1);

		fileSize -= readSize;

		file.write(buffer, 1);
	}

	cout << "File received" << endl;

	//Closing the file after saving it. 
	file.close();

	//Interaction ended with the server. Closing socket. 
	close(c.clientSocket);

	return 0;
}
