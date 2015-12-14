////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 01/29/2015 by liuwei
////////////////////////////////////////////////////////////////////////////
#include "WordIDMgr/SmartArray.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include "stdio.h"

AosSmartArray::AosSmartArray()
:
mData(0),
mStart(0),
mLen(0),
mUpdateTime(1)
{
}



AosSmartArray::AosSmartArray(const AosSmartArray &rhs)
{
	mStart = rhs.mStart;
	mLen = rhs.mLen;
	mUpdateTime = rhs.mUpdateTime;
	u64* mData = (u64*)malloc(sizeof(u64) * mLen);
	memcpy(mData, rhs.mData, mLen*sizeof(u64));
}


AosSmartArray::~AosSmartArray()
{
	if(mData)
	{
		delete []mData;
		mData = 0;
	}
}

/*
	AosSmartArray array1;
	array1[10] = 100;

				array1[n] = v;
				expand?   expand
				return &(mData[n - mStart+1?]);
				 = v;
	array1[20] = 200;
	array1[8] = 300;
	cout << array1[5] << endl;
	cout << array1[10] << endl;
	cout << array1[20] << endl;
	cout << array1[30] << endl;
////////////////////////////////////////////////////////////
	AosSmartArray<OmnString,""> array1;
	array1[10] = "aaa";
	array1[20] = "bbb";
	array1[8] = "ccc";
	cout << array1[5] << endl;
	cout << array1[10] << endl;
	cout << array1[20] << endl;
	cout << array1[30] << endl;


*/u64&
AosSmartArray:: operator [](const i64 &idx)
{
	//1. check the array is initialized
	//2. check idx in the range or not
	//3. do we need to expand memory
	//4. get the return value.
	if(mLen == 0)
	{
		i64 start = idx - 100;
		if(start < 0)
		{
			start = 0;
		}
		i64 end = idx + 300;
		mStart = start;
		mLen = end - start +1;
		if(mData)
		{
			OmnAlarm << "something wrong." << enderr;
			delete []mData;
			mData = 0;
		}
		mData = (u64*)malloc(sizeof(u64) * mLen);
		return (mData[idx - mStart]);
	}


	if(idx >= mLen + mStart)
	{	
		i64 oldlen = mLen;
		
		mLen = idx - mStart + eDownExtendSize * (mUpdateTime++);
		
		u64* data1 = (u64*)malloc(sizeof(u64) * mLen);
		memset(data1,0,sizeof(u64)*mLen);
		memcpy(data1, mData, oldlen*sizeof(u64));
		free(mData);
		mData = data1;
		/*for(u64 i = mStart; i < id; i++)
		{
			mData1[i] = mData[i];
		}
		free(mData);
		mData = mData1 + id - start;
		return mData;*/
		return (mData[idx - mStart]);
	}
	else if (idx < mStart)
	{
		i64 orig_len = mLen;
		i64 orig_start = mStart;

		mStart = idx - eDownExtendSize * (mUpdateTime++);
		if(mStart < 0)
		{
			mStart = 0;
		}
		mLen += (orig_start - mStart);
		u64* data1 = (u64*)malloc(sizeof(u64) * mLen);
		memset(data1,0,sizeof(u64)*mLen);
		memcpy(data1, mData+ (orig_start - mStart)* sizeof(u64), orig_len*sizeof(u64));
		free(mData);
		mData = data1;
		return (mData[idx - mStart]);
	}

	return (mData[idx - mStart]);
}

/*
int main()
{	
	AosSmartArray sArray;
	u64 id,data;
	//cin >> id >> data;
	sArray[id] = data;
	sArray.mData = data;
	cout << sArray[id] << endl;
}

*/






