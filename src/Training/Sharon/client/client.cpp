#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[])
{ 
    // get server IP address and port number to serve from command line
    if (argc!=3)
    {
		fprintf(stderr, "no address and port\n"); return -1;
    } 
    char * server = argv[1];
    int port_number = atol(argv[2]);

    // Step 1 Look up server to get numeric IP address
    hostent * record = gethostbyname(server);
    if (record==NULL) 
    { 
        herror("gethostbyname failed"); return -1; 
    }
    in_addr * addressptr = (in_addr *) record->h_addr;

    // Step 2 create a socket
    int main_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (main_socket<0) 
    {
        perror("socket creation"); return -1; 
    }

    // Step 3 create a sockaddr_in to describe the server
    sockaddr_in server_info;
    //server_info.sin_len = sizeof(server_info);
    server_info.sin_family = AF_INET;
    server_info.sin_addr = *addressptr;
    server_info.sin_port = htons(port_number);

    // Step 4 connect
    int r = connect(main_socket, (sockaddr *) &server_info, sizeof(server_info));
    if (r<0) 
    {   
		perror("********** Failed to connect"); 
		return -1; 
    }
    printf("Connected to %s\n", inet_ntoa(* addressptr));

	char buff[100] = "Hello world!";
	write(main_socket, buff, strlen(buff));
	int rc = read(main_socket, buff, 100);
	printf("Read something: %d, %s\n", rc, buff);
    close(main_socket); 
    return 0;
}



