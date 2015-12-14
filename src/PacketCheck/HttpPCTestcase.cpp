#include "HttpPCTestcase.h"

AosHttpPCTestcase::AosHttpPCTestcase()
{}

AosHttpPCTestcase::~AosHttpPCTestcase()
{}

int AosHttpPCTestcase::check(char* recvBuffer, int recvLen)
{
	return 0;
}

void AosHttpPCTestcase::msgRecved(const AosTcpTrafficGenClientPtr &client, 
								  const OmnConnBuffPtr &buff)
{
	
}	
							  
void AosHttpPCTestcase::connCreated(const AosTcpTrafficGenClientPtr &client, 
								  const OmnTcpClientPtr &conn)
{
	
}
								  
void AosHttpPCTestcase::connClosed(const AosTcpTrafficGenClientPtr &client, 
					  const OmnTcpClientPtr &conn)
{
	
}
					  
void AosHttpPCTestcase::readyToSend(const AosTcpTrafficGenClientPtr &client, 
					  const char *data, 
					  const int dataLen, 
					  bool &needToSend)
{
	
}
					  
void AosHttpPCTestcase::sendFinished(const AosTcpTrafficGenClientPtr &client)
{
	
}

void AosHttpPCTestcase::dataSent(const AosTcpTrafficGenClientPtr &client, 
					  const char *data, 
					  const int dataLen)
{
}					  

void AosHttpPCTestcase::sendFailed(const AosTcpTrafficGenClientPtr &client,
					  const char *data, 
					  const int dataLen,
					  const OmnRslt &rslt)
{
}

void AosHttpPCTestcase::recvFailed(const AosTcpTrafficGenClientPtr &client,
					  const OmnConnBuffPtr &buff, 
					  AosTcpTrafficGen::Action &action)
{
	
}				
	  	
void AosHttpPCTestcase::trafficGenFinished(OmnVList<AosTcpTrafficGenClientPtr> &clients)
{
}
