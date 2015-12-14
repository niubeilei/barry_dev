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
#include "Sorter/Testers/MergeVarFileSortTest.h"

#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "DataSort/DataSort.h"
#include "DataSort/Ptrs.h"
#include "Sorter/MergeFileSorter.h"
#include "DataTypes/DataColOpr.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/ThreadPool.h"
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
AosMergeVarFileSortTest::AosMergeVarFileSortTest()
{
}

bool 
AosMergeVarFileSortTest::start()
{
	bool rslt = false;
	while (1)
	{
		if (random()%2 == 0)
		{
OmnScreen << "Start  test sort : " << endl;
			for (u32 i = 0; i < 20; i++ )
			{
				testSort();
			}
			mSort = OmnNew AosMergeFileSorter(10000000, mCmpFun.getPtr(), mBigVirFiles, mRundata);
			sgThreadPool->proc(OmnNew AosGetOutputFile(mSort, mCmpFun, false, mRundata));  
			u64 time = OmnGetTimestamp();
			mSort->sort();
OmnScreen << "sort : 140G  ################## " << AosTimestampToHumanRead(OmnGetTimestamp() - time) << endl;
			deleteBigFile();
OmnScreen << "End test sort : " << endl;
		}
		else
		{
OmnScreen << "Start  test merge: " << endl;
			for (u32 i = 0; i < 20; i++ )
			{
				testMerge();
			}
			mMerge = OmnNew AosMergeFileSorter(10000000, mCmpFun.getPtr(), mBigVirFiles, mRundata);
			sgThreadPool->proc(OmnNew AosGetOutputFile(mMerge, mCmpFun, true, mRundata));  
			u64 time = OmnGetTimestamp();
			mMerge->sort();
OmnScreen << "merge : 160G  ################## " << AosTimestampToHumanRead(OmnGetTimestamp() - time) << endl;
			deleteBigFile();
OmnScreen << "Eend test sort : " << endl;
		}
	}
	return true;
}

bool
AosMergeVarFileSortTest::testSort()
{
	u64 file_id;
	AosNetFileObjPtr file;
	OmnString tmp_file_name; 
	OmnLocalFilePtr tmp_file;
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	tmp_file_name << "tmp_file" << random()%100 + 10;
	tmp_file = AosNetFileCltObj::createRaidFileStatic(
			rdata.getPtr(), file_id, tmp_file_name, 0, false);
	aos_assert_r(tmp_file && tmp_file->isGood(), false);

	mRundata = rdata;
	createSortCmpFunc(rdata);
	createSortVirFiles(rdata);
	AosMergeFileSorter *merge_sort = OmnNew AosMergeFileSorter(10000000, mCmpFun.getPtr(), mVirFiles, rdata);
	bool rslt = false;

	u64 time = OmnGetTimestamp();
	merge_sort->sort();
OmnScreen << "sort : 7G  ################## " << AosTimestampToHumanRead(OmnGetTimestamp() - time) << endl;

	AosBuffPtr buff = merge_sort->nextBuff();
	while (buff)
	{
		rslt = AosMergeFileSorter::sanitycheck_var(mCmpFun.getPtr(), buff->data(), buff->dataLen());
		aos_assert_r(rslt, false);
		rslt = tmp_file->append(buff->data(), buff->dataLen(), true); 
		buff = merge_sort->nextBuff();
	}
	file = OmnNew AosIdNetFile(0, file_id);
	mBigVirFiles.push_back(file);
	deleteFile();
	return true;
}


bool
AosMergeVarFileSortTest::createSortVirFiles(const AosRundataPtr &rdata)
{
	bool rslt;
	u64 file_id;
	OmnString file_name, tmp_file_name;
	AosNetFileObjPtr file;
	OmnLocalFilePtr tmp_file;

	//mFileLen = random() %100 + 1;
	mFileLen = 200;
	//mFileLen = 2;
	int reamin_size = 0;
	int vv = mFileLen;
	int len = 32;
	double double_value = 0.0;
	OmnString str_value = "";
	AosBuff::encodeRecordBuffLength(len);
	//AosBuff::decodeRecordBuffLength(len);
	while (vv)
	{
		tmp_file_name = "";
		tmp_file_name << "tmp_file" << vv;
		tmp_file = AosNetFileCltObj::createRaidFileStatic(
				rdata.getPtr(), file_id, tmp_file_name, 0, false);
		aos_assert_r(tmp_file && tmp_file->isGood(), false);
		AosBuffPtr buff = OmnNew AosBuff(1000000*4);
		for (u32 i = 1 ; i < 10000000; i++)
		//for (u32 i = 1 ; i < 10; i++)
		{
			buff->setInt(len);
			str_value = "";
			str_value << (random() %9000 + 1000);
			buff->setCharStr(str_value.data(), 4);
			buff->setU64(random()%9999 + 1);
			buff->setI64(random()%9999 + 1);
			double_value = random()%99 + 1 + 0.5;
			buff->setDouble(double_value);
		}
		buff->reset();
		AosBuffArrayVarPtr array = OmnNew AosBuffArrayVar(true); 
		array->setCompareFunc(mCmpFun.getPtr());
		rslt = array->setBodyBuff(buff, reamin_size);
		aos_assert_r(rslt, false);
		array->sort();
		AosBuffPtr head_buff = array->getHeadBuff(); 
		rslt = AosMergeFileSorter::sanitycheck(mCmpFun.getPtr(), head_buff->data(), head_buff->dataLen());
		aos_assert_r(rslt, false);
		AosBuffPtr body_buff;
		set<i64> bodyAddrSet;
		rslt = AosBuffArrayVar::procHeaderBuff(mCmpFun.getPtr(), head_buff, body_buff, bodyAddrSet);
		aos_assert_r(rslt, false);

		rslt = tmp_file->append(body_buff->data(),body_buff->dataLen(), true); 
		aos_assert_r(rslt, false);
		file = OmnNew AosIdNetFile(0, file_id);
		mVirFiles.push_back(file);
		vv --;
	}
	return true;
}


void
AosMergeVarFileSortTest::createSortCmpFunc(const AosRundataPtr &rdata)
{
	OmnString conf;
	conf << "<compareFun record_type=\"buff\" cmpfun_reserve=\"false\" cmpfun_size=\"36\" cmpfun_type=\"custom\" >"
		<<	"<datafields>"
		<< 		"<field type=\"str\"/>"
		<< 		"<field type=\"bin_u64\"/>"
		<< 		"<field type=\"bin_int64\"/>"
		<< 		"<field type=\"bin_double\"/>"
		<< 	"</datafields>"
		<<  "<cmp_fields>"
		<< 		"<field cmp_size=\"-1\" cmp_datatype=\"record\" field_type=\"buffstr\" cmp_pos=\"0\" cmp_reserve=\"false\"/>"
		<< 		"<field cmp_size=\"-1\" cmp_datatype=\"record\" field_type=\"u64\" cmp_pos=\"1\" cmp_reserve=\"false\"/>"
		<< 		"<field cmp_size=\"-1\" cmp_datatype=\"record\" field_type=\"i64\" cmp_pos=\"2\" cmp_reserve=\"false\"/>"
		<< 		"<field cmp_size=\"-1\" cmp_datatype=\"record\" field_type=\"double\" cmp_pos=\"3\" cmp_reserve=\"false\"/>"
		<<  "</cmp_fields>"
		<< "</compareFun>";
	AosXmlTagPtr cmp_tag =  AosXmlParser::parse(conf AosMemoryCheckerArgs);
	mCmpFun = AosCompareFun::getCompareFunc(cmp_tag);
}

bool
AosMergeVarFileSortTest::testMerge()
{
	u64 file_id;
	AosNetFileObjPtr file;
	OmnString tmp_file_name; 
	OmnLocalFilePtr tmp_file;
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	tmp_file_name << "tmp_file" << random()%100 + 10;
	tmp_file = AosNetFileCltObj::createRaidFileStatic(
			rdata.getPtr(), file_id, tmp_file_name, 0, false);
	aos_assert_r(tmp_file && tmp_file->isGood(), false);

	mRundata = rdata;
	createMergeCmpFunc(rdata);
	createMergeVirFiles(rdata);
	AosMergeFileSorter *merge_sort = OmnNew AosMergeFileSorter(10000000, mCmpFun.getPtr(), mVirFiles, rdata);
	bool rslt = false;
	u64 time = OmnGetTimestamp();
	merge_sort->sort();
OmnScreen << "merge : 8G  ################## " << AosTimestampToHumanRead(OmnGetTimestamp() - time) << endl;
	AosBuffPtr buff = merge_sort->nextBuff();
	while (buff)
	{
		rslt = AosMergeFileSorter::sanitycheck_var(mCmpFun.getPtr(), buff->data(), buff->dataLen());
		aos_assert_r(rslt, false);
		rslt = tmp_file->append(buff->data(), buff->dataLen(), true); 
		buff = merge_sort->nextBuff();
	}
	file = OmnNew AosIdNetFile(0, file_id);
	mBigVirFiles.push_back(file);
	deleteFile();
	return true;
}


bool
AosMergeVarFileSortTest::createMergeVirFiles(const AosRundataPtr &rdata)
{
	bool rslt;
	u64 file_id;
	OmnString file_name, tmp_file_name;
	AosNetFileObjPtr file;
	OmnLocalFilePtr tmp_file;

	mFileLen = 200;
	//mFileLen = 2;
	int reamin_size = 0;
	int vv = mFileLen;
	int len = 16;
	double double_value = 0.0;
	OmnString str_value = "";
	AosBuff::encodeRecordBuffLength(len);
	//AosBuff::decodeRecordBuffLength(len);
	while (vv)
	{
		tmp_file_name = "";
		tmp_file_name << "tmp_file" << vv;
		tmp_file = AosNetFileCltObj::createRaidFileStatic(
				rdata.getPtr(), file_id, tmp_file_name, 0, false);
		aos_assert_r(tmp_file && tmp_file->isGood(), false);
		AosBuffPtr buff = OmnNew AosBuff(1000000*4);
		for (u32 i = 1 ; i < 10000000; i++)
		//for (u32 i = 1 ; i < 10; i++)
		{
			buff->setInt(len);
			buff->setU64(i);
			buff->setDouble(double_value + i);

			buff->setInt(len);
			buff->setU64(i);
			buff->setDouble(double_value + i);
		}

		rslt = tmp_file->append(buff->data(),buff->dataLen(), true); 
		aos_assert_r(rslt, false);
		file = OmnNew AosIdNetFile(0, file_id);
		mVirFiles.push_back(file);
		vv --;
	}
	return true;
}

void
AosMergeVarFileSortTest::createMergeCmpFunc(const AosRundataPtr &rdata)
{
	OmnString conf;
	conf << "<compareFun record_type=\"buff\" cmpfun_reserve=\"false\" cmpfun_size=\"16\" cmpfun_type=\"custom\" >"
		<<	"<datafields>"
		<< 		"<field type=\"bin_u64\"/>"
		<< 		"<field type=\"bin_double\"/>"
		<< 	"</datafields>"
		<<  "<cmp_fields>"
		<< 		"<field cmp_size=\"-1\" cmp_datatype=\"record\" field_type=\"u64\" cmp_pos=\"0\" cmp_reserve=\"false\"/>"
		<<  "</cmp_fields>"
		<<  "<aggregations>"
		<<   "<aggregation agr_pos=\"1\" agr_type=\"record\" field_type=\"double\" agr_fun=\"norm\"/>"
		<<	 "</aggregations>"
		<< "</compareFun>";
	AosXmlTagPtr cmp_tag =  AosXmlParser::parse(conf AosMemoryCheckerArgs);
	mCmpFun = AosCompareFun::getCompareFunc(cmp_tag);
}



bool
AosMergeVarFileSortTest::deleteFile()
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
AosMergeVarFileSortTest::deleteBigFile()
{
	bool rslt = false;
	int server_id = -1;
	u64 file_id = 0;
	bool svr_death = false;
	for (u32 i = 0; i < mBigVirFiles.size(); i++)
	{
		server_id = mBigVirFiles[i]->getServerId();
		file_id = mBigVirFiles[i]->getFileId();
		rslt = AosNetFileCltObj::deleteFileStatic(file_id, server_id, svr_death, mRundata.getPtr());
		aos_assert_r(rslt, false);
	}
	mBigVirFiles.clear();
	return true;
}



bool
AosMergeVarFileSortTest::sanitycheck(
		AosCompareFun * comp,
		char *crt, 
		const int length)
{
//return true;
	int record_len = 4 + 2 * sizeof(u64);
	int entries = length/record_len;
	for (int i=0; i<entries; ++i)
	{
		aos_assert_r(*(double *)(crt + 4 + 8) == (*(u64 *)(crt +4)) * 200 * 10, false);
		//aos_assert_r(*(double *)(crt + 4 + 8) == (*(u64 *)(crt +4)) * 4 * 20, false);
		crt += record_len;
	}
	return true;
}


