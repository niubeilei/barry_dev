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
#if 0
#include "Util/Tester/BuffArrayTesterNew.h"

#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
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
#include <math.h>
using namespace std;

AosBuffArrayTesterNew::AosBuffArrayTesterNew()
{
	mName = "AosBufflTester";
	mTries = 100;
	OmnString conf;
	conf << "<compareFun cmpfun_size=\"44\" cmpfun_type=\"custom\">"
		 << 	"<cmp_fields>"
		 <<  		"<field cmp_size=\"-1\" cmp_datatype=\"u32\" cmp_pos=\"0\" cmp_size=\"4\" />"
		 << 	"</cmp_fields>"
		 <<  "<" << AOSTAG_AGGREGATIONS << ">"
		 <<   "<" << AOSTAG_AGGREGATION << " agr_pos=\"4\" agr_type=\"double\" agr_fun=\"norm\"/>"
		 <<   "<" << AOSTAG_AGGREGATION << " agr_pos=\"12\" agr_type=\"double\" agr_fun=\"max\"/>"
		 <<   "<" << AOSTAG_AGGREGATION << " agr_pos=\"20\" agr_type=\"double\" agr_fun=\"min\"/>"
		 <<	 "</" << AOSTAG_AGGREGATIONS << ">"
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
/* Chen Ding, 2014/11/24
	char *data = new char[28];
	u64 tt1 = OmnGetTimestamp();
	while (1)
	{
		map<u32, double> key;
		map<u32, double> sum;
		map<u32, double> max;
		map<u32, double> min;
		AosBuffArrayPtr buffarray = OmnNew AosBuffArray(mCmpFun, false, true, 100000);
		u64 tries = random()%999999 +100000; 
		for (u32 i=0; i<tries; i++)
		{
			u32 vvv = rand()%100;
			double v1 =  rand()%1000/3.14;
			double v2 =  rand()%2000/3.14;
			double v3 =  rand()%3000/3.14;
			*(u32 *)(&data[0]) = vvv;
			*((double *)(data+4)) = v1 ;
			*((double *)(data+12)) = v2; 
			*((double *)(data+20)) = v3;

			if (key.find(vvv) == key.end())
				key[vvv] = vvv;

			if (sum.find(vvv) == sum.end())
				sum[vvv] = v1;
			else
				sum[vvv] += v1;

			if (max.find(vvv) == max.end())
				max[vvv] = v2;
			else
				if (max[vvv] < v2 ) max[vvv] = v2;

			if (min.find(vvv) == min.end())
				min[vvv] = v3;
			else
				if (min[vvv] > v3) min[vvv] = v3;

			bool rslt = buffarray->appendEntry(data, 44, 0);
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
//		buffarray->sort();
		char *vv = new char[100] ;
		while (buffarray->nextValue(&vv))
		{
			double dd1 = sum[*(u32 *)vv];
			double dd2 = max[*(u32 *)vv];
			double dd3 = min[*(u32 *)vv];
			if (!(-0.00001 < sum[*(u32 *)vv] - *((double *)(vv+4)) < 0.00001))
			{
				printf("------------- Sum Num:       %d    %.8f\n", *(u32*)vv, *((double *)(vv+4)));
				printf("------------- Sum Map Data : %d    %.8f\n", *(u32*)vv, sum[*(u32 *)vv]);
			}
			if (!(-0.00001 < max[*(u32 *)vv] - *((double *)(vv+12)) < 0.00001))
			{
				printf("------------- max Num:       %d    %.8f\n", *(u32*)vv, *((double *)(vv+12)));
				printf("------------- max Map Data : %d    %.8f\n", *(u32*)vv, max[*(u32 *)vv]);
			}
			if (!(-0.00001 < min[*(u32 *)vv] - *((double *)(vv+20)) < 0.00001))
			{
				printf("------------- min Num:       %d    %.8f\n", *(u32*)vv, *((double *)(vv+20)));
				printf("------------- min Data :  	 %d    %.8f\n", *(u32*)vv, min[*(u32 *)vv]);
			}
		//	OmnScreen << endl;
#if 0
					
				sum[*(u32 *)vv] == *((double *)(vv+4)) &&
					max[*(u32 *)vv] == *((double *)(vv+12)) &&
					min[*(u32 *)vv] == *((double *)(vv+20)))// &&
			{
				//OmnScreen << "------------- Data     : "<< *(u32*)vvv << "   " << *((u64 *)(vvv+4)) << "    " <<   *((u64 *)(vvv+12)) << "    " <<  *((u64 *)(vvv+20))  << endl;
				//OmnScreen << "------------- Map Data : "<< *(u32 *)vvv << "   " << sum[*(u32 *)vvv] << "    " <<  max[*(u32 *)vvv] << "    " <<  min[*(u32 *)vvv] << endl;
				//OmnScreen << "------------- " << endl;
			}
			else
			{
				printf("------------- Num:       %d    %.8g    %.8g    %.8g\n", *(u32*)vv, *((double *)(vv+4)), *((double *)(vv+12)), *((double *)(vv+20)));
				printf("------------- Map Data : %d    %.8g    %.8g    %.8g\n", *(u32*)vv, sum[*(u32 *)vv], max[*(u32 *)vv],min[*(u32 *)vv] );
				OmnScreen << endl;
			//	OmnScreen << "------------- Num:       " <<  *(u32*)vv << "   " << *((double *)(vv+4))  << "    " << *((double *)(vv+12)) << "    " << *((double *)(vv+20))  << endl;
//				OmnScreen << "------------- Map Data : "<< *(u32 *)vv << "   " << sum[*(u32 *)vv] << "    " <<  max[*(u32 *)vv] << "    " <<  min[*(u32 *)vv] << endl;
				//	OmnScreen << "------------- Num:        " <<  *(u32*)vv << "    " << *((u64 *)(vv+20)) << endl;
				//	OmnScreen << "------------- Map Data : "<< *(u32 *)vv << "   " << sum[*(u32 *)vv] << endl;
				//	OmnScreen << "------------- " << endl;
			}
#endif
		}
	}

	return true;
*/
	OmnNotImplementedYet;
	return false;
}
#endif
