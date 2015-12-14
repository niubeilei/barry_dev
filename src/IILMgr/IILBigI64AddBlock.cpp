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
#include "IILMgr/IILBigI64.h"

#include "API/AosApiS.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/IILThrdShellProc.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/BitmapTreeObj.h"
#include "SEInterfaces/BitmapTreeMgrObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/QueryRsltObj.h"


bool
AosIILBigI64::batchAddSafe(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	aos_assert_r(size  ==(int)sizeof(u64) * 2 ,false);
	i64* cur_pos = (i64*)entries;
	for(i64 i = 0;i < num-1;i++)
	{
		aos_assert_r((*cur_pos) <= (*(cur_pos+2)),false);
		if((*cur_pos) == (*(cur_pos+2)))
		{
			// compare docid
			aos_assert_r( (*(u64*)(cur_pos+1)) <= (*(u64*)(cur_pos+3)) , false);
		}
		cur_pos += 2;
	}

	AOSLOCK(mLock);

	i64 start_total = mNumDocs;
	u64 start_time = OmnGetTimestamp();
	bool rslt = batchAddPriv(entries, size, num, rdata);
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
	
	if(executor->getExecutorID() == eStreamingExecutorId)
	{
		rebuildBitmapPriv1(rdata);
	}
	AOSUNLOCK(mLock);

	return rslt;
}
	

bool
AosIILBigI64::batchAddPriv(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sanityCheckPriv(rdata), false);

	bool rslt = true;
	i64 subChanged = 0;
	char * cur_entries = entries;
	i64 cur_num = num;
	
	while (cur_num > 0)
	{
		bool isTail = true;
		rslt = batchAddRecPriv(cur_entries, size,
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
AosIILBigI64::batchAddRecSafe(
		char * &entries,
		const i64 &size,
		i64 &num,
		i64 &subChanged,
		const bool isTail,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = batchAddRecPriv(entries, size,
		num, subChanged, isTail, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILBigI64::batchAddRecPriv(
		char * &entries,
		const i64 &size,
		i64 &num,
		i64 &subChanged,
		const bool isTail,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return batchAddSinglePriv(entries, size,
			num, subChanged, isTail, rdata);
	}

	aos_assert_r(num > 0, false);
	i64 entry_key = *(i64 *)entries;
	u64 entry_value = *(u64 *)&entries[size - sizeof(u64)];
	i64 sub_idx = getSubiilIndex3Priv(entry_key, entry_value, rdata);
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
				num_handle = binarySearch(entries, size, num, mMaxVals[sub_idx], mMaxDocids[sub_idx]) + 1;
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

			AosIILBigI64Ptr thisptr(this, false);
			runner = OmnNew AosIILBigI64BtachAddSingleThrd(
				thisptr, sub_iilid, sub_idx, entries,
				size, num_handle, rdata);
			runners.push_back(runner);

			//OmnScreen << "iilid : " << sub_iilid << ", iilidx:" << sub_idx << ", iil_entrynum:" << mNumEntries[sub_idx] 
			//	<< ", num_handle:" << num_handle << ", max_handle:" << max_handle << ", isTail:" << it << endl;

			subChanged += (1 + subToAdd);
			num -= num_handle;
			if (num > 0)
			{
				entries = &entries[size * num_handle];
				entry_key = *(i64 *)entries;
				entry_value = *(u64 *)&entries[size - sizeof(u64)];
			}

			sub_iilid = 0;
			if (!it && num > 0)
			{
				sub_idx = getSubiilIndex3Priv(entry_key, entry_value, rdata);
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

	AosIILBigI64Ptr subiil = getSubiilByIndexPriv(sub_idx, rdata);
	aos_assert_r(subiil, false);

	i64 num_before = num;
	bool rslt = subiil->batchAddRecSafe(entries,
		size, num, subChanged, it, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(num_before > num, false);

	return true;
}


bool
AosIILBigI64::batchAddSinglePriv(
		char * &entries,
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
		num_handle = binarySearch(entries, size, num, mValues[mNumDocs-1], mDocids[mNumDocs-1]) + 1;
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

	i64* orig_values = mValues;
	u64* orig_docids = mDocids;
	i64 orig_num = mNumDocs;
	i64 orig_idx = 0;
	mValues = 0;
	mDocids = 0;
	bool rslt = true;
	OmnThrdShellProcPtr runner;
	vector<OmnThrdShellProcPtr> runners;

	// judge if are in the tail, can we ignore rebuilding it self?
	// if the first entry is bigger than the last doc in iil, if the iil size is more than fill size
	if (orig_num >= mAddBlockFillSize && subToAdd > 0 &&
		valueMatch(entries, size, orig_values[orig_num-1], orig_docids[orig_num-1]) > 0)
	{
		//do not rebuild the orig iil
		total = num_handle;
		subToAdd = (total - 1) / mAddBlockFillSize + 1;
		subiilsize = (total + subToAdd - total % subToAdd) / subToAdd;
	
		mValues = orig_values;
		mDocids = orig_docids;
		orig_idx = orig_num;
		orig_values = 0;
		orig_docids = 0;
	}
	else
	{
		i64 new_handle = 0;
		i64 orig_handle = 0;

		//OmnScreen << "rebuild self count, num:" << num_handle << ", orig_num:"
		//	<< (orig_num - orig_idx) << ", subiilsize:" << subiilsize << endl;

		rslt = batchAddRebuildCount(entries, size, num_handle, new_handle, 
			&orig_values[orig_idx], &orig_docids[orig_idx], 
			orig_num - orig_idx, orig_handle, iilsize, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(new_handle + orig_handle > 0, false);
		aos_assert_r(new_handle <= num_handle, false);
		aos_assert_r(orig_handle <= orig_num - orig_idx, false);

		//OmnScreen << "rebuild self, iilid : " << mIILID << ", new_handle:"
		//	<< new_handle << ", orig_handle:" << orig_handle << endl;

		AosIILBigI64Ptr thisptr(this, false);
		runner = OmnNew AosIILBigI64BtachAddRebuildThrd(
			thisptr, entries, size, new_handle, &orig_values[orig_idx],
			&orig_docids[orig_idx], orig_handle, iilsize, rdata);
		runners.push_back(runner);

		entries += size * new_handle;
		num -= new_handle;
		num_handle -= new_handle;
		orig_idx += orig_handle;

	}

	vector<AosIILBigI64Ptr> subiil_list;
	if (subToAdd > 0)
	{
		// create new subiils
		AosIILBigI64Ptr newsub;
		for (i64 i=0; i<subToAdd; i++)
		{
			newsub = splitContentUtil(rdata);

			i64 new_handle = 0;
			i64 orig_handle = 0;

			//OmnScreen << "rebuild count, num:" << num << ", orig_num:"
			//	<< (orig_num - orig_idx) << ", subiilsize:" << subiilsize << endl;

			rslt = batchAddRebuildCount(entries, size, num_handle,
				new_handle, &orig_values[orig_idx], &orig_docids[orig_idx],
				orig_num - orig_idx, orig_handle, subiilsize, rdata);
			aos_assert_r(rslt, false);
			aos_assert_r(new_handle + orig_handle > 0, false);
			aos_assert_r(new_handle <= num_handle, false);
			aos_assert_r(orig_handle <= orig_num - orig_idx, false);

			//OmnScreen << "rebuild iilid : " << newsub->getIILID() << ", num_handle:" << num_handle
			//	<< ", orig_handle:" << orig_handle << ", subiilsize:" << subiilsize << endl;

			runner = OmnNew AosIILBigI64BtachAddRebuildThrd(
				newsub, entries, size, new_handle, &orig_values[orig_idx],
				&orig_docids[orig_idx], orig_handle, subiilsize, rdata);
			runners.push_back(runner);
			subiil_list.push_back(newsub);

			entries += size * new_handle;
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

	if (orig_values)
	{
		OmnDelete [] orig_values;
		orig_values = 0;
	}
	
	if (orig_docids)
	{
		OmnDelete [] orig_docids;
		orig_docids = 0;
	}
	
	subChanged++;
	subChanged += subToAdd;
	return true;
}


bool
AosIILBigI64::batchAddParentThrdSafe(
		const u64 &iilid,
		const i64 &iil_idx,
		char * &entries,
		const i64 &size,
		i64 &num,
		const AosRundataPtr &rdata)
{
	AosIILBigI64Ptr subiil = getSubiilByIndexPriv(iilid, iil_idx, rdata);
	aos_assert_r(subiil, false);

	bool rslt = subiil->batchAddSingleThrdSafe(
		entries, size, num, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILBigI64::batchAddSingleThrdSafe(
		char * &entries,
		const i64 &size,
		i64 &num,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = batchAddSingleThrdPriv(entries, size, num, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILBigI64::batchAddSingleThrdPriv(
		char * &entries,
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

	i64* orig_values = mValues;
	u64* orig_docids = mDocids;
	i64 orig_num = mNumDocs;
	i64 orig_idx = 0;
	mValues = 0;
	mDocids = 0;

	bool rslt = true;
	OmnThrdShellProcPtr runner;
	vector<OmnThrdShellProcPtr> runners;

	// judge if are in the tail, can we ignore rebuilding it self?
	// if the first entry is bigger than the last doc in iil, if the iil size is more than fill size
	if (orig_num >= mAddBlockFillSize && subToAdd > 0 && 
		valueMatch(entries, size, orig_values[orig_num-1], orig_docids[orig_num-1]) > 0)
	{
		//do not rebuild the orig iil
		total = num;
		subToAdd = (total - 1) / mAddBlockFillSize + 1;
		subiilsize = (total + subToAdd - total % subToAdd) / subToAdd;

		//OmnScreen << "do not need rebuild self, total:" << total << ", subToAdd:"
		//	<< subToAdd << ", subiilsize:" << subiilsize << endl;
	
		mValues = orig_values;
		mDocids = orig_docids;
		orig_idx = orig_num;
		orig_values = 0;
		orig_docids = 0;
	}
	else
	{
		i64 num_handle = 0;
		i64 orig_handle = 0;

		//OmnScreen << "rebuild self count, num:" << num << ", orig_num:"
		//	<< (orig_num - orig_idx) << ", subiilsize:" << subiilsize << endl;

		rslt = batchAddRebuildCount(entries, size, num, num_handle, 
			&orig_values[orig_idx], &orig_docids[orig_idx], 
			orig_num - orig_idx, orig_handle, iilsize, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(num_handle + orig_handle > 0, false);
		aos_assert_r(num_handle <= num, false);
		aos_assert_r(orig_handle <= orig_num - orig_idx, false);

		//OmnScreen << "rebuild self, iilid : " << mIILID << ", num_handle:"
		//	<< num_handle << ", orig_handle:" << orig_handle << endl;

		AosIILBigI64Ptr thisptr(this, false);
		runner = OmnNew AosIILBigI64BtachAddRebuildThrd(
			thisptr, entries, size, num_handle, &orig_values[orig_idx],
			&orig_docids[orig_idx], orig_handle, iilsize, rdata);
		runners.push_back(runner);

		entries += size * num_handle;
		num -= num_handle;
		orig_idx += orig_handle;
	}
	
	vector<AosIILBigI64Ptr> subiil_list;
	if (subToAdd > 0)
	{
		// create new subiils
		AosIILBigI64Ptr newsub;
		for (i64 i=0; i<subToAdd; i++)
		{
			newsub = splitContentUtil(rdata);

			i64 num_handle = 0;
			i64 orig_handle = 0;

			//OmnScreen << "rebuild count, num:" << num << ", orig_num:"
			//	<< (orig_num - orig_idx) << ", subiilsize:" << subiilsize << endl;

			rslt = batchAddRebuildCount(entries, size, num, num_handle, 
				&orig_values[orig_idx], &orig_docids[orig_idx],
				orig_num - orig_idx, orig_handle, subiilsize, rdata);
			aos_assert_r(rslt, false);
			aos_assert_r(num_handle + orig_handle > 0, false);
			aos_assert_r(num_handle <= num, false);
			aos_assert_r(orig_handle <= orig_num - orig_idx, false);

			//OmnScreen << "rebuild iilid : " << newsub->getIILID() << ", num_handle:" << num_handle
			//	<< ", orig_handle:" << orig_handle << ", subiilsize:" << subiilsize << endl;

			runner = OmnNew AosIILBigI64BtachAddRebuildThrd(
				newsub, entries, size, num_handle, &orig_values[orig_idx],
				&orig_docids[orig_idx], orig_handle, subiilsize, rdata);
			runners.push_back(runner);
			subiil_list.push_back(newsub);

			entries += size * num_handle;
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

	if (orig_values)
	{
		OmnDelete [] orig_values;
		orig_values = 0;
	}
	
	if (orig_docids)
	{
		OmnDelete [] orig_docids;
		orig_docids = 0;
	}
	
	return true;
}


bool
AosIILBigI64::batchAddRebuild(
		char * &entries,
		const i64 &size,
		i64 &num,
		i64 &num_handle,
		i64* orig_values,
		u64* orig_docids,
		i64 &orig_num,
		i64 &orig_idx,
		const i64 &iilsize,
		const AosRundataPtr &rdata)
{	
	mValues = OmnNew i64[iilsize + 10];
	mDocids = OmnNew u64[iilsize + 10];
	memset(mValues, 0, sizeof(i64) * (iilsize + 10));
	memset(mDocids, 0, sizeof(u64) * (iilsize + 10));
	mMemCap = iilsize + 10;
		
	//OmnScreen << "batch add rebuild iilid : " << mIILID << ", num_handle:" << num_handle
	//	<< ", orig_handle:" << (orig_num-orig_idx) << ", iilsize:" << iilsize << endl;

	i64 cur_pos = 0;
	while (cur_pos < iilsize)
	{
		if (num_handle <= 0 && orig_idx >= orig_num)
		{
			break;
		}
		
		if (num_handle > 0 && (orig_idx >= orig_num ||
			valueMatch(entries, size, orig_values[orig_idx], orig_docids[orig_idx]) < 0))
		{
			//copy entry
			mValues[cur_pos] = *(i64 *)entries;
			mDocids[cur_pos] = *(u64 *)&entries[size - sizeof(u64)];
			cur_pos++;
			num_handle--; 
			num--; 
			entries = &entries[size];
		}
		else if (orig_idx < orig_num)
		{
			//copy value/docid
			mValues[cur_pos] = orig_values[orig_idx];
			mDocids[cur_pos] = orig_docids[orig_idx];
			cur_pos++;
			orig_idx++;
		}
	}
	
	mNumDocs = cur_pos;
	return true;
}


bool
AosIILBigI64::batchAddRebuildCount(
        char * &entries,
        const i64 &size,
        const i64 &num,
        i64 &num_handle,
        i64* orig_values,
        u64* orig_docids,
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
	//5. if num < iilsize, check point [num, iilsize-num], if it is ok, use it, otherwise replace point b with it
	if (num < iilsize)
	{
		//if [num-1 < iilsize-num]
		if (valueMatch(entries + (num - 1) * size, size, orig_values[iilsize - num], orig_docids[iilsize - num]) < 0)
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
		if (valueMatch(entries + (iilsize - 1) * size, size, orig_values[0], orig_docids[0]) <= 0)
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
		if (valueMatch(entries + (iilsize - orig_num) * size, size, orig_values[orig_num - 1], orig_docids[orig_num - 1]) > 0)
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
		if (valueMatch(entries, size, orig_values[iilsize-1], orig_docids[iilsize-1]) >= 0)
		{
			num_handle = 0;
			orig_handle = iilsize;
			return true;
		}
	}
	
	while (num_b - num_a > 1)
	{
		num_cur = (num_a + num_b) / 2;
		//if ([num_cur-1 > iilsize - num_cur])
		if (valueMatch(entries + (num_cur - 1) * size, size, orig_values[iilsize - num_cur], orig_docids[iilsize - num_cur]) > 0)
		{
			num_b = num_cur;
			continue;
		}
		//if ([num_cur < iilsize - num_cur-1])
		if (valueMatch(entries + num_cur * size, size, orig_values[iilsize - num_cur - 1], orig_docids[iilsize - num_cur - 1]) < 0)
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


bool
AosIILBigI64::batchIncSafe(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);

	i64 start_total = mNumDocs;
	u64 start_time = OmnGetTimestamp();
	bool rslt = batchIncPriv(entries, size, num, dftValue, incType, rdata);
	u64 cost = OmnGetTimestamp() - start_time;
	i64 end_total = mNumDocs;

	if (AosIILMgr::smShowLog)
	{
		OmnScreen << "\n\n================================="
				  << "\nBatch Inc Finish, iilid:" << mIILID << ", entry num:" << num
				  << ", time cost:" << AosTimestampToHumanRead(cost)
				  << ", start_total:" << start_total << ", end_total:" << end_total
				  << "\n=================================\n" << endl;
	}

	AOSUNLOCK(mLock);

	return rslt;
}


bool
AosIILBigI64::batchIncPriv(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sanityCheckPriv(rdata), false);

	bool rslt = true;
	i64 subChanged = 0;
	char * cur_entries = entries;
	i64 cur_num = num;

	while (cur_num > 0)
	{
		bool isTail = true;
		rslt = batchIncRecPriv(cur_entries, size, cur_num,
			dftValue, incType, subChanged, isTail, rdata);
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
AosIILBigI64::batchIncRecSafe(
		char * &entries,
		const i64 &size,
		i64 &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		i64 &subChanged,
		const bool isTail,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = batchIncRecPriv(entries, size, num,
		dftValue, incType, subChanged, isTail, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILBigI64::batchIncRecPriv(
		char * &entries,
		const i64 &size,
		i64 &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		i64 &subChanged,
		const bool isTail,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return batchIncSinglePriv(entries, size, num,
			dftValue, incType, subChanged, isTail, rdata);
	}

	aos_assert_r(num > 0, false);
	i64 entry_key = *(i64 *)entries;
	u64 entry_value = 0;
	i64 sub_idx = getSubiilIndex3Priv(entry_key, entry_value, rdata);
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
				num_handle = binarySearchInc(entries, size, num, mMaxVals[sub_idx]) + 1;
				aos_assert_r(num_handle > 0, false);
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

			AosIILBigI64Ptr thisptr(this, false);
			runner = OmnNew AosIILBigI64BtachIncSingleThrd(
				thisptr, sub_iilid, sub_idx, entries,
				size, num_handle, dftValue, incType, rdata);
			runners.push_back(runner);

			//OmnScreen << "iilid : " << sub_iilid << ", iilidx:" << sub_idx << ", iil_entrynum:" << mNumEntries[sub_idx] 
			//	<< ", num_handle:" << num_handle << ", max_handle:" << max_handle << ", isTail:" << it << endl;

			subChanged += (1 + subToAdd);
			num -= num_handle;
			if (num > 0)
			{
				entries = &entries[size * num_handle];
				entry_key = *(i64 *)entries;
				//entry_value = *(u64 *)&entries[size - sizeof(u64)];
				entry_value = 0;
			}

			sub_iilid = 0;
			if (!it && num > 0)
			{
				sub_idx = getSubiilIndex3Priv(entry_key, entry_value, rdata);
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

	AosIILBigI64Ptr subiil = getSubiilByIndexPriv(sub_idx, rdata);
	aos_assert_r(subiil, false);

	i64 num_before = num;
	bool rslt = subiil->batchIncRecSafe(entries, size,
		num, dftValue, incType, subChanged, it, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(num_before > num, false);

	return true;
}


bool
AosIILBigI64::batchIncSinglePriv(
		char * &entries,
		const i64 &size,
		i64 &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		i64 &subChanged,
		const bool isTail,
		const AosRundataPtr &rdata)
{
	i64 num_handle = num;
	if (!isTail)
	{
		aos_assert_r(mNumDocs > 0, false);
		num_handle = binarySearchInc(entries, size, num, mValues[mNumDocs-1]) + 1;
		aos_assert_r(num_handle > 0, false);
	}

	// now we do know how many records we can handle, 
	// if it is too little, put it in one IIL,else we will fill the iils one by one.
	// finally, if the last iil is too small, it wiil share the records with the one before it.
	aos_assert_r(subChanged < (int)eMaxSubChangedToSave, false);
	bool is_last_iil = (subChanged == eMaxSubChangedToSave - 1);

	i64* orig_values = mValues;
	u64* orig_docids = mDocids;
	i64 orig_num = mNumDocs;
	i64 orig_idx = 0;
	mValues = 0;
	mDocids = 0;

	bool rslt = batchIncRebuild(entries, size, num, dftValue,
		incType, num_handle, orig_values, orig_docids, orig_num,
		orig_idx, mAddBlockFillSize, is_last_iil, rdata);
	aos_assert_r(rslt, false);

	subChanged++;
	setDirty(true);

	AosIILBigI64Ptr newsub;
	vector<AosIILBigI64Ptr> subiil_list;
	while ((num_handle > 0 || orig_idx < orig_num) 
			&& subChanged < (int)eMaxSubChangedToSave)
	{
		is_last_iil = (subChanged == eMaxSubChangedToSave - 1);
		newsub = splitContentUtil(rdata);
		rslt = newsub->batchIncRebuild(entries, size, num, dftValue,
			incType, num_handle, orig_values, orig_docids, orig_num,
			orig_idx, mAddBlockFillSize, is_last_iil, rdata);
		aos_assert_r(rslt, false);
		subiil_list.push_back(newsub);
		subChanged++;
	}

	// if the last iil is too small, it wiil share the records with the one before it.
	i64 subToAdd = subiil_list.size();
	if (subToAdd > 0)
	{
		AosIILBigI64Ptr lastiil = subiil_list[subToAdd-1];
		i64 last_num = lastiil->getNumDocs();
		if (last_num < mAddBlockMinSize)
		{
			AosIILBigI64Ptr secondiil;
			if (subToAdd == 1)
			{
				AosIILBigI64Ptr thisptr(this, false);
				secondiil = thisptr;
			}
			else
			{
				secondiil = subiil_list[subToAdd - 2];
			}
			i64 second_num = secondiil->getNumDocs();
			aos_assert_r(second_num > last_num, false);
			i64 doc_to_move = (second_num - last_num) / 2;
			rslt = moveDocsForward(secondiil, lastiil, doc_to_move);
			aos_assert_r(rslt, false);
		}
	}
	
	if (subToAdd > 0)
	{
		// mod by shawn
		// save all the leafs
		OmnThrdShellProcPtr runner;
		vector<OmnThrdShellProcPtr> runners;

		for (u32 i = 0; i < subiil_list.size(); i++)
		{
			runner = OmnNew AosIILBigI64Saver(subiil_list[i], rdata);
			runners.push_back(runner);
		}

		//OmnThreadPool::runProcSync(runners);
		AosIILMgr::smThreadPool->procSync(runners);

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

	if (orig_values)
	{
		OmnDelete [] orig_values;
		orig_values = 0;
	}
	
	if (orig_docids)
	{
		OmnDelete [] orig_docids;
		orig_docids = 0;
	}
		
	return true;
}


bool
AosIILBigI64::batchIncParentThrdSafe(
		const u64 &iilid,
		const i64 &iil_idx,
		char * &entries,
		const i64 &size,
		i64 &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	AosIILBigI64Ptr subiil = getSubiilByIndexPriv(iilid, iil_idx, rdata);
	aos_assert_r(subiil, false);

	bool rslt = subiil->batchIncSingleThrdSafe(
		entries, size, num, dftValue, incType, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILBigI64::batchIncSingleThrdSafe(
		char * &entries,
		const i64 &size,
		i64 &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = batchIncSingleThrdPriv(
		entries, size, num, dftValue, incType, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILBigI64::batchIncSingleThrdPriv(
		char * &entries,
		const i64 &size,
		i64 &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mParentIIL, false);

	i64 num_handle = num;
	bool is_last_iil = false;

	i64* orig_values = mValues;
	u64* orig_docids = mDocids;
	i64 orig_num = mNumDocs;
	i64 orig_idx = 0;
	mValues = 0;
	mDocids = 0;

	bool rslt = batchIncRebuild(entries, size, num, dftValue,
		incType, num_handle, orig_values, orig_docids, orig_num,
		orig_idx, mAddBlockFillSize, is_last_iil, rdata);
	aos_assert_r(rslt, false);

	setDirty(true);

	AosIILBigI64Ptr newsub;
	vector<AosIILBigI64Ptr> subiil_list;
	while (num_handle > 0 || orig_idx < orig_num)
	{
		newsub = splitContentUtil(rdata);
		rslt = newsub->batchIncRebuild(entries, size, num, dftValue,
			incType, num_handle, orig_values, orig_docids, orig_num,
			orig_idx, mAddBlockFillSize, is_last_iil, rdata);
		aos_assert_r(rslt, false);
		subiil_list.push_back(newsub);
	}
	
	// if the last iil is too small, it wiil share the records with the one before it.
	i64 subToAdd = subiil_list.size();
	if (subToAdd > 0)
	{
		AosIILBigI64Ptr lastiil = subiil_list[subToAdd-1];
		i64 last_num = lastiil->getNumDocs();
		if (last_num < mAddBlockMinSize)
		{
			AosIILBigI64Ptr secondiil;
			if (subToAdd == 1)
			{
				AosIILBigI64Ptr thisptr(this, false);
				secondiil = thisptr;
			}
			else
			{
				secondiil = subiil_list[subToAdd - 2];
			}
			i64 second_num = secondiil->getNumDocs();
			aos_assert_r(second_num > last_num, false);
			i64 doc_to_move = (second_num - last_num) / 2;
			rslt = moveDocsForward(secondiil, lastiil, doc_to_move);
			aos_assert_r(rslt, false);
		}
	}
	
	if (subToAdd > 0)
	{
		// mod by shawn
		// save all the leafs
		OmnThrdShellProcPtr runner;
		vector<OmnThrdShellProcPtr> runners;

		for (u32 i = 0; i < subiil_list.size(); i++)
		{
			runner = OmnNew AosIILBigI64Saver(subiil_list[i], rdata);
			runners.push_back(runner);
		}

		//OmnThreadPool::runProcSync(runners);
		AosIILMgr::smThreadPool->procSync(runners);
	}

	mParentIIL->lockUpdate();
	rslt = mParentIIL->updateIndexData(mIILIdx, true, true, rdata);
	if (rslt && subToAdd > 0)
	{
		rslt = mParentIIL->addSubiils(mIILIdx+1, subiil_list, rdata);
	}
	mParentIIL->unlockUpdate();
	aos_assert_r(rslt, false);

	if (orig_values)
	{
		OmnDelete [] orig_values;
		orig_values = 0;
	}
	
	if (orig_docids)
	{
		OmnDelete [] orig_docids;
		orig_docids = 0;
	}
		
	return true;
}


bool
AosIILBigI64::batchIncRebuild(
		char * &entries,
		const i64 &size,
		i64 &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		i64 &num_handle,
		i64* orig_values,
		u64* orig_docids,
		i64 &orig_num,
		i64 &orig_idx,
		const i64 &iilsize,
		const bool isLastSub,
		const AosRundataPtr &rdata)
{	
	bool addFlag = true;
	bool updateFlag = true;
	bool setFlag = false;
	
	switch(incType)
	{
	case AosIILUtil::eIILIncNormal:
		 addFlag = true;
		 updateFlag = true;
		 setFlag = false;
		 break;

	case AosIILUtil::eIILIncNoAdd:
		 addFlag = false;
		 updateFlag = true;
		 setFlag = false;
		 break;

	case AosIILUtil::eIILIncNoUpdate:
		 addFlag = true;
		 updateFlag = false;
		 setFlag = false;
		 break;

	case AosIILUtil::eIILIncSet:
		 addFlag = true;
		 updateFlag = true;
		 setFlag = true;
		 break;

	case AosIILUtil::eIILIncUpdateMin:
		 addFlag = true;
		 updateFlag = true;
		 setFlag = false;
		 break;

	case AosIILUtil::eIILIncUpdateMax:
		 addFlag = true;
		 updateFlag = true;
		 setFlag = false;
		 break;

	default:
		 addFlag = true;
		 updateFlag = true;
		 setFlag = false;
	}

	mValues = OmnNew i64[iilsize + 10];
	mDocids = OmnNew u64[iilsize + 10];
	memset(mValues, 0, sizeof(i64)*(iilsize + 10));
	memset(mDocids, 0, sizeof(u64)*(iilsize + 10));
	mMemCap = iilsize + 10;

	i64 cur_pos = 0;
	
	// case 1: iil full										break;
	// case 2: list_a  finished, 	list_b finished 		break;
	// case 3: list_a finished, 	list_b ok				add b;
	// case 4: list_a ok, 			list_b ok				add a+default;
	// case 5: value_b == value_a							add a to b;
	// case 6: last sub, can not add a(iil full)			add b;
	// case 7: value_a >  value_b							add b;
	// case 8: value_a <  value_b							add a+default;
	i64 cmp_rslt = 0;
	while (cur_pos < iilsize)// case 1
	{
		// case 2		
		if (num_handle <= 0 && orig_idx >= orig_num)
		{
			break;
		}
		
		if (num_handle > 0 && orig_idx < orig_num)
		{
			cmp_rslt = valueMatch(entries, orig_values[orig_idx]);
			if (cmp_rslt == 0) // case 5
			{
				if (incType == AosIILUtil::eIILIncUpdateMin)
				{
					if (orig_docids[orig_idx] > *(u64 *)&entries[size - sizeof(u64)])
					{
						orig_docids[orig_idx] = *(u64 *)&entries[size - sizeof(u64)];
					}
				}
				else if (incType == AosIILUtil::eIILIncUpdateMax)
				{
					if (orig_docids[orig_idx] < *(u64 *)&entries[size - sizeof(u64)])
					{
						orig_docids[orig_idx] = *(u64 *)&entries[size - sizeof(u64)];
					}
				}
				else if (updateFlag)
				{
					if (setFlag)
					{
						orig_docids[orig_idx] = *(u64 *)&entries[size - sizeof(u64)];
					}
					else
					{
						orig_docids[orig_idx] += *(u64 *)&entries[size - sizeof(u64)];
					}
				}
				entries = &entries[size];
				num_handle--; 
				num--; 
				continue;
			}
		}

		if (num_handle <= 0 || // case 3
		   (isLastSub && ((iilsize - cur_pos) <= (orig_num - orig_idx))) || // case 6
 		   (orig_idx < orig_num && cmp_rslt > 0))   // case 7
		{
			mValues[cur_pos] = orig_values[orig_idx];
			mDocids[cur_pos] = orig_docids[orig_idx];
			cur_pos++;
			orig_idx++;
			continue;
		}
		
		// case 4, case 8
		if (addFlag)
		{
			// handle the case if the next entry value is same as this one
			if (num > 1 && (valueMatchI64(entries, &entries[size]) == 0))
			{
				if (incType == AosIILUtil::eIILIncUpdateMin)
				{
					if ((*(u64 *)&entries[size+size-sizeof(u64)]) > (*(u64 *)&entries[size-sizeof(u64)]) )
					{
						*(u64 *)&entries[size+size-sizeof(u64)] = *(u64 *)&entries[size-sizeof(u64)];
					}
				}
				else if (incType == AosIILUtil::eIILIncUpdateMax)
				{
					if ((*(u64 *)&entries[size+size-sizeof(u64)]) < (*(u64 *)&entries[size-sizeof(u64)]))
					{
						*(u64 *)&entries[size+size-sizeof(u64)] = *(u64 *)&entries[size-sizeof(u64)];
					}
				}
				else if (updateFlag)
				{
					if (!setFlag)
					{
						*(u64 *)&entries[size+size-sizeof(u64)] += *(u64 *)&entries[size-sizeof(u64)];
					}
				}
				else
				{
					*(u64 *)&entries[size+size-sizeof(u64)] = *(u64 *)&entries[size-sizeof(u64)];
				}

				entries = &entries[size];
				num_handle--; 
				num--; 
				continue;
			}

			mValues[cur_pos] = *(i64 *)entries;
			mDocids[cur_pos] = *(u64 *)&entries[size-sizeof(u64)] + dftValue;
			cur_pos++;
		}
		entries = &entries[size];
		num_handle--; 
		num--; 
	}
	
	mNumDocs = cur_pos;
	return true;
}


i64 
AosIILBigI64::binarySearch(
		const char * entries,
		const i64 &size,
		const i64 &num,
		const i64 &key,
		const u64 &value)
{
	// find the last index:
	// 		entries[index] <= value
	aos_assert_r(entries && num > 0, -1);
	i64 left = 0;
	i64 right = num - 1;
	if (valueMatch(&entries[size * left], size, key, value) > 0) return -1;
	if (valueMatch(&entries[size * right], size, key, value) < 0) return right;
	i64 idx = 0;
	while (right - left > 1)
	{
		if (left + 1 == right)
		{
			if (valueMatch(&entries[size * right], size, key, value) <= 0) return right;
			return left;
		}

		idx = (right + left) / 2;
		if (valueMatch(&entries[size * idx], size, key, value) <= 0)
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


i64 
AosIILBigI64::binarySearchInc(
		const char * entries,
		const i64 &size,
		const i64 &num,
		const i64 &key)
{
	// find the last index:
	// 		entries[index] <= value
	aos_assert_r(entries && num > 0, -1);
	i64 left = 0;
	i64 right = num - 1;
	if (valueMatch(&entries[size * left], key) > 0) return -1;
	if (valueMatch(&entries[size * right], key) < 0) return right;
	i64 idx = 0;
	while (right - left >= 1)
	{
		if (left + 1 == right)
		{
			if (valueMatch(&entries[size * right], key) <= 0) return right;
			return left;
		}

		idx = (right + left) / 2;
		if (valueMatch(&entries[size * idx], key) == 0)
		{
			left = idx;
			break;
		}
		else if (valueMatch(&entries[size * idx], key) < 0)
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
AosIILBigI64::valueMatch(
		const char * entry,
		const i64 &size,
		const i64 &key,
		const u64 &value)
{
	i64 entry_key = *(i64 *)entry;
	if (entry_key > key) return 1;
	if (entry_key < key) return -1;

	u64 entry_value = *(u64 *)&entry[size - sizeof(u64)];
	if (entry_value > value) return 1;
	if (entry_value < value) return -1;
	return 0;
}


int
AosIILBigI64::valueMatch(
		const char * entry,
		const i64 &key)
{
	i64 entry_key = *(i64 *)entry;
	if (entry_key > key) return 1;
	if (entry_key < key) return -1;
	return 0;
}


int
AosIILBigI64::valueMatchI64(
		const char * entry1,
		const char * entry2)
{
	i64 entry_key1 = *(i64 *)entry1;
	i64 entry_key2 = *(u64 *)entry2;
	if (entry_key1 > entry_key2) return 1;
	if (entry_key1 < entry_key2) return -1;
	return 0;
}


bool
AosIILBigI64::moveDocsForward(
		const AosIILBigI64Ptr &from_iil,
		const AosIILBigI64Ptr &to_iil,
		const i64 &num_to_move)
{
	i64 num1 = from_iil->getNumDocs();
	i64 num2 = to_iil->getNumDocs();

	aos_assert_r(num_to_move > 0, false);
	aos_assert_r(num1 > num_to_move, false);
	aos_assert_r(num2 > 0, false);

	to_iil->mNumDocs += num_to_move;
	bool rslt = to_iil->expandMemoryPriv();
	aos_assert_r(rslt, false);

	i64*	values1 = from_iil->mValues;
	u64*	docids1 = from_iil->mDocids;
	i64*	values2 = to_iil->mValues;
	u64*	docids2 = to_iil->mDocids;
	
	memmove(&values2[num_to_move], values2, sizeof(i64) * num2);
	memmove(&docids2[num_to_move], docids2, sizeof(u64) * num2);

	memcpy(values2, &values1[num1-num_to_move], sizeof(i64) * num_to_move);
	memcpy(docids2, &docids1[num1-num_to_move], sizeof(u64) * num_to_move);
	
	memset(&values1[num1-num_to_move], 0, sizeof(i64) * num_to_move);
	memset(&docids1[num1-num_to_move], 0, sizeof(u64) * num_to_move);
	
	from_iil->mNumDocs -= num_to_move;
	return true;
}


bool
AosIILBigI64::batchDelSafe(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	aos_assert_r(size  ==(int)sizeof(u64) * 2 ,false);
	i64* cur_pos = (i64*)entries;
	for(i64 i = 0;i < num-1;i++)
	{
		aos_assert_r((*cur_pos) <= (*(cur_pos+2)),false);
		if((*cur_pos) == (*(cur_pos+2)))
		{
			// compare docid
			aos_assert_r( (*(u64*)(cur_pos+1)) <= (*(u64*)(cur_pos+3)) , false);
		}
		cur_pos += 2;
	}

	AOSLOCK(mLock);

	i64 start_total = mNumDocs;
	u64 start_time = OmnGetTimestamp();
	bool rslt = batchDelPriv(entries, size, num, rdata);
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
AosIILBigI64::batchDelPriv(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const AosRundataPtr &rdata)
{	
	char * cur_entries = entries;
	i64 cur_num = num;
	bool rslt = batchDelRecPriv(cur_entries, size, cur_num, rdata);
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
AosIILBigI64::batchDelRecSafe(
		char * &entries,
		const i64 &size,
		i64 &num,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = batchDelRecPriv(entries, size, num, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILBigI64::batchDelRecPriv(
		char * &entries,
		const i64 &size,
		i64 &num,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return batchDelSinglePriv(entries, size, num, rdata);
	}

	aos_assert_r(num > 0, false);
	i64 entry_key = *(i64 *)entries;
	u64 entry_value = *(u64 *)&entries[size - sizeof(u64)];
	i64 idx = getSubiilIndex3Priv(entry_key, entry_value, rdata);
	aos_assert_r(idx >= 0 && idx < mNumSubiils, false);

	i64 last_num = num;
	AosIILBigI64Ptr subiil;
	bool rslt;
	for (i64 i=idx; i<mNumSubiils; i++)
	{
		subiil = getSubiilByIndexPriv(i, rdata);
		aos_assert_r(subiil, false);
	
		rslt = subiil->batchDelRecSafe(entries, size, num, rdata);
		aos_assert_r(rslt, false);
	}
	aos_assert_r(last_num >= num, false);
	if (last_num == num) return true;
	
	rslt = resetMaxMin(idx, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosIILBigI64::batchDelSinglePriv(
		char * &entries,
		const i64 &size,
		i64 &num,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs > 0, false);
	
	setDirty(true);
	
	i64* orig_values = mValues;
	u64* orig_docids = mDocids;
	i64 orig_num = mNumDocs;
	i64 orig_idx = 0;
	
	mValues = 0;
	mDocids = 0;
	i64 iilsize = mNumDocs;

	mValues = OmnNew i64[iilsize + 10];
	memset(mValues, 0, sizeof(i64)*(iilsize + 10));
	mDocids = OmnNew u64[iilsize + 10];
	memset(mDocids, 0, sizeof(u64)*(iilsize + 10));
	mMemCap = iilsize + 10;

	i64 cur_pos = 0;
	while (cur_pos < iilsize)
	{
		if (orig_idx >= orig_num)
		{
			break;
		}
		
		if (num > 0 && (orig_idx >= orig_num ||
			valueMatch(entries, size, orig_values[orig_idx], orig_docids[orig_idx]) <= 0))
		{
			if ((orig_idx < orig_num) && valueMatch(entries, size, orig_values[orig_idx], orig_docids[orig_idx]) == 0)
			{
				orig_idx++;
			}
			num--; 
			entries = &(entries[size]);
		}
		else
		{
			mValues[cur_pos] = orig_values[orig_idx];
			mDocids[cur_pos] = orig_docids[orig_idx];
			cur_pos++;
			orig_idx++;
		}
	}
	mNumDocs = cur_pos;
	
	if (orig_values)
	{
		OmnDelete [] orig_values;
		orig_values = 0;
	}
	
	if (orig_docids)
	{
		OmnDelete [] orig_docids;
		orig_docids = 0;
	}
	return true;
}


bool
AosIILBigI64::batchDecSafe(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const bool delete_flag,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	// This function inc all the entries 'entries' into this IIL. 
	AOSLOCK(mLock);

	if (AosIILMgr::smShowLog)
	{
		OmnScreen << "\n==============================="
				  << "\n= Batch Dec Start: " << num
				  << "\n= entry size: " << size
				  << "\n= current mNumDocs: " << mNumDocs
				  << "\n= current Level: " << mLevel << ":" << mNumSubiils
				  << "\n= current mIILID: " << mIILID
				  << "\n==============================="<< endl;
	}

	bool rslt = batchDecPriv(entries, size, num, delete_flag, incType, rdata);

	if (AosIILMgr::smShowLog)
	{
		OmnScreen << "\n==============================="
				  << "\n= Batch Dec Finish: " << num
				  << "\n= entry size: " << size
				  << "\n= current mNumDocs: " << mNumDocs
				  << "\n= current Level: " << mLevel << ":" << mNumSubiils
				  << "\n= current mIILID: " << mIILID
				  << "\n==============================="<< endl;
	}

	AOSUNLOCK(mLock);

	return rslt;
}


bool
AosIILBigI64::batchDecPriv(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const bool delete_flag,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	char * cur_entries = entries;
	i64 cur_num = num;
	bool rslt = batchDecRecPriv(cur_entries, size, cur_num, delete_flag, incType, rdata);
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
AosIILBigI64::batchDecRecSafe(
		char * &entries,
		const i64 &size,
		i64 &num,
		const bool delete_flag,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = batchDecRecPriv(entries, size, num, delete_flag, incType, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILBigI64::batchDecRecPriv(
		char * &entries,
		const i64 &size,
		i64 &num,
		const bool delete_flag,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return batchDecSinglePriv(entries, size, num, delete_flag, incType, rdata);
	}

	aos_assert_r(num > 0, false);
	i64 entry_key = *(i64 *)&entries;
	u64 entry_value = 0;
	i64 idx = getSubiilIndex3Priv(entry_key, entry_value, rdata);
	aos_assert_r(idx >= 0 && idx < mNumSubiils, 0);

	i64 last_num = num;
	AosIILBigI64Ptr subiil;
	bool rslt;
	for (i64 i=idx; i<mNumSubiils; i++)
	{
		subiil = getSubiilByIndexPriv(i, rdata);
		aos_assert_r(subiil, false);
	
		rslt = subiil->batchDecRecSafe(entries, size, num, delete_flag, incType, rdata);
		aos_assert_r(rslt, false);
	}

	aos_assert_r(last_num >= num, false);
	if (last_num == num) return true;
	
	rslt = resetMaxMin(idx, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosIILBigI64::batchDecSinglePriv(
		char * &entries,
		const i64 &size,
		i64 &num,
		const bool delete_flag,
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs > 0, false);
	if (incType != AosIILUtil::eIILIncNormal || incType != AosIILUtil::eIILIncNoUpdate)
	{
		return true;
	}
	
	setDirty(true);
	
	i64* orig_values = mValues;
	u64* orig_docids = mDocids;
	i64 orig_num = mNumDocs;
	i64 orig_idx = 0;
	
	mValues = 0;
	mDocids = 0;
	i64 iilsize = mNumDocs;

	mValues = OmnNew i64[iilsize + 10];
	mDocids = OmnNew u64[iilsize + 10];
	memset(mValues, 0, sizeof(i64)*(iilsize + 10));
	memset(mDocids, 0, sizeof(u64)*(iilsize + 10));
	mMemCap = iilsize + 10;

	i64 cur_pos = 0;
	while (cur_pos < iilsize)
	{
		if ((num <= 0) && (orig_idx >= orig_num))
		{
			break;
		}
		
		if ((num > 0) && ((orig_idx >= orig_num) ||
			(valueMatch(entries, orig_values[orig_idx]) <= 0)))
		{
			if ((orig_idx < orig_num) && valueMatch(entries, orig_values[orig_idx]) == 0)
			{
				u64 docid = *(u64 *)&entries[size - sizeof(u64)]; 
				if (incType == AosIILUtil::eIILIncNormal)
				{
					orig_docids[orig_idx] -= docid;
					if (orig_docids[orig_idx] == 0 && delete_flag)
					{
						orig_idx++;
					}
				}
				else if (incType == AosIILUtil::eIILIncNoUpdate)
				{
					if (orig_docids[orig_idx] == docid)
					{
						orig_idx++;
					}
				}
			}
			num--; 
			entries = &(entries[size]);
		}
		else
		{
			//copy value/docid
			mValues[cur_pos] = orig_values[orig_idx];
			mDocids[cur_pos] = orig_docids[orig_idx];
			cur_pos++;
			orig_idx++;
		}
	}
	mNumDocs = cur_pos;
	
	if (orig_values)
	{
		OmnDelete [] orig_values;
		orig_values = 0;
	}
	
	if (orig_docids)
	{
		OmnDelete [] orig_docids;
		orig_docids = 0;
	}
	return true;
}


AosBitmapObjPtr 
AosIILBigI64::createBitmap()
{
	aos_assert_r((mNumDocs == 0) || mDocids, 0);
	aos_assert_r(mLevel == 0, 0);

	AosBitmapObjPtr bitmap = AosGetBitmap();
	aos_assert_r(bitmap, 0);

	bitmap->setBitmapId(mIILID);
	bitmap->setNodeLevel(0);
	bitmap->setIILLevel(0);

	bitmap->appendDocids(mDocids, mNumDocs);
	return bitmap;
}


bool
AosIILBigI64::rebuildBitmapSafe(const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = rebuildBitmapPriv1(rdata);
	AOSUNLOCK(mLock);
	return rslt;
}

bool
AosIILBigI64::rebuildBitmapPriv1(const AosRundataPtr &rdata)
{
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
AosIILBigI64::clearBitmapPriv(
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
AosIILBigI64::clearBitmapRecPriv(
		AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	AosIILBigI64Ptr subiil;
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
AosIILBigI64::rebuildBitmapPriv(
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
AosIILBigI64::rebuildLevelOne(
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
	AosBitmapObjPtr cur_bitmap;
	AosIILBigI64Ptr subiil;
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
AosIILBigI64::rebuildBitmapRecPriv(
		AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	AosIILBigI64Ptr subiil;
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
	AosBitmapObjPtr cur_bitmap;

	OmnThrdShellProcPtr runner;
	vector<OmnThrdShellProcPtr> runners;

	AosIILBigI64Ptr thisPtr(this, false);
	u32 max_runner_size = 24;
	for (u64 i = 0; i < parent_list.size(); i++)
	{
		runner = OmnNew AosIILBigI64RebuildBitmapThrd(
			thisPtr, cur_idx, child_num_list[i], parent_list[i], rdata);
		cur_idx += child_num_list[i];
		runners.push_back(runner);

		if (runners.size() >= max_runner_size)
		{
			//OmnThreadPool::runProcSync(runners);
			AosIILMgr::smThreadPool->procSync(runners);
			runners.clear();
		}

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
AosIILBigI64::getBitmapTree(const AosRundataPtr &rdata)
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
AosIILBigI64::saveBitmapTree(const AosRundataPtr &rdata)
{
	if(mBitmapTree)
	{
		AosBitmapTreeMgrObj::getObject()->saveBitmapTree(rdata, mBitmapTree);
	}
	return true;
}


bool 
AosIILBigI64::setBitmapTree(
		const AosBitmapTreeObjPtr &tree,
		const AosRundataPtr &rdata)
{
	//1. set bitmaptree
	//2. save the tree
	
	mBitmapTree = tree;	
	aos_assert_r(mBitmapTree->getBmpID() == mIILID, false);
	return true;
}

