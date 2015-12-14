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
// This is a utility to select docs.
//
// Modification History:
// 05/19/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_CounterServer_VirCtnrSvr_h
#define AOS_CounterServer_VirCtnrSvr_h

#include "CounterServer/Ptrs.h"
#include "CounterUtil/CounterOperations.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/Ptrs.h"
#include "CounterTime/CounterTimeInfo.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Mutex.h"
#include "TransUtil/Ptrs.h"
#include "Util/UtUtil.h"
#include "Util/File.h"
#include "Util/HashUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/SeXmlParser.h"

using namespace std;

class AosCounterTimeInfo;

class AosVirCtnrSvr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosStatProcPtr, Omn_Str_hash, compare_str> CtMap_t;
	typedef hash_map<const OmnString, AosStatProcPtr, Omn_Str_hash, compare_str>::iterator CtMapItr_t;

	enum
	{
		eMaxStatTypes = 50,
		eMaxTimeGrans = 10
	};

	static OmnMutexPtr     	smLock;
	//static OmnThreadPtr		smThread;
	static CtMap_t			smCounterMap;

public:
	AosVirCtnrSvr(const u32 id, const AosXmlTagPtr &config);
	~AosVirCtnrSvr();

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool config(const AosXmlTagPtr &config);
	bool start(const AosRundataPtr &rdata);
	bool stop();

	static u64 getIILIDByCounterId(const OmnString &counter_id, const AosRundataPtr &rdata);

	bool updateCounter(
					const OmnString &key,
					const OmnString &member,
					const int64_t &cvalue, 
					const u64 &unitime, 
					const AosStatType::E statType,
					const AosRundataPtr &rdata);

	bool queryCounters(
					const AosXmlTagPtr &request,
        			const AosRundataPtr &rdata);

	bool 	proc(
				const AosCounterTransPtr &trans,
				const AosRundataPtr &rdata);

	static void setShowLog(bool b);

private:
	bool setEmptyContents(const AosRundataPtr &rdata) const;

	AosStatProcPtr retrieveCounterProcSafe(
					const OmnString &counter_id, 
					const AosCounterOperation::E &opr,
					const AosRundataPtr &rdata);
	bool addCounter(const AosBuffPtr &buff, const AosRundataPtr &rdata);
};
#endif

