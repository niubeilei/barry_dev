////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpCommSvr.h
// Description:
//   
//
// Modification History:
// 		The definition of OmnTcpCommSvr is not good. No one is using
// 		it yet. Comment out everything. Will rework on it when needed. CD.
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_TcpCommSvr_h
#define Omn_UtilComm_TcpCommSvr_h

#include "Debug/Rslt.h"
#include "Message/MsgId.h"
#include "Message/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"




class OmnTcpCommSvr : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnTcpServerPtr		mTcpServer;
	OmnString			mName;
	int					mNIID;
	OmnMsgId::E			mProtocol;
	OmnThreadPtr		mThread;
	OmnTcpListenerPtr	mListener;
	OmnIpAddr			mLocalAddr;
	int					mLocalPort;
	int					mNumLocalPorts;

public:
	OmnTcpCommSvr(const OmnIpAddr &localAddr, 
				  const int localPort, 
				  const int num_ports,
				  const OmnString &name);
	~OmnTcpCommSvr();

	OmnRslt		readFrom(OmnConnBuffPtr &buff, OmnTcpClientPtr &client);
	OmnRslt		sendTo(const OmnMsgPtr &msg, const OmnTcpClientPtr &);
	OmnRslt		sendTo(const char *, const int, const int sock);

	OmnRslt 	startReading(const OmnTcpListenerPtr &callback);
	OmnRslt 	stopReading(const OmnTcpListenerPtr &callback);

    virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    virtual bool signal(const int threadLogicId);
    virtual void heartbeat();
    virtual bool checkThread(OmnString &err, const int thrdLogicId) const;
    virtual bool isCriticalThread() const; 

	OmnString	toString() const;

	virtual OmnIpAddr	getLocalIpAddr() const;
	virtual int			getLocalPort() const;
	virtual bool		isConnGood() const;
	virtual OmnRslt		connect(OmnString &err);
	virtual OmnRslt		reconnect(OmnString &err);
	virtual OmnRslt		closeConn();

	virtual OmnMsgId::E	getProtocol() const {return mProtocol;}
	virtual OmnRslt		forceStop();
};
#endif

