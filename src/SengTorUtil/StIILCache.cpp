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
// 2014/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorUtil/StIILCache.h"

#include "Random/RandomUtil.h"
#include "SengTorUtil/StIIL.h"
#include "XmlUtil/XmlTag.h"


AosStIILCache::AosStIILCache()
:
mLock(OmnNew OmnMutex())
{
	mMinIILNameLen = eDftMinIILNameLen;
	mMaxIILNameLen = eDftMaxIILNameLen;
}


AosStIILCache::~AosStIILCache()
{
	mIILNameMap.clear();
	mIILIDMap.clear();
	mIILArray.clear();
}


AosStIILPtr
AosStIILCache::pickIIL(
		shared_ptr<AosJimoAPIClient> &thriftClient,
		JmoCallData &call_data)
{
	// This function creates an entry for the IIL 'iilid'. 
	bool rslt;
	mLock->lock();
	if (mIILArray.size() == 0)
	{
		rslt = createIIL(thriftClient, call_data);
		aos_assert_r(rslt, 0);
	}

	aos_assert_rl(mIILArray.size() > 0, mLock, 0);
	u32 idx = rand() % mIILArray.size();
	AosStIILPtr iil = mIILArray[idx];
	mLock->unlock();
	return iil;
}


bool
AosStIILCache::createIIL(
		shared_ptr<AosJimoAPIClient> &thriftClient,
		JmoCallData &call_data)
{
	OmnString iilname;
	u64 iilid;
	JmoIILType::type iil_type = (rand()%2)?JmoIILType::eKeyDocid:JmoIILType::eIDDocid;
	JmoRundata rdata;
	//if (rand()%2)
	//{
		while (1)
		{
			iilname = OmnRandom::nextLetterDigitStr(mMinIILNameLen, mMaxIILNameLen, false);
			mapitr_t itr_iilname = mIILNameMap.find(iilname);
			if (itr_iilname != mIILNameMap.end())
			{
				thriftClient->createIILByIILName(rdata, call_data, iilname, iil_type);
				aos_assert_r(rdata.rcode != JmoReturnCode::SUCCESS, false);
				continue;
			}
			thriftClient->createIILByIILName(rdata, call_data, iilname, iil_type);
			if (rdata.rcode != JmoReturnCode::SUCCESS)
			{
				continue;
			}
			aos_assert_r(rdata.rcode == JmoReturnCode::SUCCESS, false);
			iilid = rdata.i64_value;
			break;
		}
	//}
	/*
	else
	{
		while (1)
		{
			iilid = rand();
			umapitr_t itr_iilid = mIILIDMap.find(iilid);
			if (itr_iilid != mIILIDMap.end()) 
			{
				thriftClient->createIILByIILID(rdata, call_data, iilid, iil_type);
				aos_assert_r(rdata.rcode != JmoReturnCode::SUCCESS, false);
				continue;
			}
	
			thriftClient->createIILByIILID(rdata, call_data, iilid, iil_type);
			aos_assert_r(rdata.rcode == JmoReturnCode::SUCCESS, false);
			iilid = rdata.i64_value;
			break;
		}
	}
	*/
	
	aos_assert_r(rdata.rcode = JmoReturnCode::SUCCESS, false);
	bool key_unique = rand() % 2;
	bool docid_unique = rand() % 2;
	AosStIILPtr iil;
	iilid = (u64)rdata.i64_value;		// This is the IILID returned from the backend
	if (iil_type == JmoIILType::eKeyDocid)
	{
		iil = OmnNew AosStStrIIL(iilname, iilid, key_unique, docid_unique, true);
	}
	else
	{
		iil = OmnNew AosStU64IIL(iilname, iilid, key_unique, docid_unique, true);
	}

	if (iilname != "")
	{
		mIILNameMap[iilname] = iil;
	}
	mIILIDMap[iilid] = iil;
	mIILArray.push_back(iil);

	return true;
}


AosStIILPtr 
AosStIILCache::getIILByIILName(OmnString &iilname)
{
	mapitr_t itr = mIILNameMap.find(iilname);
	if (itr == mIILNameMap.end()) return 0;
	return mIILNameMap[iilname];
	
}


AosStIILPtr
AosStIILCache::getIILByIILID(u64 &iilid)
{
	umapitr_t itr = mIILIDMap.find(iilid);
	if (itr == mIILIDMap.end()) return 0;
	return mIILIDMap[iilid];
}


bool
AosStIILCache::deleteIIL(OmnString &iilname)
{
	for (vector<AosStIILPtr>::iterator itr = mIILArray.begin(); itr != mIILArray.end(); itr++)
	{
		if ((*itr)->getIILName() == iilname)
		{
			mIILArray.erase(itr);
			itr --;
		}
	}
	return true;
}

