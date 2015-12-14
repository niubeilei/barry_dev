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
// based on the string value. It has a companion IIL that keeps the
// same set of Docids but sorted based on docids. 
//
// Modification History:
// 	Created: 03/30/2013 by Shawn 
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/IILThrdShellProc.h"
#include "IILMgr/IILBigStr.h"
#include "IILMgr/IILBigU64.h"
#include "IILMgr/IILBigI64.h"
#include "IILMgr/IILBigD64.h"
#include "IILMgr/IILBigHit.h"
#include "IILMgr/JimoTable.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "UtilData/JPID.h"
#include "UtilData/JSID.h"


//========================== AosIILBigStrSaver =============================
AosIILBigStrSaver::AosIILBigStrSaver(
		const AosIILBigStrPtr &iil,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigStrSaver"),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	mIILList.push_back(iil);
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigStrSaver::AosIILBigStrSaver(
		const vector<AosIILBigStrPtr> iil_list,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigStrSaver"),
mIILList(iil_list),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigStrSaver::~AosIILBigStrSaver()
{
}


bool		
AosIILBigStrSaver::run()
{
	// save the iil
	for(u32 i = 0; i < mIILList.size(); i++)
	{
		if (mIILList[i]->isDirty())
		{
			mIILList[i]->saveToLocalFilePriv(mRundata);
		}
	}
	return true;
}

//========================== AosIILBigU64Saver =============================
AosIILBigU64Saver::AosIILBigU64Saver(
		const AosIILBigU64Ptr &iil,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigU64Saver"),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	mIILList.push_back(iil);
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigU64Saver::AosIILBigU64Saver(
		const vector<AosIILBigU64Ptr> iil_list,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigU64Saver"),
mIILList(iil_list),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigU64Saver::~AosIILBigU64Saver()
{
}


bool		
AosIILBigU64Saver::run()
{
	// save the iil
	for(u32 i = 0; i < mIILList.size(); i++)
	{
		if (mIILList[i]->isDirty())
		{
			mIILList[i]->saveToLocalFileSafe(mRundata);
		}
	}
	return true;
}

//========================== AosIILBigI64Saver =============================
AosIILBigI64Saver::AosIILBigI64Saver(
		const AosIILBigI64Ptr &iil,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigI64Saver"),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	mIILList.push_back(iil);
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigI64Saver::AosIILBigI64Saver(
		const vector<AosIILBigI64Ptr> iil_list,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigI64Saver"),
mIILList(iil_list),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigI64Saver::~AosIILBigI64Saver()
{
}


bool		
AosIILBigI64Saver::run()
{
	// save the iil
	for(u32 i = 0; i < mIILList.size(); i++)
	{
		if (mIILList[i]->isDirty())
		{
			mIILList[i]->saveToLocalFileSafe(mRundata);
		}
	}
	return true;
}
	
	
//========================== AosIILBigD64Saver =============================
AosIILBigD64Saver::AosIILBigD64Saver(
		const AosIILBigD64Ptr &iil,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigD64Saver"),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	mIILList.push_back(iil);
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigD64Saver::AosIILBigD64Saver(
		const vector<AosIILBigD64Ptr> iil_list,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigD64Saver"),
mIILList(iil_list),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigD64Saver::~AosIILBigD64Saver()
{
}


bool		
AosIILBigD64Saver::run()
{
	// save the iil
	for(u32 i = 0; i < mIILList.size(); i++)
	{
		if (mIILList[i]->isDirty())
		{
			mIILList[i]->saveToLocalFileSafe(mRundata);
		}
	}
	return true;
}
	
//========================== AosIILBigHitSaver =============================
AosIILBigHitSaver::AosIILBigHitSaver(
		const AosIILBigHitPtr &iil,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigHitSaver"),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	mIILList.push_back(iil);
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigHitSaver::AosIILBigHitSaver(
		const vector<AosIILBigHitPtr> iil_list,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigHitSaver"),
mIILList(iil_list),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigHitSaver::~AosIILBigHitSaver()
{
}


bool		
AosIILBigHitSaver::run()
{
	// save the iil
	for(u32 i = 0; i < mIILList.size(); i++)
	{
		if (mIILList[i]->isDirty())
		{
			mIILList[i]->saveToLocalFileSafe(mRundata);
		}
	}
	return true;
}

//========================== AosIILBigStrBtachAddSingleThrd =============================
AosIILBigStrBtachAddSingleThrd::AosIILBigStrBtachAddSingleThrd(
		const AosIILBigStrPtr &parent_iil,
		const i64 &iilid,
		const i64 &iil_idx,
		char * entries,
		const int size,
		const i64 &num,
		AosIILBigStrPtr* parents,
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigStrBtachAddSingleThrd"),
mParentIIL(parent_iil),
mIILID(iilid),
mIILIdx(iil_idx),
mEntries(entries),
mSize(size),
mNum(num),
mParents(parents),
mExecutor(executor),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigStrBtachAddSingleThrd::~AosIILBigStrBtachAddSingleThrd()
{
}


bool
AosIILBigStrBtachAddSingleThrd::run()
{
	aos_assert_r(mParentIIL, false);
	i64 num = mNum;
	mParentIIL->batchAddParentThrdSafe(mIILID, mIILIdx,
		mEntries, mSize, num, mParents, mExecutor, mRundata);
	return true;
}
	
//========================== AosIILBigU64BtachAddSingleThrd =============================
AosIILBigU64BtachAddSingleThrd::AosIILBigU64BtachAddSingleThrd(
		const AosIILBigU64Ptr &parent_iil,
		const i64 &iilid,
		const i64 &iil_idx,
		char * entries,
		const int size,
		const i64 &num,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigU64BtachAddSingleThrd"),
mParentIIL(parent_iil),
mIILID(iilid),
mIILIdx(iil_idx),
mEntries(entries),
mSize(size),
mNum(num),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigU64BtachAddSingleThrd::~AosIILBigU64BtachAddSingleThrd()
{
}


bool
AosIILBigU64BtachAddSingleThrd::run()
{
	aos_assert_r(mParentIIL, false);
	i64 num = mNum;
	mParentIIL->batchAddParentThrdSafe(mIILID,
		mIILIdx, mEntries, mSize, num, mRundata);
	return true;
}


//========================== AosIILBigI64BtachAddSingleThrd =============================
AosIILBigI64BtachAddSingleThrd::AosIILBigI64BtachAddSingleThrd(
		const AosIILBigI64Ptr &parent_iil,
		const i64 &iilid,
		const i64 &iil_idx,
		char * entries,
		const int size,
		const i64 &num,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigI64BtachAddSingleThrd"),
mParentIIL(parent_iil),
mIILID(iilid),
mIILIdx(iil_idx),
mEntries(entries),
mSize(size),
mNum(num),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigI64BtachAddSingleThrd::~AosIILBigI64BtachAddSingleThrd()
{
}


bool
AosIILBigI64BtachAddSingleThrd::run()
{
	aos_assert_r(mParentIIL, false);
	i64 num = mNum;
	mParentIIL->batchAddParentThrdSafe(mIILID,
		mIILIdx, mEntries, mSize, num, mRundata);
	return true;
}


//========================== AosIILBigD64BtachAddSingleThrd =============================
AosIILBigD64BtachAddSingleThrd::AosIILBigD64BtachAddSingleThrd(
		const AosIILBigD64Ptr &parent_iil,
		const i64 &iilid,
		const i64 &iil_idx,
		char * entries,
		const int size,
		const i64 &num,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigD64BtachAddSingleThrd"),
mParentIIL(parent_iil),
mIILID(iilid),
mIILIdx(iil_idx),
mEntries(entries),
mSize(size),
mNum(num),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigD64BtachAddSingleThrd::~AosIILBigD64BtachAddSingleThrd()
{
}


bool
AosIILBigD64BtachAddSingleThrd::run()
{
	aos_assert_r(mParentIIL, false);
	i64 num = mNum;
	mParentIIL->batchAddParentThrdSafe(mIILID,
		mIILIdx, mEntries, mSize, num, mRundata);
	return true;
}

//========================== AosIILBigHitBtachAddSingleThrd =============================
AosIILBigHitBtachAddSingleThrd::AosIILBigHitBtachAddSingleThrd(
		const AosIILBigHitPtr &parent_iil,
		const i64 &iilid,
		const i64 &iil_idx,
		AosBitmapObjPtr add_bitmap,
		const int size,
		const i64 &num,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigHitBtachAddSingleThrd"),
mParentIIL(parent_iil),
mIILID(iilid),
mIILIdx(iil_idx),
mSize(size),
mNum(num),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mAddBitmap = add_bitmap->clone(0);
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigHitBtachAddSingleThrd::~AosIILBigHitBtachAddSingleThrd()
{
}


bool
AosIILBigHitBtachAddSingleThrd::run()
{
	aos_assert_r(mParentIIL, false);
	i64 num = mNum;
	mParentIIL->batchAddParentThrdSafe(mIILID,
		mIILIdx, mAddBitmap, mSize, num, mRundata);
	return true;
}

//========================== AosIILBigStrRebuildBitmapThrd =============================
AosIILBigStrRebuildBitmapThrd::AosIILBigStrRebuildBitmapThrd(
		const AosIILBigStrPtr &parentIIL,
		const u64 &index,
		const u64 &num_iils,
		const u64 &node_id,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigStrRebuildBitmapThrd"),
mParentIIL(parentIIL),
mIndex(index),
mNumIILs(num_iils),
mNodeID(node_id),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigStrRebuildBitmapThrd::~AosIILBigStrRebuildBitmapThrd()
{
}


bool
AosIILBigStrRebuildBitmapThrd::run()
{
	return mParentIIL->rebuildLevelOne(mIndex, mNumIILs, mNodeID, mRundata);
}



//========================== AosIILBigU64RebuildBitmapThrd =============================
AosIILBigU64RebuildBitmapThrd::AosIILBigU64RebuildBitmapThrd(
		const AosIILBigU64Ptr &parentIIL,
		const u64 &index,
		const u64 &num_iils,
		const u64 &node_id,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigU64RebuildBitmapThrd"),
mParentIIL(parentIIL),
mIndex(index),
mNumIILs(num_iils),
mNodeID(node_id),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigU64RebuildBitmapThrd::~AosIILBigU64RebuildBitmapThrd()
{
}


bool
AosIILBigU64RebuildBitmapThrd::run()
{
	return mParentIIL->rebuildLevelOne(mIndex, mNumIILs, mNodeID, mRundata);
}

//========================== AosIILBigI64RebuildBitmapThrd =============================
AosIILBigI64RebuildBitmapThrd::AosIILBigI64RebuildBitmapThrd(
		const AosIILBigI64Ptr &parentIIL,
		const u64 &index,
		const u64 &num_iils,
		const u64 &node_id,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigI64RebuildBitmapThrd"),
mParentIIL(parentIIL),
mIndex(index),
mNumIILs(num_iils),
mNodeID(node_id),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigI64RebuildBitmapThrd::~AosIILBigI64RebuildBitmapThrd()
{
}


bool
AosIILBigI64RebuildBitmapThrd::run()
{
	return mParentIIL->rebuildLevelOne(mIndex, mNumIILs, mNodeID, mRundata);
}


//========================== AosIILBigD64RebuildBitmapThrd =============================
AosIILBigD64RebuildBitmapThrd::AosIILBigD64RebuildBitmapThrd(
		const AosIILBigD64Ptr &parentIIL,
		const u64 &index,
		const u64 &num_iils,
		const u64 &node_id,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigD64RebuildBitmapThrd"),
mParentIIL(parentIIL),
mIndex(index),
mNumIILs(num_iils),
mNodeID(node_id),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigD64RebuildBitmapThrd::~AosIILBigD64RebuildBitmapThrd()
{
}


bool
AosIILBigD64RebuildBitmapThrd::run()
{
	return mParentIIL->rebuildLevelOne(mIndex, mNumIILs, mNodeID, mRundata);
}

//========================== AosIILBigHitRebuildBitmapThrd =============================
AosIILBigHitRebuildBitmapThrd::AosIILBigHitRebuildBitmapThrd(
		const AosIILBigHitPtr &parentIIL,
		const u64 &index,
		const u64 &num_iils,
		const u64 &node_id,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigHitRebuildBitmapThrd"),
mParentIIL(parentIIL),
mIndex(index),
mNumIILs(num_iils),
mNodeID(node_id),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigHitRebuildBitmapThrd::~AosIILBigHitRebuildBitmapThrd()
{
}


bool
AosIILBigHitRebuildBitmapThrd::run()
{
	return mParentIIL->rebuildLevelOne(mIndex, mNumIILs, mNodeID, mRundata);
}


//========================== AosIILBigStrBtachAddRebuildThrd =============================
AosIILBigStrBtachAddRebuildThrd::AosIILBigStrBtachAddRebuildThrd(
		const AosIILBigStrPtr &iil,
		char * entries,
		const int size,
		const i64 &num,
		char** orig_values,
		u64* orig_docids,
		const i64 &orig_num,
		const i64 &iilsize,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigStrBtachAddRebuildThrd"),
mIIL(iil),
mEntries(entries),
mSize(size),
mNum(num),
mOrigValues(orig_values),
mOrigDocids(orig_docids),
mOrigNum(orig_num),
mIILSize(iilsize),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigStrBtachAddRebuildThrd::~AosIILBigStrBtachAddRebuildThrd()
{
}


bool
AosIILBigStrBtachAddRebuildThrd::run()
{
	aos_assert_r(mIIL, false);
	i64 num = mNum;
	i64 num_handle = mNum;
	i64 orig_idx = 0;
	mIIL->batchAddRebuild(mEntries, mSize, num, num_handle, mOrigValues,
		mOrigDocids, mOrigNum, orig_idx, mIILSize, mRundata);
	mIIL->setDirty(true);
	mIIL->saveToLocalFilePriv(mRundata);
	return true;
}

//========================== AosIILBigU64BtachAddRebuildThrd =============================
AosIILBigU64BtachAddRebuildThrd::AosIILBigU64BtachAddRebuildThrd(
		const AosIILBigU64Ptr &iil,
		char * entries,
		const int size,
		const i64 &num,
		u64* orig_values,
		u64* orig_docids,
		i64 &orig_num,
		i64 &iilsize,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigU64BtachAddRebuildThrd"),
mIIL(iil),
mEntries(entries),
mSize(size),
mNum(num),
mOrigValues(orig_values),
mOrigDocids(orig_docids),
mOrigNum(orig_num),
mIILSize(iilsize),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigU64BtachAddRebuildThrd::~AosIILBigU64BtachAddRebuildThrd()
{
}


bool
AosIILBigU64BtachAddRebuildThrd::run()
{
	aos_assert_r(mIIL, false);
	i64 num = mNum;
	i64 num_handle = mNum;
	i64 orig_idx = 0;
	mIIL->batchAddRebuild(mEntries, mSize, num, num_handle, mOrigValues,
		mOrigDocids, mOrigNum, orig_idx, mIILSize, mRundata);
	mIIL->setDirty(true);
	mIIL->saveToLocalFilePriv(mRundata);
	return true;
}

//========================== AosIILBigI64BtachAddRebuildThrd =============================
AosIILBigI64BtachAddRebuildThrd::AosIILBigI64BtachAddRebuildThrd(
		const AosIILBigI64Ptr &iil,
		char * entries,
		const int size,
		const i64 &num,
		i64* orig_values,
		u64* orig_docids,
		i64 &orig_num,
		i64 &iilsize,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigI64BtachAddRebuildThrd"),
mIIL(iil),
mEntries(entries),
mSize(size),
mNum(num),
mOrigValues(orig_values),
mOrigDocids(orig_docids),
mOrigNum(orig_num),
mIILSize(iilsize),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigI64BtachAddRebuildThrd::~AosIILBigI64BtachAddRebuildThrd()
{
}


bool
AosIILBigI64BtachAddRebuildThrd::run()
{
	aos_assert_r(mIIL, false);
	i64 num = mNum;
	i64 num_handle = mNum;
	i64 orig_idx = 0;
	mIIL->batchAddRebuild(mEntries, mSize, num, num_handle, mOrigValues,
		mOrigDocids, mOrigNum, orig_idx, mIILSize, mRundata);
	mIIL->setDirty(true);
	mIIL->saveToLocalFilePriv(mRundata);
	return true;
}


//========================== AosIILBigD64BtachAddRebuildThrd =============================
AosIILBigD64BtachAddRebuildThrd::AosIILBigD64BtachAddRebuildThrd(
		const AosIILBigD64Ptr &iil,
		char * entries,
		const int size,
		const i64 &num,
		d64* orig_values,
		u64* orig_docids,
		i64 &orig_num,
		i64 &iilsize,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigD64BtachAddRebuildThrd"),
mIIL(iil),
mEntries(entries),
mSize(size),
mNum(num),
mOrigValues(orig_values),
mOrigDocids(orig_docids),
mOrigNum(orig_num),
mIILSize(iilsize),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigD64BtachAddRebuildThrd::~AosIILBigD64BtachAddRebuildThrd()
{
}


bool
AosIILBigD64BtachAddRebuildThrd::run()
{
	aos_assert_r(mIIL, false);
	i64 num = mNum;
	i64 num_handle = mNum;
	i64 orig_idx = 0;
	mIIL->batchAddRebuild(mEntries, mSize, num, num_handle, mOrigValues,
		mOrigDocids, mOrigNum, orig_idx, mIILSize, mRundata);
	mIIL->setDirty(true);
	mIIL->saveToLocalFilePriv(mRundata);
	return true;
}

//========================== AosIILBigHitBtachAddRebuildThrd =============================
AosIILBigHitBtachAddRebuildThrd::AosIILBigHitBtachAddRebuildThrd(
		const AosIILBigHitPtr &iil,
		AosBitmapObjPtr add_bitmap,
		const int size,
		const i64 &num,
		AosBitmapObjPtr orig_bitmap,
		i64 &orig_num,
		i64 &iilsize,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigHitBtachAddRebuildThrd"),
mIIL(iil),
mSize(size),
mNum(num),
mOrigNum(orig_num),
mIILSize(iilsize),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	mAddBitmap = add_bitmap->clone(0);
	mOrigBitmap = orig_bitmap->clone(0);
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigHitBtachAddRebuildThrd::~AosIILBigHitBtachAddRebuildThrd()
{
}


bool
AosIILBigHitBtachAddRebuildThrd::run()
{
	aos_assert_r(mIIL, false);
	i64 num = mNum;
	i64 num_handle = mNum;
	i64 orig_idx = 0;
	mIIL->batchAddRebuild(mAddBitmap, mSize, num, num_handle, mOrigBitmap, mOrigNum, orig_idx, mIILSize, mRundata);
	mIIL->setDirty(true);
	mIIL->saveToLocalFilePriv(mRundata);
	return true;
}

	
//========================== AosIILBigStrBtachIncSingleThrd =============================
AosIILBigStrBtachIncSingleThrd::AosIILBigStrBtachIncSingleThrd(
		const AosIILBigStrPtr &parent_iil,
		const i64 &iilid,
		const i64 &iil_idx,
		char * entries,
		const int size,
		const i64 &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigStrBtachIncSingleThrd"),
mParentIIL(parent_iil),
mIILID(iilid),
mIILIdx(iil_idx),
mEntries(entries),
mSize(size),
mNum(num),
mDftValue(dftValue),
mIncType(incType),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
}


AosIILBigStrBtachIncSingleThrd::~AosIILBigStrBtachIncSingleThrd()
{
}


bool
AosIILBigStrBtachIncSingleThrd::run()
{
	aos_assert_r(mParentIIL, false);
	i64 num = mNum;
	mParentIIL->batchIncParentThrdSafe(mIILID, mIILIdx, 
		mEntries, mSize, num, mDftValue, mIncType, mRundata);
	return true;
}

//========================== AosIILBigU64BtachIncSingleThrd =============================
AosIILBigU64BtachIncSingleThrd::AosIILBigU64BtachIncSingleThrd(
		const AosIILBigU64Ptr &parent_iil,
		const i64 &iilid,
		const i64 &iil_idx,
		char * entries,
		const int size,
		const i64 &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigU64BtachIncSingleThrd"),
mParentIIL(parent_iil),
mIILID(iilid),
mIILIdx(iil_idx),
mEntries(entries),
mSize(size),
mNum(num),
mDftValue(dftValue),
mIncType(incType),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigU64BtachIncSingleThrd::~AosIILBigU64BtachIncSingleThrd()
{
}


bool
AosIILBigU64BtachIncSingleThrd::run()
{
	aos_assert_r(mParentIIL, false);
	i64 num = mNum;
	mParentIIL->batchIncParentThrdSafe(mIILID, mIILIdx,
		mEntries, mSize, num, mDftValue, mIncType, mRundata);
	return true;
}

//========================== AosIILBigI64BtachIncSingleThrd =============================
AosIILBigI64BtachIncSingleThrd::AosIILBigI64BtachIncSingleThrd(
		const AosIILBigI64Ptr &parent_iil,
		const i64 &iilid,
		const i64 &iil_idx,
		char * entries,
		const int size,
		const i64 &num,
		const i64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigI64BtachIncSingleThrd"),
mParentIIL(parent_iil),
mIILID(iilid),
mIILIdx(iil_idx),
mEntries(entries),
mSize(size),
mNum(num),
mDftValue(dftValue),
mIncType(incType),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigI64BtachIncSingleThrd::~AosIILBigI64BtachIncSingleThrd()
{
}


bool
AosIILBigI64BtachIncSingleThrd::run()
{
	aos_assert_r(mParentIIL, false);
	i64 num = mNum;
	mParentIIL->batchIncParentThrdSafe(mIILID, mIILIdx,
		mEntries, mSize, num, mDftValue, mIncType, mRundata);
	return true;
}


//========================== AosIILBigD64BtachIncSingleThrd =============================
AosIILBigD64BtachIncSingleThrd::AosIILBigD64BtachIncSingleThrd(
		const AosIILBigD64Ptr &parent_iil,
		const i64 &iilid,
		const i64 &iil_idx,
		char * entries,
		const int size,
		const i64 &num,
		const d64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigD64BtachIncSingleThrd"),
mParentIIL(parent_iil),
mIILID(iilid),
mIILIdx(iil_idx),
mEntries(entries),
mSize(size),
mNum(num),
mDftValue(dftValue),
mIncType(incType),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigD64BtachIncSingleThrd::~AosIILBigD64BtachIncSingleThrd()
{
}


bool
AosIILBigD64BtachIncSingleThrd::run()
{
	aos_assert_r(mParentIIL, false);
	i64 num = mNum;
	mParentIIL->batchIncParentThrdSafe(mIILID, mIILIdx,
		mEntries, mSize, num, mDftValue, mIncType, mRundata);
	return true;
}

//========================== AosIILBigStrCopyDataSingleThrd =============================
AosIILBigStrCopyDataSingleThrd::AosIILBigStrCopyDataSingleThrd(
		const AosIILBigStrPtr &iil,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const i64 &start,
		const i64 &end,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigStrCopyDataSingleThrd"),
mIIL(iil),
mQueryRslt(query_rslt),
mQueryBitmap(query_bitmap),
mQueryContext(query_context),
mStart(start),
mEnd(end),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mRsltBitmap(0)
{
	if(mQueryBitmap && mQueryRslt)
	{
		mQueryBitmap = mQueryBitmap->clone(rdata);
	}
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigStrCopyDataSingleThrd::~AosIILBigStrCopyDataSingleThrd()
{
}


bool
AosIILBigStrCopyDataSingleThrd::run()
{
	aos_assert_r(mIIL, false);
	mIIL->copyDataSingleThrdSafe(this, mQueryRslt,
		mQueryBitmap, mQueryContext, mStart, mEnd, mRundata);
	return true;
}


//========================== AosIILBigU64CopyDataSingleThrd =============================
AosIILBigU64CopyDataSingleThrd::AosIILBigU64CopyDataSingleThrd(
		const AosIILBigU64Ptr &iil,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const i64 &start,
		const i64 &end,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigU64CopyDataSingleThrd"),
mIIL(iil),
mQueryRslt(query_rslt),
mQueryBitmap(query_bitmap),
mQueryContext(query_context),
mStart(start),
mEnd(end),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mRsltBitmap(0)
{
	//Gavin 2015/09/23
	if(mQueryBitmap && mQueryRslt)
	{
		mQueryBitmap = mQueryBitmap->clone(rdata);
	}
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigU64CopyDataSingleThrd::~AosIILBigU64CopyDataSingleThrd()
{
}


bool
AosIILBigU64CopyDataSingleThrd::run()
{
	aos_assert_r(mIIL, false);
	mIIL->copyDataSingleThrdSafe(this, mQueryRslt,
		mQueryBitmap, mQueryContext, mStart, mEnd, mRundata);
	return true;
}

//========================== AosIILBigI64CopyDataSingleThrd =============================
AosIILBigI64CopyDataSingleThrd::AosIILBigI64CopyDataSingleThrd(
		const AosIILBigI64Ptr &iil,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const i64 &start,
		const i64 &end,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigI64CopyDataSingleThrd"),
mIIL(iil),
mQueryRslt(query_rslt),
mQueryBitmap(query_bitmap),
mQueryContext(query_context),
mStart(start),
mEnd(end),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mRsltBitmap(0)
{
	//Gavin 2015/09/23
	if(mQueryBitmap && mQueryRslt)
	{
		mQueryBitmap = mQueryBitmap->clone(rdata);
	}
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigI64CopyDataSingleThrd::~AosIILBigI64CopyDataSingleThrd()
{
}


bool
AosIILBigI64CopyDataSingleThrd::run()
{
	aos_assert_r(mIIL, false);
	mIIL->copyDataSingleThrdSafe(this, mQueryRslt,
		mQueryBitmap, mQueryContext, mStart, mEnd, mRundata);
	return true;
}


//========================== AosIILBigD64CopyDataSingleThrd =============================
AosIILBigD64CopyDataSingleThrd::AosIILBigD64CopyDataSingleThrd(
		const AosIILBigD64Ptr &iil,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const i64 &start,
		const i64 &end,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigD64CopyDataSingleThrd"),
mIIL(iil),
mQueryRslt(query_rslt),
mQueryBitmap(query_bitmap),
mQueryContext(query_context),
mStart(start),
mEnd(end),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mRsltBitmap(0)
{
	if(mQueryBitmap && mQueryRslt)
	{
		mQueryBitmap = mQueryBitmap->clone(rdata);
	}
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigD64CopyDataSingleThrd::~AosIILBigD64CopyDataSingleThrd()
{
}


bool
AosIILBigD64CopyDataSingleThrd::run()
{
	aos_assert_r(mIIL, false);
	mIIL->copyDataSingleThrdSafe(this, mQueryRslt,
		mQueryBitmap, mQueryContext, mStart, mEnd, mRundata);
	return true;
}

//========================== AosIILBigHitCopyDataSingleThrd =============================
AosIILBigHitCopyDataSingleThrd::AosIILBigHitCopyDataSingleThrd(
		const AosIILBigHitPtr &iil,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const i64 &start,
		const i64 &end,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("IILBigHitCopyDataSingleThrd"),
mIIL(iil),
mQueryRslt(query_rslt),
mQueryBitmap(query_bitmap),
mQueryContext(query_context),
mStart(start),
mEnd(end),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mRsltBitmap(0)
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosIILBigHitCopyDataSingleThrd::~AosIILBigHitCopyDataSingleThrd()
{
}


bool
AosIILBigHitCopyDataSingleThrd::run()
{
	aos_assert_r(mIIL, false);
	mIIL->copyDataSingleThrdSafe(this, mQueryRslt,
		mQueryBitmap, mQueryContext, mStart, mEnd, mRundata);
	return true;
}

//========================== AosJimoTableBtachAddSingleThrd =============================
AosJimoTableBtachAddSingleThrd::AosJimoTableBtachAddSingleThrd(
		const AosJimoTablePtr &parent_iil,
		const i64 &iilid,
		const i64 &iil_idx,
		const AosBuffArrayVarPtr &array,
		const i64 &crt_idx,
		const i64 &num_handle,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("JimoTableBtachAddSingleThrd"),
mParentIIL(parent_iil),
mIILID(iilid),
mIILIdx(iil_idx),
mArray(array),
mCrtIdx(crt_idx),
mNumHandle(num_handle),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosJimoTableBtachAddSingleThrd::~AosJimoTableBtachAddSingleThrd()
{
}


bool
AosJimoTableBtachAddSingleThrd::run()
{
	aos_assert_r(mParentIIL, false);
	i64 crt_idx = mCrtIdx;
	i64 num_handle = mNumHandle;
	mParentIIL->batchAddParentThrdSafe(mIILID, mIILIdx,
		mArray, crt_idx, num_handle, mRundata);
	return true;
}
	

//========================== AosJimoTableBtachAddRebuildThrd =============================
AosJimoTableBtachAddRebuildThrd::AosJimoTableBtachAddRebuildThrd(
		const AosJimoTablePtr &iil,
		const AosBuffArrayVarPtr &array,
		const i64 &crt_idx,
		const i64 &num_handle,
		const AosBuffArrayVarPtr &orig_array,
		const i64 &orig_idx,
		const i64 &orig_handle,
		const i64 &iilsize,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("JimoTableBtachAddRebuildThrd"),
mIIL(iil),
mArray(array),
mCrtIdx(crt_idx),
mNumHandle(num_handle),
mOrigArray(orig_array),
mOrigIdx(orig_idx),
mOrigHandle(orig_handle),
mIILSize(iilsize),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	//Gavin 2015/09/23
	mRundata->setJPID(AOSJPID_SYSTEM); 
	mRundata->setJSID(AOSJSID_SYSTEM); 
}


AosJimoTableBtachAddRebuildThrd::~AosJimoTableBtachAddRebuildThrd()
{
}


bool
AosJimoTableBtachAddRebuildThrd::run()
{
	aos_assert_r(mIIL, false);
	i64 crt_idx = mCrtIdx;
	i64 num_handle = mNumHandle;
	i64 orig_idx = mOrigIdx;
	i64 orig_handle = mOrigHandle;
	mIIL->batchAddRebuild(mArray, crt_idx, num_handle,
		mOrigArray, orig_idx, orig_handle, mIILSize, mRundata);
	mIIL->setDirty(true);
	mIIL->saveToLocalFilePriv(mRundata);
	return true;
}

