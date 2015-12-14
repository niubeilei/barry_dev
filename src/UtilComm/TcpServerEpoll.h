////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpServerEpoll.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpServerEpoll_h 
#define Omn_UtilComm_TcpServerEpoll_h 

#include "Porting/Socket.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/ValList.h"
#include "Util/SPtr.h"
#include "Util/Array10.h"
#include "Util/IpAddr.h"
#include "UtilComm/Tcp.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/Ptrs.h"
#include "Util/RCObjImp.h"
#include <sys/epoll.h>
#include <vector>
using namespace std;

#define eDefMaxConn 100000 

using namespace std;

class OmnTcpServerEpoll : public OmnTcp,
						  public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxEpollServers = 10
	};

	OmnMutexPtr						mLock;
	OmnTcpListenerPtr				mListener;
	OmnThreadPtr					mThread;

	//Epoll
	int 							mEpollFd;
	epoll_event 					*mEvents;
	OmnTcpClientPtr 				*mConns;
	int 							mMaxConns;

	// Chen Ding, 2013/01/10
	AosTcpEpollReaderPtr			mReader;
	static vector<OmnTcpServerEpollPtr>	smEpollServers;
	bool							mIsBlocking;

public:
	OmnTcpServerEpoll(const OmnIpAddr &localIpAddr,
				 const int localPort,
				 const int numPorts,
				 const OmnString &name,
				 const AosTcpLengthType lt = eAosTLT_LengthIndicator,
				 const u32 maxConn = eDefMaxConn);
	OmnTcpServerEpoll(AosAddrDef &addrDef, 
				 AosPortDef &ports, 
				 const OmnString &name, 
				 const AosTcpLengthType lt,
				 const u32 maxConn = eDefMaxConn);
	OmnTcpServerEpoll(const OmnString &uPath,
				 const OmnString &name,
				 const AosTcpLengthType lt = eAosTLT_LengthIndicator,
				 const u32 maxConn = eDefMaxConn);
	~OmnTcpServerEpoll();
   
	bool 		initEpoll();
	OmnRslt		connect(OmnString &err); 
	OmnRslt		closeConn();
	OmnString	getConnInfo() const;

	void		startReading();
	void		stopReading();
	void		setListener(const OmnTcpListenerPtr &listener);

	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 
	// void		 setConnFlag(const bool connFlag);
	
	OmnRslt closeConn(const OmnTcpClientPtr &client);
	static vector<OmnTcpServerEpollPtr> getEpollServers();
	void printStatus();

	void setBlocking(const bool blocking) { mIsBlocking = blocking; }

private:
	OmnTcpClientPtr		acceptNewConn();
	void addEpollServer();
};
#endif

