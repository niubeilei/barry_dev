////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TrafficGenTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestDrivers_TrafficGen_TrafficGenTester_h
#define Omn_TestDrivers_TrafficGen_TrafficGenTester_h

#include "UtilComm/TcpTrafficGenListener.h"


class AosTcpTrafficGenTester: public AosTcpTrafficGenListener
{
	OmnDefineRCObject;

public:
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
