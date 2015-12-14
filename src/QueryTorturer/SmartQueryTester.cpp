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
// Testing the Conditions:
// 1. It creates an array of docs. 
// 	  <doc gruop_id="ddd" 	// u64
// 	  	   field_str_00="random_str"
// 	  	   ...
// 	  	   field_str_nn="random_str"
// 	  	   field_u64_00="random_str"
// 	  	   ...
// 	  	   field_u64_nn="random_str"/>
// 	  Field values are calculated as:
// 	  		value[field_id] = strFieldValue(field_id, docid);
// 	  		value[field_id] = u64FieldValue(field_id, docid);
// 	  It uses 'docid' to create the value. 
//
// 2. Once docs are created, it creates doc groups by randomly
//    picking some docs. For each doc being picked, it adds
//    a field:
//    		group_field_<group_id>="group_id"
//    Docs are selected by rules, such as all the odd docids, 
//    all the even docids, etc.
//	  A group is defined as:
//	  	Group = 
//	  	{
//	  		group_id: ddd,
//	  		doc rule: rule, // the rule that determines the membership
//			field_str_00 Condition,
//			field_str_01 Condition,
//			...
//			field_u64_00 Condition,
//			field_u64_01 Condition,
//			...
//		}
// 3. Randomly pick a group, formulate the conditions, run the query, 
//    and then check the results.
//
// 4. How to handle parallel IILs
//    In the current implementations, there are totally five string IILs:
//    		str_field_0
//    		str_field_1
//    		str_field_2
//    		str_field_3
//    		str_field_4
//    To support parallel IIL, 
//
//    a. The third str field is treated as the time field.
//    b. The first, third, and fifth fields are parallel IILs.
//    c. 
// Modification History:
// 2013/02/15	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryTorturer/SmartQueryTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "QueryTorturer/QueryGroup.h"
#include "QueryTorturer/QueryField.h"
#include "QueryTorturer/QueryFieldNum.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEUtil/IILIdx.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadPool.h"
#include "Thread/Thread.h"
#include "UtilTime/TimeUtil.h"
#include "Util/BuffArray.h"
#include "Util/File.h"
#include "Util/HashUtil.h"
#include "Util/UtUtil.h"
#include "Util1/MemMgr.h"

static OmnThreadPool sgThreadPool("field_data_creators", __FILE__, __LINE__);
static vector<AosQueryField::Pattern>	sgPatterns;

AosSmartQueryTester::AosSmartQueryTester()
:
mTries(0),
mNumStrings(0),
mPagesize(0),
mNumDocs(5000),
mNumStrFields(eDftNumStrFields),
mNumU64Fields(eDftNumU64Fields),
mMinBlockSize(eDftMinBlockSize),
mMaxBlockSize(eDftMaxBlockSize),
mMaxGroups(eDftMaxGroups),
mStartDocid(1000),
mCrtGroupId(0),
mTestFinished(0),
mNumGroupsToTry(0),
mNumGroupsTried(0),
mFieldValueMethod(1),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mMinGroupId(555),
mMaxActiveGroups(1),
mMaxBuffSize(100000000),
mNumThreadsPerField(3)
{
	AosQueryField::setTester(this);
}


AosSmartQueryTester::~AosSmartQueryTester()
{
}


bool 
AosSmartQueryTester::start()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(100);
//	mRundata->setSiteid(AosGetDftSiteId());

	AosQueryField::Pattern pattern;

	// Lowest byte goes to the highest
	pattern.right = 0;
	pattern.pattern = 0x000000ff;
	pattern.left = 24;
	pattern.max = 255;
	sgPatterns.push_back(pattern);

	// Second lowest byte goes the lowest
	pattern.right = 8;
	pattern.pattern = 0x000000ff;
	pattern.left = 0;
	pattern.max = 255;
	sgPatterns.push_back(pattern);

	// Third byte goes to the second 
	pattern.right = 16;
	pattern.pattern = 0x000000ff;
	pattern.left = 8;
	pattern.max = 255;
	sgPatterns.push_back(pattern);

	// Highest byte goes to the third 
	pattern.right = 24;
	pattern.pattern = 0x000000ff;
	pattern.left = 16;
	pattern.max = 255;
	sgPatterns.push_back(pattern);

	//set fields
	setFields();
	
	OmnScreen << "    Start Tester ..." << endl;

	// OmnString docstr = "<useracct />";
	// AosXmlTagPtr user_acct = AosStr2Xml(docstr);
	// aos_assert_r(user_acct, false);
	// mUserAcct = AosSengAdmin::getSelf()->createUserAcct(user_acct, 
	// OmnString docstr = "<doc abc=\"Chen Ding\"/>";
	// AosXmlTagPtr doc = AosCreateDoc(docstr, true, mRundata);
	// aos_assert_r(doc, false);

	config();
	init();
	bool rslt = grandQueryTorturer();
	aos_assert_r(rslt, false);
	return true;
}

static OmnString sgData0= 
	 	"database changed <content total=\"7\" start_idx=\"0\" crt_idx=\"0\" queryid=\"75\" num=\"5\" time=\"201.700000ms\">"
	 		"<record f1=\"15\" f2=\"42\"></record>"
	 		"<record f1=\"11\" f2=\"39\"></record>"
	 		"<record f1=\"12\" f2=\"40\"></record>"
	 		"<record f1=\"13\" f2=\"41\"></record>"
	 		"<record f1=\"14\" f2=\"42\"></record>"
	 		"<record f1=\"15\" f2=\"43\"></record>"
	 		"<record f1=\"16\" f2=\"38\"></record>"
	 		"<record f1=\"17\" f2=\"39\"></record>"
	 		"<record f1=\"18\" f2=\"40\"></record>"
	 		"<record f1=\"19\" f2=\"41\"></record>"
	 		"<record f1=\"20\" f2=\"42\"></record>"
	 		"<record f1=\"21\" f2=\"43\"></record>"
	 	"</content>";

static OmnString sgData1 = 
	 	"database changed <content total=\"7\" start_idx=\"0\" crt_idx=\"0\" queryid=\"75\" num=\"5\" time=\"201.700000ms\">"
	 		"<record f1=\"21\" f2=\"42\"></record>"
	 		"<record f1=\"20\" f2=\"42\"></record>"
	 		"<record f1=\"19\" f2=\"42\"></record>"
	 	"</content>";

static OmnString sgData2 = 
	 	"database changed <content total=\"7\" start_idx=\"0\" crt_idx=\"0\" queryid=\"75\" num=\"5\" time=\"201.700000ms\">"
	 		"<record f1=\"18\" f2=\"42\"></record>"
	 		"<record f1=\"17\" f2=\"42\"></record>"
	 		"<record f1=\"16\" f2=\"42\"></record>"
	 		"<record f1=\"15\" f2=\"42\"></record>"
	 	"</content>";

#define GET_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

static AosQueryField::FieldInfo sgFields[] = {
	{0, "key_field1", 42342, 42503, true, 0},
	{1, "key_field2", 2311, 2339, true, 0},
	{2, "key_field3", 87543, 87555, true, 0},
	{3, "key_field4", 987, 991, true, 0},
	{4, "key_field5", 65374, 65380, true, 0}
};

OmnString
AosSmartQueryTester::getResult()
{
	static int i = 0;

	i++;
	switch (i % 3) 
	{
		case 0:
			return sgData0;

		case 1:
			return sgData1;

		case 2:
			return sgData2;

		default:
			break;
	}

	return sgData0;
}


bool
AosSmartQueryTester::setFields()
{
	AosQueryFieldPtr field;
	AosQueryField::FieldInfo f;
	OmnString name = "";

	//hard code 5 fields for now
	//f1 --> f5
	for (u32 i = 0; i < GET_ARRAY_LEN(sgFields); i++) 
	{
		f = sgFields[i];
		field = new AosQueryFieldNum(i, false);

		field->setFieldName(f.mFieldName);
		field->setMaxU64Value(f.mMaxValue);
		field->setMinU64Value(f.mMinValue);
		field->setIsNumAlpha(f.mIsNumAlpha);
		mFields.push_back(field);
	}

	return true;
}



bool
AosSmartQueryTester::runQuery()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mGroupCreationThread = OmnNew OmnThread(thisPtr, 
			"iiltester_group_thrd", 0, false, true, __FILE__, __LINE__);
	mGroupCreationThread->start();

	mTestFinished = false;
	mNumGroupsTried = 0;
	mCrtGroupId = 0; 
	while (!mTestFinished)
	{
		OmnSleep(2);
	}
	return true;
}


bool
AosSmartQueryTester::grandQueryTorturer()
{
	// Do not create contents. It runs queries only. 
	OmnScreen << "===== To run query only =====" << endl;
	return runQuery();
}


bool
AosSmartQueryTester::init()
{
	AosSetCrtDay(AosTimeUtil::str2EpochDay("2013-12-31"));
	OmnScreen << "Today: " << AosTimeUtil::str2EpochDay("2013-12-31") << endl;
	srand(2222);
	return true;
}


bool
AosSmartQueryTester::config()
{
	mTries = eDftTries;
	mNumDocs = 5000;
	mNumStrFields = eDftNumStrFields;
	mNumU64Fields = 0; 
	mMinBlockSize = eDftMinBlockSize;
	mMaxBlockSize = eDftMaxBlockSize;
	mMaxGroups = eDftMaxGroups;
	//mTestType = tag->getAttrStr("test_type");
	mPagesize = 100;
	mNumGroupsToTry = 100;
	mFieldValueMethod = 1;
	mMaxActiveGroups = 1;
	mMaxBuffSize = 100000000;
	mNumThreadsPerField = 3;
	mMaxTimeField = AosTimeUtil::str2EpochDay("2013.12.31");
	mMinTimeField = AosTimeUtil::str2EpochDay("2010.01.01");
	mTableName = "cdrs";
	return true;
/*	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("bitmap_testers");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", eDftTries);

	mNumDocs = tag->getAttrInt64("num_docs_create", 5000);
	if (mNumDocs <= 0) mNumDocs = 5000;
	
	mNumStrFields = tag->getAttrInt("num_str_fields", 0);
	if (mNumStrFields < 0) mNumStrFields = eDftNumStrFields;
	
	mNumU64Fields = tag->getAttrInt("num_u64_fields", 0);
	if (mNumU64Fields < 0) mNumU64Fields = eDftNumU64Fields;

	if (mNumStrFields == 0 && mNumU64Fields == 0)
	{
		mNumStrFields = eDftNumStrFields;
	}
	
	// 'mMinBlockSize' and 'mMaxBlockSize' are used to control
	// the buff array size to send to IIL. It is measured in number
	// of docs per IIL call.
	mMinBlockSize = tag->getAttrInt("min_block_size", eDftMinBlockSize);
	if (mMinBlockSize <= 0) mMinBlockSize = eDftMinBlockSize;
	
	mMaxBlockSize = tag->getAttrInt("max_block_size", eDftMaxBlockSize);
	if (mMaxBlockSize <= 0) mMaxBlockSize = eDftMaxBlockSize;
	
	mMaxGroups = tag->getAttrInt("max_groups", eDftMaxGroups);
	if (mMaxGroups <= 0) mMaxGroups = eDftMaxGroups;
	
	mTestType = tag->getAttrStr("test_type");
	mPagesize = tag->getAttrInt("page_size", 100);
	if (mPagesize < 10) mPagesize = 100;

	mNumGroupsToTry = tag->getAttrInt("num_groups_to_try", 100);
	if (mNumGroupsToTry < 10) mNumGroupsToTry = 100;

	mFieldValueMethod = tag->getAttrInt("field_value_method", 1);
	mMaxActiveGroups = tag->getAttrInt("max_active_groups", 1);
	if (mMaxActiveGroups <= 0) mMaxActiveGroups = 1;

	mMaxBuffSize = tag->getAttrInt("max_buff_size", 100000000);
	mNumThreadsPerField = tag->getAttrInt("num_threads_per_field", 3);

	mMaxTimeField = tag->getAttrInt("max_time_field", -1);
	mMinTimeField = tag->getAttrInt("min_time_field", -1);
	mTableName = tag->getAttrStr("tablename");

	if (mMaxTimeField == -1)
	{
		mMaxTimeField = AosTimeUtil::str2EpochDay("2013.12.31");
	}

	if (mMinTimeField == -1)
	{
		mMinTimeField = AosTimeUtil::str2EpochDay("2010.01.01");
	}

	if (mTableName == "") mTableName = "cdrs";

	return true;
*/
}


u64
AosSmartQueryTester::getMaxDocid() const
{
	return mStartDocid + mNumDocs - 1;
}


bool
AosSmartQueryTester::groupFailed(const u64 group_id)
{
	mLock->lock();
	mNumGroupsTried++;
	mGroups.erase(group_id);
	mTestFinished = true;
	mLock->unlock();
	return true;
}


bool
AosSmartQueryTester::groupFinished(const u64 group_id)
{
	mLock->lock();
	mNumGroupsTried++;
	mGroups.erase(group_id);
	if (mNumGroupsTried >= mNumGroupsToTry) 
	{
		mTestFinished = true;
	}
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool	
AosSmartQueryTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u64 group_id = mMinGroupId;
	aos_assert_r(mMaxActiveGroups > 0, false);
	srand(9999);
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		// if (testFinished())
		// {
		// 	if (mGroups.size() == 0)
		// 	{
		// 		// All groups have finished. It is the time to exit the thread.
		// 		mLock->unlock();
		// 		break;
		// 	}
		// }

		if (mGroups.size() >= (u32)mMaxActiveGroups)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		mLock->unlock();

		try
		{
			//use hardcoded table name for now
			mTableName = "t1";
			mDatabase = "db";
			AosQueryGroupPtr group = OmnNew AosQueryGroup(this, group_id, mTableName);
			aos_assert_r(group->getGroupId() == group_id, false);
			group->setFields(mFields);
			group->setPageSize(1000);
			group->setDatabase(mDatabase);
			group->setDataType(AosQueryGroup::eDataFullCombination);
			mLock->lock();
			mGroups[group_id] = group;
			group_id++;
			// mCondVar->signal();
			mLock->unlock();
			OmnScreen << "===========================" << endl;
			OmnScreen << "run next test case: " << group_id-1 << endl;
			OmnScreen << "===========================" << endl;
			group->runQuery();
		}

		catch (...)
		{
			OmnAlarm << "Internal error" << enderr;
			return false;
		}
	}

	return true;
}


bool	
AosSmartQueryTester::signal(const int threadLogicId)
{
	return true;
}


u32
AosSmartQueryTester::getNumFields()
{
	return mFields.size();
}


vector<AosQueryFieldPtr>
AosSmartQueryTester::getFields()
{
	return mFields;
}

