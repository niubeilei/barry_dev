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
// 12/13/2012 Created by Ice
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_LRUCache_h
#define AOS_Util_LRUCache_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "Util/String.h"
#include "Util/OmnNew.h"
#include "Thread/Mutex.h"

#include <list>
#include <map>
using namespace std;

template<typename T_Id, typename T_Data, int T_Size=30>
class AosLRUCache
{
	OmnDefineRCObject;

	typedef typename list<pair<T_Id, T_Data> >::iterator 		T_MapValueType;
	typedef typename map<T_Id, T_MapValueType>::iterator 		T_MapItrType;

	OmnMutexPtr 				mLock;
	list<pair<T_Id, T_Data> >	mCache;
	map<T_Id, T_MapValueType> 	mMap;
	int 						mCached;
	int 						mTotal;

public:
	AosLRUCache():mLock(OmnNew OmnMutex()),mCached(0), mTotal(0){}

	template <typename T_DataGen>
	T_Data get(T_Id id, T_DataGen &dataGen)
	{
		T_Data data;
		mLock->lock();

		T_MapItrType mapitr = mMap.find(id);
		bool gen_data = false;
		if (mapitr == mMap.end())
		{
			if (mCache.size() < T_Size)
			{
				data = dataGen.getData(id);
				gen_data = true;
			}
			mapitr = (mMap.insert(
						make_pair<const T_Id&, T_MapValueType>(id, mCache.end()))).first;
		}
		else
		{
			mCached++;
			data = (*(mapitr->second)).second;
			mCache.erase(mapitr->second);
		}

		if (mCache.size() == T_Size)
		{
			// This means the required object was not found in mMap 
			// (i.e., data == null) and 'gen_data' == false.
			aos_assert_r(!data, data);
			aos_assert_r(!gen_data, data);
			pair<T_Id, T_Data> oldentry = mCache.back();
			T_MapItrType mapitrold = mMap.find(oldentry.first);
			mCache.erase(mapitrold->second);
			mMap.erase(mapitrold);
			if (!gen_data)
			{
				data = dataGen.getData(id, oldentry.second);
			}
		}
		mCache.push_front(make_pair(id, data));
		mapitr->second = mCache.begin();

		mTotal++;
		mLock->unlock();
		return data;
	}

	void remove(T_Id id) {
		mLock->lock();
		T_MapItrType mapitr = mMap.find(id);
		if (mapitr != mMap.end())
		{
			mCache.erase(mapitr->second);
			mMap.erase(mapitr);
		}
		mLock->unlock();
	}

	T_Data insert(T_Id id, T_Data data)
	{
		mLock->lock();
		T_Data tmpdata;
		T_MapItrType mapitr = mMap.find(id);
		if (mapitr != mMap.end())
		{
			tmpdata = mapitr->second->second;
			mCache.erase(mapitr->second);
			mMap.erase(mapitr);
		}

		if (mCache.size() == T_Size)
		{
			pair<T_Id, T_Data> oldentry = mCache.back();
			tmpdata = oldentry.second;
			T_MapItrType mapitrold = mMap.find(oldentry.first);
			mCache.erase(mapitrold->second);
			mMap.erase(mapitrold);
		}
		mCache.push_front(make_pair(id, data));
		mMap.insert(make_pair<const T_Id&, T_MapValueType>(id, mCache.begin()));
		mTotal++;
		mLock->unlock();
		return tmpdata;
	}
	double hitRate() {if (mTotal > 0) return (double)mCached/(mTotal+0.001);}
};
#endif

