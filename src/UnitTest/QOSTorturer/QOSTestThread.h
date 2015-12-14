////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TrafficGenThread.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UnitTest_QOSTorturer_TrafficGenThread_h
#define Aos_UnitTest_QOSTorturer_TrafficGenThread_h 
 
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpCltGrpListener.h"


class AosTcpTrafficGenThread: public OmnTcpCltGrpListener
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxClient = 1000
	};

	int				mGrpIndex;
	OmnIpAddr		mRemoteAddr;
	int				mRemotePort;
	int				mNumPorts;
	int				mNumClients;
	int				mGroupSize;
	AosTcpTrafficGenClientPtr	mClients[eMaxClient];
	OmnMutexPtr		mLock;
	OmnTcpCltGrpPtr	mGroup;
	AosTcpTrafficGenPtr mTrafficGen;
	AosTcpTrafficGenListenerPtr mListener;

public:
	AosTcpTrafficGenThread(
			const AosTcpTrafficGenPtr &trafficGen, 
			const int grpIndex, 
			const OmnIpAddr &remoteAddr, 
			const int remotePort, 
			const int numPorts, 
			const int groupSize,
			const AosTcpTrafficGenListenerPtr &listener); 

	virtual ~AosTcpTrafficGenThread();

	// 
	// OmnTcpCltGrpListener Interface
	//
    virtual OmnString   getTcpListenerName() const {return "TcpTrafficGenThread";}
    virtual void        msgRecved(const OmnTcpCltGrpPtr &group,
                                  const OmnConnBuffPtr &buff,
                                  const OmnTcpClientPtr &conn);
    virtual void        newConn(const OmnTcpClientPtr &conn);
	virtual bool        connClosed(const OmnTcpCltGrpPtr &group, 
								  const OmnTcpClientPtr &client);

	bool		start();
	bool		exit();
	bool		isExited();
	AosTcpTrafficGenClientPtr	getClient(const OmnTcpClientPtr &conn);
	void		printStatus() const;
	bool		isAllFinished() const;
	int			getClients(OmnVList<AosTcpTrafficGenClientPtr> &clients);
	void		setListener(const AosTcpTrafficGenListenerPtr &listener);
	int			addClient(const AosTcpTrafficGenClientPtr &client);
	OmnTcpCltGrpPtr	getGroup() const;
	u32			getBytesSent() const;
	u32 		getBytesRcvd() const;
	u32 		getTotalConns() const;
	u32 		getFailedConns() const;
	bool		checkConns();
};

#endif

