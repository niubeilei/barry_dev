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
// How to torture: 
// 1. Define a 100000 int record array. 
// 2. Randomly do one of the following:
//    a. Add a record, if possible
//    b. Add a cell by randomly pick a record, and then check whether
//       the record is full. If not, append a value.
//    c. Randomly pick a record and a value in the record. Add a random
//       value to the cell.
//    d. Randomly pick a record, check the values. 
//    e. Batch modify
//
// Modification History:
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/Testers/Vt2dTester.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "Util/DataTypes.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/TaskObj.h"

#include "DataStructs/Vector2D.h"
#include "DataStructs/Ptrs.h"
#include "DataStructs/StatIdIDGen.h"
#include "DataStructs/Testers/DataGen.h"
#include "StatUtil/Ptrs.h"
#include "StatUtil/StatRecord.h"


AosVt2dTester::AosVt2dTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mRundata->setSiteid(100);
	mRundata->setUserid(307);
	mName = "Vt2dTester";
}


AosVt2dTester::~AosVt2dTester()
{
}


bool 
AosVt2dTester::start()
{
	cout << "Start Vector2D Tester ..." << endl;
	config();
	basicTest();
	return true;
}


bool
AosVt2dTester::config()
{
	OmnString filename = "../DataStructs/Testers/stat_sdd_doc.txt";
	OmnFilePtr file = OmnNew OmnFile(filename, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), false);
	OmnString docstr;
	bool rslt = file->readToString(docstr);
	aos_assert_r(rslt, false);

	mVtConfDoc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(mVtConfDoc, false);

	filename = "../DataStructs/Testers/stat_sdd_conf.txt";
	file = OmnNew OmnFile(filename, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), false);
	docstr = "";
	rslt = file->readToString(docstr);
	aos_assert_r(rslt, false);

	mVtConf = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(mVtConf, false);

	filename = "../DataStructs/Testers/stat_root_doc.txt";
	file = OmnNew OmnFile(filename, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), false);
	docstr = "";
	rslt = file->readToString(docstr);
	aos_assert_r(rslt, false);

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);	
	buff->setInt64(0);

	AosXmlTagPtr doc = AosCreateBinaryDoc(-1, docstr, true, buff, mRundata);
	aos_assert_r(doc, false);

	OmnString ctrl_doc_objid = doc->getAttrStr(AOSTAG_OBJID, "");

	mDataGen = OmnNew AosDataGen(mRundata, ctrl_doc_objid);

	// [docid, time, value, key]
	OmnString name;
	name = "docid";
	AosStrValueInfo info;
	info.field_offset = 0;
	mFieldInfo.insert(make_pair(name, info));

	name = "time";
	AosStrValueInfo info1;
	info1.field_offset = sizeof(u64);
	mFieldInfo.insert(make_pair(name, info1));

	name = "str";
	AosStrValueInfo info2;
	info2.field_offset = sizeof(u64)+sizeof(u64) + sizeof(u64);
	mFieldInfo.insert(make_pair(name, info2));

	name = "value";
	AosStrValueInfo info3;
	info3.field_offset = sizeof(u64)+sizeof(u64);
	mFieldInfo.insert(make_pair(name, info3));
	mValueName =  name;

	return true;
}


bool 
AosVt2dTester::basicTest()
{
	bool rslt = false;
	int tries = 100000;
	while(tries--)
	{
		OmnScreen << "tries:" << tries << ";"<< endl;
		rslt = run();
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosVt2dTester::run()
{
	mDataGen->run();

	AosBuffPtr buff;
	mDataGen->getData(mRundata, buff);
	if (buff->dataLen() == 0) return true;

	OmnSleep(1);

	OmnScreen << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	AosVector2DPtr vt2d = OmnNew AosVector2D(mRundata, mVtConf);

	vector<AosValueRslt> oldvalue_rslt;
	vector<AosValueRslt> newvalue_rslt;
	vt2d->modify(
			mRundata,  
			buff, 
			AosDataGen::eRecordLen, 
			mFieldInfo,
			mValueName,
			oldvalue_rslt,
			newvalue_rslt);
	OmnScreen << "==========CheckValue=======================" << endl;

	checkValue();
	OmnScreen << "==========CheckValueEnd=======================" << endl;
	return true;
}


bool
AosVt2dTester::checkValue()
{
	AosXmlTagPtr vector2d = mVtConfDoc->getFirstChild("vector2d");
	aos_assert_r(vector2d, false);

	AosXmlTagPtr tag = mVtConfDoc->getFirstChild("internal_statistics");
	aos_assert_r(tag, false);

	tag = tag->getFirstChild("statistic");
	aos_assert_r(tag, false);

	AosXmlTagPtr measure = tag->getFirstChild("measure");
	aos_assert_r(measure, false);
	measure->setAttr("time_unit", "_day");

	AosVector2DPtr vt2d = OmnNew AosVector2D(mRundata,  vector2d, measure);
	vector<AosVector2DRecord *> records;
	vector<u64> dist_block_docids;
	vector<u64> stat_ids;
	map<u64, u32> index;
	int64_t s_time;
	int64_t e_time;

	mDataGen->getRecord(dist_block_docids, stat_ids, index, s_time, e_time);
OmnScreen << "AosVt2dTester;CheckValue;stat_ids:" << stat_ids.size() << ";" << endl;
	if (dist_block_docids.size() == 0) return true;

	vt2d->getStatDocByTester(
			mRundata,
			dist_block_docids,
			stat_ids,
			records,
			s_time,
			e_time);

	for (u32 i = 0; i< records.size(); i++)
	{
		AosVector2DRecord * rcd = records[i];
		u64 stat_id = rcd->getStatDocid();

		u32 idx = index[stat_id];
		for (u32 j = 0; j < rcd->getStatValueNum(); j++)
		{
			if (mDataGen->getSlotSize(idx) != rcd->getStatValueNum())
			{
				u32 tt = rcd->getStatValueNum();
				for (u32 k =0; k< tt; k++)
				{
					u64 time_slot = rcd->getTimeValue(k);
OmnScreen << "XXXXXXXXXxx:"<< time_slot << ";"<< endl;
				}
				mDataGen->print(idx);
			}
			aos_assert_r(mDataGen->getSlotSize(idx) == rcd->getStatValueNum(), false);

			u64 time_slot = rcd->getTimeValue(j);
			int64_t v1 = 0;
			bool isvalid = mDataGen->isVaild(idx, time_slot, v1);
			if (isvalid)
			{
				//OmnScreen << "AosVt2d; time_slot:" << time_slot << ";v1:" << v1 << ";"<< endl;
				AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);	
				rcd->getStatValue(j, buff);
				int64_t v2 = buff->getInt64(0);
				aos_assert_r(v1 == v2, false);
			}
			else
			{
				OmnScreen << "AosVt2dTester;isvalid:false; time_slot:" << time_slot << ";" << endl;
				aos_assert_r(!isvalid, false);
			}
		}
	}
	return true;
}


// 30 years
// day: 30years * 366 = 10980 
// bucket weigth: 100 day
// index entry num : 10980 / 100 = 110
//
// num_index size: sizeof(u64)
// index entry size: sizeof(u64) + sizeof(u64) = 110 * 16 + 8 = 1768 
// other
// key size: 300;
// General value: sizeof(u64)
// other_total = 308
//
// ext_doc_size = 100day * sizeof(int64_t) = 800;
// root_doc_size = 800 + 1768 + 308 = 2876; 

// ext zky_doc_per_distblock = AosDistBlock::eDfmDistBlockSize/ext_doc_size = 12500 
// root zky_doc_per_distblock = AosDistBlock::eDfmDistBlockSize/root_doc_size  = 3477 

