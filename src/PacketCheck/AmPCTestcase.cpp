#include "AmPCTestcase.h"

using namespace std;

AosAmPCTestcase::AosAmPCTestcase()
{}

AosAmPCTestcase::~AosAmPCTestcase()
{}

int AosAmPCTestcase::check(char* recvBuffer, int recvLen)
{
	char* tcRecvBuf;
	mTCData->getRecvBuf(tcRecvBuf);
	int tcRecvLen = mTCData->getRecvLength();
	if(recvLen != tcRecvLen)
	{
		return eCheckResult_Failed;
	}
	
	if(memcmp(recvBuffer, tcRecvBuf, tcRecvLen) == 0)
	{
		return eCheckResult_Ok;
	}
	else
	{
		return eCheckResult_Failed;
	}
}

void AosAmPCTestcase::msgRecved(const AosTcpTrafficGenClientPtr &client, 
								  const OmnConnBuffPtr &buff)
{
	int ret = check(buff->getBuffer(), buff->getDataLength());
	
	switch(ret)
	{
		case eCheckResult_Ok:
			cout << "RecvBuffer is correct" << endl;
			break;
		case eCheckResult_Failed:
			cout << "RecvBuffer is incorrect" << endl;
			break;
		default:
			break;
	}
		
}	
							  
void AosAmPCTestcase::connCreated(const AosTcpTrafficGenClientPtr &client, 
								  const OmnTcpClientPtr &conn)
{
	
}
								  
void AosAmPCTestcase::connClosed(const AosTcpTrafficGenClientPtr &client, 
					  const OmnTcpClientPtr &conn)
{
	
}
					  
void AosAmPCTestcase::readyToSend(const AosTcpTrafficGenClientPtr &client, 
					  const char *data, 
					  const int dataLen, 
					  bool &needToSend)
{
	
}
					  
void AosAmPCTestcase::sendFinished(const AosTcpTrafficGenClientPtr &client)
{
	
}

void AosAmPCTestcase::dataSent(const AosTcpTrafficGenClientPtr &client, 
					  const char *data, 
					  const int dataLen)
{
}					  

void AosAmPCTestcase::sendFailed(const AosTcpTrafficGenClientPtr &client,
					  const char *data, 
					  const int dataLen,
					  const OmnRslt &rslt)
{
}

void AosAmPCTestcase::recvFailed(const AosTcpTrafficGenClientPtr &client,
					  const OmnConnBuffPtr &buff, 
					  AosTcpTrafficGen::Action &action)
{
	
}				
	  	
void AosAmPCTestcase::trafficGenFinished(OmnVList<AosTcpTrafficGenClientPtr> &clients)
{

}
