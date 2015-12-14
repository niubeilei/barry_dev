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
// 01/30/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_SEModules_VersionMgr_h
#define AOS_SEModules_VersionMgr_h

#include "Database/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"

class AosVersionMgr : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxVerLen = 20
	};

private:
	AosVersionDocMgrPtr	mVersionDocMgr;
	OmnString			mDirname;
	OmnString 			mFilename;
	OmnMutexPtr			mLock;

public:
	AosVersionMgr();
	~AosVersionMgr();

	bool		stop();
	OmnString 	incrementVersion(const OmnString &version);
	bool 		addVersionObj(
					const u64 &docid,
					const AosXmlTagPtr &olddoc, 
					const AosXmlTagPtr &newdoc,
					const AosRundataPtr &rdata);
	AosXmlTagPtr getVersionObj(
					const u64 &docid,
					const OmnString &version, 
					OmnString &errmsg,
					const AosRundataPtr &rdata);
	AosXmlTagPtr getVersionObj(const u64 &docid);
	bool rebuildVersion();

	// Ketty 2011/02/14
	AosXmlTagPtr	getLastVersionObj(const u64 &docid, const AosRundataPtr &rdata);

	bool
	addVersionIdx(
		const u64 &docid,
		const u32 seqno, 
		const u32 offset, 
		const OmnString &version,
		const AosRundataPtr &rdata);
	bool	
	addVersionDoc(
		const u64 &docid,
		const OmnString &doc, 
		const OmnString &version,
		const AosRundataPtr &rdata);
};
#endif
#endif
