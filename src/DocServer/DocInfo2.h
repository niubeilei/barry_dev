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
// 2012/04/04 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_DocServer_DocInfo2_h
#define Aos_DocServer_DocInfo2_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosDocInfo2 : public OmnRCObject
{
	OmnDefineRCObject;
	
	enum
	{
		eDftFirstDocOffset = 1000
	};

	OmnMutexPtr		mLock;
	char *			mRawData;
	u64 *			mDocids;
	int				mDocSize;
	u32				mNumDocs;
	u32				mMaxDocs;
	u32				mVid;
	u64				mLastSizeid;
	u32				mFirstDocOffset;	// Chen Ding, 08/13/2012
	u64				mFirstDocid;		// Chen Ding, 08/13/2012
	u64				mMaxDocidIdx;		// Chen Ding, 08/13/2012

public:
	AosDocInfo2(const u32 vid);
	~AosDocInfo2();

	u32				getNumDocs() {return mNumDocs;}

	bool			createMemory(
						const u64 &buff_len,
						const int doc_size);

	bool			addDoc(
						const char *data, 
						const int len, 
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			sendRequestPublic(
						const AosRundataPtr &rdata, 
						const AosTaskObjPtr &task);
	
private:
	bool			sendRequestPriv(
						const AosRundataPtr &rdata, 
						const AosTaskObjPtr &task); 

};
#endif
#endif
