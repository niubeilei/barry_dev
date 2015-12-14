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
// 3/27/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AmClient_AmClient_h
#define Aos_AmClient_AmClient_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

#include "Alarm/Alarm.h"
#include "AmClient/Ptrs.h"

#include "AmClient/AmServer.h"

#include "AmUtil/AmUtil.h"
#include "AmUtil/Ptrs.h"
#include "AmUtil/ReturnCode.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/IpAddr.h"
#include "Util/String.h"
#include "Util/DynArray.h"
//#include "Util/DynArray.h"
#include "Util/AsciiHexHash.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpMsgReader.h"

class AosAmMsg;

struct AosAmClientConnInfo : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eMaxServers = 101
	};

	OmnDynArray<AosAmServerPtr>	mServers;
	OmnDynArray<u32>			mSessionId;

	bool setSessionId(const AosAmServerPtr &server, const u32 sessionId)
	{
		for (int i=0; i<mServers.entries(); i++)
		{
			if (mServers[i] == server)
			{
				mSessionId[i] = sessionId;
				return true;
			}
		}

		if (mServers.entries() > eMaxServers)
		{
			OmnAlarm << "Too many servers" << enderr;
			return false;
		}

		mServers.append(server);
		mSessionId.append(sessionId);
		return true;
	}

	AosAmServerPtr getServer() const
	{
		if (mServers.entries() > 0)
		{
			return mServers[0];
		}

		return 0;
	}

	u32 getSessionId(const AosAmServerPtr &server)
	{
		for (int i=0; i<mServers.entries(); i++)
		{
			if (mServers[i] == server)
			{
				return mSessionId[i];
			}
		}

		return 0;
	}
};

class AosAmTransMutexCondVar : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	u16 				mTransId;
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
//	OmnVList<AosAmMsgPtr>		mMsgQueue;	// receive  message queue
	AosAmMsgPtr 		mMsg;
public:
	AosAmTransMutexCondVar(u16 transId);
	virtual ~AosAmTransMutexCondVar();
};

class AosAmClient : public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum 
	{
		eDefaultServerFailureTimer = 30, 
		eMaxServers = 101, 
		eDefaultTimer = 5
	};

	enum State
	{
		eUnknown,
		eActive,
		eDown,
		eSuspended
	};

	enum SlbMethod
	{
		eRoundRobin,
		eUserPersistent,
		eLeastBusy,
		eFirstAvailable
	};

private: 
	OmnDynArray<AosAmServerPtr>			mServers;
	fd_set 								mReadFds;
	int 								mFdscnt;
	u32									mServerIdx;
	SlbMethod							mSlbMethod;
	OmnMutexPtr							mLock;		// To lock the server connections
	OmnCondVarPtr						mCondVar;	// To lock the server connections
	OmnThreadPtr						mThread;	// To create the retrieve connection
	OmnAsciiHexHash<AosAmClientConnInfoPtr, 2048>	mConnTable;	// set the session Id
//	OmnVList<AosAmMsgPtr>				mMsgQueue;	// receive  message queue
	OmnMutexPtr							mMsgQueueLock;
//	OmnCondVarPtr						mMsgQueueCondVar;
	OmnDynArray<AosAmTransMutexCondVarPtr>	mMsgQueue;/////
//	OmnMutexPtr							mTransSignalsLock;
//	AosMsgQueuePtr 						mMsgQueue;
	u32									mTimer;

public:
	AosAmClient();
	~AosAmClient();

	bool	start();
	bool	stop();
	bool	isGood() const;

	// 
	// ThreadedObj interface
	//
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId){ return true; } //  = 0;
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{return true;}// = 0;

	bool	addServer(const OmnIpAddr &addr, 
					  const u16 port, 
					  const u32 numPorts, 
					  const OmnString &name);
	bool 	userBasedAccess(const OmnString &app,
	                  const OmnString &user,
					  const OmnString &opr,
					  const OmnString &rsc, 
					  AosAmRespCode::E &code, 
					  OmnString &errmsg);
	bool	userBasedAccess(const OmnString &app, 
					  const OmnString &user, 
					  const OmnString &opr, 
					  const OmnString &rsc, 
					  AosAmApiCallback callback);
	bool 	userBasedAccess(const OmnString &user,
					  const OmnIpAddr &addr, 
					  const u16 port, 
					  AosAmRespCode::E &code, 
					  OmnString &errmsg);
	bool	userBasedAccess(const OmnString &user, 
					  const OmnIpAddr &addr, 
					  const u16 port, 
					  AosAmApiCallback callback);
	bool 	authUser(const OmnString &username, 
					  const OmnString &password, 
					  AosAmRespCode::E &respCode,
					  OmnString &errmsg);
	bool    sendMsg(const AosAmMsg &msg, AosAmServerPtr &server);
//	bool	msgRcved(const OmnConnBuffPtr &buff, const u16 transId);
	bool 	receiveResp(AosAmMsgPtr &msg, const AosAmTransMutexCondVarPtr &transSigPtr);

	void 	checkConns();
private:
	AosAmServerPtr selectServer(const OmnString &user);

};
#endif

