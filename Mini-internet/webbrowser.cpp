/*
Implement a basic HTTP, Web cache that stores recently accessed web pages to improve
retrieval speed for subsequent requests. The cache should follow a least-recently-used (LRU)
eviction policy. The cache will store web pages retrieved using HTTP GET requests.
Requirements:
● Implement a cache data structure that can store a fixed number of web pages (e.g.,
5 pages) in memory.
● Implement a function to retrieve a web page from the cache. If the requested page
is present in the cache, return the cached content. If not, fetch the page using an
HTTP GET request, store it in the cache (evicting the least recently used page if
necessary), and return the content.
● Use sockets to perform the HTTP GET request and receive the web page content.
● Use a linked list to keep track of the order of page accesses. When a page is
accessed, move it to the front of the list to represent it as the most recently used
page.
● Implement a function to display the contents of the cache, showing the order of
page accesses from most to least recently used.

Hints:
● Use socket programming to establish a connection with the web server, send an
HTTP GET request, and receive the web page content.
● Use a linked list to represent the cache, where each node contains the URL and
content of a web page.
● Example web page url that you can provide for testing:
http://quietsilverfreshmelody.neverssl.com/online/
*/

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <unordered_map>

#define BUFFER_SIZE 1024
#define CACHE_SIZE 4

using namespace std;

class LRUCache {
    struct Node{
        string url,content;
        Node* next=NULL,*prev=NULL;
        Node(string url,string content,Node* prev=NULL,Node* next=NULL) {
            this->url=url;
            this->content=content;
            this->prev=prev;
            this->next=next;
        }
    };
    Node* head=NULL, *tail=NULL;
    unordered_map<string,Node*> nodemap;
    int size=0,cap;
public:
    LRUCache(int capacity=CACHE_SIZE) : cap(capacity) {
        
    }
    pair<bool,string> get(string url) {
        if(nodemap.find(url)==nodemap.end()) return {false,""};
        put(url,nodemap[url]->content);
        return {true,nodemap[url]->content};
    }
    void printcache(){
        Node* n=head;
        cout<<"\nCache content:\n";
        while(n){
            cout<<n->url<<endl;
            n=n->next;
        }
        cout<<endl;
    }
    void put(string url, string content) {
        if(!head){
            head = new Node(url,content);
            nodemap[url]=head;
            size++; tail=head;
            return;
        }
        if(nodemap.find(url)!=nodemap.end()){
            Node* tmp=nodemap[url],*prev=tmp->prev;
            if(!prev){ head->content=content; return; }
            if(tmp==tail) tail=prev;
            if(!tail) head=NULL;
            else if(tail!=prev){ prev->next=tmp->next; prev->next->prev=prev; }
            else prev->next=NULL;
            delete(tmp);
            nodemap.erase(url); size--;
            put(url,content);
        }
        else{
            if(size<cap){
                Node* tmp=head;
                head=new Node(url,content);
                head->next=tmp;
                tmp->prev=head;
                nodemap[url]=head;
                size++;
            }
            else{
                nodemap.erase(tail->url);
                Node *temp=tail; tail=tail->prev;
                delete(temp);
                if(!tail){
                    head=NULL; size--;
                    put(url,content);
                    return;
                }
                tail->next=NULL;
                size--;
                put(url,content);
            }
        }   
    }
} WebCache;

void URLParser(const std::string& url,std::string& domain,std::string& path,int& port) {
	size_t pos = url.find("://");
    
    if (pos != std::string::npos) {
    	if(url.find(":", pos + 3)!=std::string::npos){
    		pos = url.find(":",pos+3);
    		domain = url.substr(url.find("://") + 3, pos - (url.find("://") + 3));
            if(url.find("/", pos + 1) != std::string::npos){ 
            	port = stoi(url.substr(pos+1,url.find("/", pos + 1) - (pos+1))); 
	            path = url.substr(url.find("/",pos+1)+1);
	        }
	        else{
	        	port = stoi(url.substr(pos+1));
	        	path="/";
	        }
    	}
    	else{
		    pos = url.find("/", pos + 3);
		    if (pos != std::string::npos) {
		        domain = url.substr(url.find("://") + 3, pos - (url.find("://") + 3));
		        path = url.substr(pos);
		    } else {
		        domain = url.substr(url.find("://") + 3);
		        path = "/";
		    }
        }
    } else {
    	if(url.find(":")!=std::string::npos){
    		pos = url.find(":");
    		domain = url.substr(0, pos);
            if(url.find("/", pos + 1) != std::string::npos){ 
            	port = stoi(url.substr(pos+1,url.find("/", pos + 1) - (pos+1))); 
	            path = url.substr(url.find("/",pos+1)+1);
	        }
	        else{
	        	port = stoi(url.substr(pos+1));
	        	path="/";
	        }
    	}
    	else{
		    pos = url.find("/");
		    if (pos != std::string::npos) {
		        domain = url.substr(0, pos);
		        path = url.substr(pos);
		    } else {
		        domain = url;
		        path = "/";
		    }
        }
    }
}

// Function to make a GET request to the specified URL
void HttpGETrequest(const std::string& url) {
    auto cache_output = WebCache.get(url);
    if(cache_output.first == true){
    	cout<<cache_output.second;
    	WebCache.printcache();
    	return;
    }
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Parse URL to extract domain name and path
    std::string domain, path;
    int port=80;
	
	URLParser(url,domain,path,port);
	
    // Get IP address from domain name
    struct hostent *server = gethostbyname(domain.c_str());
    if (server == NULL) {
        std::cerr << "Cannot resolve host: " << domain << std::endl;
        //WebCache.printcache();
        return;
        //exit(EXIT_FAILURE);
    }

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        //WebCache.printcache();
        return;
        //exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port); // HTTP port
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        //WebCache.printcache();
        return;
        //exit(EXIT_FAILURE);
    }

    // Send GET request
    snprintf(buffer, BUFFER_SIZE, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path.c_str(), domain.c_str());
    send(sockfd, buffer, strlen(buffer), 0);

    // Receive and print server response
    ssize_t bytes_received;
    std::string response_content;
    while ((bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, stdout);
        response_content.append(buffer, bytes_received);
    }
	//cout<<response_content<<endl;
	
    if (bytes_received < 0) {
        perror("Receive error");
    }
	else	WebCache.put(url,response_content);
	
    // Close socket
    close(sockfd);
    WebCache.printcache();
}

int main(int argc,char* argv[]) {
    int cache = (argc==2 ? stoi(argv[1]) : 4);
    WebCache = LRUCache(cache);
    
    std::string url;

    // Input URL from user
    while(true){
    	std::cout << "Enter the URL: ";
    	std::getline(std::cin, url);
		if(!url.compare("exit")) break;
    	// Make GET request
    	HttpGETrequest(url);
	}
    return 0;
}
