#include "UtilComm/TcpServer.h"

class OmnMyListener : public OmnTcpListner 
{
public:
	virtual bool	msgRecved(const OmnConnBuffPtr &buff, const OmnTcpClientPtr &client)
	{
		cout << "Message received from: " << client->getSock()
			<< ". Data length: " << buff->getDataLength()
			<< ". Data contents: " << buff->getData() << endl;
		return true;
	}
};


int startServer()
{
	OmnTcpServer server(OmnIpAddr("127.0.0.1"), 5000, 5, "chen");

	OmnString errmsg;
	if (!server.connect(errmsg))
	{
		OmnAlarm << "Failed to bind: " << errmsg << enderr;
		return -1;
	}

	OmnConnBuffPtr buff;
	OmnTcpClientPtr client;
	while (1)
	{
		if (server.anyToRead())
		{
			server.readFrom(buff, client); // new char[xxx];

			cout << "Read data from: " << client->getSock()
				<< ". Data length: " << buff->getDataLength()
				<< ". Data contents: " << buff->getData() << endl;
		}
	}

	// Passive
	OmnMyListener listener;

	OmnMyListenerPtr thePtr(this, false);
	server.setListener(thePtr);


	server.startReading();

	return 0;
}
	


