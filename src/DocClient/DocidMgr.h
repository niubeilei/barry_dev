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
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocClient_DocidMgr_h
#define AOS_DocClient_DocidMgr_h

#include "IdGen/Ptrs.h"
#include "IdGen/U64IdGen.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SearchEngine/Ptrs.h"
#include "Security/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Ptrs.h"
#include "DocClient/Docid.h"
#include "Util/RCObject.h"
#include "SEInterfaces/DocidMgrObj.h"

OmnDefineSingletonClass(AosDocidMgrSingleton,
						AosDocidMgr,
						AosDocidMgrSelf,
						OmnSingletonObjId::eDocidMgr,
						"DocidMgr");


#define AOSDOCIDKEY_NORMAL			"docid"
#define AOSDOCIDKEY_LOG_GROUP		"log_group_docid"

class AosDocidMgr : virtual public AosDocidMgrObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eNumDocids = 1,
		eMaxVirtual = 1000000
	};

private:
	u64					mInitDocid;
	OmnDocid *			mNormalDocid;
	OmnDocid *			mBinaryDocid;
	OmnDocid *			mLogGroupDocid;

public:
	AosDocidMgr();
	~AosDocidMgr();

    // Singleton class interface
    static AosDocidMgr *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool	retrieveDocidsFromServerSync(const AosRundataPtr &rdata, const int vir_id);
	void 	returnDocid(const u64 &docid, const AosRundataPtr &rdata);

	bool 	bindObjid(
				OmnString &objid,
				u64 &docid,          
				bool &objid_changed,
				const bool resolve,
				const bool keepDocid,
				const AosRundataPtr &rdata);

	// Chen Ding, 2013/11/01
	u64	nextDocid(const AosRundataPtr &rdata, const int vid);
	u64	nextLogGroupDocid(const AosRundataPtr &rdata);

	u64	nextDocid(const int &vid, 
				OmnString &objid, 
				const AosRundataPtr &rdata);

	u64	nextDocid(OmnString &objid, const AosRundataPtr &rdata);

private:
	bool verifyDocid(const u64 &docid, const AosRundataPtr &rdata);
};
#endif
