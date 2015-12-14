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
#ifndef Aos_DataScanner_Testers_FixedLengthDoc_h 
#define Aos_DataScanner_Testers_FixedLengthDoc_h 

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
	bool						mCheck;
	OmnString					mString;

public:
	AosFixedLengthDoc(
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);

	~AosFixedLengthDoc();

	bool createData(const AosRundataPtr &rdata, bool finish, int num_docs);
	u64 getStartDocid(){return mStartDocid;}
	u64 getCrtSizeid() {return mCrtSizeid;}
	u64 getCrtId() {return mId;}
	void setCheck(bool b) {mCheck = b;}
	
private:
	bool config(const AosXmlTagPtr &def);

	u64 getNextDocid(const AosRundataPtr &rdata);
	
	bool getDocids(u64 &docid, int &num_docids, const AosRundataPtr &rdata);

	bool appendEntry(const AosRundataPtr &rdata);

	bool checkData(const AosRundataPtr &rdata, int num_docs);

	OmnString composeEntry(const u64 &docid);

	bool sendRequestPriv(
			const AosRundataPtr &rdata, 
			const int &vid);
};
#endif

