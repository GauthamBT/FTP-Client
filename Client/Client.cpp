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

	Client c("127.0.0.1", 8888, "TCP");

	int i = 0;
	int fileSize;
	int readSize;
	fstream file;
	char buffer[1024];
	char filename[20];

	if(connect(c.clientSocket,(struct sockaddr *) &c.serverAddress, sizeof(c.serverAddress)) < 0)
	{
		cerr << "Error in Connect: " << strerror(errno) << endl;
		exit(0);
	}

	cout << "Client connected" << endl;


	cout << "Enter Message for Server" << endl;

	cin.getline(buffer, 1024);

	if(write(c.clientSocket, buffer, 1024) < 0)
	{
		cerr << "Error in write: " << strerror(errno) << endl;
		exit(0);
	}

	if(read(c.clientSocket, buffer, 1024) < 0)
	{
		cerr << "Error in Read: " << strerror(errno) << endl;
		exit(0);
	}

	cout << "Received message: " << endl << buffer << endl;

	cout << "Send file name" << endl;

	cin.getline(filename, 20);

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

	sscanf(buffer, "%d", &fileSize);

	cout << "File size is: " << fileSize << endl;

	file.open(filename, ios::out | ios::app | ios::binary);

	while(fileSize > 0)
	{
		readSize = read(c.clientSocket, buffer, 1);

		fileSize -= readSize;

		file.write(buffer, 1);
	}

	cout << "File received" << endl;

	file.close();

	close(c.clientSocket);

	return 0;
}
