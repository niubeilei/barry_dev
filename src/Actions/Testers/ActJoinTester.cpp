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
// 1. Create a table of records randomly.
// 2. Determine one column as the 'driving' column. This is the matched key.
// 3. Separate one or more columns into one IIL. 
//   
//
// Modification History:
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/Testers/ActJoinTester.h"

#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbRecord.h"
#include "Database/DbTable.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "IILClient/Ptrs.h"
#include "IILAssembler/IILAssembler.h"
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
#include "Util/ValueRslt.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

const OmnString sgTable1 = "actions_joiniil_table1";
const OmnString sgTable2 = "actions_joiniil_table2";
const OmnString sgResultTable = "actions_joiniil_table3";

AosActJoinTester::AosActJoinTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mName = "ActJoinTester";
}


AosActJoinTester::~AosActJoinTester()
{
}


bool 
AosActJoinTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosActJoinTester::basicTest()
{
	OmnScreen << "Hello, XuLei" << endl;
	return true;

	aos_assert_r(config(), false);
	while (1)
	{
		aos_assert_r(clearData(), false);
		aos_assert_r(createData(), false);
		aos_assert_r(createLocalResults(), false);
		aos_assert_r(runAction(), false);
		aos_assert_r(retrieveResults(), false);
		aos_assert_r(compareResults(), false);
	}
	return true;
}


bool
AosActJoinTester::config()
{
	/*
	AosXmlTagPtr app_conf = OmnApp::getAppConfig();
	aos_assert_r(app_conf, false);
	AosXmlTagPtr conf = app_conf->getFirstChild("tester_conf");
	aos_assert_r(conf, false);

	// CreateJob
	AosXmlTagPtr jobtag = conf->getFirstChild("job_conf");
	AosXmlTagPtr sdoc = conf->getFirstChild("sdoc");
	AosJobPtr job = AosJob::staticCreateJob(jobtag, sdoc, mRundata);
	aos_assert_r(job, false);

	// Create a task
	AosXmlTagPtr task_def = conf->getFirstChild("task_conf");
	aos_assert_r(task_def, false);
	mTask = AosTask::getTask(job, task_def, mRundata);
	aos_assert_r(mTask, false);
	
	// Create table1 IIL assember
	AosXmlTagPtr table1_def = conf->getFirstChild("iil_table1");
	aos_assert_r(table1_def, false);
	mTable1IILAssembler = AosIILAssembler::createIILAssembler(mTask, table1_def, mRundata);
	aos_assert_r(mTable1IILAssembler, false);

	// Create table2 IIL assember
	AosXmlTagPtr table2_def = conf->getFirstChild("iil_table2");
	aos_assert_r(table2_def, false);
	mTable2IILAssembler = AosIILAssembler::createIILAssembler(mTask, table2_def, mRundata);
	aos_assert_r(mTable2IILAssembler, false);
	
	mResultIILName = conf->getAttrStr("result_iilname");
	aos_assert_r(mResultIILName != "", false);
	
	
	mStore = OmnStoreMgr::getSelf()->getStore();            
	aos_assert_r(mStore, false);
	
	OmnString str1 = "create table ";
	str1 << sgTable1 << "(key varchar(" << eKeyMax << "), docid(int));";
	if (!mStore->runSQL(str1))
	{
		OmnAlarm << "Failed run sql: " << str1 << enderr;
		return false;
	}
	
	OmnString str2 = "create table ";
	str2 << sgTable2 << "(key varchar(" << eKeyMax << "), docid(int));";
	if (!mStore->runSQL(str2))
	{
		OmnAlarm << "Failed run sql: " << str2 << enderr;
		return false;
	}
	
	OmnString str3 = "create table ";
	str3 << sgResultTable << "(key varchar(" << eKeyMax << "), docid(int));";
	if (!mStore->runSQL(str3))
	{
		OmnAlarm << "Failed run sql: " << str2 << enderr;
		return false;
	}
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosActJoinTester::clearData()
{
	mKeys.clear();  
	mDocids.clear();
	
	OmnString str1 = "delete form ";
	str1 << sgTable1;
	
	OmnString str2 = "delete form ";
	str1 << sgTable2;
	
	OmnString str3 = "delete form ";
	str1 << sgResultTable;
	if ((!mStore->runSQL(str1))
			&& (!mStore->runSQL(str2))
			&& (!mStore->runSQL(str3)))
	{
		OmnAlarm << "Failed run clear data in mysql" << enderr;
		return false;
	}
	
	return true;
}


bool
AosActJoinTester::createLocalResults()
{
	OmnNotImplementedYet;
	return false;
}


bool
AosActJoinTester::runAction()
{
	OmnNotImplementedYet;
	return false;
}


bool
AosActJoinTester::compareResults()
{
	OmnNotImplementedYet;
	return false;
}


bool
AosActJoinTester::retrieveResults()
{
	AosQueryRsltPtr query_rslt = AosQueryRslt::getQueryRslt();
	query_rslt->setWithValues(true);
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	aos_assert_r(AosQueryColumn(mResultIILName, query_rslt, 0, query_context, mRundata), false);

	mKeys.clear();
	mDocids.clear();
	while (1)
	{
		u64 docid;
		OmnString key;
		bool finished;
		aos_assert_r(query_rslt->nextDocidValue(docid, key, finished, mRundata), false);
		if (finished) return true;
		mKeys.push_back(key);
		mDocids.push_back(docid);
	}
	return false;
}


bool
AosActJoinTester::createData()
{

	int num_record = OmnRandom::intByRange(
			100, 500, 50,
			501, 5000, 20,
			5001, 100000, 5);
	
	for (int i=0; i<num_record; i++)
	{
		int num_entry = rand()%10;
		bool big = false;
		int idx = rand()%100;
		if (idx <= 50) big = true;
		createData(big, num_entry);
	}
	sendData(eTable1);
	return true;
}


bool
AosActJoinTester::createData(const bool big, const int &num_entries)
{
	int total_len = 15;
	bool left_align = true;
	char padding = ' ';
	bool unique = false;

	
	// Here idx define one table1 to how many table2.
	int idx;
	if (big)
	{
		idx = rand()%10;
	}
	else
	{
		idx = -(rand()%10);
	}
	
	
	u64 docid1, docid2;
	OmnString table1, table2;
	int r = (rand()%num_entries > 0 ? 1 : rand()%num_entries); 
	for (int i=0; i<num_entries; i++)
	{
		table1 = "";
		table2 = "";
		if (i != 0)
		{
			table1 << "$$";
			table2 << "$$";
		}
		else
		{
			table1 << "";		
			table2 << "";		
		}
		OmnString str = OmnRandom::digitStr(total_len, left_align, padding, unique);
		table1 << str; 
		if (i < r)
		{
			table2 << str; 
		}
		else
		{
			table2 << "";	
		}
	}
	
	if (idx <= 0)
	{
		for (int j=0; j<idx; j++)
		{
			table2 << OmnRandom::digitStr(total_len, left_align, padding, unique);
			docid2 = rand();
			appendEntry(eTable2, table2, docid2);
			insertData(eTable2, table2, docid2);
		}
		docid1 = rand();
		appendEntry(eTable1, table1, docid1);
		insertData(eTable1, table1, docid1);
	}
	else
	{
		for (int j=0; j<idx; j++)
		{
			table1 << OmnRandom::digitStr(total_len, left_align, padding, unique);
			docid1 = rand();
			appendEntry(eTable1, table1, docid1);
			insertData(eTable1, table1, docid1);
		}
		docid2 = rand();
		appendEntry(eTable2, table2, docid2);
		insertData(eTable2, table2, docid2);
	}
	
	return true;
}


bool
AosActJoinTester::insertData(const E &type, const OmnString &str, const u64 &docid)
{
	OmnString stmt = "insert into ";
	
	switch(type)
	{
	case eTable1:
	{
		stmt << sgTable1 <<"(key, docid)"
			<< " values(" << str << ", " << docid << ")";
	}
	break;
	case eTable2:
	{
		stmt << sgTable1 <<"(key, docid)"
			<< " values(" << str << ", " << docid << ")";
	}
	break;
	default:
		OmnAlarm << "Undefined type" << enderr;
		return false;
	}
	
	if (!mStore->runSQL(stmt))
	{
		OmnAlarm << "Failed run sql: " << stmt << enderr;
		return false;
	}
	
	return true;
}

	
bool
AosActJoinTester::appendEntry(const E &type, const OmnString &str, const u64 &docid)
{
	AosValueRslt value_rslt;
	value_rslt.setKeyValue(str, false, docid);
	switch(type)
	{
	case eTable1:
		return mTable1IILAssembler->appendEntry(value_rslt, mRundata);

	case eTable2:
		return mTable2IILAssembler->appendEntry(value_rslt, mRundata);

	default:
		OmnAlarm << "Undefined type" << enderr;
		return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosActJoinTester::sendData(const E &type)
{
	switch(type)
	{
	case eTable1:
		return mTable1IILAssembler->sendFinish(mRundata);

	case eTable2:
		return mTable2IILAssembler->sendFinish(mRundata);

	default:
		OmnAlarm << "Undefined type" << enderr;
		return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool AosDbTestingTmp()
{
	OmnString stmt = "insert into ";
	stmt << "mytable (transid, type, length, xml)"
    	<< " values(?,?,?,?)";

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();            
	aos_assert_r(store, false);

	if (!store->runSQL(stmt))
	{
		OmnAlarm << "Failed run sql: " << stmt << enderr;
		return false;
	}

	stmt = "select field1, field2 from mytable where field1 = 123 limit 1";
	OmnDbRecordPtr record;
	OmnRslt err;
	bool rslt = OmnDbRecord::retrieveRecord(stmt, record);
	aos_assert_r(rslt, false);
	OmnScreen << "Field1: " << record->getU64(0, 0, err) << endl;
	OmnScreen << "Field2: " << record->getStr(1, "default value", err) << endl;

	stmt = "select field1, field2 from mytable where limit 100";
	OmnDbTablePtr table;
	aos_assert_r(store->query(stmt, table), false);

	table->reset();
	while (1)
	{
		record = table->next();
		if (!record) break;
		OmnScreen << "Field1: " << record->getU64(0, 0, err) << endl;
		OmnScreen << "Field2: " << record->getStr(1, "default value", err) << endl;
	}

	return true;
}

