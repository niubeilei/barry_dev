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
// 	Created By Ken Lee, 2014/09/11
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/JimoTable.h"

#include "IILMgr/IILMgr.h"
#include "IILUtil/IILSave.h"


bool
AosJimoTable::loadFromFileSafe(
		const u64 &iilid, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = loadFromFilePriv(iilid, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosJimoTable::loadFromFilePriv(
		const u64 &iilid,
		const AosRundataPtr &rdata)
{
	mIILID = iilid;

	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = AosIILMgr::getSelf()->getDocFileMgr(
		mIILID, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);

	if (!mDfmDoc)
	{
		AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
			rdata, docfilemgr, mSnapShotId, local_iilid, true);
		if (!doc) return false;

		mDfmDoc = dynamic_cast<AosDfmDocIIL *>(doc.getPtr());
	}

	bool rslt = readFromDfmDoc(mDfmDoc, rdata);
	aos_assert_r(rslt, false);

	mDfmDoc->cleanBodyBuff();
	mIsDirty = false;
	mIsNew = false;

	return true;
}


bool
AosJimoTable::readFromDfmDoc(
		const AosDfmDocIILPtr &doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc->getIILType() == mIILType, false);
	if (doc->getIILID() != mIILID)
	{
		OmnAlarm << "IILID mismatch: " << ":" << mIILID << enderr;
		failInLoadProtection();
		return false;
	}

	mWordId = doc->getWordId();
	mFlag = doc->getFlag();
	mHitcount = doc->getHitCount();
	mVersion = doc->getVersion();
	mIsPersis = doc->getPersis();
	mNumDocs = doc->getNumDocs();

	mParentIIL = 0;
	mIILIdx = -1;
	mNumSubiils = 0;

	AosBuffPtr bodybuff = doc->getBodyBuff();
	aos_assert_r(bodybuff, false);

	bodybuff->reset();
	bool rslt = setContentsProtected(bodybuff, rdata);
	if (!rslt)
	{
		OmnAlarm << "Can not read body buff" << enderr;
		failInLoadProtection();
		return false;
	}

	return true;
}


void
AosJimoTable::resetSafe(
		const u64 &wordid, 
		const u64 &iilid,
		const u32 siteid,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	resetPriv(wordid, iilid, siteid, rdata);
	AOSUNLOCK(mLock);
}


void
AosJimoTable::resetPriv(
		const u64 &wordid, 
		const u64 &iilid,
		const u32 siteid,
		const AosRundataPtr &rdata)
{
	mWordId = wordid;
	mIILID = iilid;
	mSiteid = siteid;
	resetSelf(rdata);
}


void
AosJimoTable::resetSelf(const AosRundataPtr &rdata)
{
	bool rslt = returnSubIILsPriv(rdata);
	aos_assert(rslt);

	rslt = resetSubIILInfo(rdata);
	aos_assert(rslt);

	mFlag = AOS_ACTIVE_FLAG;
	mHitcount = 0;
	mCompressedSize = 0;

	mNumDocs = 0;
	mMemSize = 0;

	mData = 0;

	mDfmDoc = 0;
	mTrans.clear();
	mSnapShotId = 0;
}


void
AosJimoTable::setSnapShotId(const u64 &snap_id)
{
	AOSLOCK(mLock);
	setSnapShotIdPriv(snap_id);
	AOSUNLOCK(mLock);
}


void
AosJimoTable::setSnapShotIdPriv(const u64 &snap_id)
{
	aos_assert(mSnapShotId == 0 || snap_id == mSnapShotId);

	mSnapShotId = snap_id;

	if (!isParentIIL()) return;
		
	AosJimoTablePtr subiil;
	for (i64 i=0; i<mNumSubiils; i++)
	{
		subiil = mSubiils[i];
		if (!subiil) continue;
		subiil->setSnapShotIdPriv(snap_id);
	}
}


void
AosJimoTable::resetSnapShotId()
{
	AOSLOCK(mLock);
	resetSnapShotIdPriv();
	AOSUNLOCK(mLock);
}


void
AosJimoTable::resetSnapShotIdPriv()
{
	mSnapShotId = 0;	

	if (!isParentIIL()) return;

	AosJimoTablePtr subiil;
	for (i64 i=0; i<mNumSubiils; i++)
	{
		subiil = mSubiils[i];
		if (!subiil) continue;
		subiil->resetSnapShotIdPriv();
	}
}


void
AosJimoTable::failInSetCtntProtection()
{
}


void 
AosJimoTable::failInLoadProtection()
{
	failInSetCtntProtection();

	mIILID = 0;
	mSiteid = 0;

    mWordId = 0;
    mHitcount = 0;
    mVersion = 0;
	mIsPersis = false;
	mCompressedSize = 0;

	mIsDirty = false;
	mIsNew = false;
	mIsGood = false;
	
	mNumDocs = 0;
	mMemSize = 0;

	mParentIIL = 0;
	mIILIdx = -1;
	mNumSubiils = 0;
}


bool
AosJimoTable::returnSubIILsPriv(
		bool &returned,
		const bool returnHeader,
		const AosRundataPtr &rdata)
{
	returned = true;

	if (!isParentIIL()) return true;

	AosJimoTablePtr subiil = 0;
	bool subIILReturned = false;
	bool rslt = false;

	for (i64 i=0; i<mNumSubiils; i++)
	{
		subiil = mSubiils[i];
		if (!subiil || subiil->isDirty()) continue;
			
		rslt = AosIILMgrSelf->returnIILPublic(
			subiil, subIILReturned, returnHeader, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(subIILReturned, false);
		mSubiils[i] = 0;
	}
	return true;
}


bool
AosJimoTable::resetSubIILInfo(const AosRundataPtr &rdata)
{
	if (mSubiils)
	{
		OmnDelete [] mSubiils;
		mSubiils = 0;
	}

	if (mIILIds)
	{
		OmnDelete [] mIILIds;
		mIILIds = 0;
	}

	if (mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}

	if (mSubMemSize)
	{
		OmnDelete [] mSubMemSize;
		mSubMemSize = 0;
	}

	mMinData = 0;
	mMaxData = 0;

	mParentIIL = 0;
	mIILIdx = -1;
	mNumSubiils = 0;

	return true;
}


bool
AosJimoTable::saveToFileSafe(const AosRundataPtr &rdata)
{
	aos_assert_r((isRootIIL() || isSingleIIL()), false);

	AOSLOCK(mLock);
	if (!mIsDirty)
	{
		AOSUNLOCK(mLock);
		return true;
	}

	rdata->setSiteid(mSiteid);
	bool rslt = saveToFilePriv(rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosJimoTable::saveToFilePriv(const AosRundataPtr &rdata) 
{
	bool rslt = true;
	if (!isBigIIL())
	{
		rslt = addId(rdata);
		aos_assert_r(rslt, false);
	}

	if (isParentIIL())
	{
		rslt = saveSubIILToLocalFileSafe(rdata);
		aos_assert_r(rslt, false);
	}

	rslt = saveToLocalFilePriv(rdata);
	aos_assert_r(rslt, false);

	if(!isBigIIL())
	{
		rslt = commitId(rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosJimoTable::saveToLocalFileSafe(const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = true;
	if (mIsDirty)
	{
		rslt = saveToLocalFilePriv(rdata);
		mIsDirty = false;
	}
	AOSUNLOCK(mLock);
	return rslt;
}	


bool
AosJimoTable::saveToLocalFilePriv(const AosRundataPtr &rdata)
{
	bool rslt = saveBitmapTree(rdata);
	aos_assert_r(rslt, false);

	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = AosIILMgr::getSelf()->getDocFileMgr(
		mIILID, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);

	if (!mDfmDoc)
	{
		AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
			rdata, docfilemgr, mSnapShotId, local_iilid, false);
		if (!doc)
		{
			mDfmDoc = OmnNew AosDfmDocIIL(local_iilid, mIILType, 
				mIILID, mWordId, mVersion, mIsPersis,
				mHitcount, mNumDocs, mFlag);
		}
		else
		{
			mDfmDoc = (AosDfmDocIIL *)doc.getPtr();
			mDfmDoc->setHitCount(mHitcount);
			mDfmDoc->setNumDocs(mNumDocs);
			mDfmDoc->setFlag(mFlag);
		}
	}
	else
	{
		mDfmDoc->setHitCount(mHitcount);
		mDfmDoc->setNumDocs(mNumDocs);
		mDfmDoc->setFlag(mFlag);
	}

	AosBuffPtr bodyBuff = getBodyBuffProtected(rdata);
	aos_assert_r(bodyBuff, false);

	mDfmDoc->setBodyBuff(bodyBuff);
	mDfmDoc->setNeedCompress(AosIIL::isCompress(bodyBuff->dataLen()));

	rslt = AosIILSave::getSelf()->saveDoc(
		rdata, docfilemgr, mTrans, mDfmDoc, mSnapShotId);
	aos_assert_r(rslt, false);

	mDfmDoc->cleanBodyBuff();
	mIsDirty = false;
	mIsNew = false;

	clearTrans();
	return true;
}


bool
AosJimoTable::saveSubIILToLocalFileSafe(const AosRundataPtr &rdata)
{
	if (!isParentIIL()) return true;
		
	bool rslt = true;
	for (i64 i=0; i<mNumSubiils; i++)
	{
		if (!mSubiils[i].notNull()) continue;
			
		rslt = mSubiils[i]->saveToLocalFileSafe(rdata);
		aos_assert_r(rslt, false);

		if (mSubiils[i]->isParentIIL())
		{
			rslt = mSubiils[i]->saveSubIILToLocalFileSafe(rdata);
			aos_assert_r(rslt, false);
		}
	}

	return true;
}


bool
AosJimoTable::setContentsProtected(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);

	mLevel = buff->getInt(0);
	aos_assert_r(mLevel >= 0, false);

	mMemSize = buff->getI64(0);
	aos_assert_r(mMemSize >= 0, false);

	OmnString cmp_str = buff->getOmnStr("");
	aos_assert_r(cmp_str != "", false);
	mCmpTag = AosXmlParser::parse(cmp_str AosMemoryCheckerArgs);
	aos_assert_r(mCmpTag, false);

	if (isLeafIIL())
	{
		return setContentsSinglePriv(buff, rdata);
	}

	mNumSubiils = buff->getI64(0);
	if (mNumSubiils <= 1 || mNumSubiils > AosIIL::mMaxSubIILs)
	{
		OmnAlarm << "Failed to retrieve mNumSubiils:" << mNumSubiils << enderr;
		mNumSubiils = 0;
		return false;
	}

	bool rslt = true;
	if (!mSubiils)
	{
		rslt = createSubiilIndex();
		aos_assert_r(rslt, false);
	}

	mMinData = AosBuffArrayVar::create(mCmpTag, true, rdata);
	aos_assert_r(mMinData, false);
	rslt = mMinData->getDataFromBuff(buff);
	aos_assert_r(rslt, false);

	mMaxData = AosBuffArrayVar::create(mCmpTag, true, rdata);
	aos_assert_r(mMaxData, false);
	rslt = mMaxData->getDataFromBuff(buff);
	aos_assert_r(rslt, false);

	rslt = buff->getU64s(mIILIds, mNumSubiils);
	aos_assert_r(rslt, false);

	rslt = buff->getI64s(mNumEntries, mNumSubiils);
	aos_assert_r(rslt, false);
	
	rslt = buff->getI64s(mSubMemSize, mNumSubiils);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosJimoTable::setContentsSinglePriv(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	aos_assert_r(mNumDocs <= AosIIL::mMaxBigIILSize, false);

	mData = AosBuffArrayVar::create(mCmpTag, true, rdata);
	aos_assert_r(mData, false);

	bool rslt = mData->getDataFromBuff(buff);
	aos_assert_r(rslt, false);

	return true;	
}


AosBuffPtr 
AosJimoTable::getBodyBuffProtected(const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return getBodyBuffSinglePriv(rdata);
	}

	i64 expect_size = mNumSubiils * 120; 
	if (expect_size <= 5000) expect_size = 5000;
	AosBuffPtr buff = OmnNew AosBuff(expect_size AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	bool rslt = buff->setInt(mLevel);
	aos_assert_r(rslt, 0);

	rslt = buff->setI64(mMemSize);
	aos_assert_r(rslt, 0);

	aos_assert_r(mCmpTag, 0);
	OmnString cmp_str = mCmpTag->toString();
	aos_assert_r(cmp_str != "", 0);
	rslt = buff->setOmnStr(cmp_str);
	aos_assert_r(rslt, 0);

	rslt = buff->setI64(mNumSubiils);
	aos_assert_r(rslt, 0);

	rslt = mMinData->setDataToBuff(buff);
	aos_assert_r(rslt, 0);

	rslt = mMaxData->setDataToBuff(buff);
	aos_assert_r(rslt, 0);

	rslt = buff->setU64s(mIILIds, mNumSubiils);
	aos_assert_r(rslt, 0);

	rslt = buff->setI64s(mNumEntries, mNumSubiils);
	aos_assert_r(rslt, 0);

	rslt = buff->setI64s(mSubMemSize, mNumSubiils);
	aos_assert_r(rslt, 0);

	return buff;
}


AosBuffPtr 
AosJimoTable::getBodyBuffSinglePriv(const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs <= AosIIL::mMaxBigIILSize, 0);

	i64 expect_size = mNumDocs * 60; 
	if (expect_size <= 5000) expect_size = 5000;
	AosBuffPtr buff = OmnNew AosBuff(expect_size AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	bool rslt = buff->setInt(mLevel);
	aos_assert_r(rslt, 0);

	rslt = buff->setI64(mMemSize);
	aos_assert_r(rslt, 0);

	aos_assert_r(mCmpTag, 0);
	OmnString cmp_str = mCmpTag->toString();
	aos_assert_r(cmp_str != "", 0);
	rslt = buff->setOmnStr(cmp_str);
	aos_assert_r(rslt, 0);

	rslt = mData->setDataToBuff(buff);
	aos_assert_r(rslt, 0);
	
	return buff;
}


bool
AosJimoTable::addId(const AosRundataPtr &rdata)
{
	bool rslt = true;
	if (mSnapShotId != 0) 
	{
		rslt = AosIILSave::getSelf()->isSmallIILTrans(mSnapShotId);
		aos_assert_r(!rslt, false);
		return true;
	}

	aos_assert_r((isRootIIL() || isSingleIIL()), false);

	u64 snap_id = 0;
	rslt = AosIILSave::getSelf()->add(rdata, mIILID, snap_id);
	aos_assert_r(rslt, false);

	aos_assert_r(snap_id != 0, false);
	setSnapShotIdPriv(snap_id);
	return true;
}


bool
AosJimoTable::commitId(const AosRundataPtr &rdata)
{
	aos_assert_r(mSnapShotId, false);

	bool rslt = AosIILSave::getSelf()->isSmallIILTrans(mSnapShotId);
	if (!rslt) return true;

	aos_assert_r((isRootIIL() || isSingleIIL()), false);

	u64 snap_id = mSnapShotId;
	aos_assert_r(snap_id, false);

	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = AosIILMgr::getSelf()->getDocFileMgr(
		mIILID, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);

	rslt = AosIILSave::getSelf()->commit(rdata, mIILID, docfilemgr, snap_id);
	aos_assert_r(rslt, false);

	resetSnapShotIdPriv();
	return true;
}


void 
AosJimoTable::countDirtyRec(
		int &parent_changed, 
		int &leaf_changed,
		const AosRundataPtr &rdata)
{
	if (mLevel == 0)
	{
		if (mIsDirty) leaf_changed++;
		return;
	}	
	
	// is parent
	if (mIsDirty) parent_changed++;
	for (i64 i = 0; i < mNumSubiils; i++)
	{
		if (mSubiils[i])
		{
			mSubiils[i]->countDirtyRec(parent_changed, leaf_changed, rdata);
		}
	}
}


bool		
AosJimoTable::saveLeafToFileSafe(
		int &numToSave,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = saveLeafToFilePriv(numToSave, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool		
AosJimoTable::saveLeafToFilePriv(
		int &numToSave,
		const AosRundataPtr &rdata)
{
	if (isRootIIL())
	{
		rdata->setSiteid(mSiteid);
	}
	return saveLeafToFileRecPriv(numToSave, rdata);
}


bool		
AosJimoTable::saveLeafToFileRecSafe(
		int &numToSave,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = saveLeafToFileRecPriv(numToSave, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool		
AosJimoTable::saveLeafToFileRecPriv(
		int &numToSave,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (mLevel > 1)
	{
		for (i64 i = 0; i < mNumSubiils; i++)
		{
			if (mSubiils[i])
			{
				mSubiils[i]->saveLeafToFileRecSafe(numToSave, rdata);
				if (numToSave <= 0) return true;
			}
		}
		return true;
	}
	
	if (mLevel == 0)
	{
		return saveToLocalFileSafe(rdata);
	}
	
	if (mLevel == 1)
	{
		for (i64 i = 0;i < mNumSubiils;i++)
		{
			if (mSubiils[i])
			{
				if (mSubiils[i]->isDirty())
				{
					rslt = mSubiils[i]->saveToLocalFileSafe(rdata);
					aos_assert_r(rslt, false);
					numToSave --;
				}
				
				// return iil
				AosIILObjPtr iil = dynamic_cast<AosJimoTable *>(mSubiils[i].getPtr());
				rslt = AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
				aos_assert_r(rslt,false);				

				mSubiils[i] = 0;
				if (numToSave <= 0)
				{
					return true;
				}
			}
		}
	}
	return true;
}





/*
bool
AosIILBigStr::returnSubIILPriv(
		const i64 &idx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(idx >= 0 && idx < mNumSubiils, false);

	AosIILBigStrPtr subiil = mSubiils[idx];
	if (!subiil) return true;
	if (subiil->isDirty()) return true;

	AosIILObjPtr iil = dynamic_cast<AosIIL *>(subiil.getPtr());
	bool rslt = AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
	aos_assert_r(rslt, false);
	mSubiils[idx] = 0;
	return true;
}


bool
AosIILBigStr::prepareMemoryForReloading()
{
	// The IIL may have been reused, which means that it
	// may have allocated some memory. This function checks 
	// whether it has enough memory (based on 'mNumDocs'). 
	// If not enough, it will allocate. 
	// Otherwise, it may thrink the memory.
	//
	// Note that 'mNumDocs' is the number of docs this new
	// IIL has, not the one used by the previous IIL. But
	// 'mMemCap' is the one set by the previous IIL. 

	if (mNumDocs < mMemCap)
	{
		// It has enough memory. If it has more than what we need, 
		// thrink it. We only need 'mNumDocs + mExtraDocids'. Release
		// the extra ones.
		for (i64 i=mNumDocs + mExtraDocids; i<mMemCap; i++)
		{
			if (!mValues[i]) break;
			
			OmnMemMgrSelf->release(mValues[i], __FILE__, __LINE__);
			mValues[i] = 0;
		}

		// IMPORTANT: the memory held in 'mValues[i]' are for 
		// the previous IIL, not this IIL's!!!
		return true;
	}

	// Grow 'mValues' now
	i64 newsize = mNumDocs + mExtraDocids;
	char **mem = OmnNew char *[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(char*) * newsize);

	if (mValues)
	{
		memcpy(mem, mValues, sizeof(char *) * mMemCap);
		OmnDelete [] mValues;
	}

	mValues = mem;

	bool rslt = expandDocidMem();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILBigStr::resetSpec()
{
	mIsNumAlpha = false;
	// Ken Lee, 2013/01/11
	if (mNumDocs != 0 && isLeafIIL())
	{
		aos_assert_r(mValues, false);
		for(i64 i=0; i<mNumDocs; i++)
		{
			aos_assert_r(mValues[i], false);
			OmnMemMgrSelf->release(mValues[i], __FILE__, __LINE__);
			// mValues[i] = 0;
		}
	}
	
	OmnDelete [] mValues;
	mValues = 0;
	if(mBitmapTree)
	{
		mBitmapTree = 0;
	}
	return true;
}


*/
