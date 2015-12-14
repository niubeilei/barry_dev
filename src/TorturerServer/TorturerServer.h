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
//
// Modification History:
// 01/23/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TorturerServer_TorturerServer_h
#define Aos_TorturerServer_TorturerServer_h
 
#include "Thread/Ptrs.h"
#include "TorturerServer/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util1/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpCltGrpListener.h"

class AosTorturerServer : public OmnTcpCltGrpListener
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxSock = 1000
	};

private:
	OmnString			mName;
	OmnIpAddr			mLocalAddr;
	int					mLocalPort;
	int					mNumLocalPorts;
	OmnMutexPtr			mLock;
	OmnTcpServerGrpPtr	mServer;
	AosTSTransPtr		mTrans[eMaxSock];

public:
	AosTorturerServer(const OmnString &name, 
			const OmnIpAddr &localAddr, 
			const int localPort, 
			const int numLocalPorts); 
	~AosTorturerServer();

	// 
	// TcpListener Interface
	//
    virtual OmnString   getTcpListenerName() const {return mName;}
	virtual void		msgRecved(const OmnTcpCltGrpPtr &group, 
								  const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);
	virtual void		newConn(const OmnTcpClientPtr&);
    virtual bool        connClosed(const OmnTcpCltGrpPtr &, 
                                const OmnTcpClientPtr &client);


	int			removeClient(const AosTSTransPtr &client);
	bool		connect(OmnString &errmsg);
	bool		startReading();
	bool		stopReading();

private:
};

#endif

