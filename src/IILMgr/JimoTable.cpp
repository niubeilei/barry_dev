////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created By Ken Lee, 2014/07/10
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/JimoTable.h"

#include "DfmUtil/DfmDocIIL.h"
#include "IILMgr/IILMgr.h"
#include "IILUtil/IILSave.h"


/*
#include "Debug/ExitHandler.h"
#include "DocUtil/DocUtil.h"
#include "TransUtil/IILTrans.h"
#include "IILMgr/IILHit.h"
#include "IILMgr/IILStr.h"
#include "IILMgr/IILU64.h"
#include "IILMgr/IILBigStr.h"
#include "IILMgr/IILBigU64.h"
#include "Porting/TimeOfDay.h"
#include "SEUtil/Passwords.h"
#include "SEUtil/SeConfig.h"
#include "SEUtil/Siteid.h"
#include "StorageMgrUtil/DevLocation.h"
#include "StorageMgr/StorageMgr.h"
#include "IILUtil/IILSave.h"
#include "WordClient/WordClient.h"
#include "Porting/ThreadDef.h"
#include "IdGen/IdGen.h"





AosJimoTable::~AosJimoTable()
{
	sgLock.lock();
	sgNumIILCreated--;
	// OmnScreen << "-------iil deleteed :" << mIILID << "," << sgNumIILCreated << ":" << this << endl;
	sgLock.unlock();

	if (mDocids)
	{
		aos_assert(mMemCap > 0);
		returnMemory((char *)mDocids, mMemCap);
		mDocids = 0;
		mMemCap = 0;
	}

	if (mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}

	if (mIILIds)
	{
		OmnDelete [] mIILIds;
		mIILIds = 0;
	}
	
	//ken 2011/08/19
	mTrans.clear();
}


i64 
AosJimoTable::staticNumIILRefs()
{
	return sgNumIILRefs;
}


bool			
AosJimoTable::staticInit(const AosXmlTagPtr &theconfig)
{
	for(int i=0; i<eAosIILType_Total; i++)
	{
		mTotalIILs[i] = 0;
	}
	
	// Its configuration is in the subtag "AOSCONFIG_IILMGR"
	aos_assert_r(theconfig, false);
	AosXmlTagPtr config = theconfig->getFirstChild(AOSCONFIG_IILMGR);
	if (!config) return true;

	mMinIILSize = config->getAttrU64(AOSCONFIG_MIN_IILSIZE, eMinIILSize);
	aos_assert_r(mMinIILSize > 0, false);

	mMaxNormIILSize = config->getAttrU64(AOSCONFIG_MAX_NORM_IILSIZE, eMaxIILSize);
	aos_assert_r(mMaxNormIILSize > 0, false);
	aos_assert_r(mMinIILSize < mMaxNormIILSize, false);

	mMaxBigIILSize = config->getAttrU64(AOSCONFIG_MAX_BIG_IILSIZE, eMaxIILSize);
	aos_assert_r(mMaxBigIILSize > 0, false);
	aos_assert_r(mMinIILSize < mMaxBigIILSize, false);

	mMinSubIILs = config->getAttrU64(AOSCONFIG_MIN_SUBIILSIZE, eMinSubIILs);
	aos_assert_r(mMinSubIILs > 0, false);

	mMaxSubIILs = config->getAttrU64(AOSCONFIG_MAX_SUBIILSIZE, eMaxSubIILs);
	aos_assert_r(mMaxSubIILs > 0, false);
	aos_assert_r(mMinSubIILs < mMaxSubIILs, false);

	// Chen Ding DDDDD, 2013/01/26
	smNeedCompress = config->getAttrBool("compress_iil", true);
	i64 nn = config->getAttrInt64("compress_size", eCompressSize);
	if (nn < 0) smCompressSize = eCompressSize;
	else smCompressSize = nn;
	
	// Ken Lee 2013/01/03
	mAddBlockMaxSize = mMaxBigIILSize;
	mAddBlockMinSize = mMaxBigIILSize * 0.2;
	mAddBlockFillSize = mMaxBigIILSize * 0.8;

	return true;
}


AosJimoTableObjPtr
AosJimoTable::staticLoadFromFile(
		const u64 &iilid,
		const u64 &snap_id,
		const AosRundataPtr &rdata) 
{
	// It creates a new instance of IIL and load it from file.
	// This function is thread safe. There is no need to lock
	// it. The actual work is done by 'staticReadHeaderBuff(...)'.
	// The header has already been read. It is in 'iil_header'.
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(iilid, local_iilid, rdata);
	aos_assert_r(docfilemgr, 0);

	AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
		rdata, docfilemgr, snap_id, local_iilid, true);
	if (!doc) return 0;
	
	AosDfmDocIILPtr iildoc = (AosDfmDocIIL*)doc.getPtr();
	AosIILType iiltype = iildoc->getIILType();

	AosIILObjPtr iil;
	u32 siteid = rdata->getSiteid();

	switch (iiltype)
	{
	case eAosIILType_Hit:
		 iil = OmnNew AosIILHit(iilid, siteid, iildoc, rdata);
		 break;

	case eAosIILType_Str:
		 iil = OmnNew AosIILStr(iilid, siteid, iildoc, rdata);
		 break;

	case eAosIILType_U64:
		 iil = OmnNew AosIILU64(iilid, siteid, iildoc, rdata);
		 break;

	case eAosIILType_BigStr:
		 iil = OmnNew AosIILBigStr(iilid, siteid, iildoc, rdata);
		 break;

	case eAosIILType_BigU64:
		 iil = OmnNew AosIILBigU64(iilid, siteid, iildoc, rdata);
		 break;

	default:
		 OmnAlarm << "Unrecognized IIL type: " << iiltype 
			 << ":" << iilid << enderr;
		 return 0;
	}
	
	aos_assert_r(iil && iil->isGood(), 0);

	iil->setSnapShotId(snap_id);

	return iil;
}


AosIILObjPtr
AosIIL::staticCreateIIL(
	const u64 &wordid,
	const u64 &iilid, 
	const u32 siteid, 
	const AosIILType iiltype,
	const bool isPersis,
	const OmnString &iilname, 
	const AosRundataPtr &rdata)
{
	// There is no need to lock it.
	AosIILObjPtr iil;
	switch (iiltype)
	{
	case eAosIILType_Hit:
		 iil = OmnNew AosIILHit(wordid, iilid, isPersis, iilname, rdata);
		 break;

	case eAosIILType_Str:
		 {
		 	 iil = OmnNew AosIILStr(wordid, iilid, isPersis, iilname, rdata);
			 const char * name = iilname.data();
			 if (AosDocUtil::isNumAlpha(name, iilname.length()))
			 {
			 	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
				striil->setNumAlpha();
			 }
		 }
		 break;
	
	case eAosIILType_NumAlpha:
		 {
			 iil = OmnNew AosIILStr(wordid, iilid, isPersis, iilname, rdata);
			 AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
			 striil->setNumAlpha();
		 }
		 break;
	case eAosIILType_U64:
		 iil = OmnNew AosIILU64(wordid, iilid, isPersis, iilname, rdata);
		 break;
	
	case eAosIILType_BigStr:
		 iil = OmnNew AosIILBigStr(wordid, iilid, isPersis, iilname, rdata);
		 break;

	case eAosIILType_BigU64:
		 iil = OmnNew AosIILBigU64(wordid, iilid, isPersis, iilname, rdata);
		 break;

	default:
		 OmnAlarm << "Unrecognized IIL Type: " << iiltype << enderr;
		 return 0;
	}

	aos_assert_r(iil && iil->isGood(), 0);
	iil->setSiteid(siteid);
	return iil;
}


AosIILObjPtr
AosIIL::staticCreateIIL(
	const AosIILType iiltype,
	const AosRundataPtr &rdata)
{
	// There is no need to lock it.
	AosIILObjPtr iil;
	switch (iiltype)
	{
	case eAosIILType_Hit:
		 iil = OmnNew AosIILHit();
		 break;

	case eAosIILType_Str:
		 {
		 	 iil = OmnNew AosIILStr();
		 }
		 break;
	
	case eAosIILType_NumAlpha:
		 {
			 iil = OmnNew AosIILStr();
			 AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
			 striil->setNumAlpha();
		 }
		 break;
	case eAosIILType_U64:
		 iil = OmnNew AosIILU64();
		 break;
	
	case eAosIILType_BigStr:
		 iil = OmnNew AosIILBigStr();
		 break;

	case eAosIILType_BigU64:
		 iil = OmnNew AosIILBigU64();
		 break;

	default:
		 OmnAlarm << "Unrecognized IIL Type: " << iiltype << enderr;
		 return 0;
	}

	aos_assert_r(iil && iil->isGood(), 0);
	return iil;
}


u64
AosIIL::getDocIdSafe1(
		i64 &idx,
		i64 &iilidx,
		const AosRundataPtr &rdata)
{
	return getDocIdSafe(idx, rdata);
}


u64
AosIIL::nextDocIdSafe2(
		i64 &idx,
		i64 &iilidx,
		const bool reverse)
{
	AOSLOCK(mLock);
	u64 did = nextDocIdPriv2(idx, iilidx, reverse);
	AOSUNLOCK(mLock);
	return did;
}


u64
AosIIL::nextDocIdPriv2(
		i64 &idx,
		i64 &iilidx,
		const bool reverse)
{
	// if the iil is empty
	if (mNumDocs <= 0)
	{
		idx = -5;
		iilidx = -5;	// Chen Ding, 12/23/2010
		return AOS_INVDID;
	}

	iilidx = mIILIdx;
	if (idx == -10)
	{
		// need a new one
		if (reverse)
		{
			idx = mNumDocs - 1;
			return mDocids[idx];
		}
		else
		{
			idx = 0;
			return mDocids[0];
		}
	}
	else
	{
		aos_assert_r(idx >= 0,AOS_INVDID)
		if (reverse)
		{
			idx --;
		}
		else
		{
			idx ++;
		}
	}

	if (idx >= 0 && idx <= mNumDocs)
	{
		return mDocids[idx];
	}
	
	// idx out of range
	idx = -5;
	iilidx = -5;
	return AOS_INVDID;
}


u64
AosIIL::getDocIdSafe(
		const i64 &idx,
		const AosRundataPtr &rdata) const
{
	// This function retrieves the 'idx' docID from the list. 
	// Each IIL is an array of DocIds. Each DocId is eight 
	// bytes long. There is a max size of which the list is 
	// loaded into memory. If this max size is exceeded, the
	// list needs to be loaded in segments. In the current
	// implementation, segmentation is not supported.
	AOSLOCK(mLock);
	u64 did = getDocIdPriv(idx, rdata);
	AOSUNLOCK(mLock);
	return did;
}

u64
AosIIL::getDocIdPriv(
		const i64 &idx,
		const AosRundataPtr &rdata) const
{
	if (!mDocids) 
	{
		return AOS_INVDID;
	}

	if (idx >= 0 && idx < mNumDocs)
	{
		u64 did = mDocids[idx];
		return did;
	}
	return AOS_INVDID;
}


bool
AosIIL::docExistSafe(const u64 &docid, const AosRundataPtr &rdata) const
{
	AOSLOCK(mLock);
//	bool rslt = docExistSafe(docid, rdata);
	AOSUNLOCK(mLock);
	OmnNotImplementedYet;
	return false;
}


bool
AosIIL::returnMemory(const char *mem, const i64 &size) 
{
	OmnDelete [] mem;
	return true;
}


char *
AosIIL::getMemory(
		const i64 &newsize, 
		const char *oldmem, 
		const i64 &oldsize)
{
	aos_assert_r(newsize > oldsize, 0);
	char *mem = OmnNew char[newsize+10];
	aos_assert_r(mem, 0);
	memset(mem, 0, newsize);

	if (oldmem)
	{
		// Copy the contents from the old buffer to 'mem'
		memcpy(mem, oldmem, oldsize);
		OmnDelete [] oldmem;
	}

	return mem;
}


bool
AosIIL::adjustMemoryProtected()
{
	if (mNumDocs < mMemCap) return true;
	i64 newcap = mNumDocs + mExtraDocidsInit;
	u64 *newmem = OmnNew u64[newcap+10];
	aos_assert_r(newmem, 0);
	if (mDocids)
	{
		OmnDelete []mDocids;
		mDocids = 0;
	}
	mDocids = newmem;
	mMemCap = newcap;
	return true;
}


AosBuffPtr
AosIIL::getBodyBuffMem(const i64 &size) const
{
	AosBuffPtr buff = OmnNew AosBuff(size, 0 AosMemoryCheckerArgs);
	return buff;
}


bool
AosIIL::expandDocidMem()
{
	// This function expands the space for the IIL. The new memory
	// will be (mNumDocs + mExtraDocids)*8.
	
	i64 newcapacity = mNumDocs + mExtraDocids;
	if (newcapacity <= mMemCap) return true;
	i64 newsize = newcapacity << 3;

	char *mem = getMemory(newsize, (char *)mDocids, (mMemCap << 3));
	aos_assert_r(mem, false);
	mDocids = (u64 *)mem;
	mMemCap = newcapacity;
	return true;
}


bool
AosIIL::setDocidContents(AosBuffPtr &buff)
{
	bool rslt = buff->getU64s(mDocids, mNumDocs);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIIL::prepareMemoryForReloading()
{
	return adjustMemoryProtected();
}


bool
AosIIL::numEntriesIncrease(const i64 &index)
{
	aos_assert_r(mNumEntries,false);
	aos_assert_r(index>=0 && index < mNumSubiils, false);
	mNumEntries[index]++;
	return true;
}

bool
AosIIL::numEntriesDecrease(const i64 &index)
{
	aos_assert_r(mNumEntries,false);
	aos_assert_r(index>=0 && index < mNumSubiils, false);
	mNumEntries[index]--;
	return true;
}


bool
AosIIL::resetSpec()
{
	return true;
}

bool
AosIIL::moveTo(const i64 &startIdx, i64 &idx, i64 &iilidx)
{
	if (!isParentIIL())
	{
		idx = startIdx;
		iilidx = -1;
		return true;
	}

	idx = startIdx;
	iilidx = 0;
	while(idx > mNumEntries[iilidx] && iilidx < mNumSubiils -1)
	{
		idx -= mNumEntries[iilidx];
		iilidx ++;
	}
	return true;
}


bool
AosIIL::copyDocidsPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const i64 &start,
		const i64 &end)
{
	// Chen Ding, 12/15/2010
	aos_assert_r(mDocids, false);
	aos_assert_r(end < 0 || end < mNumDocs, false);

	i64 s = start;
	if (s<0)s = 0;
	i64 e = end;
	if (e<0)e = mNumDocs -1;
	i64 size = e - s + 1;
	aos_assert_r(size>=0,false);
	
	if (!query_rslt && query_bitmap)
	{
		query_bitmap->appendDocids(&(mDocids[s]),size);
		return true;
	}
	
	bool rslt = true;	
	u64 docid = 0;
	if (query_rslt)
	{
		for(i64 i = s;i <= e;i++)
		{
			docid = mDocids[i];
			if (query_bitmap && (!query_bitmap->checkDoc(docid))) continue;
			query_rslt->appendDocid(docid);
		}
	}

	return rslt;
}


void 
AosIIL::addTrans(const AosIILTransPtr &trans)
{
	// All transactions processed by an IIL are stored (temporarily) in mTrans.
	// When this queue becomes too long, it will save the contents into disks.
	AosTransId gid = trans->getTransId();
	AOSLOCK(mLock);
	mTrans.push_back(gid);
	AOSUNLOCK(mLock);
}


void
AosIIL::finishTrans()
{
	AOSLOCK(mLock);
	vector<AosTransId> trans = mTrans;
	mTrans.clear();
	AOSUNLOCK(mLock);

	if (trans.size() > 0)
	{
		AosFinishTrans(trans);
	}
}


u64		
AosIIL::getIILIDWithCompFlag() 
{
	return mIILID;
}


bool	
AosIIL::queryRangeSafe(
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &query_bitmap,
			const AosQueryContextObjPtr &query_context,
			const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosIIL::deleteFromLocalFilePriv(const AosRundataPtr &rdata)
{
	// This function saves the IIL to file. It does not save its 
	// subiil even if it is a root.
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(mIILID, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);

	if (!mDfmDoc)
	{
		AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
			rdata, docfilemgr, mSnapShotId, local_iilid, false);
		if (!doc) return false;
		mDfmDoc = (AosDfmDocIIL*)doc.getPtr();
	}

	aos_assert_r(mDfmDoc->getDocid() == local_iilid, false);

	bool rslt = AosIILSave::getSelf()->deleteDoc(
		rdata, docfilemgr, mTrans, mDfmDoc, mSnapShotId);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIIL::deleteFromLocalFileByID(
		const u64 iilid,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	// This function saves the IIL to file. It does not save its 
	// subiil even if it is a root.
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(iilid, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);

	AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
		rdata, docfilemgr, snap_id, local_iilid, false);
	aos_assert_r(doc, false);

	bool rslt = AosIILSave::getSelf()->deleteDoc(
		rdata, docfilemgr, mTrans, doc, snap_id);
	aos_assert_r(rslt, false);

	return true;
}


// Ketty, 10/29/2012
bool
AosIIL::saveToLocalFilePriv(const AosRundataPtr &rdata)
{
	// This function saves the IIL to file. It does not save its
	// subiil even if it is a root.
	// 3. Save the body
	bool rslt = false;
	rslt = saveBitmapTree(rdata);
	aos_assert_r(rslt, false);

	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(mIILID, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);

	if (!mDfmDoc)
	{
		AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
			rdata, docfilemgr, mSnapShotId, local_iilid, false);
		if (!doc)
		{
			// means maybe it's a new iil.
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

	AosBuffPtr bodyBuff = getBodyBuffProtected();
	aos_assert_r(bodyBuff, false);
	mDfmDoc->setBodyBuff(bodyBuff);
	mDfmDoc->setNeedCompress( isCompress(bodyBuff->dataLen()) );

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
AosIIL::computeQueryResults(
		const AosRundataPtr &rdata, 
		const OmnString &iilname, 
		const AosQueryContextObjPtr &context, 
		const AosBitmapObjPtr &bitmap, 
		const u64 &query_id, 
		const int physical_id)
{
	OmnShouldNeverComeHere;
	return false;
}


int
AosIIL::getNumIILsCreated()
{
	sgLock.lock();
	int nn = sgNumIILCreated;
	sgLock.unlock();
	return nn;
}

*/

