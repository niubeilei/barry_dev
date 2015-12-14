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
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILMgr_IILThrdShellProc_h
#define AOS_IILMgr_IILThrdShellProc_h

#include "IILMgr/Ptrs.h"
#include "IILUtil/IILUtil.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Util/BuffArrayVar.h"


class AosIILBigStrSaver : public OmnThrdShellProc
{
	OmnDefineRCObject;

	vector<AosIILBigStrPtr> 	mIILList;
	AosRundataPtr   			mRundata;

public:
	AosIILBigStrSaver(
			const AosIILBigStrPtr &iil,
			const AosRundataPtr &rdata);
	AosIILBigStrSaver(
			const vector<AosIILBigStrPtr> iil_list,
			const AosRundataPtr &rdata);
	~AosIILBigStrSaver();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigU64Saver : public OmnThrdShellProc
{
	OmnDefineRCObject;

	vector<AosIILBigU64Ptr> 	mIILList;
	AosRundataPtr   			mRundata;

public:
	AosIILBigU64Saver(
			const AosIILBigU64Ptr &iil,
			const AosRundataPtr &rdata);
	AosIILBigU64Saver(
			const vector<AosIILBigU64Ptr> iil_list,
			const AosRundataPtr &rdata);
	~AosIILBigU64Saver();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigI64Saver : public OmnThrdShellProc
{
	OmnDefineRCObject;

	vector<AosIILBigI64Ptr> 	mIILList;
	AosRundataPtr   			mRundata;

public:
	AosIILBigI64Saver(
			const AosIILBigI64Ptr &iil,
			const AosRundataPtr &rdata);
	AosIILBigI64Saver(
			const vector<AosIILBigI64Ptr> iil_list,
			const AosRundataPtr &rdata);
	~AosIILBigI64Saver();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigD64Saver : public OmnThrdShellProc
{
	OmnDefineRCObject;

	vector<AosIILBigD64Ptr> 	mIILList;
	AosRundataPtr   			mRundata;

public:
	AosIILBigD64Saver(
			const AosIILBigD64Ptr &iil,
			const AosRundataPtr &rdata);
	AosIILBigD64Saver(
			const vector<AosIILBigD64Ptr> iil_list,
			const AosRundataPtr &rdata);
	~AosIILBigD64Saver();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigHitSaver : public OmnThrdShellProc
{
	OmnDefineRCObject;

	vector<AosIILBigHitPtr> 	mIILList;
	AosRundataPtr   			mRundata;

public:
	AosIILBigHitSaver(
			const AosIILBigHitPtr &iil,
			const AosRundataPtr &rdata);
	AosIILBigHitSaver(
			const vector<AosIILBigHitPtr> iil_list,
			const AosRundataPtr &rdata);
	~AosIILBigHitSaver();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigStrBtachAddSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigStrPtr			mParentIIL;
	i64						mIILID;
	i64						mIILIdx;
	char *					mEntries;
	int						mSize;
	i64						mNum;
	AosIILBigStrPtr*		mParents;
	AosIILExecutorObjPtr	mExecutor; 
	AosRundataPtr   		mRundata;

public:
	AosIILBigStrBtachAddSingleThrd(
			const AosIILBigStrPtr &parent_iil,
			const i64 &iilid,
			const i64 &iil_idx,
			char * entries,
			const int size,
			const i64 &num,
			AosIILBigStrPtr* parents,
			const AosIILExecutorObjPtr &executor,
			const AosRundataPtr &rdata);
	~AosIILBigStrBtachAddSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigU64BtachAddSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigU64Ptr		mParentIIL;
	i64					mIILID;
	i64					mIILIdx;
	char *				mEntries;
	int					mSize;
	i64					mNum;
	AosRundataPtr   	mRundata;

public:
	AosIILBigU64BtachAddSingleThrd(
			const AosIILBigU64Ptr &parent_iil,
			const i64 &iilid,
			const i64 &iil_idx,
			char * entries,
			const int size,
			const i64 &num,
			const AosRundataPtr &rdata);
	~AosIILBigU64BtachAddSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigI64BtachAddSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigI64Ptr		mParentIIL;
	i64					mIILID;
	i64					mIILIdx;
	char *				mEntries;
	int					mSize;
	i64					mNum;
	AosRundataPtr   	mRundata;

public:
	AosIILBigI64BtachAddSingleThrd(
			const AosIILBigI64Ptr &parent_iil,
			const i64 &iilid,
			const i64 &iil_idx,
			char * entries,
			const int size,
			const i64 &num,
			const AosRundataPtr &rdata);
	~AosIILBigI64BtachAddSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigD64BtachAddSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigD64Ptr		mParentIIL;
	i64					mIILID;
	i64					mIILIdx;
	char *				mEntries;
	int					mSize;
	i64					mNum;
	AosRundataPtr   	mRundata;

public:
	AosIILBigD64BtachAddSingleThrd(
			const AosIILBigD64Ptr &parent_iil,
			const i64 &iilid,
			const i64 &iil_idx,
			char * entries,
			const int size,
			const i64 &num,
			const AosRundataPtr &rdata);
	~AosIILBigD64BtachAddSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigHitBtachAddSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigHitPtr		mParentIIL;
	i64					mIILID;
	i64					mIILIdx;
	AosBitmapObjPtr		mAddBitmap;
	AosBitmapObjPtr		mOrigBitmap;
	int					mSize;
	i64					mNum;
	AosRundataPtr   	mRundata;

public:
	AosIILBigHitBtachAddSingleThrd(
			const AosIILBigHitPtr &parent_iil,
			const i64 &iilid,
			const i64 &iil_idx,
			AosBitmapObjPtr add_bitmap,
			const int size,
			const i64 &num,
			const AosRundataPtr &rdata);
	~AosIILBigHitBtachAddSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigStrRebuildBitmapThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigStrPtr		mParentIIL;
	u64					mIndex;
	u64					mNumIILs;
	u64					mNodeID;
	vector<u64>			mParentList;
	vector<u64>			mMemlist;
	AosRundataPtr   	mRundata;

public:
	AosIILBigStrRebuildBitmapThrd(
			const AosIILBigStrPtr &parentIIL,
			const u64 &index,
			const u64 &num_iils,
			const u64 &node_id,
			const AosRundataPtr &rdata);
	~AosIILBigStrRebuildBitmapThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigU64RebuildBitmapThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigU64Ptr		mParentIIL;
	u64					mIndex;
	u64					mNumIILs;
	u64					mNodeID;
	vector<u64>			mParentList;
	vector<u64>			mMemlist;
	AosRundataPtr   	mRundata;

public:
	AosIILBigU64RebuildBitmapThrd(
			const AosIILBigU64Ptr &parentIIL,
			const u64 &index,
			const u64 &num_iils,
			const u64 &node_id,
			const AosRundataPtr &rdata);
	~AosIILBigU64RebuildBitmapThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigI64RebuildBitmapThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigI64Ptr		mParentIIL;
	u64					mIndex;
	u64					mNumIILs;
	u64					mNodeID;
	vector<u64>			mParentList;
	vector<u64>			mMemlist;
	AosRundataPtr   	mRundata;

public:
	AosIILBigI64RebuildBitmapThrd(
			const AosIILBigI64Ptr &parentIIL,
			const u64 &index,
			const u64 &num_iils,
			const u64 &node_id,
			const AosRundataPtr &rdata);
	~AosIILBigI64RebuildBitmapThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigD64RebuildBitmapThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigD64Ptr		mParentIIL;
	u64					mIndex;
	u64					mNumIILs;
	u64					mNodeID;
	vector<u64>			mParentList;
	vector<u64>			mMemlist;
	AosRundataPtr   	mRundata;

public:
	AosIILBigD64RebuildBitmapThrd(
			const AosIILBigD64Ptr &parentIIL,
			const u64 &index,
			const u64 &num_iils,
			const u64 &node_id,
			const AosRundataPtr &rdata);
	~AosIILBigD64RebuildBitmapThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigHitRebuildBitmapThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigHitPtr		mParentIIL;
	u64					mIndex;
	u64					mNumIILs;
	u64					mNodeID;
	vector<u64>			mParentList;
	vector<u64>			mMemlist;
	AosRundataPtr   	mRundata;

public:
	AosIILBigHitRebuildBitmapThrd(
			const AosIILBigHitPtr &parentIIL,
			const u64 &index,
			const u64 &num_iils,
			const u64 &node_id,
			const AosRundataPtr &rdata);
	~AosIILBigHitRebuildBitmapThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigStrBtachAddRebuildThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigStrPtr		mIIL;
	char *				mEntries;
	int					mSize;
	i64					mNum;
	char **				mOrigValues;
	u64 * 				mOrigDocids;
	i64 				mOrigNum;
	i64					mIILSize;
	AosRundataPtr   	mRundata;

public:
	AosIILBigStrBtachAddRebuildThrd(
			const AosIILBigStrPtr &iil,
			char * entries,
			const int size,
			const i64 &num,
			char** orig_values,
			u64* orig_docids,
			const i64 &orig_num,
			const i64 &iilsize,
			const AosRundataPtr &rdata);
	~AosIILBigStrBtachAddRebuildThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigU64BtachAddRebuildThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigU64Ptr		mIIL;
	char *				mEntries;
	int					mSize;
	i64					mNum;
	u64*				mOrigValues;
	u64 * 				mOrigDocids;
	i64 				mOrigNum;
	i64					mIILSize;
	AosRundataPtr   	mRundata;

public:
	AosIILBigU64BtachAddRebuildThrd(
			const AosIILBigU64Ptr &iil,
			char * entries,
			const int size,
			const i64 &num,
			u64* orig_values,
			u64* orig_docids,
			i64 &orig_num,
			i64 &iilsize,
			const AosRundataPtr &rdata);
	~AosIILBigU64BtachAddRebuildThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigI64BtachAddRebuildThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigI64Ptr		mIIL;
	char *				mEntries;
	int					mSize;
	i64					mNum;
	i64*				mOrigValues;
	u64 * 				mOrigDocids;
	i64 				mOrigNum;
	i64					mIILSize;
	AosRundataPtr   	mRundata;

public:
	AosIILBigI64BtachAddRebuildThrd(
			const AosIILBigI64Ptr &iil,
			char * entries,
			const int size,
			const i64 &num,
			i64* orig_values,
			u64* orig_docids,
			i64 &orig_num,
			i64 &iilsize,
			const AosRundataPtr &rdata);
	~AosIILBigI64BtachAddRebuildThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigD64BtachAddRebuildThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigD64Ptr		mIIL;
	char *				mEntries;
	int					mSize;
	i64					mNum;
	d64*				mOrigValues;
	u64 * 				mOrigDocids;
	i64 				mOrigNum;
	i64					mIILSize;
	AosRundataPtr   	mRundata;

public:
	AosIILBigD64BtachAddRebuildThrd(
			const AosIILBigD64Ptr &iil,
			char * entries,
			const int size,
			const i64 &num,
			d64* orig_values,
			u64* orig_docids,
			i64 &orig_num,
			i64 &iilsize,
			const AosRundataPtr &rdata);
	~AosIILBigD64BtachAddRebuildThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigHitBtachAddRebuildThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigHitPtr		mIIL;
	AosBitmapObjPtr		mAddBitmap;
	AosBitmapObjPtr		mOrigBitmap;
	int					mSize;
	i64					mNum;
	i64 				mOrigNum;
	i64					mIILSize;
	AosRundataPtr   	mRundata;

public:
	AosIILBigHitBtachAddRebuildThrd(
			const AosIILBigHitPtr &iil,
			AosBitmapObjPtr add_bitmap,
			const int size,
			const i64 &num,
			AosBitmapObjPtr orig_bitmap,
			i64 &orig_num,
			i64 &iilsize,
			const AosRundataPtr &rdata);
	~AosIILBigHitBtachAddRebuildThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigStrBtachIncSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigStrPtr				mParentIIL;
	i64							mIILID;
	i64							mIILIdx;
	char *						mEntries;
	int							mSize;
	i64							mNum;
	u64							mDftValue;
	AosIILUtil::AosIILIncType	mIncType;
	AosRundataPtr   			mRundata;

public:
	AosIILBigStrBtachIncSingleThrd(
			const AosIILBigStrPtr &parent_iil,
			const i64 &iilid,
			const i64 &iil_idx,
			char * entries,
			const int size,
			const i64 &num,
			const u64 &dftValue,
			const AosIILUtil::AosIILIncType incType,
			const AosRundataPtr &rdata);
	~AosIILBigStrBtachIncSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};


class AosIILBigU64BtachIncSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigU64Ptr				mParentIIL;
	i64							mIILID;
	i64							mIILIdx;
	char *						mEntries;
	int							mSize;
	i64							mNum;
	u64							mDftValue;
	AosIILUtil::AosIILIncType	mIncType;
	AosRundataPtr   			mRundata;

public:
	AosIILBigU64BtachIncSingleThrd(
			const AosIILBigU64Ptr &parent_iil,
			const i64 &iilid,
			const i64 &iil_idx,
			char * entries,
			const int size,
			const i64 &num,
			const u64 &dftValue,
			const AosIILUtil::AosIILIncType incType,
			const AosRundataPtr &rdata);
	~AosIILBigU64BtachIncSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigI64BtachIncSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigI64Ptr				mParentIIL;
	i64							mIILID;
	i64							mIILIdx;
	char *						mEntries;
	int							mSize;
	i64							mNum;
	i64							mDftValue;
	AosIILUtil::AosIILIncType	mIncType;
	AosRundataPtr   			mRundata;

public:
	AosIILBigI64BtachIncSingleThrd(
			const AosIILBigI64Ptr &parent_iil,
			const i64 &iilid,
			const i64 &iil_idx,
			char * entries,
			const int size,
			const i64 &num,
			const i64 &dftValue,
			const AosIILUtil::AosIILIncType incType,
			const AosRundataPtr &rdata);
	~AosIILBigI64BtachIncSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigD64BtachIncSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigD64Ptr				mParentIIL;
	i64							mIILID;
	i64							mIILIdx;
	char *						mEntries;
	int							mSize;
	i64							mNum;
	d64							mDftValue;
	AosIILUtil::AosIILIncType	mIncType;
	AosRundataPtr   			mRundata;

public:
	AosIILBigD64BtachIncSingleThrd(
			const AosIILBigD64Ptr &parent_iil,
			const i64 &iilid,
			const i64 &iil_idx,
			char * entries,
			const int size,
			const i64 &num,
			const d64 &dftValue,
			const AosIILUtil::AosIILIncType incType,
			const AosRundataPtr &rdata);
	~AosIILBigD64BtachIncSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigHitBtachIncSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigHitPtr				mParentIIL;
	i64							mIILID;
	i64							mIILIdx;
	char *						mEntries;
	int							mSize;
	i64							mNum;
	u64							mDftValue;
	AosIILUtil::AosIILIncType	mIncType;
	AosRundataPtr   			mRundata;

public:
	AosIILBigHitBtachIncSingleThrd(
			const AosIILBigHitPtr &parent_iil,
			const i64 &iilid,
			const i64 &iil_idx,
			char * entries,
			const int size,
			const i64 &num,
			const u64 &dftValue,
			const AosIILUtil::AosIILIncType incType,
			const AosRundataPtr &rdata);
	~AosIILBigHitBtachIncSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigStrCopyDataSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigStrPtr			mIIL;
	AosQueryRsltObjPtr		mQueryRslt;
	AosBitmapObjPtr			mQueryBitmap;
	AosQueryContextObjPtr	mQueryContext;
	i64						mStart;
	i64						mEnd;
	AosRundataPtr   		mRundata;

public:
	AosBitmapObjPtr			mRsltBitmap;

public:
	AosIILBigStrCopyDataSingleThrd(
			const AosIILBigStrPtr &iil,
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &query_bitmap,
			const AosQueryContextObjPtr &query_context,
			const i64 &start,
			const i64 &end,
			const AosRundataPtr &rdata);
	~AosIILBigStrCopyDataSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};


class AosIILBigU64CopyDataSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigU64Ptr			mIIL;
	AosQueryRsltObjPtr		mQueryRslt;
	AosBitmapObjPtr			mQueryBitmap;
	AosQueryContextObjPtr	mQueryContext;
	i64						mStart;
	i64						mEnd;
	AosRundataPtr   		mRundata;

public:
	AosBitmapObjPtr			mRsltBitmap;

public:
	AosIILBigU64CopyDataSingleThrd(
			const AosIILBigU64Ptr &iil,
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &query_bitmap,
			const AosQueryContextObjPtr &query_context,
			const i64 &start,
			const i64 &end,
			const AosRundataPtr &rdata);
	~AosIILBigU64CopyDataSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigHitCopyDataSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigHitPtr			mIIL;
	AosQueryRsltObjPtr		mQueryRslt;
	AosBitmapObjPtr			mQueryBitmap;
	AosQueryContextObjPtr	mQueryContext;
	i64						mStart;
	i64						mEnd;
	AosRundataPtr   		mRundata;

public:
	AosBitmapObjPtr			mRsltBitmap;

public:
	AosIILBigHitCopyDataSingleThrd(
			const AosIILBigHitPtr &iil,
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &query_bitmap,
			const AosQueryContextObjPtr &query_context,
			const i64 &start,
			const i64 &end,
			const AosRundataPtr &rdata);
	~AosIILBigHitCopyDataSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigI64CopyDataSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigI64Ptr			mIIL;
	AosQueryRsltObjPtr		mQueryRslt;
	AosBitmapObjPtr			mQueryBitmap;
	AosQueryContextObjPtr	mQueryContext;
	i64						mStart;
	i64						mEnd;
	AosRundataPtr   		mRundata;

public:
	AosBitmapObjPtr			mRsltBitmap;

public:
	AosIILBigI64CopyDataSingleThrd(
			const AosIILBigI64Ptr &iil,
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &query_bitmap,
			const AosQueryContextObjPtr &query_context,
			const i64 &start,
			const i64 &end,
			const AosRundataPtr &rdata);
	~AosIILBigI64CopyDataSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

class AosIILBigD64CopyDataSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosIILBigD64Ptr			mIIL;
	AosQueryRsltObjPtr		mQueryRslt;
	AosBitmapObjPtr			mQueryBitmap;
	AosQueryContextObjPtr	mQueryContext;
	i64						mStart;
	i64						mEnd;
	AosRundataPtr   		mRundata;

public:
	AosBitmapObjPtr			mRsltBitmap;

public:
	AosIILBigD64CopyDataSingleThrd(
			const AosIILBigD64Ptr &iil,
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &query_bitmap,
			const AosQueryContextObjPtr &query_context,
			const i64 &start,
			const i64 &end,
			const AosRundataPtr &rdata);
	~AosIILBigD64CopyDataSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};





class AosJimoTableBtachAddRebuildThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosJimoTablePtr		mIIL;
	AosBuffArrayVarPtr	mArray;
	i64					mCrtIdx;
	i64					mNumHandle;
	AosBuffArrayVarPtr	mOrigArray;
	i64					mOrigIdx;
	i64					mOrigHandle;
	i64					mIILSize;
	AosRundataPtr   	mRundata;

public:
	AosJimoTableBtachAddRebuildThrd(
			const AosJimoTablePtr &iil,
			const AosBuffArrayVarPtr &array,
			const i64 &crt_idx,
			const i64 &num_handle,
			const AosBuffArrayVarPtr &orig_array,
			const i64 &orig_idx,
			const i64 &orig_handle,
			const i64 &iilsize,
			const AosRundataPtr &rdata);
	~AosJimoTableBtachAddRebuildThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};


class AosJimoTableBtachAddSingleThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosJimoTablePtr		mParentIIL;
	i64					mIILID;
	i64					mIILIdx;
	AosBuffArrayVarPtr	mArray;
	i64					mCrtIdx;
	i64					mNumHandle;
	AosRundataPtr   	mRundata;

public:
	AosJimoTableBtachAddSingleThrd(
			const AosJimoTablePtr &parent_iil,
			const i64 &iilid,
			const i64 &iil_idx,
			const AosBuffArrayVarPtr &array,
			const i64 &crt_idx,
			const i64 &num_handle,
			const AosRundataPtr &rdata);
	~AosJimoTableBtachAddSingleThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};

#endif

