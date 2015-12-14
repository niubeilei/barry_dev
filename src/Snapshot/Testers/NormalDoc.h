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
#ifndef Aos_Snapshot_Testers_NormalDoc_h 
#define Aos_Snapshot_Testers_NormalDoc_h 

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "QueryRslt/Ptrs.h"
#include "BitmapMgr/Bitmap.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Opr.h"

#include <vector>

class AosNormalDoc : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:

	u64 						mStartDocid;
	u64 						mEndDocid;
	u64							mTaskDocid;
	map<int, u64>				mSnapIds;
	static u64					smDocid;

public:
	AosNormalDoc(const u64 &task_docid, const AosRundataPtr &rdata);

	~AosNormalDoc();

	bool createData(const u64 &docid, const AosRundataPtr &rdata);
	u64 getStartDocid(){return mStartDocid;}
	u64 getEndDocid(){return mEndDocid;}
	void resetDocid() {mStartDocid = 0; mEndDocid = 0;}
	void setSnapIds(map<int, u64> &id)
	{
		mSnapIds.clear();
		mSnapIds = id;
	}

	bool checkData(
		const AosRundataPtr &rdata,
		const u64 &start_docid, 
		const u64 &end_docid);
};
#endif

