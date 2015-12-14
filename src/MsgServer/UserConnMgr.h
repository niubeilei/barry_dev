////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_MsgServer_UserConnMgr_h
#define AOS_MsgServer_UserConnMgr_h

#include "Thread/ThreadedObj.h"
#include "MsgServer/Ptrs.h"
#include "MsgServer/SysUser.h"

OmnDefineSingletonClass(AosUserConnMgrSingleton,
						AosUserConnMgr,
						AosUserConnMgrSelf,
						OmnSingletonObjId::eAosUserConnMgr,
						"AosUserConnMgr");


class AosUserConnMgr : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

typedef hash_map<OmnString, AosSysUserPtr, Omn_Str_hash, compare_str> OmnUserMap;
typedef hash_map<OmnString, AosSysUserPtr, Omn_Str_hash, compare_str>::iterator OmnUserMapItr;
private:
	enum
	{
		eOfflineTime = 40,
		eTimeOutTime = 10,
		eInitTransId = 100,
		eHeartbeatThrdId = 0,
		eReadThrdId = 1,
		eSendThrdId = 2
	};

	OmnThreadPtr        mThread;
	OmnMutexPtr			mLock;
	OmnUserMap			mSysUsers;

public:
	AosUserConnMgr();
	~AosUserConnMgr();

    // Singleton class interface
    static AosUserConnMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	virtual bool    signal(const int threadLogicId);
	bool start(const AosXmlTagPtr &config);

	bool getSysUser(
			const OmnString &cid, 
			AosSysUserPtr &user, 
			bool &wait);

	void removeSysUser(
			const OmnString &cid); 
};
#endif

