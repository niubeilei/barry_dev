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
// Modification History:
// 2014/01/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IndexMgr_IndexMgr_h
#define Aos_IndexMgr_IndexMgr_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IndexMgr/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IndexMgrObj.h"
#include "SEInterfaces/Caller.h"
#include "SEInterfaces/QueryProcCallback.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"

#include <vector>
#include <queue>
using namespace std;

class AosIndexMgr : public AosIndexMgrObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxLevels = 100,
		eDftCheckFreq = 60*60*12
	};

private:
	struct Request
	{
		OmnString	mFromIILName;
		OmnString	mToIILName;
	};

	enum Status
	{
		eInvalid,


		eNormIIL,
		eParalIIL,
		eInvalidParalIIL
	};

	typedef hash_map<const OmnString, AosParalIILPtr, Omn_Str_hash, compare_str> iilmap_t;
	typedef hash_map<const OmnString, AosParalIILPtr, Omn_Str_hash, compare_str>::iterator iilitr_t;

	typedef hash_map<const OmnString, OmnString, Omn_Str_hash, compare_str> keymap_t;
	typedef hash_map<const OmnString, OmnString, Omn_Str_hash, compare_str>::iterator keyitr_t;

	typedef hash_map<const OmnString, Status, Omn_Str_hash, compare_str> bmap_t;
	typedef hash_map<const OmnString, Status, Omn_Str_hash, compare_str>::iterator bitr_t;

	typedef hash_map<const u64, AosXmlTagPtr, u64_hash, u64_cmp> docmap_t;
	typedef hash_map<const u64, AosXmlTagPtr, u64_hash, u64_cmp>::iterator docitr_t;

	typedef hash_map<const OmnString, u64, Omn_Str_hash, compare_str> docidmap_t;
	typedef hash_map<const OmnString, u64, Omn_Str_hash, compare_str>::iterator dociditr_t;

	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;

	docmap_t		mDocid2DocMap;
	docidmap_t		mIILName2DocidMap;
	keymap_t		mKey2IILNameMap;
	keymap_t		mIILNameMap;
	bmap_t			mIILStatusMap;
	iilmap_t		mParalIILMap;
	queue<Request>	mRequests;
	i64				mCheckFreq;

public:
	AosIndexMgr(const int version);
	~AosIndexMgr();

	virtual bool convertIILName(
					const AosRundataPtr &rdata, 
					const int epoch_day, 
					const OmnString &iilname, 
					bool &converted,
					OmnString &new_iilname);

	virtual bool createEntry(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &def);
	
	virtual bool modifyEntry(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &def);

	virtual bool removeEntry(
					const AosRundataPtr &rdata,
					const OmnString &table_name,
					const OmnString &field_name); 

	virtual bool resolveIndex(
					const AosRundataPtr &rdata, 
					const OmnString &table_name,
					const OmnString &field_name,
					OmnString &iilname);

	virtual bool getIILNames(
					const AosRundataPtr &rdata, 
					const AosQueryReqObjPtr &query_req, 
					const OmnString &iilname, 
					vector<OmnString> &iilnames);

	virtual AosXmlTagPtr getIndexDefDoc(
					const AosRundataPtr &rdata,
					const OmnString &iil_name);

	virtual AosXmlTagPtr getIndexDefDoc(
					const AosRundataPtr &rdata,
					const OmnString &table_name,
					const OmnString &field_name);

	virtual bool isParalIIL(
					const AosRundataPtr &rdata, 
					const OmnString &iilname);

	virtual AosJimoPtr cloneJimo() const;

	virtual bool createEntry(
					const AosRundataPtr &rdata, 
					const OmnString iilname, 
					const OmnString &table_name, 
					const OmnString &field_name, 
					const OmnString &index_type);
			
private:
	AosParalIILPtr getParalIIL(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &index_doc);

	bool	logIndexAlreadyExist(
					const AosRundataPtr &rdata, 
					const OmnString &field_name, 
					const OmnString &iilname);

	bool	getDefaultIILName(
					const AosRundataPtr &rdata, 
					const OmnString &table_name, 
					const OmnString &field_name, 
					OmnString &iil_name);

	bool	isGlobalFieldname(
					const OmnString &fieldname, 
					OmnString &new_name);

	OmnString getKey(const OmnString &tname, 
					const OmnString &fname);

	bool removeFieldnameIndexEntry(
					const AosRundataPtr &rdata, 
					const OmnString &table_name, 
					const OmnString &field_name, 
					const u64 docid);

	bool addFieldnameIndexEntry(
					const AosRundataPtr &rdata, 
					const OmnString &table_name, 
					const OmnString &field_name, 
					const u64 docid);

	bool addIILNameIndexEntry(
					const AosRundataPtr &rdata, 
					const OmnString &iil_name, 
					const u64 docid);

	bool removeIILNameIndexEntry(
					const AosRundataPtr &rdata, 
					const OmnString &iilname, 
					const u64 docid);

	u64 getIILNameIndexEntry(
					const AosRundataPtr &rdata, 
					const OmnString &iilname);

	bool isLocal(const u64 docid);

	AosXmlTagPtr getIndexDefDocFromRemote(
					const AosRundataPtr &rdata, 
					const u64 docid,
					const OmnString &iilname);

	AosXmlTagPtr getIndexDefDocFromRemote(
					const AosRundataPtr &rdata, 
					const u64 docid,
					const OmnString &table_name, 
					const OmnString &field_name);

	bool modifyEntryRemote(
					const AosRundataPtr &rdata, 
					const u64 docid, 
					const AosXmlTagPtr &doc);

	bool removeEntryRemote(
					const AosRundataPtr &rdata, 
					const u64 docid, 
					const OmnString &table_name, 
					const OmnString &field_name);

	bool checkMerge();

	bool createEntryRemote(
					const AosRundataPtr &rdata, 
					const u64 docid);

	bool addPeriod(
					const AosRundataPtr &rdata, 
					const u64 docid, 
					const int level, 
					const int period);

	bool addPeriodRemote(
					const AosRundataPtr &rdata, 
					const u64 docid, 
					const int level, 
					const int period);

	AosXmlTagPtr getIndexDefDocLocked(
					const AosRundataPtr &rdata, 
					const u64 docid);
};
#endif

