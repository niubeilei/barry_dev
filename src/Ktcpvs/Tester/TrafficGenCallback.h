////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TrafficGenCallback.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Ktcpvs_Tester_TrafficGenCallback_h
#define Omn_Ktcpvs_Tester_TrafficGenCallback_h

#include "UtilComm/TcpTrafficGenListener.h"

class AosKtcpvsTester;
class AosTrafficGenCallback: public AosTcpTrafficGenListener
{
	OmnDefineRCObject;

private:
	bool			mFinished;
	AosKtcpvsTester	*mTester;

public:
	AosTrafficGenCallback(AosKtcpvsTester *tester)
	:
	mTester(tester)
	{
		mFinished = false;
	}

	bool	isFinished() const {return mFinished;}

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
