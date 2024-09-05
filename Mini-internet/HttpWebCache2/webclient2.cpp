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
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFFER_SIZE 1024

// Function to print SSL errors
void print_ssl_errors() {
    unsigned long err;
    while ((err = ERR_get_error()) != 0) {
        char buf[256];
        ERR_error_string_n(err, buf, sizeof(buf));
        std::cerr << "SSL error: " << buf << std::endl;
    }
}

// Function to make a GET request to the specified URL
void make_get_request(const std::string& url) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Initialize SSL
    SSL_load_error_strings();
    SSL_library_init();
    SSL_CTX *ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!ssl_ctx) {
        std::cerr << "SSL context creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);

    // Enable all available cipher suites
    SSL_CTX_set_cipher_list(ssl_ctx, "ALL");

    // Parse URL to extract domain name
    std::string domain;
    size_t pos = url.find("://");
    if (pos != std::string::npos) {
        pos = url.find("/", pos + 3);
        if (pos != std::string::npos) {
            domain = url.substr(url.find("://") + 3, pos - (url.find("://") + 3));
        } else {
            domain = url.substr(url.find("://") + 3);
        }
    } else {
        pos = url.find("/");
        if (pos != std::string::npos) {
            domain = url.substr(0, pos);
        } else {
            domain = url;
        }
    }

    // Get IP address from domain name
    struct hostent *server = gethostbyname(domain.c_str());
    if (server == NULL) {
        std::cerr << "Cannot resolve host: " << domain << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(443); // HTTPS port
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Establish SSL connection
    SSL *ssl = SSL_new(ssl_ctx);
    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL connection failed" << std::endl;
        print_ssl_errors(); // Print SSL errors
        exit(EXIT_FAILURE);
    }

    // Send GET request
    snprintf(buffer, BUFFER_SIZE, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", domain.c_str());
    SSL_write(ssl, buffer, strlen(buffer));

    // Receive and print server response
    ssize_t bytes_received;
    while ((bytes_received = SSL_read(ssl, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, bytes_received, stdout);
    }

    if (bytes_received < 0) {
        perror("Receive error");
    }

    // Close SSL connection and socket
    SSL_shutdown(ssl);
    close(sockfd);
    SSL_free(ssl);
    SSL_CTX_free(ssl_ctx);
}

int main() {
    std::string url;

    // Input URL from user
    std::cout << "Enter the URL: ";
    std::getline(std::cin, url);

    // Make GET request
    make_get_request(url);

    return 0;
}

