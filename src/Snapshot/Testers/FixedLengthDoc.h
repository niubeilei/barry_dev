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
// 09/20/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Snapshot_Testers_FixedLengthDoc_h 
#define Aos_Snapshot_Testers_FixedLengthDoc_h 

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "QueryRslt/Ptrs.h"
#include "BitmapMgr/Bitmap.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Opr.h"

#include <vector>

class AosFixedLengthDoc : virtual public OmnRCObject 
{
	OmnDefineRCObject;
	enum
	{
		eMaxDocs = 1000
	};

private:

	OmnMutexPtr					mLock;
	u64 						mCrtSizeid;
	int                         mDocSize;
	u64                         mDocid;
	int                         mNumDocids;
	AosXmlTagPtr				mRecordDoc;
	int 						mNumVirtuals;
	int							mNumServers;
	u64 						mId;
	u64 *						mDocids[50];
	u64 *						mGlobalDocids[50];
	char *						mRawData[50];
	int							mIndex[50];
	u64							mFirstDocid[50];
	u64							mStartDocid;
	u64							mEndDocid;
	OmnString					mString;
	u64							mTaskDocid;
	map<int, u64>				mSnapIds;

public:
	AosFixedLengthDoc(
			const AosXmlTagPtr &def, 
			const u64 &task_docid,
			const AosRundataPtr &rdata);

	~AosFixedLengthDoc();

	bool createData(const AosRundataPtr &rdata);
	u64 getStartDocid(){return mStartDocid;}
	u64 getEndDocid(){return mEndDocid;}
	void resetDocid() {mStartDocid = 0; mEndDocid = 0;}
	u64 getCrtSizeid() {return mCrtSizeid;}
	u64 getCrtId() {return mId;}

	void setSnapIds(map<int, u64> &id)
	{
		mSnapIds.clear();
		mSnapIds = id;
	}
	
	bool checkData(
		const AosRundataPtr &rdata,
		const u64 &start_docid, 
		const u64 &end_docid);

	bool sendRequest(const AosRundataPtr &rdata);
private:
	bool config(const AosXmlTagPtr &def);

	u64 getNextDocid(const AosRundataPtr &rdata);
	
	bool getDocids(u64 &docid, int &num_docids, const AosRundataPtr &rdata);

	bool appendEntry(const AosRundataPtr &rdata);

	OmnString composeEntry(const u64 &docid);

	bool sendRequestPriv(
			const AosRundataPtr &rdata, 
			const int &vid);
};
#endif

