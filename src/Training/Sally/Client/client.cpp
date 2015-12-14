#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>     
#include <iostream.h>

int main()
{ 
	
    static const char * host_name = "127.0.0.1";
	struct hostent * server_host_name;
	server_host_name = gethostbyname(host_name);

	int port = 5010;
	struct sockaddr_in pin;
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = htonl(INADDR_ANY);
	pin.sin_port = htons(port);

	int socket_descriptor;
	socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	int rc = connect(socket_descriptor, (sockaddr *)&pin, sizeof(pin));
	if (rc < 0)
	{
		cout << "********** Failed to connect: " << rc << endl;
		return -1;
	}

	cout << "Connected!" << endl;

	// Send something
	write(socket_descriptor, "Sharon", 6);

	
//	close(socket_descriptor); 
//	return 0;
}
