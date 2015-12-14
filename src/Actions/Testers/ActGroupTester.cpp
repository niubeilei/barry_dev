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
// 1. Randomly create a buff array. The array has 'key' and 'value'. The
//    'key' contains a group id. When creating data, it intentionally 
//    creates records so that some records have groups and some do  not.
// 2. Add the data to database and to the blob data.
// 3. Use the action to do the grouping. 
// 4. Verify the results with the database version.
//
// Torturer Variables:
// 1. The total number of records to create
// 2. The record length and groupid length
// 3. Some records are in groups and some are not in groups.
//
// Modification History:
// 06/20/2012 Created by Xu Lei
////////////////////////////////////////////////////////////////////////////
#include "Actions/Testers/ActGroupTester.h"

#if 0
#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/SdocAction.h"
#include "Actions/ActGroupbyProc.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbRecord.h"
#include "Database/DbTable.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "DataBlob/DataBlob.h"
#include "DataBlob/DataBlob.h"
#include "Job/Job.h"
#include "QueryUtil/QueryContext.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "TaskMgr/TaskAction.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"
#include "Util/CompareFun.h"
#include "Util/BuffArray.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

#include "XmlUtil/SeXmlParser.h"
#include "Random/RandomStr.h"
#include "Random/Ptrs.h"

const OmnString sgTable1 = "actions_joiniil_table1";
const OmnString sgTable2 = "actions_joiniil_table2";
const OmnString sgResultTable = "actions_joiniil_table3";

AosActGroupTester::AosActGroupTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mName = "ActGroupTester";
}


AosActGroupTester::~AosActGroupTester()
{
}


bool 
AosActGroupTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosActGroupTester::basicTest()
{
	// 	<config ...>
	// 		<testers ...>
	// 			<blobconfig .../>
	// 			...
	// 		</testers>
	// 		...
	// 	</config>

	aos_assert_r(config(), false);  // note , use config in as aos_assert_r
	MySQLInit();
	OmnScreen << "Hello, XuLei 2222" << endl;   
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(100);

	for (int i=0; i<mNumTries; i++)
	{
		config(); 
		clearData();
		createData();
		runAction();
		compareResults();
		//mBlobData->clear();
		OmnScreen << "----------------------------------------------" << endl << endl ;
		sleep(3);
	
	}
 
	return true;
}


bool
AosActGroupTester::clearData()
{
	MySQLClearData();
	mKeys.clear(); 
	return true;
}


bool
AosActGroupTester::createBlob()
{
	// 2. Create the blob
	mBlobData = AosDataBlob::createDataBlob(mBlobconf, mRundata);
	return true;
}


bool
AosActGroupTester::createData()
{
	// 1. Create the number of rows
	/*mCountRows = OmnRandom::intByRange(
			3, 3, 10,
			1, 5, 10,
			6, 500, 10,
			501, 1000000, 0, 
			1000001, 10000000, 0);
			*/
	/*		
	mCountRows = OmnRandom::intByRange(
			30, 50, 70,
			1, 5, 10,
			6, 500, 10,
			30, 50, 0, 
			50, 80, 0);
	*/
	mCountRows = OmnRandom::intByRange(
			3, 3, 70,
			3, 3, 10,
			3, 3, 10,
			30, 50, 0, 
			50, 80, 0);	
	// 2. Create the blob
	aos_assert_r(createBlob(), false);
	
	OmnScreen << "Blobdata Created ." << endl ;
	// 3. Create the data. 
	OmnString key, record;
	u64 value;
	for (int c = 0; c < mCountRows; c++)
	{
		record = composeKey(key);
		value = rand();
		AosValueRslt value_rslt;
		value_rslt.setKeyValue(record, false, value);
		mBlobData->appendEntry(value_rslt, mRundata);
		insertDataToMySQL(key, record, value);
	}	 
	OmnScreen << "Data Inserted." << endl;
	return true;
}


OmnString
AosActGroupTester::composeKey(OmnString &key)
{
	// Randomly determine whether to create a new key or use an existing key.
	mGroupIdLen = 4;

	if (rand() % 100 < 50 && mKeys.size() > 0)
	{
		// Use an existing key. 
		int idx = rand() % mKeys.size();
		key = mKeys[idx];
	}
	else
	{
		// Generate a new key
		key = OmnRandom::letterStr(mGroupIdLen, mGroupIdLen);
		mKeys.push_back(key);
	}

	mRecordLen = 7;
	mGroupIdLen = 4;
	int delta = mRecordLen - mGroupIdLen;
	OmnString record = key;
	
	record << OmnRandom::letterStr(delta, delta);
	return record;
}	


bool
AosActGroupTester::config()
{
	mApp_conf = OmnApp::getAppConfig();   
	aos_assert_r(mApp_conf, false);
	mConf = mApp_conf->getFirstChild("testers");
	aos_assert_r(mConf, false);

	//mActionConf = mConf->getFirstChild("action");
	//aos_assert_r(mActionConf, false);

	mBlobconf = mConf->getFirstChild("blobconfig");
	aos_assert_r(mBlobconf, false);

	mNumTries = mConf->getAttrInt("num_tries", 100);
	aos_assert_r(mNumTries > 0 , false);
	
	mGroupconf = mConf->getFirstChild("groupconfig");
	aos_assert_r(mConf, false);
	
	return true;
}


bool
AosActGroupTester::runAction()//const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_r(mRundata->getSiteid() > 0, false);
	mBlobData->sort();
	mActionConf = mBlobconf->getFirstChild("action");
	aos_assert_r(mActionConf, false);
	AosActionObjPtr action = AosActionObj::createAction(mActionConf, mRundata);
	aos_assert_r(action, false);
	
	return action->run(mBlobData, mRundata);
}


bool
AosActGroupTester::compareResults()
{
	bool flag = true;
	/*
	mBlobData->resetRecordLoop();
	while(flag)
	{
		datatmp = mBlobData->nextRecord(flag);
		OmnScreen << datatmp << endl ;
	}
	*/
	flag = false ;
	int vi=0;
	mBlobData->resetRecordLoop(); 
	AosValueRslt value_rslt;
	while(!flag)
	{
		vi++;
		mBlobData->nextValue(value_rslt, false);
		//	OmnScreen << str_value << v << endl ;

		int data_len;
		char *data = (char *)value_rslt.getCharStr(data_len);
		OmnString groupid(data, data_len);
		u64 value = value_rslt.getDocid();
		OmnScreen << "GroupID: "  << groupid << ", Value: " << value << endl;
	}

	OmnScreen <<  "ResultRows:"  << vi << endl <<"Start to compare results . " << endl;
	OmnString rawkey;
	u64 rawdocid;
	u32 rawnum = mData.getNumEntries();
	u32 newnum = mKeys.size();
	OmnScreen << "Create Data Lines:" << mCountRows << endl ; 
	OmnScreen << "Raw size:" << rawnum  << endl;
	OmnScreen << "New size:" << newnum  << endl;
	aos_assert_r(rawnum == newnum, false);
	for(u32 i=0; i<rawnum; i++)
	{
		rawkey = mData.getValue(i);
		rawdocid = mData.getU64Value(i);
		rawkey.normalizeWhiteSpace(false, true);
		aos_assert_r(rawkey == mKeys[i], false);
		aos_assert_r(rawdocid == mDocids[i], false);
	}

	return true;
}


bool
AosActGroupTester::insertDataToMySQL(
		const OmnString &key,
		const OmnString &record, 
		const u64 &value)
{
	//	char *str=	mBlobData->nextRecord(flag);
	OmnString sql = "";
	sql << " insert into grouptest(`keys`,record, v ) values('"<<key << "','"<< record << "','"<<value<< "') ;  ";
	//OmnScreen << sql << endl ;  
	MySQLExec(sql);
	return false;
}


bool
AosActGroupTester::runMySQLGroup()
{
	return true;
}


bool
AosActGroupTester::MySQLExec(OmnString sql)
{
	aos_assert_r(mStore, false);
	aos_assert_r(mStore->runSQL(sql), false);
	return true;
}


bool
AosActGroupTester::MySQLGetResult()
{
	OmnString sql = "select id  , v  from grouptestresult";
	OmnDbTablePtr table;
	aos_assert_r(mStore->query(sql, table), false);
	OmnRslt err;
	table->reset();
	OmnDbRecordPtr record;
	
	while (record = table->next())
	{
		OmnScreen << "Field1: " << record->getStr(0, 0, err) << endl;
		OmnScreen << "Field2: " << record->getStr(1, "default value", err) << endl;
	}
	return true;
}


bool
AosActGroupTester::MySQLInit()
{
	mStore = OmnStoreMgr::getSelf()->getStore();            
	aos_assert_r(mStore, false);
	OmnScreen << "MySQL inited ." << endl;
	return true;
}


bool
AosActGroupTester::MySQLClearData()
{
	OmnString sql="";
	sql = "delete from grouptest";
	MySQLExec(sql);
	//sql="delete from grouptestresult";
	//MySQLExec(sql);
	return true;
}
#endif
