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
// 2013/02/12	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/BuffArrayTesterNew.h"

#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "DataSort/DataSort.h"
#include "DataSort/Ptrs.h"
#include "Sorter/MergeSorter.h"
#include "Sorter/MultiFileSorter.h"
#include "DataTypes/DataColOpr.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/BuffArray.h"
#include "Util/CompareFun.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <set>
#include <vector>
using namespace std;

AosBuffArrayTesterNew::AosBuffArrayTesterNew()
{
	mName = "AosBufflTester";
	mTries = 100;
	OmnString conf;
	conf << "<compareFun cmpfun_reserve=\"false\" cmp_size1=\"4\" cmp_datatype1=\"u32\" cmp_pos1=\"0\" cmpfun_size=\"44\" cmpfun_type=\"custom\">"
		 <<  "<aggregations>"
		 <<   "<aggregation agr_pos=\"4\" agr_type=\"u64\" agr_fun=\"norm\"/>"
		 <<   "<aggregation agr_pos=\"12\" agr_type=\"u64\" agr_fun=\"max\"/>"
		 <<   "<aggregation agr_pos=\"20\" agr_type=\"u64\" agr_fun=\"min\"/>"
		 <<	 "</aggregations>"
		 << "</compareFun>";
	AosXmlTagPtr cmp_tag =  AosXmlParser::parse(conf AosMemoryCheckerArgs);
	mCmpFun = AosCompareFun::getCompareFunc(cmp_tag);
}

struct u64comp
{
	bool operator()(const char *s1, const char *s2)
	{
		return (*(u64 *)s1) < (*(u64 *)s2);
	}
};


bool AosBuffArrayTesterNew::start()
{
	testInsert();
	return true;
}


bool
AosBuffArrayTesterNew::testInsert()
{
	/*
	AosRundataPtr rdata = OmnApp::getRundata();
	char *data = new char[28];
	u64 tt1 = OmnGetTimestamp();
	while (1)
	{
		map<u32, u64> key;
		map<u32, u64> sum;
		map<u32, u64> max;
		map<u32, u64> min;
		AosBuffArrayPtr buffarray = OmnNew AosBuffArray(mCmpFun, false, true, 100000);
		AosBuffArrayPtr buffarray2 = OmnNew AosBuffArray(mCmpFun, false, true, 100000);
		u64 tries = random()%999999 +100000; 
		for (int i=0; i<tries; i++)
		{
			u32 vvv = rand()%100;
			u64 v1 =  rand()%10;
			u64 v2 =  rand()%1000;
			u64 v3 =  rand()%10;
			*(u32 *)(&data[0]) = vvv;
			*((u64 *)(data+4)) = v1 ;
			*((u64 *)(data+12)) = v2; 
			*((u64 *)(data+20)) = v3;

			if (key.find(vvv) == key.end())
				key[vvv] = vvv;

			if (sum.find(vvv) == sum.end())
				sum[vvv] = v1 *2;
			else
				sum[vvv] += v1 *2;

			if (max.find(vvv) == max.end())
				max[vvv] = v2;
			else
				if (max[vvv] < v2 ) max[vvv] = v2;

			if (min.find(vvv) == min.end())
				min[vvv] = v3;
			else
				if (min[vvv] > v3) min[vvv] = v3;

			bool rslt = buffarray->addValue(data, 44, 0);
			rslt = buffarray2->addValue(data, 44, 0);
			aos_assert_r(rslt, false);
			if (i % 100000 == 0) 
			{
				u64 tt2 = OmnGetTimestamp();
				OmnScreen << "Num Entries: " << i << ":" << tt2 -tt1 << endl;
				tt1 = tt2;
			}
		}
		AosDataColOpr::E opr = AosDataColOpr::eNormal;
		buffarray->sort(opr);
		buffarray2->sort(opr);

		AosMergeSorter<AosBuffArrayPtr> mergeSorter(buffarray, buffarray2, opr, mCmpFun, NULL, 0, (void*)false, rdata);
		AosBuffPtr buff = mergeSorter.sort(rdata);
		int len = 0;

//		buffarray->sort();
		char *vv = buff->data();
		while (len < buff->dataLen())
		{
			if (sum[*(u32 *)vv]  == *((u64 *)(vv+4)) &&
					max[*(u32 *)vv] == *((u64*)(vv+12)) &&
					min[*(u32 *)vv] == *((u64*)(vv+20)))
			{
				//OmnScreen << "------------- Data     : "<< *(u32*)vvv << "   " << *((u64 *)(vvv+4)) << "    " <<   *((u64 *)(vvv+12)) << "    " <<  *((u64 *)(vvv+20))  << endl;
				//OmnScreen << "------------- Map Data : "<< *(u32 *)vvv << "   " << sum[*(u32 *)vvv] << "    " <<  max[*(u32 *)vvv] << "    " <<  min[*(u32 *)vvv] << endl;
				//OmnScreen << "------------- " << endl;
			}
			else
			{
				OmnScreen << "------------- Num:        " <<  *(u32*)vv << "   " << *((u64 *)(vv+4+len))  << "    " << *((u64 *)(vv+12+len)) << "    " << *((u64 *)(vv+20+len))  << endl;
				OmnScreen << "------------- Map Data : "<< *(u32 *)vv << "   " << sum[*(u32 *)vv] << "    " <<  max[*(u32 *)vv] << "    " <<  min[*(u32 *)vv] << endl;
				//	OmnScreen << "------------- Num:        " <<  *(u32*)vv << "    " << *((u64 *)(vv+20)) << endl;
				//	OmnScreen << "------------- Map Data : "<< *(u32 *)vv << "   " << sum[*(u32 *)vv] << endl;
				//	OmnScreen << "------------- " << endl;
			}
			len += mCmpFun->size;
			vv += mCmpFun->size;
		}
	}
*/

	return true;
}

