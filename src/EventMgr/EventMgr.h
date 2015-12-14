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
//
// Modification History:
// 2011/01/20	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_EventMgr_EventMgr_h
#define Omn_EventMgr_EventMgr_h

#include "EventMgr/EventHook.h"
#include "EventMgr/EventIds.h"
#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/Ptrs.h"

#include <vector>
#include <queue>
#include <map>

using namespace std;

OmnDefineSingletonClass(AosEventMgrSingleton, 
						AosEventMgr,
						AosEventMgrSelf,
						OmnSingletonObjId::eEventMgr, 
						"EventMgr");

typedef hash_map<const OmnString, AosSmartDocObjPtr, Omn_Str_hash, compare_str> AosEventMgrHash;

class AosRundata;
class AosEventMgr : public OmnThreadedObj
{
	OmnDefineRCObject;

	struct RegiEntry
	{
		OmnString			key;
		AosSmartDocObjPtr		sdoc;

		RegiEntry(const OmnString &key, const AosSmartDocObjPtr &sdoc)
			:
		key(key),
		sdoc(sdoc)
		{
		}

		RegiEntry() {}
	};

	struct Entry
	{
		AosEventHook	hook;
		AosRundataPtr	rdata;
	
		Entry(const AosEventHook hook, const AosRundataPtr &rdata)
			:
		hook(hook),
		rdata(rdata)
		{
		}
		Entry() {}
	};

	enum
	{
		eMaxReadAttempts = 10
	};

private:
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	queue<Entry>		mQueue;
	u32					mMaxRegs[eAosHookMax];
	AosEventMgrHash		mSdocs[eAosHookMax];

public:
	AosEventMgr();
	~AosEventMgr();

	// Singleton Class interface
	static AosEventMgr *	getSelf();
	bool		start();
	bool		stop();
	bool		config(const AosXmlTagPtr &configData);

	bool	registerSdoc(
				const OmnString &hook, 
				const OmnString &key,
				const OmnString &sdoc, 
				const AosRundataPtr &rdata);
	bool	unregisterSdoc(
				const AosEventHook hook, 
				const OmnString &key,
				const AosSmartDocObjPtr &sdoc, 
				const AosRundataPtr &rdata);
	bool	procEvent(
				const AosEventHook hook, 
				const AosHookPoint hookpoint,
				const OmnString &key,
				const AosRundataPtr &rdata);

	// OmnThreadedObj interface
	virtual bool threadFunc(OmnThrdStatus::E &state, 
							const OmnSPtr<OmnThread> &thread);
	virtual bool signal(const int threadLogicId);
    virtual void heartbeat(const int tid);
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 

private:
	bool	addToQueueLocked(
				const AosEventHook hook, 
				const OmnString &key,
				const AosSmartDocObjPtr &sdoc, 
				const AosRundataPtr &rdata);
	// bool addToQueue(
	// 		const AosEventHook hook,
	// 		const OmnString &key, 
	// 		const AosSmartDocObjPtr sdoc, 
	// 		const AosRundataPtr &rdata, 
	// 		const bool is_special);
	bool addToIILLocked(
			const AosEventHook hook, 
			const OmnString &key, 
			const OmnString &sdoc_objid, 
			const AosRundataPtr &rdata);
	bool addToIILLocked(
			const AosEventHook hook, 
			const OmnString &key, 
			const u64 &docid, 
			const AosRundataPtr &rdata);

	AosSmartDocObjPtr
	getAsyncSdocs(const AosEventHook hook, const AosRundataPtr &rdata);

	AosSmartDocObjPtr
	getSdocs(const AosEventHook hook, 
			const OmnString &key, 
			const AosRundataPtr &rdata);
};

#endif




