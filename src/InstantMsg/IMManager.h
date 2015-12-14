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
// 10/31/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_InstantMsg_IMManager_h
#define AOS_InstantMsg_IMManager_h

#include "Rundata/Rundata.h"

OmnDefineSingletonClass(AosIMManagerSingleton,
						AosIMManager,
						AosIMManagerSelf,
						OmnSingletonObjId::eIMManager,
						"IMManager");

class AosIMManager : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;

public:
	AosIMManager();
	~AosIMManager();

    // Singleton class interface
    static AosIMManager *	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);
	bool 	start(const AosXmlTagPtr &config);

	//define IMManager management function
	bool applyFriend(
			const OmnString &friend_cid,
			OmnString &self_gid,
			const OmnString &msg,
			const AosRundataPtr &rdata);
	bool removeFriendGroup(
			const OmnString &groupid,
			const AosRundataPtr &rdata);

	bool moveTo(
			const OmnString &friend_cid,
			const OmnString &srcgroupid,
			const OmnString &desgroupid,
			const AosRundataPtr &rdata);

	bool addFriend(
			const OmnString &friend_cid,
			OmnString &self_gid,
			OmnString &friend_gid,
			const AosRundataPtr &rdata);

	bool denyFriend(
			const OmnString &friend_cid,
			const OmnString &msg,
			const AosRundataPtr &rdata);

	bool removeFriend(
			const OmnString &friend_cid,
			const OmnString &self_gid,
			const OmnString &friend_gid,
			const AosRundataPtr &rdata);

	bool sendTalkGroupMsg(
			const OmnString &type,
			const OmnString &toid, 
			const OmnString &msg,
			const AosRundataPtr &rdata);
		
	bool sendInstantMsg(
			const OmnString &type,
			const OmnString &toid, 
			const OmnString &friend_cid, 
			const OmnString &msg,
			const AosRundataPtr &rdata);

	bool notifyOnlineStatus(
			const OmnString &status,
			const AosRundataPtr &rdata);

	bool getOnlineFriends(
			const AosRundataPtr &rdata);

	bool removeTalkGroup(
		const OmnString &talkgroupid,
		const AosRundataPtr &rdata);

	bool createTalkGroup(
		const AosXmlTagPtr &talkgroup,
		const AosRundataPtr &rdata);

private:
	bool checkGroupid(
		const OmnString &cid,
		OmnString &gid,
		const AosRundataPtr &rdata);
	
	bool isYourFriend(
		const OmnString &friend_cid,
		const AosRundataPtr &rdata);

};

#endif
