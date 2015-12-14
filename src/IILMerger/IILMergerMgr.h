////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILMerger_IILMergerMgr_h
#define AOS_IILMerger_IILMergerMgr_h

#include "IILMerger/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/BuffArray.h"
#include <list>

using namespace std;

OmnDefineSingletonClass(AosIILMergerMgrSingleton,
						AosIILMergerMgr,
						AosIILMergerMgrSelf,
						OmnSingletonObjId::eIILMergerMgr,
						"IILMergerMgr");

class AosIILMergerMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;
private:
	static OmnMutexPtr 					smLock; 
	static map<OmnString, AosIILMergerPtr>	smMergers;

public:
	AosIILMergerMgr();
	~AosIILMergerMgr();
	
	// Singleton class interface
	static AosIILMergerMgr*   getSelf();
	virtual bool        start();
	virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);


	static bool create(
			const OmnString &tableid, 
			const AosIILMergerHandlerPtr &handler,
			const int num_syncs,
			const int maxbkt,
			const u32 maxmementries,
			const u32 maxentries,
			const AosRundataPtr &rdata);

	static bool add(
			const OmnString &tableid, 
			const AosBuffPtr &buff, 
			const AosRundataPtr &rdata);
	static u64 getNumEntries(const OmnString &tableid);

	static bool finish(
			const OmnString &tableid,
			const u64 num_entries,
			const u64 &jobdocid,
			const int &jobServerId,
			const OmnString &tokenid,
			const AosRundataPtr &rdata);
	bool	deleteMerger(const OmnString tableid);
};
#endif

