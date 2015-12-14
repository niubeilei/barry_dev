////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 06/16/2011	Created by Michael Yang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_InstantMsg_MsgSvrOnlineMgr_h
#define AOS_InstantMsg_MsgSvrOnlineMgr_h

#include "Rundata/Rundata.h"

OmnDefineSingletonClass(AosMsgSvrOnlineMgrSingleton,
						AosMsgSvrOnlineMgr,
						AosMsgSvrOnlineMgrSelf,
						OmnSingletonObjId::eMsgSvrOnlineMgr,
						"MsgSvrOnlineMgr");

class AosMsgSvrOnlineMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;

public:
	AosMsgSvrOnlineMgr();
	~AosMsgSvrOnlineMgr();

    // OmnThreadedObj Interface

    // Singleton class interface
    static AosMsgSvrOnlineMgr *	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);
	bool 	start(const AosXmlTagPtr &config);

	//define MsgSvrOnlineMgr management function
	bool getOnlineFriends(
			const AosRundataPtr &rdata);

	bool getOnlineFollowers(
			const AosRundataPtr &rdata);

	bool getUserOnlineStatus(
		int &status,
		const OmnString &friend_cid,
		const AosRundataPtr &rdata);

	bool setUserOnlineStatus(
		const OmnString &status,
		const AosRundataPtr &rdata);
};

#endif
