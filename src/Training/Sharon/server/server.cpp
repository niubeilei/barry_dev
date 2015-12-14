#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

extern int errno;
int procReq(const int session_socket);

int main(int argc, char *argv[])
{ 
  	// get port number to serve from command line
  	if (argc!=2)
  	{ 
		fprintf(stderr, "Missing command arguments. \n"
			"Usage: myprog <port_number>\n"); 
		return -1; 
	}
  	int port_number = atol(argv[1]);
	if (port_number <= 1023 || port_number >= 65535)
	{
		fprintf(stderr, "Incorrect port number. Must be in the range [1024, 65534]");
		return -1;
	}

  	// Step 1 create a socket
  	int main_socket = socket(AF_INET, SOCK_STREAM, 0);
  	if (main_socket<0) 
	{ 
		perror("********** socket creation failed"); 
		return -1; 
	}

  	// Step 2 create a sockaddr_in to describe the service
  	sockaddr_in server_info;
  	server_info.sin_family = AF_INET;
  	server_info.sin_addr.s_addr = htonl(INADDR_ANY);
  	server_info.sin_port = htons(port_number);

  	// Step 3 use bind to set the socket according to the sockaddr_in
  	int r1 = bind(main_socket, (sockaddr *) &server_info, sizeof(server_info));
  	if (r1<0) 
	{ 
		perror("********** Port in use!"); 
		return -1; 
	}

  	// Step 4 use listen to turn the server on
  	int r2 = listen(main_socket, 3);
  	if (r2<0) 
	{ 
		perror("********** Failed to listen!"); 
		return -1; 
	}
  	printf("Listening on port %d\n", port_number);  

  	// Servers usually accept multiple clients, so a loop is used
  	int session_number=0;
  	while (1)
  	{ 
   		// Step 5 set up another sockaddr_in to receive information on the client
   		sockaddr_in client_info;
   		unsigned int client_info_size = sizeof(client_info);
    
    	// Step 6 use accept to wait for and accept the next client
    	int session_socket = accept(main_socket, (sockaddr *) &client_info, &client_info_size);
    	if (session_socket<0)
    	{ 
			if (errno==EINTR) continue;  // EINTR is not a problem
      		perror("accept");
      		usleep(100000); 
            // Other errors are probably a reason to stop, but for a robust server
            // just give the error a chance to go away and try again
      		continue; 
		}

    	session_number++;

   		// accept filled in the provided sockaddr_in with information on the client
   		char * who_is_it = inet_ntoa(client_info.sin_addr);
   		printf("[connection %d accepted from %s]\n", session_number, who_is_it);
 
    	// At this point, a heavy-duty server would use fork to create a sub-process, and
    	// let the sub-process deal with the client, so we can wait for other clients.
		procReq(session_socket);
	}
}


int procReq(const int session_socket)
{
   	// Step 7 (Optional) Create normal files for communication with the client
   	//        Or just use read and write directly on the session_socket itself
	// Read something from the connection
	char buff[1000]; 
	int rc = read(session_socket, buff, 1000);
	if (rc < 0)
	{
		fprintf(stderr, "********** Failed to read: %d\n", rc);
	}
	else if (rc == 0)
	{
		// Received EOF. Do nothing
	}
	else
	{
		// Read something. print it.
		buff[rc] = 0;
		printf("Read something: %s\n", buff);
		write(session_socket, buff, rc);
		printf("Sent the data: %s\n", buff);
	}
	return 0;
}




