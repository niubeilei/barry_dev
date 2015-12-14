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
// 04/28/2012 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILMgrBig/IILStr.h"

#include "IILUtil/IILFuncType.h"


bool
AosIILStr::distributeIILPriv(const bool iilmgrlocked, const AosRundataPtr &rdata)
{
	//1. split some subiil(level 0) to remote side
	int 			num_distrs = 0;
	vector<u64> 	iilids;
	vector<int> 	entries;
	int				cur_entries = 0;
	// int 			strlen = 80;
	// int				num_sync = -1;
	// u64				asm_id = 0;
	
	// create IILs on each machine
//wait chen finish.
//aos_network_mgr->createIILsOnDistr(iilids,num_distrs,rdata);// notes: set all the subiils type as distr_client
		
	// AosIILFuncType::E type = AosIILFuncType::eStrBatchAddStart;
		
	for(int i = 0;i < num_distrs-1;i++)
	{
	/*	AosIILClient::getSelf()->sendAddAttrBatchStart(
		                type,
                		iilids[i],// const OmnString &iilname,change to const u64 iilid
                		false,// const bool isPersis,
                		false,//const bool value_unique,
                		false,//const bool docid_unique,
                		AosOrder::eAlphabetic,
						num_sync,
						strlen,
						asm_id,
						rdata);//const AosRundataPtr &rdata);
				*/
	}

	// now we distribute all the entries into these iils.
	// we assuming that the current iil is a level 1 IIL 
	aos_assert_r(mLevel == 1,false);
	aos_assert_r((int)mNumSubiils > num_distrs, false);
	int num_each = mNumSubiils/ (num_distrs + 1);
	int sub_in_first = mNumSubiils - num_distrs * num_each;
	int cur_sub = sub_in_first;

	OmnString cur_value;
	vector<OmnString> 	newMinValues;
	vector<u64>			newMinDocids;
	vector<OmnString> 	newMaxValues;
	vector<u64>			newMaxDocids;
	cur_value = mMinVals[0];
	newMinValues.push_back(cur_value);
	newMinDocids.push_back(mMinDocids[0]);
	cur_value = mMaxVals[cur_sub-1];
	newMinValues.push_back(cur_value);
	newMinDocids.push_back(mMaxDocids[cur_sub-1]);
	
	for(int i = 0;i < sub_in_first;i++)
	{
		cur_entries += mNumEntries[i];
	}
	entries.push_back(cur_entries);

	for(int i = 0;i < num_distrs-1;i++)
	{
		cur_value = mMinVals[cur_sub];
		newMinValues.push_back(cur_value);
		newMinDocids.push_back(mMinDocids[cur_sub]);
		cur_entries = 0;
		for(int j = 0;j<num_each;j++)
		{
			// send iil [cur_sub] to iilids[i]
			getSubiilByIndexPriv(cur_sub,
								 iilmgrlocked,
								 rdata)->sendContentToDistr(iilids[i],rdata);
			cur_sub ++;
			cur_entries += mNumEntries[cur_sub];
		}
		cur_value = mMaxVals[cur_sub-1];
		newMinValues.push_back(cur_value);
		newMinDocids.push_back(mMaxDocids[cur_sub-1]);
		entries.push_back(cur_entries);
	}	

	for(int i = 0;i < num_distrs;i++)
	{
	/*	rslt = AosIILClient::getSelf()->sendBatchFinish(
		                type,
                		iilids[i],
						0, 
						"",
						asm_id,
                		rdata);
		aos_assert_r(rslt,false);	
		*/
	}
	
	// move sub_in_first subiils to new subiil
	AosIILStrPtr subiil = splitContentUtil(iilmgrlocked, rdata);
	aos_assert_r(subiil, false);
	
	subiil->initSubiilParent(
		mMinDocids, 
		mMinVals,	
		mMaxDocids,
		mMaxVals,
		mIILIds, 
		mNumEntries, 
		mSubiils, 
		sub_in_first, 
		iilmgrlocked);
	subiil->setDistrStatus(eDistr1_Branch);

	memset(mMinVals, 0, sizeof(char*)*sub_in_first);
	memset(mMinDocids, 0, sizeof(u64)*sub_in_first);
	memset(mMaxVals, 0, sizeof(char*)*sub_in_first);
	memset(mMaxDocids, 0, sizeof(u64)*sub_in_first);
	for(int i = sub_in_first-1;i < (int)mNumSubiils;i++)
	{
		mMinDocids[i] = 0;
		mMaxDocids[i] = 0;
		if(mMinVals[i])
		{
			OmnMemMgrSelf->release(mMinVals[i], __FILE__, __LINE__);
		}			
		if(mMaxVals[i])
		{
			OmnMemMgrSelf->release(mMaxVals[i], __FILE__, __LINE__);
		}			
	}

	mNumSubiils = num_distrs;
	int length = 0;
	char* ptr = 0;
	for(int i = 0;i < num_distrs;i++)
	{
		mMinDocids[i] = newMinDocids[i];
		mMaxDocids[i] = newMaxDocids[i];

		length = newMinValues[i].length();
		length = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
		ptr = OmnMemMgrSelf->allocate(length+1, __FILE__, __LINE__);
		aos_assert_r(ptr, false);
		mMinVals[i] = ptr;
		strcpy(mMinVals[i],newMinValues[i].data());

		length = newMaxValues[i].length();
		length = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
		ptr = OmnMemMgrSelf->allocate(length+1, __FILE__, __LINE__);
		aos_assert_r(ptr, false);
		mMaxVals[i] = ptr;
		strcpy(mMaxVals[i],newMaxValues[i].data());

	}
	
	mDistrStatus = eDistr1_Root;
	mNumEntries[0] = entries[0];
	mIILIds[0] = subiil->getIILID();
	for(int i = 1;i < num_distrs;i++)
	{
		mIILIds[i] = iilids[i-1];
		mNumEntries[i] = entries[i];
	}
	
	return true;
}

bool 	
AosIILStr::addDocDistr(
			const OmnString &value, 
			const u64 &docid, 
			const bool value_unique, 
			const bool docid_unique, 
			const AosRundataPtr &rdata)
{
	
	return true;
}


bool	
AosIILStr::removeDocDistr(
			const OmnString &value,
			const u64 &docid,
			const AosRundataPtr &rdata)
{
	return true;
}



bool 	
AosIILStr::modifyDocDistr(
			const OmnString &oldvalue, 
			const OmnString &newvalue, 
			const u64 &docid, 
			const bool value_unique,
			const bool docid_unique,
			const AosRundataPtr &rdata)
{
	return true;
}

bool			
AosIILStr::addBlockDistrPriv(
				char * &entries,
				const int size,
				const int num,
				const bool iilmgrLocked,
				const AosRundataPtr &rdata)
{
	
	return true;
}

bool			
AosIILStr::getDistrInfo(int &distr_num,
						vector<u64>		  &iilids, 
						vector<OmnString> &values,
						vector<u64>		  &docids)
{
	return true;
}


bool
AosIILStr::sendContentToDistr(
		const u64 &iilid,
		const AosRundataPtr &rdata)
{
	//AosOrder::E type = AosOrder::eAlphabetic;
	//AosCompareFun1Ptr cmp = OmnNew AosCompareFun1();
	//AosStrU64ArrayPtr   array = OmnNew AosStrU64Array(cmp, false);
	
	//for(int i = 0;i < (int)mNumDocs;i++)
	//{
	//	array->addValue(mValues[i],mDocids[i],rdata);
	//}
	//AosBuffPtr buff = array->getBuff();
	///AosIILFuncType::E func_type = AosIILFuncType::eStrBatchAdd;
	//aos_assert_r(buff, false);
	//AosIILClient::getSelf()->sendBatch(func_type, iilid, buff, rdata);	
	return true;
}


void
AosIILStr::updateDistrInfo()
{
	aos_assert(getDistrType() == eDistrType1 &&
   	   getDistrStatus() == eDistr1_Root);
}


bool
AosIILStr::needCallDistr1()
{
	if(getDistrType() == eDistrType1 &&
	getDistrStatus() == eDistr1_Root)
	{
		return true;
	}
	return false;
}
