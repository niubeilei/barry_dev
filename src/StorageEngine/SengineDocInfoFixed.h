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
// 2014/01/11 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_SengineDocInfoFixed_h
#define Aos_StorageEngine_SengineDocInfoFixed_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "StorageEngine/SengineDocInfo.h"
#include "StorageEngine/GroupDocOpr.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "XmlUtil/Ptrs.h"

class AosSengineDocInfoFixed : virtual public AosSengineDocInfo
{
	OmnDefineRCObject;
	
	enum
	{
		eDftFirstDocOffset = 1000
	};

	OmnMutexPtr			mLock;
	char *				mRawData;
	u64 *				mDocids;
	int					mDocSize;
	u64					mNumDocs;
	u64					mMaxDocs;
	int					mVid;
	u64					mCrtSizeid;
	u64					mFirstDocOffset;
	u64					mFirstDocid;
	u64					mMaxDocidIdx;
	OmnSemPtr       	mSem;
	int             	mTotalReqs;	
	int					mTempPost;
	u64					mSnapId;
	u64					mTaskDocid;
	bool				mSvrDeath;
	AosGroupDocOpr::E	mOpr;

public:
	AosSengineDocInfoFixed(const AosGroupDocOpr::E opr, const int vid, const u64 &snap_id, const u64 &task_docid);
	~AosSengineDocInfoFixed();

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
	virtual bool	deleteDoc(
						const u64 &docid,
						const AosRundataPtr &rdata);

	virtual bool	updateDoc(
						const char *data, 
						const int len, 
						const u64 &docid,
						const AosRundataPtr &rdata)
	{
		OmnNotImplementedYet;
		return false;
	}

	bool			sendRequestPublic(const AosRundataPtr &rdata);
	
private:
	bool			sendRequestPriv(const AosRundataPtr &rdata); 

};
#endif
