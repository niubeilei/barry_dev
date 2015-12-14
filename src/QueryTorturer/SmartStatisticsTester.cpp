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
#include "QueryTorturer/SmartStatisticsTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h" 
#include "QueryTorturer/QueryStatistics.h"
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
#include "Porting/Sleep.h"
#include "UtilTime/TimeUtil.h"
#include "Util/BuffArray.h"
#include "Util/File.h"
#include "Util/HashUtil.h"
#include "Util/UtUtil.h"
#include "Util1/MemMgr.h"

static OmnThreadPool sgThreadPool("field_data_creators", __FILE__, __LINE__);
static vector<AosQueryField::Pattern>								sgPatterns;


AosSmartStatisticsTester::AosSmartStatisticsTester()
:
mMaxBuffSize(100000000),
mStartDocid(1000),
mCrtGroupId(0),
mTestFinished(0),
mNumGroupsToTry(0),
mNumGroupsTried(0),
mFieldValueMethod(1),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mPagesize(0),
mMinGroupId(555),
mNumDocs(5000),
mTries(0),
mMaxActiveGroups(1),
mMinBlockSize(eDftMinBlockSize),
mNumThreadsPerField(3),
mNumStrings(0),
mNumStrFields(eDftNumStrFields),
mNumU64Fields(eDftNumU64Fields),
mMaxBlockSize(eDftMaxBlockSize),
mMaxGroups(eDftMaxGroups),
mMin(1),
mMax(2)
{
	AosQueryField::setTester(this);
	//genFieldInfo(3);
	//setVectors();
	//fout.open("stat.txt");
	//generateData("",0);      
	//fout << flush;
	//fout.close();
	//exit(0);
}


AosSmartStatisticsTester::~AosSmartStatisticsTester()
{
}


bool 
AosSmartStatisticsTester::start()
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

	//produce data 
	genFieldForGenData();
	setVectors();
	fout.open("/home/wumeng/StatTorturer/stat_other.txt");
	generateData("",0); 
	fout << flush;
	fout.close();

	//kill all server
	//OmnString cmd = "cd /home/wumeng/AOS_cvs/rel_2_0/AOS/src/SvrProxyExe && killall SvrProxy.exe access.exe";
	//system(cmd.data());
	//OmnSleep(5);
	
	OmnString cmd = "cd /home/wumeng/Jimo_Servers0 && ./jimodb stop 9";
	system(cmd.data());
	OmnSleep(5);


	//clean data
	//cmd = "cd /home/wumeng/AOS_cvs/rel_2_0/AOS/src/SvrProxyExe && ./rmdata";
	//system(cmd.data());
	
	cmd = "cd /home/wumeng/Jimo_Servers0 && ./jimodb clean";
	system(cmd.data());



	//start server
	//cmd = "cd /home/wumeng/AOS_cvs/rel_2_0/AOS/src/SvrProxyExe && ./SvrProxy.exe -config config_proxy.txt &";
	//system(cmd.data());
	//OmnSleep(5);
	//cmd = "cd /home/wumeng/AOS_cvs/rel_2_0/AOS/src/SvrProxyExe && nohup ./access.exe -config config_access.txt &";
	//system(cmd.data());
	//OmnSleep(3);
	
	cmd = "cd /home/wumeng/Jimo_Servers0 && ./jimodb start";
	system(cmd.data());

	OmnString result;
	FILE *fp = NULL;
	char buf[1024];
	
	/*
	fp = popen(cmd.getBuffer(), "r");
	if (fgets(buf, 1024, fp)) 
	{
		result = buf;
	}
	OmnSleep(2);
	fclose(fp);
	*/

	//run job
	OmnString command;
	OmnString sour;
	sour <<"\" source '/home/wumeng/StatTorturer/json_stat_job.txt';\""; 

	command << "cd /home/wumeng/Jimo_Servers0 &&"
			<< " ./JQLClient.exe -p 30570 -f \"xml\" -h 192.168.99.182 -t " 
			<< sour;
	
	cout << command << endl;

	fp = popen(command.getBuffer(), "r");
	while (fgets(buf, 1024, fp)) 
	{
		result << buf;
	}
	cout << result << endl;
	//system(command.data());

	//exit(0);
	//sleep 100s
	OmnSleep(100);

	//set fields
	setFields();
	
	cout << "    Start Tester ..." << endl;

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

/*
static AosQueryField::FieldInfo sgKeyFields[] = {
	{0, "k1", 1, 2, true, 0},
	{1, "k2", 1, 2, true, 0},
	{2, "k3", 1, 2, true, 0}
	//{3, "k4", 987, 991, true, 0},
	//{4, "k5", 987, 991, true, 0},
	//{5, "k6", 65374, 65380, true, 0}
};

static AosQueryField::FieldInfo sgMeasureFields[] = {
	{0, "v1", 1, 2, true, 0},
	{1, "v2", 1, 2, true, 0}
	//{2, "v3", 87543, 87555, true, 0}
};
*/

static OmnString sgAggrFuncs[] = {
	//"max", "min", "sum", "count", "dist_count"
	"max", "min", "sum", "count"
	//"max", "min", "count"
};

static AosQueryField::FieldInfo sgTimeFields[] = {
	{0, "d1", 10468, 10473, true, 0}
};

static OmnString sgTimeFieldName = "d1";
static OmnString sgTimeUnit = "day";
static AosQueryField::FieldInfo sgTimeunits[] = {
	{0, "min", 0, 10, true, 0},
	{1, "hour", 2, 10, true, 0},
	{2, "day", 3, 10, true, 0},
	{3, "month", 4, 10, true, 0},
	{4, "year", 6, 10, true, 0}
};

//2 times data
//static  AosQueryField::FieldInfo sgKeyFields_k1[2], sgKeyFields_k2[2], sgKeyFields_k3[2];
//static  AosQueryField::FieldInfo sgMeasureFields_v1[2], sgMeasureFields_v2[2];


bool
AosSmartStatisticsTester::genFieldForGenData()
{
	sgKeyFields_k.clear();
	sgMeasureFields_v.clear();

	int min = mMin;
	int max = mMax;
	sgKeyFields_k.push_back({0, "k1", 1, 9, true, 0});
	sgKeyFields_k.push_back({1, "k2", 2, 8, true, 0});
	sgKeyFields_k.push_back({2, "k3", 3, 6, true, 0});
	//sgKeyFields_k.push_back({3, "k4", 4, 6, true, 0});
	//sgKeyFields_k.push_back({4, "k5", 6, 9, true, 0});

	sgMeasureFields_v.push_back({0, "v1", 1, 9, true, 0});
	sgMeasureFields_v.push_back({1, "v2", 2, 8, true, 0});

	min+=2;
	max+=2;

	return true;
}



bool
AosSmartStatisticsTester::genFieldInfo(int num)
{
	sgKeyFields.clear();
	sgMeasureFields.clear();
	//int min = ran()%10;
	//int max = ran()%10 + min;
	int min = 1;
	int max = 2;

	for(int i=0; i<num; i++)
	{
		sgKeyFields_k.clear();
		sgMeasureFields_v.clear();
		sgKeyFields_k.push_back({0, "k1", 1, 9, true, 0});
		sgKeyFields_k.push_back({1, "k2", 2, 8, true, 0});
		sgKeyFields_k.push_back({2, "k3", 3, 6, true, 0});
		//sgKeyFields_k.push_back({3, "k4", 4, 6, true, 0});
		//sgKeyFields_k.push_back({4, "k5", 6, 9, true, 0});
		sgKeyFields.push_back(sgKeyFields_k);

		sgMeasureFields_v.push_back({0, "v1", 1, 9, true, 0});
		sgMeasureFields_v.push_back({1, "v2", 2, 8, true, 0});
		sgMeasureFields.push_back(sgMeasureFields_v);

		min+=2;
		max+=2;
	}

	return true;
}


bool 
AosSmartStatisticsTester::setVectors()
{
	AosQueryField::FieldInfo f;

	for (u32 i = 0; i < sgKeyFields_k.size(); i++) 
	{
		f = sgKeyFields_k[i];

		OmnString k_name = f.mFieldName;
		int k_max = f.mMaxValue;
		int k_min = f.mMinValue;
		mKeyNames.push_back(k_name);
		mKeyMaxs.push_back(k_max);
		mKeyMins.push_back(k_min);
	}


	for (u32 i = 0; i < sgMeasureFields_v.size(); i++) 
	{   
			f = sgMeasureFields_v[i];

			OmnString k_name = f.mFieldName;
			int k_max = f.mMaxValue;
			int k_min = f.mMinValue;
			mKeyNames.push_back(k_name);
			mKeyMaxs.push_back(k_max);
			mKeyMins.push_back(k_min);
	}


	for (u32 i = 0; i < GET_ARRAY_LEN(sgTimeFields); i++) 
	{
		f = sgTimeFields[i];

		OmnString k_name = f.mFieldName;
		int k_max = f.mMaxValue;
		int k_min = f.mMinValue;
		mKeyNames.push_back(k_name);
		mKeyMaxs.push_back(k_max);
		mKeyMins.push_back(k_min);
	}  
	return true;
}


bool
AosSmartStatisticsTester::generateData(string prefix,int idx)
{   
	string fname=mKeyNames[idx];
	int max=mKeyMaxs[idx];
	int min=mKeyMins[idx];
	int count=max-min+1;
	int step=1;
	int value= -1;
	OmnString line;

	if(prefix != "")
	{
		prefix += ", ";
	}
	if(fname=="v1" || fname=="v2")
	{
		if(fname=="v1")
		{
			value=mmap.find("k1")->second;
		}
		else if(fname=="v2")
		{
			value=mmap.find("k2")->second;
		}   
		line="";
		line << prefix  <<  value;
		if (idx == (mKeyMaxs.size() - 1) ) 
		{ 
			//write line 
			fout << line << "\n";
		}
		else
		{
			generateData(line,idx+1);
		}   
	}
	else
	{
		for (int i = 0; i < count; i++) 
		{
			mmap[fname]=min;
			line ="";
			if(fname=="d1")
			{
				line << prefix  <<  min;
			}
			else
			{
				line << prefix << fname << "_" <<  min;
			}
			if (idx == (mKeyMaxs.size() - 1) ) 
			{
				//write line
				fout << line << "\n";
			}
			else
			{
				generateData(line,idx+1);
			}
				 min += step;
		}
	}
	return true;
}
		                                                                                                                                    

bool
AosSmartStatisticsTester::setFields()
{
	genFieldInfo(1);

	OmnString field_name = "";
	OmnString aggr_func = "";

	AosQueryField::FieldInfo f;
	vector<AosQueryField::FieldInfo> vect_f;
	OmnString name = "";

	//set key list
	//for (u32 i = 0; i < GET_ARRAY_LEN(sgKeyFields_k1); i++) 
	for (u32 j = 0; j < sgKeyFields.size(); j++)
	{
		//vect_f = sgKeyFields[j];
		mKeyFields_k.clear();
		for (u32 i = 0; i < sgKeyFields[j].size(); i++) 
		{
			f = sgKeyFields[j][i];
			AosQueryFieldKey* key = OmnNew AosQueryFieldKey(i, false);

			key->setFieldName(f.mFieldName);
			key->setMaxU64Value(f.mMaxValue);
			key->setMinU64Value(f.mMinValue);
			key->setIsNumAlpha(f.mIsNumAlpha);
			mKeyFields_k.push_back(key);
		}
		mKeyFields.push_back(mKeyFields_k);
	}


	//set time list
	for (u32 i = 0; i < GET_ARRAY_LEN(sgTimeFields); i++) 
	{
		f = sgTimeFields[i];
		AosQueryFieldTime* time = OmnNew AosQueryFieldTime(i, false);

		time->setFieldName(f.mFieldName);
		time->setMaxU64Value(f.mMaxValue);
		time->setMinU64Value(f.mMinValue);
		time->setIsNumAlpha(f.mIsNumAlpha);
		mTimeField.push_back(time);
	}


	//set value list
	for (u32 j = 0; j < sgMeasureFields.size(); j++)
	{
		mMeasureFields_v.clear();
		for (u32 i = 0; i < sgMeasureFields[j].size(); i++)
		{
			f = sgMeasureFields[j][i];
			AosQueryFieldMeasure* value = OmnNew AosQueryFieldMeasure(i, false);
		
			value->setFieldName(f.mFieldName);
			value->setMaxU64Value(f.mMaxValue);
			value->setMinU64Value(f.mMinValue);
			value->setIsNumAlpha(f.mIsNumAlpha);
			mMeasureFields_v.push_back(value);	
		}
		mMeasureFields.push_back(mMeasureFields_v);	
	}


	//set measure list
	for (u32 i = 0; i < GET_ARRAY_LEN(sgAggrFuncs); i++)
	{
		AosQueryFieldMeasure* measure = OmnNew AosQueryFieldMeasure(i, false);
		
		measure->setAggrFunc(sgAggrFuncs[i]);
		mAggrFields.push_back(measure);
	}


	//set time field
	for (u32 i = 0; i < GET_ARRAY_LEN(sgTimeunits); i++) 
	{
		f = sgTimeunits[i];
		AosQueryFieldTime* time = OmnNew AosQueryFieldTime(i, true);

		time->setFieldName(f.mFieldName);
		mTimeField.push_back(time);
	}


	return true;
}


OmnString
AosSmartStatisticsTester::getResult()
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
AosSmartStatisticsTester::runQuery()
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
AosSmartStatisticsTester::grandQueryTorturer()
{
	// Do not create contents. It runs queries only. 
	cout << "===== To run query only =====" << endl;
	return runQuery();
}


bool
AosSmartStatisticsTester::init()
{
	AosSetCrtDay(AosTimeUtil::str2EpochDay("2013-12-31"));
	cout << "Today: " << AosTimeUtil::str2EpochDay("2013-12-31") << endl;
	srand(2222);
	return true;
}


bool
AosSmartStatisticsTester::config()
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
AosSmartStatisticsTester::getMaxDocid() const
{
	return mStartDocid + mNumDocs - 1;
}


bool
AosSmartStatisticsTester::groupFailed(const u64 group_id)
{
	mLock->lock();
	mNumGroupsTried++;
	mStats.erase(group_id);
	mTestFinished = true;
	mLock->unlock();
	return true;
}


bool
AosSmartStatisticsTester::groupFinished(const u64 group_id)
{
	mLock->lock();
	mNumGroupsTried++;
	mStats.erase(group_id);
	if (mNumGroupsTried >= mNumGroupsToTry) 
	{
		mTestFinished = true;
	}
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool	
AosSmartStatisticsTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
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

		if (mStats.size() >= (u32)mMaxActiveGroups)
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
			AosQueryStatisticsPtr stat = OmnNew AosQueryStatistics(this, group_id, mTableName);
			aos_assert_r(stat->getGroupId() == group_id, false);
			stat->setKeyList(mKeyFields);
			stat->setTimeField(mTimeField);
			stat->setMeasureList(mMeasureFields);
			stat->setAggrList(mAggrFields);
			stat->setPageSize(1000);
			stat->setDatabase(mDatabase);
			stat->setDataType(AosQueryStatistics::eDataFullCombination);
			mLock->lock();
			mStats[group_id] = stat;
			group_id++;
			// mCondVar->signal();
			mLock->unlock();
			cout << "===========================" << endl;
			cout << "run next test case: " << group_id-1 << endl;
			cout << "===========================" << endl;
			stat->runQuery();
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
AosSmartStatisticsTester::signal(const int threadLogicId)
{
	return true;
}


u32
AosSmartStatisticsTester::getNumFields()
{
	return mFields.size();
}


vector<AosQueryFieldPtr>
AosSmartStatisticsTester::getFields()
{
	return mFields;
}

