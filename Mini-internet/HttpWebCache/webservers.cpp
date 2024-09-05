#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include <unordered_set>

#define WEB_SERVERS 1
#define MAX_CLIENTS 10

#define SERVER_IP "192.168.0.119"

using namespace std;

/*struct RequestNode{
	struct RequestNode *prev,*next;
}
class server{
	string ip,domain_name;
	unordered_set<string> paths;
	priority_queue<pair<int,HttpRequest>> requests;
	server(string ip) : ip(ip) {}
};

vector<server> servers;*/
/*void server_app(string ip){

}*/
/*void* resource_handler(void* arg){
	int i=(int*)arg;
	server calling_server=servers[i];
	auto request=calling_server.requests;
	int clientSocket=requests.first; HttpRequest req=requests.second;
	while(1){
		if(!requests.empty()){
			HttpRequest
}*/
int serverSocket,serverSocket1;	//global array of server socketfds

//pid_t pid;

void handleInterrupt(int signum){
    // This is the parent process
    /*pid_t ppid=getppid();
    if(ppid==pid){
    	close(serverSocket1);
		cout<<"\nServer1 is closed."<<endl;
		exit(EXIT_SUCCESS);
    }*/
    int status;
    for (int i = 0; i < WEB_SERVERS; ++i) {
    	wait(&status);
    }
    close(serverSocket);
	cout<<"\nLoadbalancer is closed.";
	close(serverSocket1);
	cout<<"\nServer1 is closed."<<endl;
    exit(signum);
}
/*void handleInterruptChild(int signum){
	close(serverSocket1);
	cout<<"\nServer1 is closed."<<endl;
	exit(EXIT_SUCCESS);
}*/
int main(){
	
	//signal(SIGINT,handleInterrupt);
	//pid=getpid();
		
    struct sockaddr_in clientAddr;
        
	/*cout<<"Sorry, the requested site not found.\n\n";
	cout<<"Error 404, the requested URL /sexx not found on this server.\n\n";*/
		
	int status;
	
	for (int i = 0; i < WEB_SERVERS; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {     // Child process	 //each child process represents a server. Each server has two threads. One thread handles incoming http client connections, push the request and client socketfds into queue and the other thread serves the resource which might sleep for a few moment simulating resource fetching.. 
            // Start server in child process
            /*char ip_address[16]; // Assuming IPv4
            snprintf(ip_address, sizeof(ip_address), "127.0.0.%d", i + 1);
            start_server(ip_address, PORT_BASE + i);
            */
            //signal(SIGINT,handleInterruptChild);
            cout<<"Server Process "<<(i+1)<<endl;
            if(i==0){
            	//int serverSocket;
            	struct sockaddr_in serverAddr, clientAddr;
    			socklen_t addrSize = sizeof(struct sockaddr_in);
    
    			serverSocket1 = socket(AF_INET, SOCK_STREAM, 0);
    			if (serverSocket1 == -1)
   	 			{
   	 				char error_message[100];

    				snprintf(error_message, sizeof(error_message), "Server%d: Error creating socket", i+1);

        			perror(error_message);
        			exit(EXIT_FAILURE);
    			}
    			// Set up server address struct
    			memset(&serverAddr, 0, sizeof(serverAddr));
    			serverAddr.sin_family = AF_INET;
    			serverAddr.sin_addr.s_addr = inet_addr("192.168.0.120");
    			serverAddr.sin_port = htons(8080);

    			int reuse = 1;
    			#ifdef SO_REUSEADDR
    			if (setsockopt(serverSocket1, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        			perror("setsockopt(SO_REUSEADDR) failed");
    			#endif
    			#ifdef SO_REUSEPORT
    			if (setsockopt(serverSocket1, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
        			perror("setsockopt(SO_REUSEPORT) failed");
    			#endif
    			
    			// Bind the socket to the specified address and port
	    		if (bind(serverSocket1, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    			{
    				char error_message[100];

    				snprintf(error_message, sizeof(error_message), "Server%d: Error binding socket", i+1);

    	    		perror(error_message);
    	    		exit(EXIT_FAILURE);
    			}

    			// Listen for incoming connections
    			if (listen(serverSocket1, MAX_CLIENTS) == -1)
    			{
    				char error_message[100];

    				snprintf(error_message, sizeof(error_message), "Server%d: Error listening for connections", i+1);

    	    		perror(error_message);
    	    		exit(EXIT_FAILURE);
    			}
    			cout<<"Server"<<(i+1)<<": listening on ipv4 192.168.0.120 port number "<<8080<<endl;
    			while(1){
    			int clientSocket = accept(serverSocket1, (struct sockaddr *)&clientAddr, &addrSize);
    			if(clientSocket==-1){
    				char error_message[100];

    				snprintf(error_message, sizeof(error_message), "Server%d: Error accepting connection", i+1);
    				
    				perror(error_message);
				    exit(EXIT_FAILURE);
				}
				cout<<"Server"<<(i+1)<<": 1 client connected to server. id: "<<clientSocket<<endl;
				char mssg[50];
				snprintf(mssg, sizeof(mssg), "Welocome to web server%d !\n", i+1);
				sleep(10);
				send(clientSocket,mssg,sizeof(mssg),0);
				close(clientSocket);
				}
            }
            exit(EXIT_SUCCESS);
        }
        else{
        	signal(SIGINT,handleInterrupt);
        	//sleep(1);
        	//int serverSocket;
            struct sockaddr_in serverAddr;
    		socklen_t addrSize = sizeof(struct sockaddr_in);
    
    		serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    		if (serverSocket == -1)
   	 		{
        		perror("Loadbalancer: Error creating socket");
        		exit(EXIT_FAILURE);
    		}
    		// Set up server address struct
    		memset(&serverAddr, 0, sizeof(serverAddr));
    		serverAddr.sin_family = AF_INET;
    		serverAddr.sin_addr.s_addr = inet_addr("192.168.0.119");
    		serverAddr.sin_port = htons(8080);
    		
    		int reuse = 1;
    		#ifdef SO_REUSEADDR
    		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        		perror("setsockopt(SO_REUSEADDR) failed");
    		#endif
    		#ifdef SO_REUSEPORT
    		if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
        		perror("setsockopt(SO_REUSEPORT) failed");
    		#endif
    		// Bind the socket to the specified address and port
    		if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    		{
        		perror("Loadbalancer: Error binding socket");
        		exit(EXIT_FAILURE);
    		}

    		// Listen for incoming connections
    		if (listen(serverSocket, MAX_CLIENTS) == -1)
    		{
        		perror("Loadbalancer: Error listening for connections");
        		exit(EXIT_FAILURE);
    		}
    		cout<<"Loadbalancer: listening on ipv4 192.168.0.119 port number "<<8080<<endl;
    		while(1){
    		int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
    		if(clientSocket==-1){
    			perror("Loadbalancer: Error accepting connection");
			    exit(EXIT_FAILURE);
			}
			cout<<"Loadbalancer: 1 client connected to proxy server. id: "<<clientSocket<<endl;
			
			//char buff[50];
			//int n=recv(clientSocket,mssg,sizeof(mssg),0);
			//if(n>0){
				char mssg[50]="Welcome to proxy server !\n";
    			int n=send(clientSocket,mssg,sizeof(mssg),0);
				//if(n>0){
				//cout<<"yes";
				close(clientSocket);
				//}
				//else cout<<"no";
			}
        }
    }
    
    return 0;
}
