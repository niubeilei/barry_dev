////////////////////////////////////////////////////////////////////////////
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
// 08/18/2010: Created by James
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SEModules_OnlineMgr_h
#define Aos_SEModules_OnlineMgr_h

#include <map>
#include <vector>
#include <ext/hash_map>
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEServer/Ptrs.h"
#include "SEModules/OnlineUser.h"
#include "Util/HashUtil.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"


OmnDefineSingletonClass(AosOnlineMgrSingleton,
						AosOnlineMgr,
						AosOnlineMgrSelf,
						OmnSingletonObjId::eOnlineMgr,
						"OnlineMgr");

class AosOnlineMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr     mLock;
	bool		mIsStopping;
	enum OnlineStatus
	{
		eOffline=0,
		eOnline

	};
	enum
	{
		eInitSize = 5,
		eIncSize = 5,
		eMaxSize = 50000
	};
	//AosIILU64Ptr mOnlineIIL;
	OmnDynArray<u64, eInitSize, eIncSize, eMaxSize> mBuddyList;
	OmnDynArray<u64, eInitSize, eIncSize, eMaxSize> mfriendId;
	hash_map<int, AosOnlineUserPtr>  mUsers;

public:
	AosOnlineMgr();
	~AosOnlineMgr();

    // Singleton class interface
    static AosOnlineMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool procLogin(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root, 
		const u64 &userid,
		const OmnString &siteid,
		OmnString &buddyStatus,
		const AosRundataPtr &rdata);

	bool procLogout(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root, 
		const u64 &userid,
		const OmnString &siteid,
		const AosRundataPtr &rdata);


	bool procOnlineHeartbeat(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root, 
		const u64 &userid);

	bool updateOnlineInfo(
		const OmnString &siteid,
		const u64 &userid, 
		const int status,
		const AosRundataPtr &rdata);

//	bool retrieveBuddyStatus(
//		const OmnString &siteid,
//		const u64 &userid,
//		OmnString &buddyStatus);
	
	bool getFriendStatus(
		const u64 &iilid,
		const u64 &userid,
		OmnString &buddyStatus);

	bool CheckOnlineStatus(
			const u64 &userid,
			OmnString &tm,
			u32	 &status, 
			const AosRundataPtr &rdata);

	bool getTime(time_t tt, OmnString &stm);

	bool addFriend(
		OmnString &errmsg,
		const OmnString &iilname,
		const u64 &userid,
		const u64 &fid,
		const OmnString &siteid,
		bool flag);

	bool pullOnlineMsg(
		const u64 &userid,
		const AosXmlTagPtr &root, 
		OmnString &contents,
		AosXmlRc &errcode, 
		OmnString &errmsg);

	bool sendOnlineMsg(
		const u64 &userid, 
		const OmnString &msg);

	AosOnlineUserPtr getUser(
		const u64 &userid, 
		bool needJoin);
};
#endif
#endif
