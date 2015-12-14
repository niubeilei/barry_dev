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
// 06/16/2011	Created by JozhiPeng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Microblog_Microblog_h
#define AOS_Microblog_Microblog_h

#include "Rundata/Rundata.h"
#include "Thread/ThreadedObj.h"

OmnDefineSingletonClass(AosMicroblogSingleton,
						AosMicroblog,
						AosMicroblogSelf,
						OmnSingletonObjId::eMicroblog,
						"Microblog");

class AosMicroblog : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;

public:
	AosMicroblog();
	~AosMicroblog();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	

    // Singleton class interface
    static AosMicroblog *	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool 	start(const AosXmlTagPtr &config);

	//define Microblog management function
	bool addFollower(
			const OmnString &byfollower_cid,
			const AosRundataPtr &rdata);

	bool removeFollower(
			const OmnString &byfollower_cid,
			const AosRundataPtr &rdata);

	bool sendMBlogMsg(
			const OmnString &msg,
			const AosRundataPtr &rdata);

	bool sendPrivateMsg(
			const OmnString &msg,
			const OmnString &follower_cid,
			const AosRundataPtr &rdata);

	bool removeMBlogMsg(
			const OmnString &log_docid,
			const AosRundataPtr &rdata);

	///////////////////////////////////
	bool registFollower(
			const OmnString &byfollower_cid,
			const OmnString &gicid,
			const OmnString &userdata,
			const AosRundataPtr &rdata);

	bool sendCirculationMsg(
			const AosXmlTagPtr &msg,
			const OmnString &follower_cid,
			const AosRundataPtr &rdata);


private:
	bool saveMBlogMsg(
			u64 &log_docid,
			const OmnString &msg,
			const AosRundataPtr &rdata);
};
#endif
