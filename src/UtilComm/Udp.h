////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Udp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_Udp_h
#define Omn_UtilComm_Udp_h

#include "Debug/ErrId.h"
#include "Thread/Ptrs.h"
#include "Util/SPtr.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Conn.h"
#include "Util/RCObjImp.h"

class OmnMutex;
class OmnConnBuffer;
class OmnIpAddr;
class OmnSysObj;


class OmnUdp : public OmnConn
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxUdpBufferSize = 10000
	};

	//
	// Class static member data
	//
private:
	static int			csMaxSelectTry;
	static int			csSelectTryIntervalSec;

	int					mSock;
	OmnMutexPtr			mLock;
	OmnIpAddr			mLocalIpAddr;
	int					mLocalPort;
	OmnString			mName;
	bool				mLastReadTimeout;
	bool				mDebug;

	// Do not use these two
	OmnUdp(const OmnUdp& rhs);
	OmnUdp& operator = (const OmnUdp& rhs);

public:
	OmnUdp(const OmnString &name, 
		const OmnIpAddr &localIpAddr, 
		const int localPort);
	virtual ~OmnUdp();

	virtual bool		isConnGood() const;
	virtual bool		closeConn();
	virtual OmnRslt		connect(OmnString &);
	virtual OmnRslt		reconnect(OmnString &);
	bool				isLastReadTimeout() const {return mLastReadTimeout;}

	virtual ConnType	getConnType() const {return eUdp;}
	virtual OmnString	toString() const;

	virtual OmnString	getName() const {return mName;}
	virtual int			getLocalPort() const {return mLocalPort;}

	virtual int			getSock() const {return mSock;}
	virtual void		setDebug(const bool flag) {mDebug = flag;}
	virtual bool		hasPacketToRead();

	virtual	OmnIpAddr	getLocalIpAddr() const {return mLocalIpAddr;}

	OmnRslt				sendTo(const OmnSysObj &so, 
								const OmnIpAddr &remoteIp,
								const int remotePort);

	OmnRslt				readFrom(OmnConnBuffPtr &buff,
								const int timerSec,
								const int timeruSec, 
								bool &isTimeout);

	OmnRslt				writeTo(const char *data, 
								const int dataLength,
								const OmnIpAddr &remoteIpAddr, 
								const int remotePort);
private:

};
#endif
