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
#include "IILMgrBig/IILU64.h"


static bool sgShowLog = false;

bool
AosIILU64::addBlockSafe(
		u64* entries,
		const int num,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	// This function adds all the entries 'entries' into this IIL. 
	bool rslt = false;

	if(sgShowLog)
	{
		OmnScreen << "==============================="<< endl;
		OmnScreen << "= To add block: " << num << "  =" << endl;
		OmnScreen << "= current mNumDocs: " << mNumDocs << "  =" << endl;
		OmnScreen << "= current mNumSubiils: " << mNumSubiils<< "  =" << endl;
		OmnScreen << "= current mIILID: " << mIILID << "  =" << endl;
		OmnScreen << "==============================="<< endl;
	}

//OmnScreen << "To add block: " << num << endl;
//OmnScreen << "Level: " << mLevel << ":" << mNumSubiils << ":" << mNumDocs << endl;
	
	// Chen Ding, 04/24/2012
	// Sorted in iilclient. Ken Lee 2012/04/25
	//AosU64U64Array::sort(entries, num);
	int subChanged = 0;
	AOSLOCK(mLock);
	aos_assert_r(sanityCheckPriv(rdata), false);
	u64* cur_entries = entries;
	int cur_num = num;
	while(cur_num > 0)
	{
		if(needCallDistr1())
		{
			rslt = addBlockDistrPriv(cur_entries, cur_num, iilmgrLocked, rdata);
			AOSLOCK(mLock);
			return rslt;
		}
			
			
		if (isLeafIIL())
		{
			aos_assert_r(sanityCheckPriv(rdata), false);
			addBlockSinglePriv(cur_entries, cur_num, subChanged, true, iilmgrLocked, rdata);
			aos_assert_r(sanityCheckPriv(rdata), false);
		}
		else
		{
			aos_assert_r(sanityCheckPriv(rdata), false);
			addBlockRecPriv(cur_entries, cur_num, subChanged, true, iilmgrLocked, rdata);
			// aos_assert_r(sanityCheckPriv(rdata), false);
			if (!sanityCheckPriv(rdata))
				sanityCheckPriv(rdata);
		}
		
		// Save the changes when either there are too many processed or all contents
		// were processed.
		if (subChanged >= eMaxSubChangedToSave - 1 || cur_num <= 0)
		{
			AOSUNLOCK(mLock);
			saveToFileSafe(false,
						   false,
						   false, 
						   rdata);
			bool returned = false;
			returnSubIILsSafe(iilmgrLocked,
					returned,
					rdata,
					false);
			AOSLOCK(mLock);
			subChanged = 0;
			
			if(getDistrType() == eDistrType1 && 
			   getDistrStatus() == eDistr_Local &&
			   mNumDocs > eNumDocsToDistr)
			{
				rslt = distributeIILPriv(iilmgrLocked,rdata);
				aos_assert_rl(rslt,mLock,false);
			}
		}
	}	
	if(sgShowLog)
	{
		OmnScreen << "Finished merge: " << mLevel << ":" << mNumSubiils << ":" << mNumDocs << endl;
	}
	
	aos_assert_rl(sanityCheckPriv(rdata), mLock,false);
	AOSUNLOCK(mLock);
	
	return true;
}

bool
AosIILU64::addBlockRecSafe(
		u64* &entries,
		int &num,
		int &subChanged,
		const bool &isTail,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = false;
	if(isLeafIIL())
	{
		rslt = addBlockSinglePriv(entries,
							 num,
							 subChanged,
							 isTail,
							 iilmgrLocked,
							 rdata);
	}
	else
	{
		rslt = addBlockRecPriv(entries,
							 num,
							 subChanged,
							 isTail,
							 iilmgrLocked,
							 rdata);
	}
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILU64::addBlockRecPriv(
		u64* &entries,
		int &num,
		int &subChanged,
		const bool &isTail,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(num > 0,false);
	bool rslt = false;
	AosIILU64Ptr subiil;
	bool it = false;

	AosIILIdx idx;
	subiil = getSubiil3Priv(entries[0], 
					  entries[1], 
					  false,
					  rdata);
	aos_assert_r(subiil,false);

	it = isTail &&(subiil->getIILIdx() == ((int)mNumSubiils-1));

	int num_before = num;
	aos_assert_r(sanityCheckPriv(rdata), false);
	rslt = subiil->addBlockRecSafe(entries,
						 num,
						 subChanged,
						 it,
						 iilmgrLocked,
						 rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(num_before > num, false);
//	mNumDocs += num_before - num;
	aos_assert_r(sanityCheckPriv(rdata), false);

	return rslt;
}


bool
AosIILU64::addBlockSinglePriv(
		u64* &entries,
		int &num,
		int &subChanged,
		const bool isTail,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	int num_handle = num;
	if (!isTail)
	{
		aos_assert_r(mNumDocs > 0, false);
		num_handle = binarySearch(entries,
								  num,
								  mValues[mNumDocs-1],
								  mDocids[mNumDocs-1]) + 1 ;
		aos_assert_r(num_handle > 0,false);
	}

	int max_handle = (eMaxSubChangedToSave - subChanged) * mAddBlockFillSize - mNumDocs;
	if(num_handle > max_handle)
	{
		num_handle = max_handle;
	}

	int total = num_handle + mNumDocs;	
	int subToAdd = (total -1) / mAddBlockFillSize;
	if(total < (int)mAddBlockMaxSize)
	{
		subToAdd = 0;
	}
	int subiilsize = total/(subToAdd+1);
	int iilsize = total - subiilsize * subToAdd;

	u64* orig_values = mValues;
	u64* orig_docids = mDocids;
	int orig_num = mNumDocs;
	int orig_idx = 0;
	
	mValues = 0;
	mDocids = 0;
	
	bool rslt = addBlockRebuild(entries,
								num,
								num_handle,
								orig_values,
								orig_docids,
								orig_num,
								orig_idx,
								iilsize);

	aos_assert_r(rslt,false);
	if (subToAdd > 0)
	{
		// create new subiils
		vector<AosIILU64Ptr> subiil_list;
		AosIILU64Ptr newsub;
		for(int i = 0;i < subToAdd;i++)
		{
			newsub= splitContentUtil(iilmgrLocked,rdata);
			rslt = newsub->addBlockRebuild(entries,
											num,
											num_handle,
											orig_values,
											orig_docids,
											orig_num,
											orig_idx,
											subiilsize);
			subiil_list.push_back(newsub);
			aos_assert_r(rslt,false);
		
		}
		
		if(isSingleIIL())
		{
			// This IIL is a leaf IIL and has no parent IIL. 
			splitListSinglePriv(subiil_list, iilmgrLocked,rdata);
		}
		else
		{
			aos_assert_r(hasParentIIL(),false);
			aos_assert_r(mParentIIL, false);
			aos_assert_r(mParentIIL->getIILType() == eAosIILType_U64, false);
			AosIILU64 *parent = (AosIILU64*)mParentIIL.getPtr();
			rslt = parent->subiilSplited(mIILIdx, subiil_list, iilmgrLocked, rdata);		
		}
	}
	else
	{
		if(hasParentIIL())
		{
			aos_assert_r(mParentIIL, false);
			AosIILU64 *parent = (AosIILU64*)mParentIIL.getPtr();
			parent->updateIndexData(mIILIdx,true,true,rdata);
		}
	}	

	if(orig_values)
	{
		OmnDelete []orig_values;
		orig_values = 0;
	}
	
	if(orig_docids)
	{
		OmnDelete []orig_values;
		orig_values = 0;
	}
	
	subChanged ++;
	subChanged += subToAdd;
	
	return true;
}

int 
AosIILU64::binarySearch(
		u64* &entries,
		const int num,
		u64 &value,
		const u64 &docid)
{
	// find the last index:
	// 		entries[index] <= value
	aos_assert_r(entries && num > 0,-1);
	int left = 0;
	int right = num-1;
	if (valueMatch(entries[0], value, docid) > 0) return -1;
	if (valueMatch(entries[right], value, docid) < 0) return right;
	int idx = 0;
	while (right-left > 1)
	{
		if (left + 1 == right)
		{
			if (valueMatch(entries[right], value, docid) <= 0) return right;
			return left;
		}

		idx = (right + left)/2;
		if(valueMatch(entries[idx],value,docid) <= 0)
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
AosIILU64::valueMatch(
		const u64 &entry,
		const u64 &value,
		const u64 &docid)
{
	if(entry > value)
	{
		return 1;
	}
	
	if(entry < value)
	{
		return -1;
	}
	
//	if(entry[1] > docid)
	if(entry > docid)
	{
		 return 1;
	}
	
//	if(entry[1] < docid)
	if(entry < docid)
	{
		 return -1;
	}
	return 0;
}

bool
AosIILU64::addBlockRebuild(u64* &entries,
				int &num,
				int &num_handle,
				u64* &orig_values,
				u64* &orig_docids,
				int &orig_num,
				int &orig_idx,
				const int iilsize)
{	
	if(mValues)
	{
		OmnDelete[] mValues;
		mValues = 0;
	}

	if(mDocids)
	{
		OmnDelete[] mDocids;
		mDocids = 0;
	}
	
	mValues = OmnNew u64[iilsize + 10];
	memset(mValues,0,sizeof(u64)*(iilsize + 10));
	mDocids = OmnNew u64[iilsize + 10];
	memset(mDocids,0,sizeof(u64)*(iilsize + 10));
	mMemCap = iilsize + 10;

	int cur_pos = 0;
	while(cur_pos < iilsize)
	{			
		if((num_handle <= 0) && (orig_idx >= orig_num))
		{
			// merged
			// OmnScreen << "Finished: " << num_handle << ":" << orig_idx << ":" << orig_num << endl;
			break;
		}
		
		if((num_handle > 0) && 
			((orig_idx >= orig_num)||
			(valueMatch(entries[0],orig_values[orig_idx],orig_docids[orig_idx])< 0)))
		{
			mValues[cur_pos] = entries[0];
			mDocids[cur_pos] = entries[1];
			entries = &(entries[1]);
			num_handle --; 
			num --; 
			cur_pos ++;
		}
		else
		{
			//copy value/docid
			mValues[cur_pos] = orig_values[orig_idx];
			mDocids[cur_pos] = orig_docids[orig_idx];
			orig_idx ++;
			cur_pos ++;
		}
	}
	
	mNumDocs = cur_pos;
	return true;
}



bool
AosIILU64::increaseBlockSafe(
		u64* entries,
		const int num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	// This function increases all the entries 'entries' into this IIL. 
	
	if(sgShowLog)
	{
		OmnScreen << "To increase block: " << num << endl;
		OmnScreen << "Level: " << mLevel << ":" << mNumSubiils << ":" << mNumDocs << endl;

		OmnScreen << "*******************************"<< endl;
		OmnScreen << "* To increse block: " << num << "  *" << endl;
		OmnScreen << "* current mNumDocs: " << mNumDocs << "  *" << endl;
		OmnScreen << "* current mIILID: " << mIILID << "  *" << endl;
		OmnScreen << "*******************************"<< endl;
	}

	int subChanged = 0;
	AOSLOCK(mLock);
	aos_assert_r(sanityCheckPriv(rdata), false);
	u64* cur_entries = entries;
	int cur_num = num;
	while(cur_num > 0)
	{
		if (isLeafIIL())
		{
			aos_assert_r(sanityCheckPriv(rdata), false);
			increaseBlockSinglePriv(cur_entries, cur_num, dftValue, incType, subChanged, true, iilmgrLocked, rdata);
			aos_assert_r(sanityCheckPriv(rdata), false);
		}
		else
		{
			aos_assert_r(sanityCheckPriv(rdata), false);
			increaseBlockRecPriv(cur_entries, cur_num, dftValue, incType, subChanged, true, iilmgrLocked, rdata);
			// aos_assert_r(sanityCheckPriv(rdata), false);
			if (!sanityCheckPriv(rdata))
				sanityCheckPriv(rdata);
		}
		
		// Save the changes when either there are too many processed or all contents
		// were processed.
		if (subChanged >= eMaxSubChangedToSave - 1 || cur_num <= 0)
		{
			AOSUNLOCK(mLock);
			saveToFileSafe(false,
						   false,
						   false, 
						   rdata);
			bool returned = false;
			returnSubIILsSafe(iilmgrLocked,
					returned,
					rdata,
					false);
			AOSLOCK(mLock);
			subChanged = 0;
		}
	}	
	
	if(sgShowLog)
	{
		OmnScreen << "Finished merge: " << mLevel << ":" << mNumSubiils << ":" << mNumDocs << endl;
	}
	aos_assert_r(sanityCheckPriv(rdata), false);
	AOSUNLOCK(mLock);
	return true;
}

bool
AosIILU64::increaseBlockRecSafe(
		u64* &entries,
		int &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		int &subChanged,
		const bool &isTail,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = false;
	if(isLeafIIL())
	{
		rslt = increaseBlockSinglePriv(entries,
							 num,
							 dftValue,
							 incType,
							 subChanged,
							 isTail,
							 iilmgrLocked,
							 rdata);
	}
	else
	{
		rslt = increaseBlockRecPriv(entries,
							 num,
							 dftValue,
							 incType, 
							 subChanged,
							 isTail,
							 iilmgrLocked,
							 rdata);
	}
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILU64::increaseBlockRecPriv(
		u64* &entries,
		int &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		int &subChanged,
		const bool &isTail,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(num > 0,false);
	bool rslt = false;
	AosIILU64Ptr subiil;
	bool it = false;

	AosIILIdx idx;
	subiil = getSubiil3Priv(entries[0], 
					  entries[1], 
					  false,
					  rdata);
	aos_assert_r(subiil,false);

	it = isTail &&(subiil->getIILIdx() == ((int)mNumSubiils-1));

	int num_before = num;
	aos_assert_r(sanityCheckPriv(rdata), false);
	rslt = subiil->increaseBlockRecSafe(entries,
						 num,
						 dftValue,
						 incType, 
						 subChanged,
						 it,
						 iilmgrLocked,
						 rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(num_before > num, false);
//	mNumDocs += num_before - num;
	aos_assert_r(sanityCheckPriv(rdata), false);

	return rslt;
}


bool
AosIILU64::increaseBlockSinglePriv(
		u64* &entries,
		int &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		int &subChanged,
		const bool isTail,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	int num_handle = num;
	if (!isTail)
	{
		aos_assert_r(mNumDocs > 0, false);
		num_handle = binarySearch(entries,
								  num,
								  mValues[mNumDocs-1],
								  mDocids[mNumDocs-1]) + 1 ;
		aos_assert_r(num_handle > 0,false);
	}

	// now we do know how many records we can handle, 
	// if it is too little, put it in one IIL,else we will fill the iils one by one.
	// finally, if the last iil is too small, it wiil share the records with the one before it.
	aos_assert_r(subChanged < eMaxSubChangedToSave,false);
	bool is_last_iil = (subChanged == eMaxSubChangedToSave -1);
	
	int subToAdd = 0;

	u64* orig_values = mValues;
	u64* orig_docids = mDocids;
	int orig_num = mNumDocs;
	int orig_idx = 0;

	mValues = 0;
	mDocids = 0;

	bool rslt = increaseBlockRebuild(entries,
								num,
								dftValue,
								incType, 
								num_handle,
								orig_values,
								orig_docids,
								orig_num,
								orig_idx,
								mAddBlockMaxSize,
								is_last_iil);
	aos_assert_r(rslt,false);
	subChanged ++;
	
	vector<AosIILU64Ptr> subiil_list;
	AosIILU64Ptr newsub;
	while(num_handle > 0 && subChanged < eMaxSubChangedToSave)
	{
		is_last_iil = (subChanged == eMaxSubChangedToSave -1);
		newsub= splitContentUtil(iilmgrLocked,rdata);
		rslt = newsub->increaseBlockRebuild(entries,
										num,
										dftValue,
										incType, 
										num_handle,
										orig_values,
										orig_docids,
										orig_num,
										orig_idx,
										mAddBlockFillSize,
										is_last_iil);
		subiil_list.push_back(newsub);
		aos_assert_r(rslt,false);
		subChanged ++;
		subToAdd ++;
	}
	
	// if the last iil is too small, it wiil share the records with the one before it.
	if(subToAdd > 0)
	{
		AosIILU64Ptr lastiil = subiil_list[subToAdd-1];
		int last_num = lastiil->getNumDocs();
		if(last_num < (int)mAddBlockMinSize)
		{
			AosIILU64Ptr secondiil;
			if(subToAdd == 1)
			{
				AosIILU64Ptr thisPtr(this, false);
				secondiil = thisPtr;
			}
			else
			{
				secondiil = subiil_list[subToAdd-2];
			}
			int second_num = secondiil->getNumDocs();
			aos_assert_r(second_num > last_num,false);
			int doc_to_move = (second_num - last_num)/2;
			rslt = moveDocsForward(secondiil,lastiil,doc_to_move);
			aos_assert_r(rslt,false);
		}
	}
	
	if (subToAdd > 0)
	{
		if(isSingleIIL())
		{
			// This IIL is a leaf IIL and has no parent IIL. 
			splitListSinglePriv(subiil_list, iilmgrLocked,rdata);
		}
		else
		{
			aos_assert_r(hasParentIIL(),false);
			aos_assert_r(mParentIIL, false);
			aos_assert_r(mParentIIL->getIILType() == eAosIILType_Str, false);
			AosIILU64 *parent = (AosIILU64*)mParentIIL.getPtr();
			rslt = parent->subiilSplited(mIILIdx, subiil_list, iilmgrLocked, rdata);		
		}
	}
	else
	{
		if(hasParentIIL())
		{
			aos_assert_r(mParentIIL, false);
			AosIILU64 *parent = (AosIILU64*)mParentIIL.getPtr();
			parent->updateIndexData(mIILIdx,true,true,rdata);
		}
	}	

	if(orig_values)
	{
		OmnDelete []orig_values;
		orig_values = 0;
	}
	
	if(orig_docids)
	{
		OmnDelete []orig_values;
		orig_values = 0;
	}
		
	return true;
}

bool
AosIILU64::increaseBlockRebuild(u64* &entries,
				int &num,
				const u64 &dftValue,
				const AosIILUtil::AosIILIncType incType,
				int &num_handle,
				u64* &orig_values,
				u64* &orig_docids,
				int &orig_num,
				int &orig_idx,
				const int maxiilsize,
				const bool isLastSub)
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

		default:
			addFlag = true;
			updateFlag = true;
			setFlag = false;
	}
	if(mValues)
	{
		OmnDelete[] mValues;
		mValues = 0;
	}

	if(mDocids)
	{
		OmnDelete[] mDocids;
		mDocids = 0;
	}
	
	mValues = OmnNew u64[maxiilsize + 10];
	memset(mValues,0,sizeof(u64)*(maxiilsize + 10));
	mDocids = OmnNew u64[maxiilsize + 10];
	memset(mDocids,0,sizeof(u64)*(maxiilsize + 10));
	mMemCap = maxiilsize + 10;

	int cur_pos = 0;
	
	// case 1: iil full										break;
	// case 2: list_a  finished, 	list_b finished 		break;
	// case 3: list_a finished, 	list_b ok				add b;
	// case 4: list_a ok, 			list_b ok				add a+default;
	// case 5: value_b == value_a							add a to b;
	// case 6: last sub, can not add a(iil full)			add b;
	// case 7: value_a >  value_b							add b;
	// case 8: value_a <  value_b							add a+default;
	int cmp_rslt = 0;
	while(cur_pos < maxiilsize)// case 1
	{
		// case 2		
		if((num_handle <= 0) && (orig_idx >= orig_num))
		{
			break;
		}
		
		if((num_handle > 0) && (orig_idx < orig_num))
		{
			cmp_rslt = valueMatch(entries[0],orig_values[orig_idx]);
			if(cmp_rslt == 0) // case 5
			{
				if(updateFlag)
				{
					if(setFlag)
					{
						orig_docids[orig_idx] = entries[1];
					}
					else
					{
						orig_docids[orig_idx] += entries[1];
					}
				}
				entries = &(entries[1]);
				num_handle --; 
				num --; 
				continue;
			}
		}

		if((num_handle <= 0) 	|| // case 3
		   (isLastSub && ((maxiilsize-cur_pos) <= (orig_num-orig_idx))) || // case 6
 		   ((orig_idx < orig_num) && (cmp_rslt > 0)))   // case 7
		{			
			mValues[cur_pos] = orig_values[orig_idx];
			mDocids[cur_pos] = orig_docids[orig_idx];
			orig_idx ++;
			cur_pos ++;
			continue;
		}
		
		// case 4, case 8
		if(addFlag)
		{
			// handle the case if the next entry value is same as this one
			if(num > 1 && 
			   (valueMatchU64(entries[0],entries[1]) == 0))
			{
				if(updateFlag)
				{
					if(setFlag)
					{
					}
					else
					{
						entries[1] += entries[1];
					}
				}
				else
				{
					entries[1] = entries[1];
				}
				entries = &(entries[1]);
				num_handle --; 
				num --; 
				continue;
			}

			mValues[cur_pos] = entries[0];
			mDocids[cur_pos] = entries[1] + dftValue;
			cur_pos ++;
		}
		entries = &(entries[1]);
		num_handle --; 
		num --; 
	}
	
	mNumDocs = cur_pos;
	return true;
}

int
AosIILU64::valueMatch(
		const u64 &entry,
		const u64 &value)
{
	if(entry > value)return 1;
	if(entry < value)return -1;
	return 0;
}

int
AosIILU64::valueMatchU64(
		const u64 &entry1,
		const u64 &entry2)
{
	if(entry1 > entry2)return 1;
	if(entry1 < entry2)return -1;
	return 0;
}

bool
AosIILU64::moveDocsForward(
		const AosIILU64Ptr &from_iil,
		const AosIILU64Ptr &to_iil,
		const int &num_to_move)
{
	int num1 = from_iil->getNumDocs();
	int num2 = to_iil->getNumDocs();

	aos_assert_r(num1 > num_to_move,false);
	aos_assert_r(num2 > 0,false);
	aos_assert_r(num_to_move > 0,false);

	to_iil->mNumDocs += num_to_move;
	aos_assert_r(to_iil->expandMemoryPriv(), false);

	u64* 	values1 = from_iil->mValues;
	u64*   	docids1 = from_iil->mDocids;
	u64* 	values2 = to_iil->mValues;
	u64*   	docids2 = to_iil->mDocids;
	
	memmove(&values2[num_to_move], values2, sizeof(u64) * num2);
	memmove(&docids2[num_to_move], docids2, sizeof(u64) * num2);

	memcpy(values2,&values1[num1-num_to_move],sizeof(u64) * num_to_move);
	memcpy(docids2,&docids1[num1-num_to_move],sizeof(u64) * num_to_move);
	
	memset(&values1[num1-num_to_move], 0, sizeof(u64) * num_to_move);
	memset(&docids1[num1-num_to_move], 0, sizeof(u64) * num_to_move);
	
	from_iil->mNumDocs -= num_to_move;
	return true;
}
