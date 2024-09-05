#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

void validate_domain_name(const char *domain_name) {
    FILE *hosts_file = fopen("/etc/hosts", "r");
    if (hosts_file == NULL) {
        perror("Error opening hosts file");
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Read each line from the hosts file
    while ((read = getline(&line, &len, hosts_file)) != -1) {
        // Check if the line contains the domain name
        if (strstr(line, domain_name) != NULL) {
            // Extract the IPv4 address
            char *ip_address = strtok(line, " \t\n");
            if (ip_address != NULL && ip_address[0] != '#') {
                printf("Domain '%s' is present in /etc/hosts\n", domain_name);
                printf("IPv4 Address: %s\n", ip_address);
                free(line);
                fclose(hosts_file);
                return;
            }
        }
    }

    printf("Domain '%s' is not present in /etc/hosts\n", domain_name);
    fclose(hosts_file);
    free(line);
}

int main() {
    const char *domain_name = "localhost";
    validate_domain_name(domain_name);
    return 0;
}

