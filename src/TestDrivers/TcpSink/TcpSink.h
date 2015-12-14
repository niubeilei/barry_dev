////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpSink.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_TestDriver_TcpSink_TcpSink_h
#define Aos_TestDriver_TcpSink_TcpSink_h
 
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/Ptrs.h"


class AosTcpSink : public OmnTcpListener
{
	OmnDefineRCObject;

private:
	OmnString		mName;
	OmnIpAddr		mLocalAddr;
	int				mLocalPort;
	int				mNumLocalPorts;
	OmnTcpServerPtr	mServer;

public:
	AosTcpSink(const OmnString &name, 
			   const OmnIpAddr &localAddr, 
			   const int localPort, 
			   const int numLocalPorts);

	virtual ~AosTcpSink() {}

	// 
	// TcpListener Interface
	//
    virtual OmnString   getTcpListenerName() const {return mName;}
    virtual void        msgRecved(const OmnConnBuffPtr &buff,
                                  const OmnTcpClientPtr &conn);

	bool		connect(OmnString &errmsg);
	bool		startReading();

};

#endif

