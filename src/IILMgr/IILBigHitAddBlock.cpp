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
// 	Created: 2012/04/16 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/IILBigHit.h"

#include "API/AosApiS.h"
#include "IILMgr/IILMgr.h"
#include "BitmapMgr/Bitmap.h"
#include "IILMgr/IILThrdShellProc.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/BitmapTreeObj.h"
#include "SEInterfaces/BitmapTreeMgrObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/QueryRsltObj.h"

bool
AosIILBigHit::batchAddSafe(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	AosBitmapObjPtr bitmap = AosGetBitmap();
	aos_assert_r(size  ==(int)sizeof(u64),false);
	u64* cur_pos = (u64*)entries;
	for(i64 i = 0;i < num-1;i++)
	{
		aos_assert_r((*cur_pos) <= (*(cur_pos+1)),false);
		cur_pos += 1;
	}
	bitmap->appendDocids((u64*)entries, num);

	AOSLOCK(mLock);

	i64 start_total = mNumDocs;
	u64 start_time = OmnGetTimestamp();
	bool rslt = batchAddPriv(bitmap, size, num, rdata);
	u64 cost = OmnGetTimestamp() - start_time;
	i64 end_total = mNumDocs;

	OmnString num_str;
	num_str << num;
	num_str.convertToScientific();
	OmnString total_str;
	total_str << mNumDocs;
	total_str.convertToScientific();

	OmnString ss;
	ss << "IILID: " << mIILID
		<< ", Entries: " << num_str 
		<< ", Total: " << total_str
		<< ", Time: " << AosTimestampToHumanRead(cost);
	rdata->setArg1("batch_add_stat", ss);

	OmnScreen << "\n\n================================="
			  << "\nBatch Add Finish, iilid:" << mIILID << ", entry num:" << num
			  << ", time cost:" << AosTimestampToHumanRead(cost)
			  << ", start_total:" << start_total << ", end_total:" << end_total
			  << "\n=================================\n" << endl;
	
	AOSUNLOCK(mLock);

	return rslt;
}
	
bool
AosIILBigHit::batchAddPriv(
		AosBitmapObjPtr bitmap,
		const i64 &size,
		const i64 &num,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sanityCheckPriv(rdata), false);

	bool rslt = true;
	i64 subChanged = 0;
	i64 cur_num = num;
	
	while (cur_num > 0)
	{
		bool isTail = true;
		rslt = batchAddRecPriv(bitmap, size,
			cur_num, subChanged, isTail, rdata);
		aos_assert_r(rslt, false);

		// Save the changes when either there are too many processed or all contents
		// were processed.

		if (cur_num <= 0 || subChanged >= (int)eMaxSubChangedToSave - 1)
		{
			int parent_changed = 0;
			int child_changed = 0;
			countDirtyRec(parent_changed, child_changed, rdata);

			if (cur_num <= 0 || parent_changed >= (int)eMaxSubChangedToSave)
			{
				rslt = saveToFilePriv(rdata);
				aos_assert_r(rslt, false);

				rslt = returnSubIILsPriv(rdata);
				aos_assert_r(rslt, false);

				subChanged = 0;
				if (getDistrType() == eDistrType1 && getDistrStatus() == eDistr_Local && mNumDocs > eNumDocsToDistr)
				{
					//rslt = distributeIILPriv(rdata);
					//aos_assert_r(rslt, false);
				}
			}
			else
			{
				int child_to_change = child_changed - 1;
				rslt = saveLeafToFilePriv(child_to_change, rdata);
				aos_assert_r(rslt, false);				
				subChanged = 0;			
			}
		}
	}

	aos_assert_r(sanityCheckPriv(rdata), false);
	return true;
}


bool
AosIILBigHit::batchAddRecSafe(
		AosBitmapObjPtr bitmap,
		const i64 &size,
		i64 &num,
		i64 &subChanged,
		const bool isTail,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = batchAddRecPriv(bitmap, size,
		num, subChanged, isTail, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}

bool
AosIILBigHit::batchAddRecPriv(
		AosBitmapObjPtr bitmap,
		const i64 &size,
		i64 &num,
		i64 &subChanged,
		const bool isTail,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return batchAddSinglePriv(bitmap, size,
			num, subChanged, isTail, rdata);
	}

	aos_assert_r(num > 0, false);
	u64 entry_docid;
	bool rslt = false;

//	rslt = bitmap->firstDocid(entry_docid);
//////////////////
bitmap->reset();
rslt = bitmap->nextDocid(entry_docid);
/////////////////
	aos_assert_r(rslt, false);
	i64 sub_idx = getSubiilIndex3Priv(entry_docid, rdata);
	aos_assert_r(sub_idx >= 0 && sub_idx < mNumSubiils, false);

	i64 sub_iilid = mIILIds[sub_idx];
	bool it = isTail && (sub_idx == (mNumSubiils - 1));

	if (mLevel == 1)
	{
		OmnThrdShellProcPtr runner;
		vector<OmnThrdShellProcPtr> runners;

		while (sub_iilid && subChanged < (int)eMaxSubChangedToSave && num > 0)
		{
			i64 num_handle = num;
			if (!it)
			{
				num_handle = binarySearch(bitmap, size, num, mMaxDocids[sub_idx]) + 1;
				if(num_handle <= 0)
				{
					// the level 0 is the last but the level 1 is not the last one
					sub_iilid = 0;
					break;
				}
			}

			i64 max_handle = (eMaxSubChangedToSave - subChanged + 1) * mAddBlockFillSize - mNumEntries[sub_idx];
			if (num_handle > max_handle)
			{
				num_handle = max_handle;
			}
	
			i64 total = num_handle + mNumEntries[sub_idx];
			i64 subToAdd = (total - 1) / mAddBlockFillSize;
			if (total < mAddBlockMaxSize)
			{
				subToAdd = 0;
			}

			AosIILBigHitPtr thisptr(this, false);
			runner = OmnNew AosIILBigHitBtachAddSingleThrd(
				thisptr, sub_iilid, sub_idx, bitmap,
				size, num_handle, rdata);
			runners.push_back(runner);

			//OmnScreen << "iilid : " << sub_iilid << ", iilidx:" << sub_idx << ", iil_entrynum:" << mNumEntries[sub_idx] 
			//	<< ", num_handle:" << num_handle << ", max_handle:" << max_handle << ", isTail:" << it << endl;

			subChanged += (1 + subToAdd);
			num -= num_handle;
			if (num > 0)
			{
				AosBitmapObjPtr tmp_bitmap = AosGetBitmap();
				//rslt = bitmap->getNDocids(0, num_handle, tmp_bitmap, true);
/////////////////
vector<u64> tmp_docids;
rslt = bitmap->getDocids(tmp_docids);
tmp_docids.resize(num_handle);
tmp_bitmap->appendDocids((u64*)(&tmp_docids[0]), num_handle);
////////////////
				aos_assert_r(rslt, false);
				rslt = bitmap->removeBits(tmp_bitmap);
				aos_assert_r(rslt, false);
			}

			sub_iilid = 0;
			if (!it && num > 0)
			{
				sub_idx = getSubiilIndex3Priv(entry_docid, rdata);
				if (sub_idx >= 0 && sub_iilid < mNumSubiils)
				{
					sub_iilid = mIILIds[sub_idx];
					it = isTail && (sub_idx == (mNumSubiils - 1));
				}
			}
		}
			
		//OmnThreadPool::runProcSync(runners);
		AosIILMgr::smThreadPool->procSync(runners);

		bool rslt = subiilSplited(0, rdata);
		aos_assert_r(rslt, false);

		rslt = returnSubIILsPriv(rdata);
		aos_assert_r(rslt, false);

		return true;
	}

	AosIILBigHitPtr subiil = getSubiilByIndexPriv(sub_idx, rdata);
	aos_assert_r(subiil, false);

	i64 num_before = num;
	rslt = subiil->batchAddRecSafe(bitmap,
		size, num, subChanged, it, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(num_before > num, false);

	return true;
}

//liuwei_hit
bool
AosIILBigHit::batchAddSinglePriv(
		AosBitmapObjPtr bitmap,
		const i64 &size,
		i64 &num,
		i64 &subChanged,
		const bool isTail,
		const AosRundataPtr &rdata)
{
	i64 num_handle = num;
	if (!isTail)
	{
		aos_assert_r(mNumDocs > 0, false);
		u64 last_docid;
		bool rslt = false;
		rslt = mDocBitmap->lastDocid(last_docid);
		aos_assert_r(rslt, false);
		num_handle = binarySearch(bitmap, size, num, last_docid) + 1;
		aos_assert_r(num_handle > 0, false);
	}

	i64 max_handle = (eMaxSubChangedToSave - subChanged + 1) * mAddBlockFillSize - mNumDocs;
	if (num_handle > max_handle)
	{
		num_handle = max_handle;
	}

	i64 total = num_handle + mNumDocs;	
	i64 subToAdd = (total - 1) / mAddBlockFillSize;
	if (total < mAddBlockMaxSize)
	{
		subToAdd = 0;
	}
	i64 subiilsize = (total - 1) / (subToAdd + 1) + 1;
	i64 iilsize = total - subiilsize * subToAdd;

	i64 orig_num = mNumDocs;
	i64 orig_idx = 0;
	bool rslt = false;
	OmnThrdShellProcPtr runner;
	vector<OmnThrdShellProcPtr> runners;

	// judge if are in the tail, can we ignore rebuilding it self?
	// if the first entry is bigger than the last doc in iil, if the iil size is more than fill size
	u64 first_docid;
	u64 last_docid;
/*	
	rslt = bitmap->firstDocid(first_docid);
	aos_assert_r(rslt, false);
*/
/////////////////
bitmap->reset();
rslt = bitmap->nextDocid(first_docid);
aos_assert_r(rslt, false);
////////////////


	if (orig_num >= mAddBlockFillSize && subToAdd > 0)
	{
		//do not rebuild the orig iil
		rslt = mDocBitmap->lastDocid(last_docid);
		aos_assert_r(rslt, false);
		if(first_docid > last_docid)
		{
			total = num_handle;
			subToAdd = (total - 1) / mAddBlockFillSize + 1;
			subiilsize = (total + subToAdd - total % subToAdd) / subToAdd;

			orig_idx = orig_num;
		}
	}
	else
	{
		i64 new_handle = 0;
		i64 orig_handle = 0;

		//OmnScreen << "rebuild self count, num:" << num_handle << ", orig_num:"
		//	<< (orig_num - orig_idx) << ", subiilsize:" << subiilsize << endl;

		rslt = batchAddRebuildCount(bitmap, size, num_handle, new_handle, 
			mDocBitmap, orig_num - orig_idx, orig_handle, iilsize, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(new_handle + orig_handle > 0, false);
		aos_assert_r(new_handle <= num_handle, false);
		aos_assert_r(orig_handle <= orig_num - orig_idx, false);

		//OmnScreen << "rebuild self, iilid : " << mIILID << ", new_handle:"
		//	<< new_handle << ", orig_handle:" << orig_handle << endl;

		AosIILBigHitPtr thisptr(this, false);
		
		runner = OmnNew AosIILBigHitBtachAddRebuildThrd(
			thisptr, bitmap, size, new_handle, mDocBitmap, orig_handle, iilsize, rdata);
		runners.push_back(runner);

		num -= new_handle;
		num_handle -= new_handle;
		orig_idx += orig_handle;

	}

	vector<AosIILBigHitPtr> subiil_list;
	if (subToAdd > 0)
	{
		// create new subiils
		AosIILBigHitPtr newsub;
		for (i64 i=0; i<subToAdd; i++)
		{
			newsub = splitContentUtil(rdata);

			i64 new_handle = 0;
			i64 orig_handle = 0;

			//OmnScreen << "rebuild count, num:" << num << ", orig_num:"
			//	<< (orig_num - orig_idx) << ", subiilsize:" << subiilsize << endl;
				
			rslt = batchAddRebuildCount(bitmap, size, num_handle,
				new_handle, mDocBitmap, orig_num - orig_idx, orig_handle, subiilsize, rdata);
			aos_assert_r(rslt, false);
			aos_assert_r(new_handle + orig_handle > 0, false);
			aos_assert_r(new_handle <= num_handle, false);
			aos_assert_r(orig_handle <= orig_num - orig_idx, false);

			//OmnScreen << "rebuild iilid : " << newsub->getIILID() << ", num_handle:" << num_handle
			//	<< ", orig_handle:" << orig_handle << ", subiilsize:" << subiilsize << endl;
				
			runner = OmnNew AosIILBigHitBtachAddRebuildThrd(
				newsub, bitmap, size, new_handle, mDocBitmap, orig_handle, subiilsize, rdata);
			runners.push_back(runner);
			subiil_list.push_back(newsub);

			num -= new_handle;
			num_handle -= new_handle;
			orig_idx += orig_handle;
		}
		
	}

	//OmnThreadPool::runProcSync(runners);
	AosIILMgr::smThreadPool->procSync(runners);

	if (subToAdd > 0)
	{
		if (isSingleIIL())
		{
			// This IIL is a leaf IIL and has no parent IIL. 
			rslt = splitListSinglePriv(subiil_list, rdata);
			aos_assert_r(rslt, false);
		}
		else
		{
			aos_assert_r(mParentIIL, false);
			rslt = mParentIIL->subiilSplited(mIILIdx, subiil_list, 0, rdata);
			aos_assert_r(rslt, false);
		}
	}
	else
	{
		if (isChildIIL())
		{
			aos_assert_r(mParentIIL, false);
			rslt = mParentIIL->updateIndexData(mIILIdx, true, true, rdata);
			aos_assert_r(rslt, false);
		}
	}
		
	subChanged++;
	subChanged += subToAdd;
	return true;
}

bool
AosIILBigHit::batchAddParentThrdSafe(
		const u64 &iilid,
		const i64 &iil_idx,
		AosBitmapObjPtr bitmap,
		const i64 &size,
		i64 &num,
		const AosRundataPtr &rdata)
{
	AosIILBigHitPtr subiil = getSubiilByIndexPriv(iilid, iil_idx, rdata);
	aos_assert_r(subiil, false);

	bool rslt = subiil->batchAddSingleThrdSafe(
		bitmap, size, num, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILBigHit::batchAddSingleThrdSafe(
		AosBitmapObjPtr bitmap,
		const i64 &size,
		i64 &num,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = batchAddSingleThrdPriv(bitmap, size, num, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}

//liuwei_hit
bool
AosIILBigHit::batchAddSingleThrdPriv(
		AosBitmapObjPtr bitmap,
		const i64 &size,
		i64 &num,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mParentIIL, false);

	i64 total = num + mNumDocs;	
	i64 subToAdd = (total - 1) / mAddBlockFillSize;
	if (total < mAddBlockMaxSize)
	{
		subToAdd = 0;
	}
	i64 subiilsize = (total - 1) / (subToAdd + 1) + 1;
	i64 iilsize = total - subiilsize * subToAdd;

	//OmnScreen << "total:" << total << ", subToAdd:" << subToAdd 
	//	<< ", subiilsize:" << subiilsize << ", iilsize:" << iilsize << endl;

	i64 orig_num = mNumDocs;
	i64 orig_idx = 0;

	bool rslt = false;
	OmnThrdShellProcPtr runner;
	vector<OmnThrdShellProcPtr> runners;

	// judge if are in the tail, can we ignore rebuilding it self?
	// if the first entry is bigger than the last doc in iil, if the iil size is more than fill size
	u64 first_docid;
	u64 last_docid;
/*
	rslt = bitmap->firstDocid(first_docid);
	aos_assert_r(rslt, false);
*/
/////////////////
bitmap->reset();
rslt = bitmap->nextDocid(first_docid);
aos_assert_r(rslt, false);
////////////////

	rslt = mDocBitmap->lastDocid(last_docid);
	aos_assert_r(rslt, false);
	if (orig_num >= mAddBlockFillSize && subToAdd > 0 && 
		first_docid > last_docid)
	{
		//do not rebuild the orig iil
		total = num;
		subToAdd = (total - 1) / mAddBlockFillSize + 1;
		subiilsize = (total + subToAdd - total % subToAdd) / subToAdd;

		//OmnScreen << "do not need rebuild self, total:" << total << ", subToAdd:"
		//	<< subToAdd << ", subiilsize:" << subiilsize << endl;

		orig_idx = orig_num;
	}
	else
	{
		i64 num_handle = 0;
		i64 orig_handle = 0;

		//OmnScreen << "rebuild self count, num:" << num << ", orig_num:"
		//	<< (orig_num - orig_idx) << ", subiilsize:" << subiilsize << endl;
			
		rslt = batchAddRebuildCount(bitmap, size, num, num_handle, 
			mDocBitmap, orig_num - orig_idx, orig_handle, iilsize, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(num_handle + orig_handle > 0, false);
		aos_assert_r(num_handle <= num, false);
		aos_assert_r(orig_handle <= orig_num - orig_idx, false);

		//OmnScreen << "rebuild self, iilid : " << mIILID << ", num_handle:"
		//	<< num_handle << ", orig_handle:" << orig_handle << endl;

		AosIILBigHitPtr thisptr(this, false);
			
		runner = OmnNew AosIILBigHitBtachAddRebuildThrd(
			thisptr, bitmap, size, num_handle, mDocBitmap, orig_handle, iilsize, rdata);
		runners.push_back(runner);

		num -= num_handle;
		orig_idx += orig_handle;
	}
	
	vector<AosIILBigHitPtr> subiil_list;
	if (subToAdd > 0)
	{
		// create new subiils
		AosIILBigHitPtr newsub;
		for (i64 i=0; i<subToAdd; i++)
		{
			newsub = splitContentUtil(rdata);

			i64 num_handle = 0;
			i64 orig_handle = 0;

			//OmnScreen << "rebuild count, num:" << num << ", orig_num:"
			//	<< (orig_num - orig_idx) << ", subiilsize:" << subiilsize << endl;

			//rslt = batchAddRebuildCount(entries, size, num, num_handle, 
			//	&orig_values[orig_idx], &orig_docids[orig_idx],
			//	orig_num - orig_idx, orig_handle, subiilsize, rdata);
				
			rslt = batchAddRebuildCount(bitmap, size, num, num_handle, mDocBitmap,
				orig_num - orig_idx, orig_handle, subiilsize, rdata);
			aos_assert_r(rslt, false);
			aos_assert_r(num_handle + orig_handle > 0, false);
			aos_assert_r(num_handle <= num, false);
			aos_assert_r(orig_handle <= orig_num - orig_idx, false);

			//OmnScreen << "rebuild iilid : " << newsub->getIILID() << ", num_handle:" << num_handle
			//	<< ", orig_handle:" << orig_handle << ", subiilsize:" << subiilsize << endl;
				
			runner = OmnNew AosIILBigHitBtachAddRebuildThrd(
				newsub, bitmap, size, num_handle, mDocBitmap, orig_handle, subiilsize, rdata);
				
			runners.push_back(runner);
			subiil_list.push_back(newsub);

			//entries += size * num_handle;
			AosBitmapObjPtr tmp_bitmap = AosGetBitmap();
			//rslt = bitmap->getNDocids(0, num_handle, tmp_bitmap, true);
/////////////////
vector<u64> tmp_docids;
rslt = bitmap->getDocids(tmp_docids);
tmp_docids.resize(num_handle);
tmp_bitmap->appendDocids((u64*)(&tmp_docids[0]), num_handle);
////////////////
			aos_assert_r(rslt, false);
			rslt = bitmap->removeBits(tmp_bitmap);
			aos_assert_r(rslt, false);
			
			num -= num_handle;
			orig_idx += orig_handle;
		}
	}
	aos_assert_r(orig_idx == orig_num, false);
	aos_assert_r(num == 0, false);

	//OmnThreadPool::runProcSync(runners);
	AosIILMgr::smThreadPool->procSync(runners);

	mParentIIL->lockUpdate();
	rslt = mParentIIL->updateIndexData(mIILIdx, true, true, rdata);
	if (rslt && subToAdd > 0)
	{
		rslt = mParentIIL->addSubiils(mIILIdx+1, subiil_list, rdata);
	}
	mParentIIL->unlockUpdate();
	aos_assert_r(rslt, false);
	
	return true;
}

//liuwei_hit
bool
AosIILBigHit::batchAddRebuild(
		AosBitmapObjPtr add_bitmap,
		const i64 &size,
		i64 &num,
		i64 &num_handle,
		AosBitmapObjPtr orig_bitmap,
		i64 &orig_num,
		i64 &orig_idx,
		const i64 &iilsize,
		const AosRundataPtr &rdata)
{	
	AosBitmapObjPtr tmp_bitmap = AosGetBitmap();
	bool rslt = false;
	//rslt = add_bitmap->getNDocids(0, num_handle, tmp_bitmap, true);

/////////////////
vector<u64> tmp_docids;
rslt = add_bitmap->getDocids(tmp_docids);
tmp_docids.resize(num_handle);
tmp_bitmap->appendDocids((u64*)(&tmp_docids[0]), num_handle);
////////////////

	aos_assert_r(rslt, false);
	orig_bitmap->orBitmap(tmp_bitmap);
	rslt = add_bitmap->removeBits(tmp_bitmap);
	aos_assert_r(rslt, false);
	mDocBitmap = orig_bitmap->clone(rdata);
	mNumDocs = mDocBitmap->getNumBits();

return true;

	u64* docids = OmnNew u64[iilsize + 10];
	memset(docids, 0, sizeof(u64) * (iilsize + 10));
	mMemCap = iilsize + 10;
		
	//OmnScreen << "batch add rebuild iilid : " << mIILID << ", num_handle:" << num_handle
	//	<< ", orig_handle:" << (orig_num-orig_idx) << ", iilsize:" << iilsize << endl;

	i64 cur_pos = 0;
	i64 tmp_num_handle = num_handle;
	while (cur_pos < iilsize)
	{
		if (num_handle <= 0 && orig_idx >= orig_num)
		{
			break;
		}
		
		u64 first_docid;
		bool rslt = false;
/*
		rslt = add_bitmap->firstDocid(first_docid);
		aos_assert_r(rslt, false);
*/
/////////////////
add_bitmap->reset();
rslt = add_bitmap->nextDocid(first_docid);
aos_assert_r(rslt, false);
////////////////

		u64 orig_docid;
/*
		rslt = mDocBitmap->getDocByIdx(orig_idx, orig_docid);
		aos_assert_r(rslt, false);
*/
//////////////////////
mDocBitmap->reset();
for(int i=0; i<=orig_idx; i++)
{
	rslt = mDocBitmap->nextDocid(orig_docid);
	aos_assert_r(rslt, false);
}
//////////////////////
		
		if (num_handle > 0 && (orig_idx >= orig_num ||
			first_docid < orig_docid))
		{
			//copy entry
			//mValues[cur_pos] = *(u64 *)entries;
			//docids[cur_pos] = *(u64 *)&entries;
			cur_pos++;
			num_handle--; 
			num--; 
			//entries = &entries[size];
		}
		else if (orig_idx < orig_num)
		{
			//copy value/docid
			//mValues[cur_pos] = orig_values[orig_idx];
			//docids[cur_pos] = orig_docids[orig_idx];
			cur_pos++;
			orig_idx++;
		}
	}
	
	mNumDocs = cur_pos;
	mDocBitmap->appendDocids(docids, mNumDocs);

	return true;
}

bool
AosIILBigHit::batchAddRebuildCount(
		AosBitmapObjPtr add_bitmap,
        const i64 &size,
        const i64 &num,
        i64 &num_handle,
        AosBitmapObjPtr orig_bitmap,
        const i64 &orig_num,
        i64 &orig_handle,
        const i64 &iilsize,
        const AosRundataPtr &rdata)
{	
	//1. if orig_num + num <= iilsize, use them all[tail tail]
	if (orig_num + num <= iilsize)
	{
		num_handle = num;
		orig_handle = orig_num;
		return true;
	}

	//2. if orig_num <= 0, split to [tail,num - iilsize]
	if (orig_num <= 0)
	{
		aos_assert_r(num >= iilsize, false);
		num_handle = iilsize;
		orig_handle = 0;
		return true;		
	}

	//3. if num <= 0, split to [orig_num -iilsize, tail] // normally it doesn't happen
	if (num <= 0)
	{
		aos_assert_r(orig_num >= iilsize, false);
		num_handle = 0;
		orig_handle = iilsize;
		return true;		
	}

	//4. point a is [0, iilsize], point b is [iilsize,0]
	i64 num_a = 0;
	i64 num_b = iilsize;
	i64 num_cur = 0;
	bool rslt = false;
	u64 docid1;
	u64 docid2;
	//5. if num < iilsize, check point [num, iilsize-num], if it is ok, use it, otherwise replace point b with it
	if (num < iilsize)
	{
		//if [num-1 < iilsize-num]
/*
		rslt = add_bitmap->getDocByIdx(num-1, docid1);
		aos_assert_r(rslt, false);
*/
//////////////////////
add_bitmap->reset();
for(int i=0; i<=num-1; i++)
{
	rslt = add_bitmap->nextDocid(docid1);
	aos_assert_r(rslt, false);
}
//////////////////////
		
/*
		rslt = orig_bitmap->getDocByIdx(iilsize-num, docid2);
		aos_assert_r(rslt, false);
*/
//////////////////////
orig_bitmap->reset();
for(int i=0; i<=iilsize-num; i++)
{
	rslt = add_bitmap->nextDocid(docid2);
	aos_assert_r(rslt, false);
}
//////////////////////
		
		if (docid1 < docid2)
		{
			num_handle = num;
			orig_handle = iilsize - num;
			return true;
		}
		else
		{
			num_b = num;
		}
	}
	else // num >= iilsize
	{
/*
		rslt = add_bitmap->getDocByIdx(iilsize-1, docid1);
		aos_assert_r(rslt, false);
*/
//////////////////////
add_bitmap->reset();
for(int i=0; i<=iilsize-1; i++)
{
	rslt = add_bitmap->nextDocid(docid1);
	aos_assert_r(rslt, false);
}
//////////////////////
		
/*
		rslt = orig_bitmap->getDocByIdx(0, docid2);
		aos_assert_r(rslt, false);
*/
//////////////////////
orig_bitmap->reset();
rslt = orig_bitmap->nextDocid(docid2);
aos_assert_r(rslt, false);
//////////////////////
		
		if (docid1<=docid2)
		{
			num_handle = iilsize;
			orig_handle = 0;
			return true;
		}		
	}

	//6. if orig_num < iilsize, check point[iilsize-orig_num, orig_num], if it is ok, use it, otherwise replace point a with it
	if (orig_num < iilsize)
	{
		//if [iilsize - orig_num > orig_num-1]
/*
		rslt = add_bitmap->getDocByIdx(iilsize-orig_num, docid1);
		aos_assert_r(rslt, false);
*/
//////////////////////
add_bitmap->reset();
for(int i=0; i<=iilsize-orig_num; i++)
{
	rslt = add_bitmap->nextDocid(docid1);
	aos_assert_r(rslt, false);
}
//////////////////////
/*	
		rslt = orig_bitmap->getDocByIdx(orig_num-1, docid2);
		aos_assert_r(rslt, false);
*/
//////////////////////
orig_bitmap->reset();
for(int i=0; i<=orig_num-1; i++)
{
	rslt = orig_bitmap->nextDocid(docid2);
	aos_assert_r(rslt, false);
}
//////////////////////
		
		if (docid1>docid2)
		{
			num_handle = iilsize - orig_num;
			orig_handle = orig_num;
			return true;
		}
		else
		{
			num_a = iilsize - orig_num;
		}
	}
	else // orig_num >= iilsize
	{
/*
		rslt = add_bitmap->getDocByIdx(0, docid1);
		aos_assert_r(rslt, false);
*/
//////////////////////
add_bitmap->reset();
for(int i=0; i<=0; i++)
{
	rslt = add_bitmap->nextDocid(docid1);
	aos_assert_r(rslt, false);
}
//////////////////////
/*	
		rslt = orig_bitmap->getDocByIdx(iilsize-1, docid2);
		aos_assert_r(rslt, false);
*/
//////////////////////
orig_bitmap->reset();
for(int i=0; i<=iilsize-1; i++)
{
	rslt = orig_bitmap->nextDocid(docid2);
	aos_assert_r(rslt, false);
}
//////////////////////
		
		if (docid1>=docid2)
		{
			num_handle = 0;
			orig_handle = iilsize;
			return true;
		}
	}
	
	while (num_b - num_a > 1)
	{
/*
		rslt = add_bitmap->getDocByIdx(num_cur-1, docid1);
		aos_assert_r(rslt, false);
*/
//////////////////////
add_bitmap->reset();
for(int i=0; i<=num_cur-1; i++)
{
	rslt = add_bitmap->nextDocid(docid1);
	aos_assert_r(rslt, false);
}
//////////////////////
/*	
		rslt = orig_bitmap->getDocByIdx(iilsize-num_cur, docid2);
		aos_assert_r(rslt, false);
*/
//////////////////////
orig_bitmap->reset();
for(int i=0; i<=iilsize-num_cur; i++)
{
	rslt = orig_bitmap->nextDocid(docid2);
	aos_assert_r(rslt, false);
}
//////////////////////
		
		num_cur = (num_a + num_b) / 2;
		//if ([num_cur-1 > iilsize - num_cur])
		if (docid1>docid2)
		{
			num_b = num_cur;
			continue;
		}
		//if ([num_cur < iilsize - num_cur-1])
/*	
		rslt = add_bitmap->getDocByIdx(num_cur, docid1);
		aos_assert_r(rslt, false);
*/
//////////////////////
add_bitmap->reset();
for(int i=0; i<=num_cur; i++)
{
	rslt = add_bitmap->nextDocid(docid1);
	aos_assert_r(rslt, false);
}
//////////////////////
/*		
		rslt = orig_bitmap->getDocByIdx(iilsize-num_cur-1, docid2);
		aos_assert_r(rslt, false);
*/
//////////////////////
orig_bitmap->reset();
for(int i=0; i<=iilsize-num_cur-1; i++)
{
	rslt = orig_bitmap->nextDocid(docid2);
	aos_assert_r(rslt, false);
}
//////////////////////
		
		if (docid1<docid2)
		{
			num_a = num_cur;
			continue;
		}
		num_handle = num_cur;
		orig_handle = iilsize-num_cur;
		return true;		
	}

	OmnAlarm << "Something wrong here:" 
		<< num_a << ":" 
		<< num_b << ":" 
		<< num_cur << enderr;
	
	num_handle = num_a + 1;
	orig_handle = iilsize - num_a - 1;
	
	return true;
}

i64 
AosIILBigHit::binarySearch(
		AosBitmapObjPtr bitmap,
		const i64 &size,
		const i64 &num,
		const u64 &docid)
{
	// find the last index:
	// 		entries[index] <= value
	aos_assert_r(bitmap && num > 0, -1);
	i64 left = 0;
	i64 right = num - 1;
	bool rslt = false;
	u64 docid1;
/*	
	rslt = bitmap->getDocByIdx(left, docid1);
	aos_assert_r(rslt, false);
*/
//////////////////////
bitmap->reset();
for(int i=0; i<=left; i++)
{
	rslt = bitmap->nextDocid(docid1);
	aos_assert_r(rslt, false);
}
//////////////////////
		
	if (docid1 > docid) return -1;
/*	
	rslt = bitmap->getDocByIdx(right, docid1);
	aos_assert_r(rslt, false);
*/
//////////////////////
bitmap->reset();
for(int i=0; i<=right; i++)
{
	rslt = bitmap->nextDocid(docid1);
	aos_assert_r(rslt, false);
}
//////////////////////
		
	if (docid1 < docid) return right;
	
	i64 idx = 0;
	while (right - left > 1)
	{
		if (left + 1 == right)
		{
/*
			rslt = bitmap->getDocByIdx(right, docid1);
			aos_assert_r(rslt, false);
*/
//////////////////////
bitmap->reset();
for(int i=0; i<=right; i++)
{
	rslt = bitmap->nextDocid(docid1);
	aos_assert_r(rslt, false);
}
//////////////////////
		
			if (docid1 < docid) return right;
			return left;
		}

		idx = (right + left) / 2;
/*
		rslt = bitmap->getDocByIdx(idx, docid1);
		aos_assert_r(rslt, false);
*/
//////////////////////
bitmap->reset();
for(int i=0; i<=idx; i++)
{
	rslt = bitmap->nextDocid(docid1);
	aos_assert_r(rslt, false);
}
//////////////////////
		
		if (docid1 <= docid)
		{
			left = idx;
		}
		else
		{
			right = idx;
		}
	}
	return left;
}

int
AosIILBigHit::valueMatch(
		const char * entry,
		const i64 &size,
		const u64 &docid)
{
	u64 entry_docid = *(u64 *)entry;
	if (entry_docid > docid) return 1;
	if (entry_docid < docid) return -1;

	return 0;
}

int
AosIILBigHit::valueMatch(
		const char * entry,
		const u64 &docid)
{
	u64 entry_docid = *(u64 *)entry;
	if (entry_docid > docid) return 1;
	if (entry_docid < docid) return -1;
	return 0;
}

int
AosIILBigHit::valueMatchU64(
		const char * entry1,
		const char * entry2)
{
	u64 entry_doc1 = *(u64 *)entry1;
	u64 entry_doc2 = *(u64 *)entry2;
	if (entry_doc1 > entry_doc2) return 1;
	if (entry_doc1 < entry_doc2) return -1;
	return 0;
}

bool
AosIILBigHit::batchDelSafe(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	aos_assert_r(size  == (int)sizeof(u64),false);
	u64* cur_pos = (u64*)entries;
	for(i64 i = 0;i < num-1;i++)
	{
		aos_assert_r((*cur_pos) <= (*(cur_pos+1)),false);
		cur_pos += 1;
	}
	AosBitmapObjPtr bitmap = AosGetBitmap();
	bitmap->appendDocids((u64*)entries, num);
	AOSLOCK(mLock);

	i64 start_total = mNumDocs;
	u64 start_time = OmnGetTimestamp();
	bool rslt = batchDelPriv(bitmap, size, num, rdata);
	u64 cost = OmnGetTimestamp() - start_time;
	i64 end_total = mNumDocs;

	if (AosIILMgr::smShowLog)
	{
		OmnScreen << "\n\n================================="
				  << "\nBatch Del Finish, iilid:" << mIILID << ", entry num:" << num
				  << ", time cost:" << AosTimestampToHumanRead(cost)
				  << ", start_total:" << start_total << ", end_total:" << end_total
				  << "\n=================================\n" << endl;
	}

	AOSUNLOCK(mLock);

	return rslt;
}


bool
AosIILBigHit::batchDelPriv(
		AosBitmapObjPtr bitmap,
		const i64 &size,
		const i64 &num,
		const AosRundataPtr &rdata)
{	
	i64 cur_num = num;
	bool rslt = batchDelRecPriv(bitmap, size, cur_num, rdata);
	aos_assert_r(rslt, false);

	if (isRootIIL() && mNumDocs == 0)
	{
		rslt = clearIILPriv(rdata);
		aos_assert_r(rslt, false);
	}
	
	rslt = saveToFilePriv(rdata);
	aos_assert_r(rslt, false);
			
	rslt = returnSubIILsPriv(rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILBigHit::batchDelRecSafe(
		AosBitmapObjPtr bitmap,
		const i64 &size,
		i64 &num,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = batchDelRecPriv(bitmap, size, num, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}

bool
AosIILBigHit::batchDelRecPriv(
		AosBitmapObjPtr bitmap,
		const i64 &size,
		i64 &num,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return batchDelSinglePriv(bitmap, size, num, rdata);
	}

	aos_assert_r(num > 0, false);
	u64 first_docid;
	bool rslt = false;
/*
	rslt = bitmap->firstDocid(first_docid);
	aos_assert_r(rslt, false);
*/	
////////////////////////
bitmap->reset();
rslt = bitmap->nextDocid(first_docid);
aos_assert_r(rslt, false);
////////////////////////
	
	i64 idx = getSubiilIndex3Priv(first_docid, rdata);
	aos_assert_r(idx >= 0 && idx < mNumSubiils, false);

	i64 last_num = num;
	AosIILBigHitPtr subiil;
	for (i64 i=idx; i<mNumSubiils; i++)
	{
		subiil = getSubiilByIndexPriv(i, rdata);
		aos_assert_r(subiil, false);
	
		rslt = subiil->batchDelRecSafe(bitmap, size, num, rdata);
		aos_assert_r(rslt, false);
	}
	aos_assert_r(last_num >= num, false);
	if (last_num == num) return true;
	
	rslt = resetMaxMin(idx, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}

//liuwei_hit
bool
AosIILBigHit::batchDelSinglePriv(
		AosBitmapObjPtr bitmap,
		const i64 &size,
		i64 &num,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs > 0, false);
	bool rslt = false;
	u64 last_docid; 
	rslt = mDocBitmap->lastDocid(last_docid);
	aos_assert_r(rslt, false);
	AosBitmapObjPtr bmp_need_removed = AosGetBitmap();

	//rslt = bitmap->getCountDocids(last_docid, bmp_need_removed);

/////////////////////////////
u64 tmp_docid = 0;
bmp_need_removed->clean();
bitmap->reset();
rslt = bitmap->nextDocid(tmp_docid);
aos_assert_r(rslt, false);
for(;tmp_docid<=last_docid;)
{
	bmp_need_removed->appendDocid(tmp_docid);
	if(bmp_need_removed->getNumBits()>=num)
	{
		break;
	}
	rslt = bitmap->nextDocid(tmp_docid);
	aos_assert_r(rslt, false);
}
/////////////////////////////

	aos_assert_r(rslt, false);
	//mDocBitmap = AosBitmap::countAndAnotB(mDocBitmap, bmp_need_removed);
	rslt = mDocBitmap ->removeBits(bmp_need_removed);
	aos_assert_r(rslt, false);
	rslt = bitmap->removeBits(bmp_need_removed);
	aos_assert_r(rslt, false);
	mNumDocs = mDocBitmap->getNumBits();
	return true;
}

AosBitmapObjPtr 
AosIILBigHit::createBitmap()
{
	aos_assert_r((mNumDocs == 0) || mDocBitmap, 0);
	aos_assert_r(mLevel == 0, 0);

	AosBitmapObjPtr bitmap = AosGetBitmap();
	aos_assert_r(bitmap, 0);
	bitmap = mDocBitmap->clone(0);
	return bitmap;
}


bool
AosIILBigHit::rebuildBitmapSafe(const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	AosIILExecutorObjPtr executor = AosBitmapEngineObj::getBitmapEngine()->getIILExecutor(rdata);
	aos_assert_rl(!executor->isDisable(), mLock, false);

	OmnScreen << "=======clear bitmaps if exist ========== IILID:" << mIILID << endl;
	bool rslt = clearBitmapPriv(executor, rdata);
	OmnScreen << "=======clear bitmaps finished ========== IILID:" << mIILID << endl;

	executor->finish(rdata, 0);
	executor = AosBitmapEngineObj::getBitmapEngine()->getIILExecutor(rdata);

	OmnScreen << "=======rebuild bitmap========== IILID:" << mIILID << endl;
	rslt = rebuildBitmapPriv(executor, rdata);
	OmnScreen << "=======rebuild bitmap finished========== IILID:" << mIILID << endl;

	if (AosIILMgr::smShowLog)
	{
		OmnScreen << "\n\n================================="
				  << "\nRebuild Bitmap Finish, iilid:" << mIILID
				  << ", total:" << mNumDocs 
				  << "\n=================================\n" << endl;
	}
	
	executor->finish(rdata, 0);
	AOSUNLOCK(mLock);

	return rslt;
}

	
bool
AosIILBigHit::clearBitmapPriv(
		AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	if (!executor)
	{
		executor = AosBitmapEngineObj::getBitmapEngine()->getIILExecutor(rdata);
	}

	bool rslt = false;
	if (isLeafIIL())
	{
		rslt = executor->removeBitmap(rdata,0,0,mIILID,mIILID);
		aos_assert_r(rslt, false);
		return true;
	}

	// The root iil is a parent IIL
	rslt = clearBitmapRecPriv(executor,rdata);
	aos_assert_r(rslt, false);
	
	return true;
}	


bool
AosIILBigHit::clearBitmapRecPriv(
		AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	AosIILBigHitPtr subiil;
	bool rslt = false;
	bool iil_returned = false;
	if (mLevel > 1)
	{
		for (int i = 0; i < mNumSubiils; i++)
		{
			subiil = getSubiilByIndexPriv(i, rdata);
			aos_assert_r(subiil, false);

			rslt = subiil->clearBitmapRecPriv(executor,rdata);
			aos_assert_r(rslt, false);

			rslt = AosIILMgrSelf->returnIILPublic(subiil, iil_returned, false, rdata);
	 		aos_assert_r(rslt, false);

			mSubiils[i] = 0;
		}
	}
	
	AosBitmapTreeObjPtr tree = getBitmapTree(rdata);
	if (!tree)
	{
		return true;
	}

	rslt = tree->removeBitmaps(rdata, executor);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILBigHit::rebuildBitmapPriv(
		AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	//case 1. single iil
	//case 2. level 1 iil
	//case 3. level 2+ iil
	bool rslt = false;
	if (isLeafIIL())
	{
		rslt = executor->createLeaf(rdata, AosBitmapObj::eNewLeaf, createBitmap());
		aos_assert_r(rslt, false);
		return true;
	}
	
	rslt = rebuildBitmapRecPriv(executor, rdata);
	aos_assert_r(rslt,false);
	
	return true;	
}	


bool
AosIILBigHit::rebuildLevelOne(
		const u64 &index, 
		const u64 num, 
		const u64 node_id,
		const AosRundataPtr &rdata)
{
	OmnScreen << "============== start ====================" << endl;
	vector<u64> p_list;
	vector<u64> m_list;
	u64 cur_idx = index;
	AosIILExecutorObjPtr executor = AosBitmapEngineObj::getBitmapEngine()->getIILExecutor(rdata);
	bool rslt;
	bool iil_returned; 
	AosBitmapObjPtr cur_bitmap = AosGetBitmap();
	AosIILBigHitPtr subiil;
	for (u64 j = 0; j < num; j++)
	{
		p_list.push_back(mIILIds[cur_idx]);
		m_list.push_back(mIILIds[cur_idx]);
		mBRLock->lock();
		subiil = getSubiilByIndexPriv(cur_idx, rdata);
		mBRLock->unlock();
		aos_assert_r(subiil, false);

		cur_bitmap = subiil->createBitmap();
		OmnScreen << "Create Leaf " << cur_idx << endl;
		executor->createLeaf(rdata, AosBitmapObj::eNewLeaf, cur_bitmap);

		mBRLock->lock();
		rslt = AosIILMgrSelf->returnIILPublic(subiil, iil_returned, false, rdata);
		mBRLock->unlock();
		aos_assert_r(rslt, false);

		mSubiils[cur_idx] = 0;
		cur_idx ++;
	}
	executor->rebuildBitmap(rdata, 1, 1, node_id, p_list, m_list);	
	executor->finish(rdata, 0);
	executor = 0;	
	OmnScreen << "============== end ====================" << endl;
	return true;
}


bool
AosIILBigHit::rebuildBitmapRecPriv(
		AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	AosIILBigHitPtr subiil;
	aos_assert_r(mLevel > 0, false);

	bool iil_returned = false;
	if (mLevel > 1)
	{
		for (int i = 0; i < mNumSubiils; i++)
		{
	 		subiil = getSubiilByIndexPriv(i, rdata);
	 		aos_assert_r(subiil, false);

			OmnScreen << "Rebuild IIL " << mLevel << ":" << i << endl;
	 		subiil->rebuildBitmapRecPriv(executor, rdata);
			rslt = AosIILMgrSelf->returnIILPublic(subiil, iil_returned, false, rdata);
	 		aos_assert_r(rslt, false);

			mSubiils[i] = 0;
		}

		mBitmapTree = AosBitmapTreeMgrObj::getObject()->createTree(rdata, mIILID, mLevel, mMaxSubIILs);
		mBitmapTree->rebuild(rdata, executor, mIILIds, mNumSubiils);
		AosBitmapTreeMgrObj::getObject()->saveBitmapTree(rdata, mBitmapTree);
		return true;
	}
	
	// mLevel == 1
	aos_assert_r(mLevel == 1, false);
	mBitmapTree = AosBitmapTreeMgrObj::getObject()->createTree(rdata, mIILID, mLevel, mMaxSubIILs);
	aos_assert_r(mBitmapTree, false);

	// rebuild tree without rebuilding parents
	mBitmapTree->rebuildStruct(rdata, executor, mIILIds, mNumSubiils);
	AosBitmapTreeMgrObj::getObject()->saveBitmapTree(rdata, mBitmapTree);

	vector<u64> parent_list;
	vector<u64> child_num_list;
	u64 cur_idx = 0;
	mBitmapTree->getLevel2Relations(rdata, parent_list, child_num_list);
	AosBitmapObjPtr cur_bitmap = AosGetBitmap();

	OmnThrdShellProcPtr runner;
	vector<OmnThrdShellProcPtr> runners;

	AosIILBigHitPtr thisPtr(this, false);
	u32 max_runner_size = 24; 
	for (u64 i = 0; i < parent_list.size(); i++)
	{
		runner = OmnNew AosIILBigHitRebuildBitmapThrd(
			thisPtr, cur_idx, child_num_list[i], parent_list[i], rdata);
		cur_idx += child_num_list[i];
		runners.push_back(runner);

		if (runners.size() >= max_runner_size)
		{
			//OmnThreadPool::runProcSync(runners);
			AosIILMgr::smThreadPool->procSync(runners);
			runners.clear();
		}

		//1. build child bitmaps
		/*
		vector<u64> p_list;
		vector<u64> m_list;
		for(u64 j = 0;j < child_num_list[i];j++)
		{
			p_list.push_back(mIILIds[cur_idx]);
			m_list.push_back(mIILIds[cur_idx]);
	 		subiil = getSubiilByIndexPriv(cur_idx, rdata);
	 		aos_assert_r(subiil,false);
			cur_bitmap = subiil->createBitmap();
			OmnScreen << "Create Leaf " << cur_idx << endl;
	 		executor->createLeaf(rdata, AosBitmapObj::eNewLeaf, cur_bitmap);

			rslt = AosIILMgrSelf->returnIILPublic(subiil, iil_returned, false, rdata);
	 		aos_assert_r(rslt,false);
			mSubiils[cur_idx] = 0;
	 		cur_idx ++;
		}*/
		
/*		executor->rebuildBitmap(
				rdata,
				1,
				1,
				parent_list[i],
				p_list,
				m_list);	
		if(executor->isFull())
		{
			executor->finish(rdata, 0);
			executor = AosBitmapEngineObj::getBitmapEngine()->getIILExecutor(rdata);
		}
*/
		//2. build parent bitmaps in tree
	}
	
	if (runners.size() > 0)
	{
		//OmnThreadPool::runProcSync(runners);
		AosIILMgr::smThreadPool->procSync(runners);
		runners.clear();
	}

	//mBitmapTree->buildLevel2Node(rdata,executor,parent_list[i]);
	mBitmapTree->buildHighLevelNodes(rdata,executor);
	
	return true;
}


AosBitmapTreeObjPtr 
AosIILBigHit::getBitmapTree(const AosRundataPtr &rdata)
{
	if (mBitmapTree)
	{
		aos_assert_r(mBitmapTree->getBmpID() == mIILID, 0);
		return mBitmapTree;
	}

	// get bitmap from bitmapTreeMgr		
	mBitmapTree = AosBitmapTreeMgrObj::getObject()->getBitmapTree(mIILID,mMaxSubIILs,rdata);
//	aos_assert_r(mBitmapTree, 0);
//	aos_assert_r(mBitmapTree->getBmpID() == mIILID, 0);
	return mBitmapTree;
}


bool 
AosIILBigHit::saveBitmapTree(const AosRundataPtr &rdata)
{
	if(mBitmapTree)
	{
		AosBitmapTreeMgrObj::getObject()->saveBitmapTree(rdata, mBitmapTree);
	}
	return true;
}


bool 
AosIILBigHit::setBitmapTree(
		const AosBitmapTreeObjPtr &tree,
		const AosRundataPtr &rdata)
{
	//1. set bitmaptree
	//2. save the tree
	
	mBitmapTree = tree;	
	aos_assert_r(mBitmapTree->getBmpID() == mIILID, false);
	return true;
}

