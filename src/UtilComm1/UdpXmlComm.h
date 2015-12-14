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
// 03/10/2012 Copied from UdpCommNew.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm1_UdpXmlComm_h
#define Omn_UtilComm1_UdpXmlComm_h

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
#include "UtilComm1/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosUdpXmlComm : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eReadFailIntervalTimerSec = 3
	};

	OmnMutexPtr			mLock;
	OmnThreadPtr		mReadingThread;
	OmnString			mName;
	OmnIpAddr			mLocalAddr;
	int					mLocalPort;
	OmnUdpPtr			mUdpConn;
	u64					mSeqno;

	AosUdpXmlCommListenerPtr	mListener;	

	// Do not use the following
	AosUdpXmlComm(const AosUdpXmlComm &rhs);
	AosUdpXmlComm & operator = (const AosUdpXmlComm &rhs);

public:
	AosUdpXmlComm(
				const OmnString &localAddr,
			   	const int port, 
			   	const OmnString &name);
	virtual ~AosUdpXmlComm();

	// Implement OmnThreadedObj interface
	virtual bool		threadFunc(OmnThrdStatus::E &state, 
								   const OmnThreadPtr &thread);
	virtual bool		signal(const int threadLogicId);
    virtual bool 		checkThread(OmnString &errmsg, const int tid) const; 

	int			getSock() const;
	OmnString	toString() const;
	bool 		startReading(const AosUdpXmlCommListenerPtr &requester);
	bool 		stopReading(const AosUdpXmlCommListenerPtr &requester);
	bool 		forceStop();
	OmnIpAddr	getLocalIpAddr() const {return mLocalAddr;}
	int			getLocalPort() const {return mLocalPort;}
	void		setLocalPort(const int port) {mLocalPort = port;}
	bool		isConnGood() const;
	bool		closeConn();
	bool		connect(OmnString &errmsg);
	bool		reconnect(OmnString &errmsg);

	bool readFrom(		AosXmlTagPtr &msg,
						const int timerSec,
						const int timeruSec, 
						bool &isTimeout, 
						OmnIpAddr &remote_addr,
						int &remote_port);

	bool sendTo(		const AosXmlTagPtr &msg,
						const OmnIpAddr &remoteIpAddr, 
						const int remotePort);
};
#endif

