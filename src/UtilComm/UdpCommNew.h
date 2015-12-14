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
// 2013/06/15 Copied from UdpComm.h
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_UdpCommNew_h
#define Omn_UtilComm_UdpCommNew_h

#include "UtilComm/UdpComm.h"

#include "Debug/ErrId.h"
#include "Message/MsgId.h"
#include "Network/Ptrs.h"
#include "Rundata/Ptrs.h"
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
#include "XmlUtil/Ptrs.h"




class AosUdpComm : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnString			mName;
	OmnThreadPtr		mReadingThread;
	OmnIpAddr			mLocalIpAddr;
	int					mLocalPort;
	OmnUdpPtr			mUdpConn;
	OmnCommListenerPtr	mRequester;	
	bool				mThreadStatus;
	AosRundataPtr		mRundata;
	int					mNumRunners;

	// Do not use the following
	AosUdpComm(const AosUdpComm &rhs);
	AosUdpComm & operator = (const AosUdpComm &rhs);

public:
	AosUdpComm(const AosRundataPtr &rdata, 
			   const OmnString &name,
			   const OmnIpAddr &localAddr,
			   const int localPort);
	virtual ~AosUdpComm();

	// OmnThreadedObj interface
	virtual bool		threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool		signal(const int threadLogicId);
    virtual bool 		checkThread(OmnString &errmsg, const int tid) const; 

	bool				config(const AosXmlTagPtr &conf);
	virtual OmnRslt 	reconnect(OmnString &err);
	virtual int			getSock() const;
	virtual bool		isConnGood() const;
	virtual OmnRslt		connect(OmnString &err);
	virtual OmnRslt		closeConn();
	virtual OmnString	toString() const;
	virtual OmnRslt		startReading(const OmnCommListenerPtr &requester);
	virtual OmnRslt		stopReading(const OmnCommListenerPtr &requester);
	virtual OmnRslt		forceStop();

	virtual bool		readFrom(
							const AosRundataPtr &rdata, 
							OmnConnBuffPtr &buff,
							const int timerSec,
							const int timeruSec, 
							bool &isTimeout);

	virtual bool		sendTo(
							const AosRundataPtr &rdata, 
							const char *data, 
							const int length,
							const OmnIpAddr &remoteIpAddr, 
							const int remotePort);

	virtual OmnIpAddr	getLocalIpAddr() const {return mLocalIpAddr;}
	virtual int			getLocalPort() const {return mLocalPort;}
	void				setPort(const int port) {mLocalPort = port;}
};
#endif

