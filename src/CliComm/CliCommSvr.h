////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCommSvr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_CliComm_CliCommSvr_h
#define Aos_CliComm_CliCommSvr_h
 
#include "CliComm/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/ValList.h"
#include "UtilComm/TcpCltGrpListener.h"
#include "UtilComm/Ptrs.h"


class AosCliCommSvr : public OmnTcpCltGrpListener
{
	OmnDefineRCObject;

private:
	OmnString			mName;
	OmnMutexPtr			mLock;
	OmnIpAddr			mLocalAddr;
	int					mLocalPort;
	int					mNumLocalPorts;
	OmnTcpServerGrpPtr	mServer;
	OmnVList<AosCliSessionPtr>	mSessions;

public:
	AosCliCommSvr(const OmnString &name, 
			   const OmnIpAddr &localAddr, 
			   const int localPort, 
			   const int numLocalPorts);

	virtual ~AosCliCommSvr() {}

	// 
	// TcpListener Interface
	//
    virtual OmnString   getTcpListenerName() const {return mName;}
	virtual void		msgRecved(const OmnTcpCltGrpPtr &group, 
								  const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);
	virtual void		newConn(const OmnTcpClientPtr &conn);

	bool		connect(OmnString &errmsg);
	bool		startReading();
	bool 		connAccepted(const OmnTcpClientPtr &client);

private:
	AosCliSessionPtr	getSession(const OmnTcpClientPtr &client);

};

#endif

