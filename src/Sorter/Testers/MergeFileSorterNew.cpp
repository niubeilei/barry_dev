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
#include "Sorter/Testers/MergeFileSorterNew.h"

#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "DataSort/DataSort.h"
#include "DataSort/Ptrs.h"
#include "Sorter/MergeFileSorter.h"
#include "DataTypes/DataColOpr.h"
#include "Tester/Test.h"
#include "Thread/ThreadPool.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/BuffArray.h"
#include "Util/CompareFun.h"
#include "Util/LocalFile.h"
#include "NetFile/NetFile.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <set>
#include <vector>
using namespace std;
static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("MergeVarFileSortTest", __FILE__, __LINE__);   

AosMergeFileSorterNew::AosMergeFileSorterNew()
{
}

bool 
AosMergeFileSorterNew::start()
{
	//while (1)
	{
		testInsert();
	}
	return true;
}

bool
AosMergeFileSorterNew::createVirFiles()
{
	AosNetFileObjPtr file;
	u64 file_id = 9223372036854775910;
	for (u32 i = 0; i < 50; i++)
	{
		file = OmnNew AosIdNetFile(0, file_id + i*2);
		mVirFiles.push_back(file);
	}
	return true;
}

bool
AosMergeFileSorterNew::createVirFiles(const AosRundataPtr &rdata)
{
	bool rslt;
	u64 file_id;
	OmnString file_name, tmp_file_name;
	AosNetFileObjPtr file;
	OmnLocalFilePtr tmp_file;

	mFileLen = 100;
	//mFileLen = 10;
	int vv = mFileLen;
	while (vv)
	{
		tmp_file_name = "";
		tmp_file_name << "tmp_file" << vv;
		tmp_file = AosNetFileCltObj::createRaidFileStatic(
				rdata.getPtr(), file_id, tmp_file_name, 0, false);
		aos_assert_r(tmp_file && tmp_file->isGood(), false);
		AosBuffPtr buff = OmnNew AosBuff(1000000*4);
		for (u32 i = 1 ; i < 10000000; i++)
		//for (u32 i = 1 ; i < 100; i++)
		{
			buff->setU64(i);
			buff->setU64(i);

			buff->setU64(i);
			buff->setU64(i);
		}
		rslt = tmp_file->append(buff->data(),buff->dataLen(), true); 
		aos_assert_r(rslt, false);
		//aos_assert_r(tmp_file->getLength() == sizeof(u32) * 1000000, false)
		OmnScreen << file_id << endl;
		file = OmnNew AosIdNetFile(0, file_id);
		mVirFiles.push_back(file);
		vv --;
	}
	return true;
}


void
AosMergeFileSorterNew::createCmpFunc(const AosRundataPtr &rdata)
{
	mName = "AosBufflTester";
	OmnString conf;
	conf << "<compareFun cmpfun_reserve=\"false\" cmpfun_size=\"16\" cmpfun_type=\"custom\" >"
		<<  "<cmp_fields>"
		<<     "<field cmp_size=\"-1\" cmp_datatype=\"u64\" cmp_pos=\"0\" cmp_size=\"8\"/>"
		<<  "</cmp_fields>"
		<<  "<aggregations>"
		<<   "<aggregation agr_pos=\"8\" agr_type=\"u64\" agr_fun=\"norm\"/>"
		<<   "</aggregations>" 
		<< "</compareFun>";
	AosXmlTagPtr cmp_tag =  AosXmlParser::parse(conf AosMemoryCheckerArgs);
	mCmpFun = AosCompareFun::getCompareFunc(cmp_tag);
}

bool
AosMergeFileSorterNew::testInsert()
{
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	mRundata = rdata;
	createCmpFunc(rdata);
//	createVirFiles(rdata);
	createVirFiles();
	mSort = OmnNew AosMergeFileSorter(10000000, mCmpFun.getPtr(), mVirFiles, rdata);
	sgThreadPool->proc(OmnNew AosGetOutputFile(mSort, mCmpFun, false, mRundata));  
	bool rslt = false;

	u64 time = OmnGetTimestamp();
	mSort->sort();
	OmnScreen << "sort : 32G ################## " << AosTimestampToHumanRead(OmnGetTimestamp() - time) << endl;
	deleteFile();
OmnScreen << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	return true;
}


bool
AosMergeFileSorterNew::deleteFile()
{
	bool rslt = false;
	int server_id = -1;
	u64 file_id = 0;
	bool svr_death = false;
	for (u32 i = 0; i < mVirFiles.size(); i++)
	{
		server_id = mVirFiles[i]->getServerId();
		file_id = mVirFiles[i]->getFileId();
		rslt = AosNetFileCltObj::deleteFileStatic(file_id, server_id, svr_death, mRundata.getPtr());
		aos_assert_r(rslt, false);
	}
	mVirFiles.clear();
	return true;
}


bool
AosMergeFileSorterNew::sanitycheck(
		AosCompareFun * comp,
		char *crt, 
		const int length)
{
return true;
	int record_len = comp->size;
	int entries = length/record_len;
	for (int i=0; i<entries; ++i)
	{
		// 1. check file is sorted
		if (i>0)
		{
			//aos_assert_r(comp->cmp(crt, crt-record_len) >= 0, false);
			if (comp->cmp(crt, crt-record_len) < 0)
			{
				OmnAlarm << enderr;
				return comp->cmp(crt, crt-record_len);
			}
		}

		// 2. check the entry whether is right
//		bool rslt = checkEntry(crt);
//		aos_assert_r(rslt, false);
		
		aos_assert_r((*(u64 *)crt) * mFileLen * 2 == (*(u64 *)(crt+sizeof(u64))), false);
		crt += record_len;
	}
	return true;
}


