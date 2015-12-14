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
// 2012/12/15 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILEntryMap/IILEntryMapMgr.h"

#include "API/AosApi.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Thread/ThreadPool.h"


static OmnMutexPtr sgLock = OmnNew OmnMutex();
static map<OmnString, AosIILEntryMapPtr> sgIILEntryMaps;
static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("iilentrymap", __FILE__, __LINE__);


AosIILEntryMapPtr
AosIILEntryMapMgr::retrieveIILEntryMap(
		const OmnString &iilname,
		const AosRundataPtr &rdata)
{
	return retrieveIILEntryMap(iilname, false, "", false, false, rdata);
}


AosIILEntryMapPtr
AosIILEntryMapMgr::retrieveIILEntryMap(
		const OmnString &iilname,
		const bool need_swap,
		const AosRundataPtr &rdata)
{
	return retrieveIILEntryMap(iilname, false, "", need_swap, false, rdata);
}


AosIILEntryMapPtr
AosIILEntryMapMgr::retrieveIILEntryMap(
		const OmnString &iilname,
		const bool need_split,
		const OmnString &sep,
		const bool need_swap,
		const bool use_key_as_value,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", 0);

	if (need_split) aos_assert_r(sep != "", 0);

	OmnString tmp_iilname = iilname;
	tmp_iilname << "_" << need_split << "_" << sep 
		<< need_swap << "_" << use_key_as_value;
	
	AosIILEntryMapPtr iilentrymap;
	sgLock->lock();
	map<OmnString, AosIILEntryMapPtr>::iterator itr = sgIILEntryMaps.find(tmp_iilname);
	if (itr != sgIILEntryMaps.end())
	{
		iilentrymap = itr->second;
		sgLock->unlock();
		return iilentrymap;
	}
	
	iilentrymap = OmnNew AosIILEntryMap(iilname, need_split, sep, need_swap, use_key_as_value);
	sgIILEntryMaps.insert(make_pair(tmp_iilname, iilentrymap));
	
	aos_assert_rl(sgThreadPool, sgLock, 0);
	OmnThrdShellProcPtr runner = OmnNew queryThrd(iilentrymap, rdata);
	bool rslt = sgThreadPool->proc(runner);
	aos_assert_rl(rslt, sgLock, 0);

	iilentrymap->wait();
	sgLock->unlock();

	return iilentrymap;
}


void
AosIILEntryMapMgr::clear()
{
	AosIILEntryMapPtr iilentrymap;
	sgLock->lock();
	map<OmnString, AosIILEntryMapPtr>::iterator itr = sgIILEntryMaps.begin();
	while (itr != sgIILEntryMaps.end())
	{
		iilentrymap = itr->second;
		iilentrymap->writelock();
		iilentrymap->clear();
		iilentrymap->unlock();
		itr++;
	}
	sgIILEntryMaps.clear();
	sgLock->unlock();
	OmnScreen << "Clear All IILEntryMap" << endl;
}


bool
AosIILEntryMapMgr::queryThrd::run()
{
	aos_assert_r(mIILMap, false);

	mIILMap->writelock();
	OmnScreen << "iil map write lock:" << mIILMap->mIILName << ":" << (u64)mIILMap.getPtr() << endl;
	mIILMap->post();

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	aos_assert_r(query_context, false);
	
	query_context->setOpr(eAosOpr_an);
	query_context->setReverse(false);
	query_context->setStrValue("*");
	query_context->setBlockSize(1000000);
	
	u64 value;
	OmnString key, value_str;
	bool rslt = true;
	bool finished = false;
	AosQueryRsltObjPtr query_rslt;

	OmnString sep = mIILMap->mSep;
	vector<OmnString> keys;
	if (mIILMap->mNeedSplit)
	{
		AosConvertAsciiBinary(sep);
		aos_assert_r(sep != "", false);	
	}

	while(1)
	{
		query_rslt = AosQueryRsltObj::getQueryRsltStatic();
		query_rslt->setWithValues(true);
		
		rslt = AosQueryColumn(mIILMap->mIILName, query_rslt, 0, query_context, mRundata);
		aos_assert_r(rslt, false);
		
		while(1)
		{
			rslt = query_rslt->nextDocidValue(value, key, finished, mRundata);
			aos_assert_r(rslt, 0);
			
			if (finished)
			{
				break;
			}

			value_str = "";
			if (mIILMap->mNeedSplit)
			{
				AosSplitStr(key, sep.data(), keys, 10);
				aos_assert_r(keys.size() == 2, false);
				aos_assert_r(keys[0] != "" && keys[1] != "", false);
				key = keys[0];
				value_str = keys[1];
			}
			else
			{
				value_str << (int64_t)value;
			}

			if (mIILMap->mNeedSwap)
			{
				if (mIILMap->mUseKeyAsValue)
				{
					mIILMap->insert(make_pair(value_str, value_str));
				}
				else
				{
					mIILMap->insert(make_pair(value_str, key));
				}
			}
			else
			{
				if (mIILMap->mUseKeyAsValue)
				{
					mIILMap->insert(make_pair(key, key));
				}
				else
				{
					mIILMap->insert(make_pair(key, value_str));
				}
			}
		}
		
		if (query_context->finished())
		{
			break;
		}
	}

	u32 size = mIILMap->size();
	if (size == 0)
	{
		AosSetErrorU(mRundata, "iil entry map is empty:") << mIILMap->mIILName;
		//OmnAlarm << mRundata->getErrmsg() << enderr;
	}
	OmnScreen << "iil map size:" << mIILMap->mIILName << ":" << size << endl; 

	return true;
}


bool
AosIILEntryMapMgr::queryThrd::procFinished()
{
	aos_assert_r(mIILMap, false);
	OmnScreen << "iil map write unlock:" << mIILMap->mIILName << ":" << (u64)mIILMap.getPtr() << endl;
	mIILMap->unlock();
	return true;
}


bool
AosIILEntryMapMgr::getMapValues(
		const AosIILEntryMapPtr &mp,
		vector<OmnString> &keys,
		vector<OmnString> &values,
		const OmnString &dft_value,
		const bool need_create,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mp, false);

	AosIILEntryMapItr itr;
	values.clear();

	set<OmnString> miss_values;
	mp->readlock();
	for (size_t i=0; i<keys.size(); i++)
	{
		itr = mp->find(keys[i]);
		if (itr != mp->end())
		{
			values.push_back(itr->second);
		}
		else
		{
			if (need_create)
			{
				miss_values.insert(keys[i]);	
			}
			else
			{
				values.push_back(dft_value);
			}
		}
	}
	mp->unlock();

	if (miss_values.size() > 0) 
	{
		vector<u64> rslt_values;
		bool rslt = AosIILClientObj::getIILClient()->getMapValues(mp->mIILName, miss_values, rslt_values, rdata);
		if (!rslt || miss_values.size() != rslt_values.size())
		{
			OmnAlarm << "get missing value faild :" << mp->mIILName << enderr;
			return false;
		}

		mp->writelock();
		OmnString value_str,key_str;
		u32 i = 0;
		set<OmnString>::iterator itr_t = miss_values.begin();
		for (; itr_t != miss_values.end(); itr_t++)
		{
			value_str = "";
			value_str << rslt_values[i];
			mp->insert(make_pair(*itr_t, value_str));
			i++;
		}
		// get value's from backend
		// here need some code

		values.clear();

		for (i = 0; i < keys.size(); i++)
		{
			itr = mp->find(keys[i]);
			if (itr != mp->end())
			{
				values.push_back(itr->second);
			}
			else
			{
				mp->unlock();
				OmnAlarm << "missing value, key:" << keys[i] << enderr;
				return false;
			}
		}
		mp->unlock();
	}

	return true;
}

