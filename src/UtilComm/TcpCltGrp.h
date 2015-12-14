////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpCltGrp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpCltGrp_h 
#define Omn_UtilComm_TcpCltGrp_h 

#include "Porting/Socket.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/ValList.h"
#include "Util/SPtr.h"
#include "Util/Array10.h"
#include "Util/IpAddr.h"
#include "UtilComm/Tcp.h"
#include "UtilComm/Ptrs.h"
#include "Util/RCObjImp.h"


class OmnTcpCltGrp : public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eMaxSock = 1024,
		eDefaultMaxConns = 4000,//50
	};

private:
	OmnVList<OmnTcpClientPtr>	mClients;
	fd_set 						mReadFds; 		  
	fd_set						mWorkingFds;
	int 						mFdscnt;
	OmnTcpCltGrpListenerPtr		mListener;
	OmnThreadPtr				mThread;
	OmnTcpClientPtr				mCtlSock;
	OmnUdpPtr					mReader;
	OmnUdpPtr					mWriter;
	OmnMutexPtr					mLock;
	int							mNumConns;
	int							mMaxConns;
	OmnTcpServerGrpPtr			mServer;
	bool						mIsRight;

public:
	OmnTcpCltGrp();
	~OmnTcpCltGrp();
   
	bool		closeConn();

	void		startReading();
	void		stopReading();
	bool		isStopped();
	void		closeReading();
	bool		isClosed();
	void		setListener(const OmnTcpCltGrpListenerPtr &proc);

	bool		sendTo(const OmnMsgPtr &msg);
	OmnRslt		writeTo(const char *data, const int length, const int sock);
	OmnRslt		readFrom(OmnConnBuffPtr &buf, OmnTcpClientPtr &conn);

	//
	// ThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
    virtual void heartbeat();
    virtual bool checkThread(OmnString &err, const int thrdLogicId) const;

    //
    // E007, 07/13/2003, 2003-0073
    //
    virtual bool checkThread(OmnString &errmsg) const; 
    virtual bool isCriticalThread() const; 

	bool 	addConn(const OmnTcpClientPtr &client);
	bool	removeConn(const OmnTcpClientPtr &conn);
	int		checkConns();
	bool	consistencyCheck(OmnString &rslt);
	void	setServer(const OmnTcpServerGrpPtr &p) ;
	bool	clientClosed(const OmnTcpCltGrpPtr &, const OmnTcpClientPtr &);
	bool	isRight(){return mIsRight;}

private:
	OmnTcpClientPtr		getClient(const int sockId);
	bool 				checkReading(OmnTcpClientPtr &client);
	bool				closeConn(const OmnTcpClientPtr &client);
	OmnTcpClientPtr		getConn(const int theSock);
	bool 				getConnEvent(int &conn_id, OmnTcpClientPtr &client);
	bool 				resetFds();
};
#endif

