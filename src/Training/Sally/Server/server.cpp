#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream.h>

#define INVALID_SOCKET -1 
#define SOCKET_ERROR -1
#define SOCKET int


int procReq(int *sockarray, int n);

// Insert the sock 'sock' into the array 'pSock'. 
// It finds the first unused slot. If found, 
// insert 'sock' into the slot and return true.
// Otherwise (no unused slots, which means there are
// too many active socks), it returns false.
bool InsertSock(int *pSock, int sock) 
{
	int nIndex;
	for(nIndex=0; nIndex < FD_SETSIZE-1; nIndex++)
	{
		if(pSock[nIndex] == INVALID_SOCKET)
		{
			pSock[nIndex] = sock;
			return true;
		}
	}

	printf("********** Warning: no more unused slots!");
	return false;
}


int main()
{
	// Create a socket 
	cout << "To create socket" << endl;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
	{ 
		cout << "socket creation is wrong" << endl; 
		return -1;
	}

	int port = 5010;
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(port);
        
    //bind sin to socket
	cout << "To bind: " << sock << endl;
	int rt1 = bind(sock, (sockaddr * )&sin, sizeof(sin));
    if (rt1 < 0) 
	{ 
		cout << "********** Failed to bind: " << rt1 << endl;
		return -1;
	}
        
    //listening to the port
	cout << "Bind successful, to listen!" << endl;
	int rt2 = listen(sock, 3);
    if (rt2 < 0) 
	{ 
		cout << "*************Listening error!"; 
	} 
	cout << "Listen success: " << rt2 << endl;
	struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
	
	// Init clientsocketarray
	int clientsocketarray[FD_SETSIZE - 1];	
	for(int nIndex=0; nIndex < FD_SETSIZE-1;nIndex++)
	{
		clientsocketarray[nIndex] = INVALID_SOCKET;
	}

	for(;;)
	{
		int clientsock;

		memset((char *) &cli_addr, 0, sizeof(cli_addr));
		cout << "To accept: " << endl;

		clientsock = accept(sock, (struct sockaddr *) &cli_addr, &clilen);

		if (clientsock < 0)
		{	
			cout << "********** Failed to accept: " << clientsock << endl;
			return 0;
		}
		
		cout << "Accept success!  " << endl;
		
		if(!InsertSock(clientsocketarray, clientsock))
		{
			printf("********** the number of client is over %d and connection is failed.\n", 
				FD_SETSIZE);
			close(clientsock);
			continue;
		}
	
		procReq(clientsocketarray, FD_SETSIZE-1);
	}

	return 0;
}


// Process the request. 'sockarray' is an array of socks
// and 'n' is the size of the array.
int procReq(int *sockarray, int n)
{
	static fd_set readFD;

	// Find the max sock
	int maxfd = sockarray[0];
	int i;
	for(i = 0; i < n; i++)
	{
		if(sockarray[i] > maxfd) maxfd = sockarray[i];
	}

	// Set the FD data
	FD_ZERO(&readFD);
	for(i = 0; i < n; i++)
	{
		if(sockarray[i] != INVALID_SOCKET) FD_SET(sockarray[i],&readFD);
	}	
	
	int nRet = select(maxfd+1, &readFD, 0, 0, 0);
	if(nRet < 0)
	{
		cout << "************** Failed to select: " << nRet << endl;
		return -1;
	}

	// For each sock that has contents, we process it.
	const int bufSize = 10000;
	char buff[bufSize];
	for(i = 0; i < n; i++)	
	{	
		if(sockarray[i] != INVALID_SOCKET)
		{
			// read the cientsocket one by one

			if(FD_ISSET(sockarray[i], &readFD))
			{
				nRet = read(sockarray[i], buff, bufSize); 
				if(nRet == 0)
				{
					cout << "End of File and close the port: " << sockarray[i] << endl;
					close(sockarray[i]);
					sockarray[i] = INVALID_SOCKET;
					continue;
				}

				if(nRet < 0) 
				{	
					cout << "********** Failed to read: " << sockarray[i] << endl;
					close(sockarray[i]);
					sockarray[i] = INVALID_SOCKET;
					continue;
				}

				cout << "Read data from port: " << sockarray[i] << endl; 
				cout << "Read data: " << nRet << endl; 
				cout << "Read data: " << buff << endl;
				cout << "\n" << endl;
			}
		}
	}
	return 0;
}
