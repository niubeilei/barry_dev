////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 03/10/2012 Copied from UdpComm.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm1_UdpCommNew_h
#define Omn_UtilComm1_UdpCommNew_h

#include "Debug/ErrId.h"
#include "Message/MsgId.h"
#include "Network/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/Ptrs.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommProt.h"
#include "UtilComm/Comm.h"
#include "XmlParser/Ptrs.h"



class AosUdpCommNew : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eReadFailIntervalTimerSec = 3
	};

	OmnThreadPtr		mReadingThread;
	OmnString			mName;
	OmnIpAddr			mLocalAddr;
	int					mLocalPort;
	OmnUdpPtr			mUdpConn;
	OmnCommListenerPtr	mRequester;	

	// Do not use the following
	AosUdpCommNew(const AosUdpCommNew &rhs);
	AosUdpCommNew & operator = (const AosUdpCommNew &rhs);

public:
	AosUdpCommNew(const OmnString &localAddr,
			   const int port, 
			   const OmnString &name);
	virtual ~AosUdpCommNew();

	// Writes
	bool readFrom(OmnConnBuffPtr &buff,
							const int timerSec,
							const int timeruSec, 
							bool &isTimeout);
	bool sendTo(const char *data, const int length,
							const OmnIpAddr &remoteIpAddr, 
							const int remotePort);
	inline bool sendTo(const OmnString &data, 
				const OmnString &remote_addr, 
				const int remote_port)
	{
		return sendTo(data.data(), data.length(), remote_addr, remote_port);
	}

	int		getSock() const;
	OmnString	toString() const;
	bool startReading(const OmnCommListenerPtr &requester);
	bool stopReading(const OmnCommListenerPtr &requester);
	bool forceStop();

	// Implement OmnThreadedObj interface
	virtual bool		threadFunc(OmnThrdStatus::E &state, 
								   const OmnThreadPtr &thread);
	virtual bool		signal(const int threadLogicId);
    virtual bool 		checkThread(OmnString &errmsg, const int tid) const; 

	OmnIpAddr	getLocalIpAddr() const {return mLocalAddr;}
	int			getLocalPort() const {return mLocalPort;}
	void		setLocalPort(const int port) {mLocalPort = port;}
	bool		isConnGood() const;
	bool		closeConn();
	bool		connect(OmnString &errmsg);
	bool		reconnect(OmnString &errmsg);
};
#endif

