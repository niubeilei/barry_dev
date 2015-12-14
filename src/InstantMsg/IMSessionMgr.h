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
// 10/25/2011	Created by Joshi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IMSessionMgr_IMSessionMgr_h
#define AOS_IMSessionMgr_IMSessionMgr_h

#include "Rundata/Rundata.h"
#include "Util/HashUtil.h"
#include "InstantMsg/Ptrs.h"
#include "InstantMsg/IMSession.h"

OmnDefineSingletonClass(AosIMSessionMgrSingleton,
						AosIMSessionMgr,
						AosIMSessionMgrSelf,
						OmnSingletonObjId::eIMSessionMgr,
						"IMSessionMgr");

class AosIMSessionMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;
	typedef hash_map<OmnString, AosIMSessionPtr, Omn_Str_hash, compare_str> OmnIMSessionMap;
	typedef hash_map<OmnString, AosIMSessionPtr, Omn_Str_hash, compare_str>::iterator OmnIMSessionMapItr;

private:
	OmnMutexPtr			mLock;
	OmnIMSessionMap 	mIMSessions;

public:
	AosIMSessionMgr();
	~AosIMSessionMgr();

    // Singleton class interface
    static AosIMSessionMgr *	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);
	bool 	start(const AosXmlTagPtr &config);

	bool addMsg(
			const OmnString &recver_cid,
			const OmnString &msg, 
			const AosRundataPtr &rdata);

	bool finishSession(
			const AosXmlTagPtr &notes,
			const AosRundataPtr &rdata);
private:
	bool getIMSession(
		AosIMSessionPtr &session,
		const OmnString &recver_cid,
		const AosRundataPtr &rdata);
};
#endif
