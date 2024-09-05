#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

using namespace std;

int main()
{
	/*if(argc!=3){
		cout<<"Usage: "<<argv[0]<<" <server ipv4 address>"<<" <server port address>"<<endl;
		exit(1);
	}*/
    //int clientSocket;

    while(1){
		int c;
		cout<<"Connect to server ";
		cin>>c;
		//if(c==0) break;
		// Create socket
    	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    	if (clientSocket == -1)
    	{
        	perror("Error creating socket");
        	exit(EXIT_FAILURE);
    	}
    	struct sockaddr_in serverAddr;
		// Set up server address struct
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = inet_addr(c==1 ? "192.168.0.119" : "192.168.0.120");  // IP address of the server
		serverAddr.sin_port = htons(8080);

		// Connect to the server
		int cid=connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
		
		if (cid == -1)
		{
		    perror("Error connecting to server");
		    exit(EXIT_FAILURE);
		}
		else{ 
			char data[50];
			sleep(5);
			int n=recv(clientSocket, data, sizeof(data), 0);
			if(n<0){
				perror("Error fetching page!");
			}
			else cout<<data;
		}
		//close(clientSocket);
    }
}
