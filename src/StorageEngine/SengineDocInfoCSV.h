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
#ifndef Aos_StorageEngine_SengineDocInfoCSV_h
#define Aos_StorageEngine_SengineDocInfoCSV_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "StorageEngine/SengineDocInfo.h"
#include "StorageEngine/SengineImportDocRequest.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "XmlUtil/Ptrs.h"

#include <map>


class AosSengineDocInfoCSV : virtual public AosSengineDocInfo
{
	OmnDefineRCObject;
	
	enum
	{
		eDftMaxBuffLen = 100000000
	};

	OmnMutexPtr			mLock;
	int					mVid;
	int					mDocSize;
	u64					mNumDocs;
	u64					mCrtSizeid;
	u64					mMaxBuffLen;
	AosBuffPtr			mBuff;
	map<u64, u64>		mOffsetMap;
	OmnSemPtr       	mSem;
	int             	mTotalReqs;	
	int					mTempPost;
	u64					mSnapId;
	u64					mTaskDocid;
	bool				mSvrDeath;
	AosGroupDocOpr::E	mOpr;

public:
	AosSengineDocInfoCSV(
			const AosGroupDocOpr::E opr,
			const int vid, 
			const u64 &snap_id, 
			const u64 &task_docid);
	~AosSengineDocInfoCSV();

	virtual void 	callback(const bool svr_death);

	u64				getNumDocs() {return mNumDocs;}

	bool			createMemory(
						const u64 &buff_len,
						const int doc_size);

	bool			addDoc(
						const char *data, 
						const int len, 
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			deleteDoc(
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			updateDoc(
						const char* doc,
						const int len,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			sendRequestPublic(const AosRundataPtr &rdata);
private:
	bool			sendRequestPriv(const AosRundataPtr &rdata); 

};
#endif
