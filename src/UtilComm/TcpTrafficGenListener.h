////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpTrafficGenListener.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpTrafficGenListener_h
#define Omn_UtilComm_TcpTrafficGenListener_h

#include "Util/RCObject.h"
#include "Util/String.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpTrafficGen.h"


class OmnRslt;
class AosTcpTrafficGenListener : public virtual OmnRCObject
{
public:
	virtual void		msgRecved(const AosTcpTrafficGenClientPtr &client, 
								  const OmnConnBuffPtr &buff) = 0;
	virtual void		connCreated(const AosTcpTrafficGenClientPtr &client, 
								  const OmnTcpClientPtr &conn) = 0;
	virtual void		connClosed(const AosTcpTrafficGenClientPtr &client, 
								  const OmnTcpClientPtr &conn) = 0;
	virtual void		readyToSend(const AosTcpTrafficGenClientPtr &client, 
								  const char *data, 
								  const int dataLen, 
								  bool &needToSend) = 0;
	virtual void		sendFinished(const AosTcpTrafficGenClientPtr &client) = 0;
	virtual void		dataSent(const AosTcpTrafficGenClientPtr &client, 
								  const char *data, 
								  const int dataLen) = 0;
	virtual void		sendFailed(const AosTcpTrafficGenClientPtr &client,
								  const char *data, 
								  const int dataLen,
								  const OmnRslt &rslt) = 0;
	virtual void		recvFailed(const AosTcpTrafficGenClientPtr &client,
								  const OmnConnBuffPtr &buff, 
								  AosTcpTrafficGen::Action &action) = 0;
	virtual void		trafficGenFinished(OmnVList<AosTcpTrafficGenClientPtr> &clients) = 0;
};
#endif
