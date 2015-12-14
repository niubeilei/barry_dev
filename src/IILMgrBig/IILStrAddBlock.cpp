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
#include "IILMgrBig/IILStr.h"


static bool sgShowLog = true;


bool
AosIILStr::addBlockSafe(
		char * &entries,
		const int size,
		const int num,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	
	if (sgShowLog)
	{
		OmnScreen << "\n==============================="
				  << "\n= To add block: " << num
				  << "\n= entry size: " << size
				  << "\n= current mNumDocs: " << mNumDocs
				  << "\n= current Level: " << mLevel << ":" << mNumSubiils
				  << "\n= current mIILID: " << mIILID
				  << "\n===============================" << endl;
	}
	
	aos_assert_rl(sanityCheckPriv(rdata), mLock, false);
	
	// This function adds all the entries 'entries' into this IIL. 
	bool rslt = false;
	bool returned = false;
	int subChanged = 0;
	char * cur_entries = entries;
	int cur_num = num;

	while (cur_num > 0)
	{
		if (needCallDistr1())
		{
			//rslt = addBlockDistrPriv(cur_entries, size, cur_num, iilmgrLocked, rdata);
			//AOSUNLOCK(mLock);
			//return rslt;
		}
			
		aos_assert_rl(sanityCheckPriv(rdata), mLock, false);
		if (isLeafIIL())
		{
			rslt = addBlockSinglePriv(cur_entries, size, cur_num, subChanged, true, iilmgrLocked, rdata);
		}
		else
		{
			rslt = addBlockRecPriv(cur_entries, size, cur_num, subChanged, true, iilmgrLocked, rdata);
		}
		aos_assert_rl(rslt, mLock, false);
		aos_assert_rl(sanityCheckPriv(rdata), mLock, false);
		
		// Save the changes when either there are too many processed or all contents
		// were processed.
		if (subChanged >= eMaxSubChangedToSave - 1 || cur_num <= 0)
		{
			if(!mIsDirty)
			{
				OmnAlarm << "dirty flag error" << enderr;
			}
			
			rslt = saveToFileSafe(true, iilmgrLocked, false, rdata);
			aos_assert_rl(rslt, mLock, false);
			
			returned = false;
			rslt = returnSubIILsPriv(iilmgrLocked, returned, rdata, false);
			aos_assert_rl(rslt, mLock, false);

			if (mLevel > 0)
			{
				for (u32 i=0; i<mNumSubiils; i++)
				{
					if (!mSubiils[i].isNull())
					{
						OmnAlarm << "not returned all, isDirty:" << mIsDirty << enderr;
					}
				}
			}
			
			subChanged = 0;
			if (getDistrType() == eDistrType1 && getDistrStatus() == eDistr_Local && mNumDocs > eNumDocsToDistr)
			{
				//rslt = distributeIILPriv(iilmgrLocked, rdata);
				//aos_assert_rl(rslt, mLock, false);
			}
		}
	}
	aos_assert_rl(sanityCheckPriv(rdata), mLock, false);

	if(sgShowLog)
	{
		OmnScreen << "Finished merge: " << mLevel << ":" << mNumSubiils << ":" << mNumDocs << endl;
	}
	
	AOSUNLOCK(mLock);
	return true;
}


bool
AosIILStr::addBlockRecSafe(
		char * &entries,
		const int size,
		int &num,
		int &subChanged,
		const bool &isTail,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	AOSLOCK(mLock);
	if(isLeafIIL())
	{
		rslt = addBlockSinglePriv(entries, size, num, subChanged, isTail, iilmgrLocked, rdata);
	}
	else
	{
		rslt = addBlockRecPriv(entries, size, num, subChanged, isTail, iilmgrLocked, rdata);
	}
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILStr::addBlockRecPriv(
		char * &entries,
		const int size,
		int &num,
		int &subChanged,
		const bool &isTail,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(num > 0, false);
	
	u64 value = *(u64 *)&entries[size - sizeof(u64)];
	AosIILStrPtr subiil = getSubiil3Priv(entries, value, iilmgrLocked, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(sanityCheckPriv(rdata), false);
	
	bool it = isTail && (subiil->getIILIdx() == ((int)mNumSubiils-1));
	int num_before = num;
	bool rslt = subiil->addBlockRecSafe(entries, size, num, subChanged, it, iilmgrLocked, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(num_before > num, false);
	aos_assert_r(sanityCheckPriv(rdata), false);
	return rslt;
}


bool
AosIILStr::addBlockSinglePriv(
		char * &entries,
		const int size,
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
		num_handle = binarySearch(entries, size, num, mValues[mNumDocs-1], mDocids[mNumDocs-1]) + 1;
		aos_assert_r(num_handle > 0, false);
	}

	int max_handle = (eMaxSubChangedToSave - subChanged) * mAddBlockFillSize - mNumDocs;
	if (num_handle > max_handle)
	{
		num_handle = max_handle;
	}

	int total = num_handle + mNumDocs;	
	int subToAdd = (total - 1) / mAddBlockFillSize;
	if (total < (int)mAddBlockMaxSize)
	{
		subToAdd = 0;
	}
//  int subiilsize = total / (subToAdd + 1);
//  // mod by shawn     int subiilsize = total / (subToAdd + 1);
    int subiilsize = (total-1) / (subToAdd + 1) +1;         
// end of mod
	int iilsize = total - subiilsize * subToAdd;

	char** orig_values = mValues;
	u64* orig_docids = mDocids;
	int orig_num = mNumDocs;
	int orig_idx = 0;
	mValues = 0;
	mDocids = 0;
	bool rslt = addBlockRebuild(entries, size, num, num_handle, orig_values, orig_docids, orig_num, orig_idx, iilsize);
	aos_assert_r(rslt, false);
	// 20120911 shawn
	setDirty(true);

	if (subToAdd > 0)
	{
		// create new subiils
		vector<AosIILStrPtr> subiil_list;
		AosIILStrPtr newsub;
		for (int i=0; i<subToAdd; i++)
		{
			newsub = splitContentUtil(iilmgrLocked, rdata);
			rslt = newsub->addBlockRebuild(entries, size, num, num_handle, orig_values, orig_docids, orig_num, orig_idx, subiilsize);
			aos_assert_r(rslt, false);
			subiil_list.push_back(newsub);
		}
		
		if (isSingleIIL())
		{
			// This IIL is a leaf IIL and has no parent IIL. 
			rslt = splitListSinglePriv(subiil_list, iilmgrLocked, rdata);
			aos_assert_r(rslt, false);
		}
		else
		{
			aos_assert_r(hasParentIIL(), false);
			aos_assert_r(mParentIIL, false);
			aos_assert_r(mParentIIL->getIILType() == eAosIILType_Str, false);
			AosIILStr *parent = (AosIILStr*)mParentIIL.getPtr();
			rslt = parent->subiilSplited(mIILIdx, subiil_list, iilmgrLocked, rdata);
			aos_assert_r(rslt, false);
		}
	}
	else
	{
		if(hasParentIIL())
		{
			aos_assert_r(mParentIIL, false);
			AosIILStr *parent = (AosIILStr*)mParentIIL.getPtr();
			rslt = parent->updateIndexData(mIILIdx, true, true, rdata);
			aos_assert_r(rslt, false);
		}
	}

	if(!orig_values || !orig_docids)
	{
		OmnMark;
	}

	if(orig_values)
	{
		for (int i=0; i<orig_num; i++)
		{
			if (!orig_values[i])
			{
				OmnAlarm << "orig_values is empty, i:" << i << enderr;
				continue;
			}
			
			OmnMemMgrSelf->release(orig_values[i], __FILE__, __LINE__);
			orig_values[i] = 0;
		}
		OmnDelete [] orig_values;
		orig_values = 0;
	}
	
	if(orig_docids)
	{
		OmnDelete [] orig_docids;
		orig_docids = 0;
	}
	
	subChanged++;
	subChanged += subToAdd;
	return true;
}


int 
AosIILStr::binarySearch(
		const char * entries,
		const int size,
		const int num,
		const char * value,
		const u64 &docid)
{
	// find the last index:
	// 		entries[index] <= value
	aos_assert_r(entries && num > 0, -1);
	int left = 0;
	int right = num-1;
	if (valueMatch(&entries[size * left], size, value, docid) > 0) return -1;
	if (valueMatch(&entries[size * right], size, value, docid) < 0) return right;
	int idx = 0;
	while (right - left > 1)
	{
		if (left + 1 == right)
		{
			if (valueMatch(&entries[size * right], size, value, docid) <= 0) return right;
			return left;
		}

		idx = (right + left) / 2;
		if(valueMatch(&entries[size * idx], size, value, docid) <= 0)
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

//felicia,2013/03/15
int 
AosIILStr::binarySearchInc(
		const char * entries,
		const int size,
		const int num,
		const char * value,
		const u64 &docid)
{
	// find the last index:
	// 		entries[index] <= value
	aos_assert_r(entries && num > 0, -1);
	int left = 0;
	int right = num-1;
	if (valueMatch(&entries[size * left], value) > 0) return -1;
	if (valueMatch(&entries[size * right], value) < 0) return right;
	//if (valueMatch(&entries[size * left], size, value, docid) > 0) return -1;
	//if (valueMatch(&entries[size * right], size, value, docid) < 0) return right;
	int idx = 0;
	while (right - left >= 1)
	{
		if (left + 1 == right)
		{
			//if (valueMatch(&entries[size * right], size, value, docid) <= 0) return right;
			if (valueMatch(&entries[size * right], value) <= 0) return right;
			return left;
		}

		idx = (right + left) / 2;
		//if(valueMatch(&entries[size * idx], size, value, docid) <= 0)
		if(valueMatch(&entries[size * idx], value) == 0)
		{
			left = idx;
			break;
		}
		else if(valueMatch(&entries[size * idx], value) < 0)
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
AosIILStr::valueMatch(
		const char * entry,
		const int size,
		const char * value,
		const u64 &docid)
{
	int rslt = strcmp(entry, value);
	if(rslt != 0) return rslt;

	u64 entry_value = *(u64 *)&entry[size - sizeof(u64)];
	if(entry_value > docid)
	{
		 return 1;
	}
	else if(entry_value == docid)
	{
		 return 0;
	}
	else
	{
		 return -1;
	}
	return 0;
}


bool
AosIILStr::addBlockRebuild(
		char * &entries,
		const int size,
		int &num,
		int &num_handle,
		char** &orig_values,
		u64* &orig_docids,
		int &orig_num,
		int &orig_idx,
		const int iilsize)
{	
	if(mValues || mDocids)
	{
		OmnAlarm << "error" << enderr;
	}
	mValues = OmnNew char*[iilsize + 10];
	memset(mValues, 0, sizeof(char*)*(iilsize + 10));
	mDocids = OmnNew u64[iilsize + 10];
	memset(mDocids, 0, sizeof(u64)*(iilsize + 10));
	mMemCap = iilsize + 10;
	
	int cur_pos = 0;
	int str_len = 0;
	char *mem;
	while(cur_pos < iilsize)
	{
		if ((num_handle <= 0) && (orig_idx >= orig_num))
		{
			// merged
			// OmnScreen << "Finished: " << num_handle << ":" << orig_idx << ":" << orig_num << endl;
			break;
		}
		
		if((num_handle > 0) &&
			((orig_idx >= orig_num)||
			(valueMatch(entries, size, orig_values[orig_idx], orig_docids[orig_idx])< 0)))
		{
			//copy entry
			str_len = strlen(entries);
			if(str_len > 0)
			{
				mem = OmnMemMgrSelf->allocate(str_len+1, __FILE__, __LINE__); 
				aos_assert_r(mem, false);

				strncpy(mem, entries, str_len);
				mem[str_len] = 0;
			
				mValues[cur_pos] = mem;
				mDocids[cur_pos] = *(u64 *)&entries[size - sizeof(u64)];
				cur_pos++;
			}
			num_handle--; 
			num--; 
			entries = &(entries[size]);
		}
		else
		{
			//copy value/docid
			
			//ken 2012/10/25
			//mValues[cur_pos] = orig_values[orig_idx];
			str_len = strlen(orig_values[orig_idx]);
			if(str_len <= 0)
			{
				OmnAlarm << "error" << enderr;
			}
			else
			{
				mem = OmnMemMgrSelf->allocate(str_len+1, __FILE__, __LINE__); 
				aos_assert_r(mem, false);

				strncpy(mem, orig_values[orig_idx], str_len);
				mem[str_len] = 0;
			
				mValues[cur_pos] = mem;
				mDocids[cur_pos] = orig_docids[orig_idx];
				cur_pos++;
			}
			orig_idx++;
		}
	}
	
	mNumDocs = cur_pos;
	return true;
}


bool
AosIILStr::incBlockSafe(
		char * &entries,
		const int size,
		const int num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);

	if (sgShowLog)
	{
		OmnScreen << "\n==============================="
				  << "\n= To inc block: " << num
				  << "\n= entry size: " << size
				  << "\n= current mNumDocs: " << mNumDocs
				  << "\n= current Level: " << mLevel << ":" << mNumSubiils
				  << "\n= current mIILID: " << mIILID
				  << "\n===============================" << endl;
	}

	aos_assert_rl(sanityCheckPriv(rdata), mLock, false);
	
	bool rslt = true;
	bool returned = false;
	int subChanged = 0;
	char * cur_entries = entries;
	int cur_num = num;

	while(cur_num > 0)
	{
		aos_assert_rl(sanityCheckPriv(rdata), mLock, false);
		if (isLeafIIL())
		{
			rslt = incBlockSinglePriv(cur_entries, size, cur_num, dftValue, incType, subChanged, true, iilmgrLocked, rdata);
		}
		else
		{
			rslt = incBlockRecPriv(cur_entries, size, cur_num, dftValue, incType, subChanged, true, iilmgrLocked, rdata);
		}
		aos_assert_rl(rslt, mLock, false);
		aos_assert_rl(sanityCheckPriv(rdata), mLock, false);
		
		// Save the changes when either there are too many processed or all contents
		// were processed.
		if (subChanged >= eMaxSubChangedToSave - 1 || cur_num <= 0)
		{
			if (!mIsDirty)
			{
				OmnAlarm << "dirty flag error" << enderr;
			}

			rslt = saveToFileSafe(true, iilmgrLocked, false, rdata);
			aos_assert_rl(rslt, mLock, false);
			
			returned = false;
			rslt = returnSubIILsPriv(iilmgrLocked, returned, rdata, false);
			aos_assert_rl(rslt, mLock, false);

			if(mLevel > 0)
			{
				for(u32 i=0; i<mNumSubiils; i++)
				{
					if(!mSubiils[i].isNull())
					{
						OmnAlarm << "not returned all, isDirty:" << mIsDirty << enderr;
					}
				}
			}
			subChanged = 0;
		}
	}	
			
	aos_assert_rl(sanityCheckPriv(rdata), mLock, false);
	
	if(sgShowLog)
	{
		OmnScreen << "Finished merge: " << mLevel << ":" << mNumSubiils << ":" << mNumDocs << endl;
	}
	
	AOSUNLOCK(mLock);
	return true;
}


bool
AosIILStr::incBlockRecSafe(
		char * &entries,
		const int size,
		int &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		int &subChanged,
		const bool &isTail,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	AOSLOCK(mLock);
	if (isLeafIIL())
	{
		rslt = incBlockSinglePriv(entries, size, num, dftValue, incType, subChanged, isTail, iilmgrLocked, rdata);
	}
	else
	{
		rslt = incBlockRecPriv(entries, size, num, dftValue, incType, subChanged, isTail, iilmgrLocked, rdata);
	}
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILStr::incBlockRecPriv(
		char * &entries,
		const int size,
		int &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		int &subChanged,
		const bool &isTail,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(num > 0, false);

	//felicia, 2013/03/18
	//u64 value = *(u64 *)&entries[size - sizeof(u64)];
	u64 value = 0; 
	AosIILStrPtr subiil = getSubiil3Priv(entries, value, iilmgrLocked, rdata); 
	aos_assert_r(subiil, false);
	aos_assert_r(sanityCheckPriv(rdata), false);

	bool it = isTail &&(subiil->getIILIdx() == ((int)mNumSubiils-1));
	int num_before = num;
	bool rslt = subiil->incBlockRecSafe(entries, size, num, dftValue, incType, subChanged, it, iilmgrLocked, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(num_before > num, false);
	aos_assert_r(sanityCheckPriv(rdata), false);
	return rslt;
}


bool
AosIILStr::incBlockSinglePriv(
		char * &entries,
		const int size,
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
		//num_handle = binarySearch(entries, size, num, mValues[mNumDocs-1], mDocids[mNumDocs-1]) + 1;
		num_handle = binarySearchInc(entries, size, num, mValues[mNumDocs-1], mDocids[mNumDocs-1]) + 1;
		aos_assert_r(num_handle > 0, false);
	}

	// now we do know how many records we can handle, 
	// if it is too little, put it in one IIL,else we will fill the iils one by one.
	// finally, if the last iil is too small, it wiil share the records with the one before it.
	aos_assert_r(subChanged < eMaxSubChangedToSave, false);
	bool is_last_iil = (subChanged == eMaxSubChangedToSave - 1);
	int subToAdd = 0;

	char** orig_values = mValues;
	u64* orig_docids = mDocids;
	int orig_num = mNumDocs;
	int orig_idx = 0;
	mValues = 0;
	mDocids = 0;
	bool rslt = incBlockRebuild(
		entries, size, num, dftValue, incType, num_handle, orig_values,
		orig_docids, orig_num, orig_idx, mAddBlockMaxSize, is_last_iil);
	aos_assert_r(rslt, false);

	// 20120911 shawn
	setDirty(true);

	subChanged++;
	vector<AosIILStrPtr> subiil_list;
	AosIILStrPtr newsub;
	while ((num_handle > 0 || orig_idx < orig_num) && subChanged < eMaxSubChangedToSave)
	{
		is_last_iil = (subChanged == eMaxSubChangedToSave - 1);
		newsub = splitContentUtil(iilmgrLocked, rdata);
		rslt = newsub->incBlockRebuild(
			entries, size, num, dftValue, incType, num_handle, orig_values,
			orig_docids, orig_num, orig_idx, mAddBlockFillSize, is_last_iil);
		aos_assert_r(rslt, false);
		subiil_list.push_back(newsub);
		subChanged++;
		subToAdd++;
	}
	
	// if the last iil is too small, it wiil share the records with the one before it.
	if(subToAdd > 0)
	{
		AosIILStrPtr lastiil = subiil_list[subToAdd-1];
		int last_num = lastiil->getNumDocs();
		if(last_num < (int)mAddBlockMinSize)
		{
			AosIILStrPtr secondiil;
			if(subToAdd == 1)
			{
				AosIILStrPtr thisPtr(this, false);
				secondiil = thisPtr;
			}
			else
			{
				secondiil = subiil_list[subToAdd - 2];
			}
			int second_num = secondiil->getNumDocs();
			aos_assert_r(second_num > last_num, false);
			int doc_to_move = (second_num - last_num) / 2;
			rslt = moveDocsForward(secondiil, lastiil, doc_to_move);
			aos_assert_r(rslt, false);
		}
	}
	
	if (subToAdd > 0)
	{
		if(isSingleIIL())
		{
			// This IIL is a leaf IIL and has no parent IIL. 
			rslt = splitListSinglePriv(subiil_list, iilmgrLocked, rdata);
			aos_assert_r(rslt, false);
		}
		else
		{
			aos_assert_r(hasParentIIL(), false);
			aos_assert_r(mParentIIL, false);
			aos_assert_r(mParentIIL->getIILType() == eAosIILType_Str, false);
			AosIILStr *parent = (AosIILStr*)mParentIIL.getPtr();
			rslt = parent->subiilSplited(mIILIdx, subiil_list, iilmgrLocked, rdata);		
			aos_assert_r(rslt, false);
		}
	}
	else
	{
		if(hasParentIIL())
		{
			aos_assert_r(mParentIIL, false);
			AosIILStr *parent = (AosIILStr*)mParentIIL.getPtr();
			rslt = parent->updateIndexData(mIILIdx, true, true, rdata);
			aos_assert_r(rslt, false);
		}
	}	
	
	if(!orig_values || !orig_docids)
	{
		OmnMark;
	}

	if(orig_values)
	{
		for (int i=0; i<orig_num; i++)
		{
			if (!orig_values[i])
			{
				OmnAlarm << "orig_values is empty, i:" << i << enderr;
				continue;
			}
			
			OmnMemMgrSelf->release(orig_values[i], __FILE__, __LINE__);
			orig_values[i] = 0;
		}
		OmnDelete []orig_values;
		orig_values = 0;
	}
	
	if(orig_docids)
	{
		OmnDelete []orig_docids;
		orig_docids = 0;
	}
		
	return true;
}


bool
AosIILStr::incBlockRebuild(
		char * &entries,
		const int size,
		int &num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		int &num_handle,
		char** &orig_values,
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
	
	mValues = OmnNew char*[maxiilsize + 10];
	memset(mValues, 0, sizeof(char*)*(maxiilsize + 10));
	mDocids = OmnNew u64[maxiilsize + 10];
	memset(mDocids, 0, sizeof(u64)*(maxiilsize + 10));
	mMemCap = maxiilsize + 10;

	int cur_pos = 0;
	int str_len = 0;
	char *mem;
	
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
			cmp_rslt = valueMatch(entries, orig_values[orig_idx]);
			if(cmp_rslt == 0) // case 5
			{
				if(incType == AosIILUtil::eIILIncUpdateMin)
				{
					if(orig_docids[orig_idx] > *(u64 *)&entries[size - sizeof(u64)])
					{
						orig_docids[orig_idx] = *(u64 *)&entries[size - sizeof(u64)];
					}
				}
				else if(incType == AosIILUtil::eIILIncUpdateMax)
				{
					if(orig_docids[orig_idx] < *(u64 *)&entries[size - sizeof(u64)])
					{
						orig_docids[orig_idx] = *(u64 *)&entries[size - sizeof(u64)];
					}
				}
				else if(updateFlag)
				{

					if(setFlag)
					{
						orig_docids[orig_idx] = *(u64 *)&entries[size - sizeof(u64)];
					}
					else
					{
						orig_docids[orig_idx] += *(u64 *)&entries[size - sizeof(u64)];
OmnScreen << " $$$$$$$$ iilid : " << mIILID << "  idx : " << orig_idx << "  entries : " << entries << " docids : " << orig_docids[orig_idx] << endl;
					}
				}
				entries = &(entries[size]);
				num_handle--; 
				num--; 
				continue;
			}
		}

		if((num_handle <= 0) 	|| // case 3
		   (isLastSub && ((maxiilsize-cur_pos) <= (orig_num-orig_idx))) || // case 6
 		   ((orig_idx < orig_num) && (cmp_rslt > 0)))   // case 7
		{			
			//ken 2012/10/25
			//mValues[cur_pos] = orig_values[orig_idx];
			str_len = strlen(orig_values[orig_idx]);
			if(str_len <= 0)
			{
				OmnAlarm << "error" << enderr;
			}
			else
			{	
				mem = OmnMemMgrSelf->allocate(str_len+1, __FILE__, __LINE__); 
				aos_assert_r(mem, false);

				//ken 2012/11/19
				//strncpy(mem, entries, str_len);
				strncpy(mem, orig_values[orig_idx], str_len);
				mem[str_len] = 0;
			
				mValues[cur_pos] = mem;
				mDocids[cur_pos] = orig_docids[orig_idx];
				cur_pos++;
			}
			orig_idx++;
			continue;
		}
		
		// case 4, case 8
		if(addFlag)
		{
			// handle the case if the next entry value is same as this one
			if(num > 1 && (valueMatchStr(entries, &entries[size]) == 0))
			{
				if(incType == AosIILUtil::eIILIncUpdateMin)
				{
					if((*(u64 *)&entries[size+size-sizeof(u64)]) > (*(u64 *)&entries[size-sizeof(u64)]) )
					{
						*(u64 *)&entries[size+size-sizeof(u64)] = *(u64 *)&entries[size-sizeof(u64)];
					}
				}
				else if(incType == AosIILUtil::eIILIncUpdateMax)
				{
					if((*(u64 *)&entries[size+size-sizeof(u64)]) < (*(u64 *)&entries[size-sizeof(u64)]))
					{
						*(u64 *)&entries[size+size-sizeof(u64)] = *(u64 *)&entries[size-sizeof(u64)];
					}
				}
				else if(updateFlag)
				{
					if(!setFlag)
					{
						*(u64 *)&entries[size+size-sizeof(u64)] += *(u64 *)&entries[size-sizeof(u64)];
					}
				}
				else
				{
					*(u64 *)&entries[size+size-sizeof(u64)] = *(u64 *)&entries[size-sizeof(u64)];
				}

				entries = &(entries[size]);
				num_handle--; 
				num--; 
				continue;
			}

			str_len = strlen(entries);
			if(str_len > 0)
			{
				mem = OmnMemMgrSelf->allocate(str_len+1, __FILE__, __LINE__);
				aos_assert_r(mem, false);

				strncpy(mem, entries, str_len);
				mem[str_len] = 0;
					
				mValues[cur_pos] = mem;
				mDocids[cur_pos] = *(u64 *)&entries[size-sizeof(u64)] + dftValue;
				cur_pos++;
			}
		}
		entries = &(entries[size]);
		num_handle--; 
		num--; 
	}
	
	mNumDocs = cur_pos;
	return true;
}


int
AosIILStr::valueMatch(
		const char * entry,
		const char * value)
{
	return strcmp(entry, value);
}


int
AosIILStr::valueMatchStr(
		const char * entry1,
		const char * entry2)
{
	return strcmp(entry1, entry2);
}


bool
AosIILStr::moveDocsForward(
		const AosIILStrPtr &from_iil,
		const AosIILStrPtr &to_iil,
		const int &num_to_move)
{
	int num1 = from_iil->getNumDocs();
	int num2 = to_iil->getNumDocs();

	aos_assert_r(num1 > num_to_move, false);
	aos_assert_r(num2 > 0, false);
	aos_assert_r(num_to_move > 0, false);

	to_iil->mNumDocs += num_to_move;
	aos_assert_r(to_iil->expandMemoryPriv(), false);

	char** values1 = from_iil->mValues;
	u64*   docids1 = from_iil->mDocids;
	char** values2 = to_iil->mValues;
	u64*   docids2 = to_iil->mDocids;
	
	memmove(&values2[num_to_move], values2, sizeof(char*) * num2);
	memmove(&docids2[num_to_move], docids2, sizeof(u64) * num2);

	memcpy(values2, &values1[num1-num_to_move], sizeof(char*) * num_to_move);
	memcpy(docids2, &docids1[num1-num_to_move], sizeof(u64) * num_to_move);
	
	memset(&values1[num1-num_to_move], 0, sizeof(char*) * num_to_move);
	memset(&docids1[num1-num_to_move], 0, sizeof(u64) * num_to_move);
	
	from_iil->mNumDocs -= num_to_move;
	return true;
}


bool
AosIILStr::delBlockSafe(
		char * &entries,
		const int size,
		const int num,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	
	if(sgShowLog)
	{
		OmnScreen << "\n==============================="
				  << "\n= To del block: " << num
				  << "\n= entry size: " << size
				  << "\n= current mNumDocs: " << mNumDocs
				  << "\n= current Level: " << mLevel << ":" << mNumSubiils
				  << "\n= current mIILID: " << mIILID
				  << "\n==============================="<< endl;
	}
	
	bool rslt = false;
	char * cur_entries = entries;
	int cur_num = num;
	
	if (isLeafIIL())
	{
		rslt = delBlockSinglePriv(cur_entries, size, cur_num, iilmgrLocked, rdata);
	}
	else
	{
		rslt = delBlockRecPriv(cur_entries, size, cur_num, iilmgrLocked, rdata);
	}
	aos_assert_rl(rslt, mLock, false);

	if (isRootIIL() && mNumDocs == 0)
	{
		rslt = clearIILPriv(rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	
	rslt = saveToFileSafe(true, iilmgrLocked, false, rdata);
	aos_assert_rl(rslt, mLock, false);
			
	bool returned = false;
	rslt = returnSubIILsPriv(iilmgrLocked, returned, rdata, false);
	aos_assert_rl(rslt, mLock, false);

	if(sgShowLog)
	{
		OmnScreen << "Finished merge: " << mLevel << ":" << mNumSubiils << ":" << mNumDocs << endl;
	}
	
	AOSUNLOCK(mLock);
	return true;
}


bool
AosIILStr::delBlockRecSafe(
		char * &entries,
		const int size,
		int &num,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	AOSLOCK(mLock);
	if(isLeafIIL())
	{
		rslt = delBlockSinglePriv(entries, size, num, iilmgrLocked, rdata);
	}
	else
	{
		rslt = delBlockRecPriv(entries, size, num, iilmgrLocked, rdata);
	}
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILStr::delBlockRecPriv(
		char * &entries,
		const int size,
		int &num,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(num > 0, false);
	
	u64 value = *(u64 *)&entries[size - sizeof(u64)];
	OmnString vv(entries, strlen(entries));
	int idx = getSubiil3Priv(vv, value, iilmgrLocked, rdata);
	aos_assert_r((idx >= 0) && (idx < (int)mNumSubiils), 0);

	int last_num = num;
	AosIILStrPtr subiil;
	bool rslt;
	for(int i=idx; i<(int)mNumSubiils; i++)
	{
		subiil = getSubiilByIndexPriv(idx, iilmgrLocked, rdata);
		aos_assert_r(subiil, false);
	
		rslt = subiil->delBlockRecSafe(entries, size, num, iilmgrLocked, rdata);
		aos_assert_r(rslt, false);
	}
	aos_assert_r(last_num >= num, false);
	if(last_num == num) return true;
	
	rslt = resetMaxMin(idx, iilmgrLocked, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosIILStr::delBlockSinglePriv(
		char * &entries,
		const int size,
		int &num,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs > 0, false);
	
	setDirty(true);
	
	char** orig_values = mValues;
	u64* orig_docids = mDocids;
	int orig_num = mNumDocs;
	int orig_idx = 0;
	
	mValues = 0;
	mDocids = 0;
	int iilsize = mNumDocs;

	mValues = OmnNew char*[iilsize + 10];
	memset(mValues, 0, sizeof(char*)*(iilsize + 10));
	mDocids = OmnNew u64[iilsize + 10];
	memset(mDocids, 0, sizeof(u64)*(iilsize + 10));
	mMemCap = iilsize + 10;

	int cur_pos = 0;
	int str_len = 0;
	char *mem;
	while(cur_pos < iilsize)
	{
		if((num <= 0) && (orig_idx >= orig_num))
		{
			break;
		}
		
		if((num > 0) && ((orig_idx >= orig_num) ||
			(valueMatch(entries, size, orig_values[orig_idx], orig_docids[orig_idx]) <= 0)))
		{
			if((orig_idx < orig_num) && valueMatch(entries, size, orig_values[orig_idx], orig_docids[orig_idx]) == 0)
			{
				orig_idx++;
			}
			num--; 
			entries = &(entries[size]);
		}
		else
		{
			//copy value/docid
			str_len = strlen(orig_values[orig_idx]);
			if(str_len <= 0)
			{
				OmnAlarm << "error" << enderr;
			}
			else
			{
				mem = OmnMemMgrSelf->allocate(str_len+1, __FILE__, __LINE__); 
				aos_assert_r(mem, false);

				strncpy(mem, orig_values[orig_idx], str_len);
				mem[str_len] = 0;
			
				mValues[cur_pos] = mem;
				mDocids[cur_pos] = orig_docids[orig_idx];
				cur_pos++;
			}
			orig_idx++;
		}
	}
	mNumDocs = cur_pos;
	
	if(orig_values)
	{
		for (int i=0; i<orig_num; i++)
		{
			if (!orig_values[i])
			{
				OmnAlarm << "orig_values is empty, i:" << i << enderr;
				continue;
			}
			
			OmnMemMgrSelf->release(orig_values[i], __FILE__, __LINE__);
			orig_values[i] = 0;
		}
		OmnDelete [] orig_values;
		orig_values = 0;
	}
	
	if(orig_docids)
	{
		OmnDelete [] orig_docids;
		orig_docids = 0;
	}
	return true;
}


bool
AosIILStr::decBlockSafe(
		char * &entries,
		const int size,
		const int num,
		const bool delete_flag,
		const AosIILUtil::AosIILIncType incType,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	// This function inc all the entries 'entries' into this IIL. 
	AOSLOCK(mLock);

	if (sgShowLog)
	{
		OmnScreen << "\n==============================="
				  << "\n= To dec block: " << num
				  << "\n= entry size: " << size
				  << "\n= current mNumDocs: " << mNumDocs
				  << "\n= current Level: " << mLevel << ":" << mNumSubiils
				  << "\n= current mIILID: " << mIILID
				  << "\n==============================="<< endl;
	}
	
	bool rslt = false;
	char * cur_entries = entries;
	int cur_num = num;
	
	if (isLeafIIL())
	{
		rslt = decBlockSinglePriv(cur_entries, size, cur_num, delete_flag, incType, iilmgrLocked, rdata);
	}
	else
	{
		rslt = decBlockRecPriv(cur_entries, size, cur_num, delete_flag, incType, iilmgrLocked, rdata);
	}
	aos_assert_rl(rslt, mLock, false);

	if (isRootIIL() && mNumDocs == 0)
	{
		rslt = clearIILPriv(rdata);
		aos_assert_rl(rslt, mLock, false);
	}

	rslt = saveToFileSafe(true, iilmgrLocked, false, rdata);
	aos_assert_rl(rslt, mLock, false);
			
	bool returned = false;
	rslt = returnSubIILsPriv(iilmgrLocked, returned, rdata, false);
	aos_assert_rl(rslt, mLock, false);

	if(sgShowLog)
	{
		OmnScreen << "Finished merge: " << mLevel << ":" << mNumSubiils << ":" << mNumDocs << endl;
	}
	
	AOSUNLOCK(mLock);
	return true;
}


bool
AosIILStr::decBlockRecSafe(
		char * &entries,
		const int size,
		int &num,
		const bool delete_flag,
		const AosIILUtil::AosIILIncType incType,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	AOSLOCK(mLock);
	if(isLeafIIL())
	{
		rslt = decBlockSinglePriv(entries, size, num, delete_flag, incType, iilmgrLocked, rdata);
	}
	else
	{
		rslt = decBlockRecPriv(entries, size, num, delete_flag, incType, iilmgrLocked, rdata);
	}
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILStr::decBlockRecPriv(
		char * &entries,
		const int size,
		int &num,
		const bool delete_flag,
		const AosIILUtil::AosIILIncType incType,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(num > 0, false);
	
	u64 value = *(u64 *)&entries[size - sizeof(u64)];
	OmnString vv(entries, strlen(entries));
	int idx = getSubiil3Priv(vv, value, iilmgrLocked, rdata);
	aos_assert_r((idx >= 0) && (idx < (int)mNumSubiils), 0);

	int last_num = num;
	AosIILStrPtr subiil;
	bool rslt;
	for(int i=idx; i<(int)mNumSubiils; i++)
	{
		subiil = getSubiilByIndexPriv(idx, iilmgrLocked, rdata);
		aos_assert_r(subiil, false);
	
		rslt = subiil->decBlockRecSafe(entries, size, num, delete_flag, incType, iilmgrLocked, rdata);
		aos_assert_r(rslt, false);
	}

	aos_assert_r(last_num >= num, false);
	if(last_num == num) return true;
	
	rslt = resetMaxMin(idx, iilmgrLocked, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosIILStr::decBlockSinglePriv(
		char * &entries,
		const int size,
		int &num,
		const bool delete_flag,
		const AosIILUtil::AosIILIncType incType,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	if(incType != AosIILUtil::eIILIncNormal 
		|| incType != AosIILUtil::eIILIncNoUpdate)
	{
		return true;
	}
	aos_assert_r(mNumDocs > 0, false);
	
	setDirty(true);
	
	char** orig_values = mValues;
	u64* orig_docids = mDocids;
	int orig_num = mNumDocs;
	int orig_idx = 0;
	
	mValues = 0;
	mDocids = 0;
	int iilsize = mNumDocs;

	mValues = OmnNew char*[iilsize + 10];
	memset(mValues, 0, sizeof(char*)*(iilsize + 10));
	mDocids = OmnNew u64[iilsize + 10];
	memset(mDocids, 0, sizeof(u64)*(iilsize + 10));
	mMemCap = iilsize + 10;

	int cur_pos = 0;
	int str_len = 0;
	char *mem;
	while(cur_pos < iilsize)
	{
		if((num <= 0) && (orig_idx >= orig_num))
		{
			break;
		}
		
		if((num > 0) && ((orig_idx >= orig_num) ||
			(valueMatch(entries, orig_values[orig_idx]) <= 0)))
		{
			if((orig_idx < orig_num) && valueMatch(entries, orig_values[orig_idx]) == 0)
			{
				u64 docid = *(u64 *)&entries[size - sizeof(u64)]; 
				if(incType == AosIILUtil::eIILIncNormal)
				{
					orig_docids[orig_idx] -= docid;
					if(orig_docids[orig_idx] == 0 && delete_flag)
					{
						orig_idx++;
					}
				}
				else if(incType == AosIILUtil::eIILIncNoUpdate)
				{
					if(orig_docids[orig_idx] == docid)
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
			str_len = strlen(orig_values[orig_idx]);
			if(str_len <= 0)
			{
				OmnAlarm << "error" << enderr;
			}
			else
			{
				mem = OmnMemMgrSelf->allocate(str_len+1, __FILE__, __LINE__); 
				aos_assert_r(mem, false);

				strncpy(mem, orig_values[orig_idx], str_len);
				mem[str_len] = 0;
			
				mValues[cur_pos] = mem;
				mDocids[cur_pos] = orig_docids[orig_idx];
				cur_pos++;
			}
			orig_idx++;
		}
	}
	mNumDocs = cur_pos;
	
	if(orig_values)
	{
		for (int i=0; i<orig_num; i++)
		{
			if (!orig_values[i])
			{
				OmnAlarm << "orig_values is empty, i:" << i << enderr;
				continue;
			}
			
			OmnMemMgrSelf->release(orig_values[i], __FILE__, __LINE__);
			orig_values[i] = 0;
		}
		OmnDelete [] orig_values;
		orig_values = 0;
	}
	
	if(orig_docids)
	{
		OmnDelete [] orig_docids;
		orig_docids = 0;
	}
	return true;
}

