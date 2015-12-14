////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpComm.h
// Description:
//	This class maintains one local port. All communications
//  go through this local port.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_UdpComm_h
#define Omn_UtilComm_UdpComm_h


#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/Ptrs.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Comm.h"
#include "XmlUtil/Ptrs.h"


class OmnUdpComm : public OmnComm,
				   public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnThreadPtr		mReadingThread;

	OmnIpAddr			mLocalIpAddr;
	int					mLocalPort;
	OmnIpAddr			mRemoteIpAddr;
	int					mRemotePort;

	OmnUdpPtr			mUdpConn;
	OmnCommListenerPtr	mRequester;	

	// Do not use the following
	OmnUdpComm(const OmnUdpComm &rhs);
	OmnUdpComm & operator = (const OmnUdpComm &rhs);

public:
	OmnUdpComm(const OmnCommProt::E protocol, 
			   const bool proxySenderAddr,
			   const OmnXmlItemPtr &def,
			   const int targetId, 
			   const OmnNetEtyType::E targetType,
			   const OmnCommGroupPtr &comm);
	OmnUdpComm(const OmnString &name,
			   const OmnCommProt::E protocol,
			   const bool proxySenderAddr,
			   const OmnIpAddr &localAddr,
			   const int localPort,
			   const OmnCommGroupPtr &comm);
	OmnUdpComm(const OmnIpAddr &localAddr,
			   const int port, 
			   const OmnString &name);
	virtual ~OmnUdpComm();

	OmnRslt				config(const OmnXmlItemPtr &def);
	bool				config(const AosXmlTagPtr &conf);
	virtual OmnRslt		readFrom(
							OmnConnBuffPtr &buff,
							const int timerSec,
							const int timeruSec, 
							bool &isTimeout);
	virtual OmnRslt		sendTo(
							const char *data,
							const int length,
							const OmnIpAddr &remoteIpAddr, 
							const int remotePort);
	virtual OmnRslt		reconnect(OmnString &err);
	virtual int			getSock() const;

	virtual bool		isConnGood() const;
	virtual OmnRslt		connect(OmnString &err);
	virtual OmnRslt		closeConn();
	virtual OmnString	toString() const;

	virtual OmnRslt		startReading(const OmnCommListenerPtr &requester);
	virtual OmnRslt		stopReading(const OmnCommListenerPtr &requester);
	virtual OmnRslt		forceStop();

	// OmnThreadedObj interface
	virtual bool		threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool		signal(const int threadLogicId);
    virtual bool 		checkThread(OmnString &errmsg, const int tid) const; 

	virtual OmnIpAddr	getLocalIpAddr() const {return mLocalIpAddr;}
	virtual int			getLocalPort() const {return mLocalPort;}
	void				setPort(const int port) {mLocalPort = port;}

	OmnIpAddr			getRemoteAddr() const {return mRemoteIpAddr;}
	int					getRemotePort() const {return mRemotePort;}
	OmnRslt				sendBuff(const OmnIpAddr &recvAddr, 
								 const int recvPort, 
								 const OmnString &buff);
};
#endif

