#ifndef _Aos_AMPCTESTCASE
#define _Aos_AMPCTESTCASE
#include "PCTestcase.h"

class AosAmPCTestcase : public AosPCTestcase
{
		OmnDefineRCObject;
public:
	AosAmPCTestcase();
	~AosAmPCTestcase();

	virtual int 		check(char* recvBuffer, int recvLen);
	
	virtual void		msgRecved(const AosTcpTrafficGenClientPtr &client, 
								  const OmnConnBuffPtr &buff);
	virtual void		connCreated(const AosTcpTrafficGenClientPtr &client, 
								  const OmnTcpClientPtr &conn);
	virtual void		connClosed(const AosTcpTrafficGenClientPtr &client, 
								  const OmnTcpClientPtr &conn);
	virtual void		readyToSend(const AosTcpTrafficGenClientPtr &client, 
								  const char *data, 
								  const int dataLen, 
								  bool &needToSend);
	virtual void		sendFinished(const AosTcpTrafficGenClientPtr &client);
	virtual void		dataSent(const AosTcpTrafficGenClientPtr &client, 
								  const char *data, 
								  const int dataLen);
	virtual void		sendFailed(const AosTcpTrafficGenClientPtr &client,
								  const char *data, 
								  const int dataLen,
								  const OmnRslt &rslt);
	virtual void		recvFailed(const AosTcpTrafficGenClientPtr &client,
								  const OmnConnBuffPtr &buff, 
								  AosTcpTrafficGen::Action &action);
	virtual void		trafficGenFinished(OmnVList<AosTcpTrafficGenClientPtr> &clients);	  	
};

#endif
