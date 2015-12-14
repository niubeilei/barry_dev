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
#include "Actions/Testers/ActImportTester.h"

#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "IILClient/Ptrs.h"
#include "IILAssembler/IILAssembler.h"
#include "Job/Job.h"
#include "QueryUtil/QueryContext.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/TaskAction.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosActImportTester::AosActImportTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mRundata->setSiteid(100);
	mRundata->setUserid(307);
	mName = "ActImportTester";
}


AosActImportTester::~AosActImportTester()
{
}


bool 
AosActImportTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosActImportTester::basicTest()
{
	aos_assert_r(config(), false);
	int tries = 0;
	while (tries < 5)
	{
OmnScreen << "Tries   ======  " << tries << endl; 
		clearData();
		createData();
		//createLocalResults();
		runAction();
		retrieveResults();
		compareResults();
		tries++;
	}
	return true;
}


bool
AosActImportTester::config()
{
	AosXmlTagPtr app_conf = OmnApp::getAppConfig();
	aos_assert_r(app_conf, false);
	AosXmlTagPtr conf = app_conf->getFirstChild("testers");
	aos_assert_r(conf, false);

	// CreateJob
	//AosXmlTagPtr jobtag = app_conf->getFirstChild("job_conf");
	//AosJobPtr job = AosJob::staticCreateJob(jobtag, mRundata);
	//aos_assert_r(job, false);

	// Create a task
	//AosXmlTagPtr task_def = conf->getFirstChild("task_conf");
	//aos_assert_r(task_def, false);
	//mTask = AosTask::getTask(job, task_def, mRundata);
	//aos_assert_r(mTask, false);

	// Create IIL assember
	// AosXmlTagPtr assembler_def = conf->getFirstChild("iil_assembler");
	// aos_assert_r(assembler_def, false);
	// mIILAssembler = AosIILAssembler::createIILAssembler(mTask, assembler_def, mRundata);
	// aos_assert_r(mIILAssembler, false);

	// CreateAction
	AosXmlTagPtr sdoc = conf->getFirstChild("sdoc");
	aos_assert_r(sdoc, false);
	mAction = sdoc->getFirstChild("action");
	aos_assert_r(mAction, false);

	mResultIILName = conf->getAttrStr("result_iilname");
	aos_assert_r(mResultIILName != "", false);

	return true;
}


bool
AosActImportTester::clearData()
{
	mKeys.clear();
	mDocids.clear();
	mData.clear();
	return true;
}


bool
AosActImportTester::createLocalResults()
{
	
	return true;
}

bool
AosActImportTester::runAction()//const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_r(mAction, false);
	aos_assert_r(mRundata->getSiteid() > 0, false);

	AosActionObjPtr action = AosActionObj::createAction(mAction, mRundata);
	aos_assert_r(action, false);
	
	return action->run(mAction, mRundata);
}


bool
AosActImportTester::compareResults()
{
	OmnString rawkey;
	u64 rawdocid;
	u32 rawnum = mData.getNumEntries();
	u32 newnum = mKeys.size();
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
AosActImportTester::retrieveResults()
{
	AosQueryRsltPtr query_rslt = AosQueryRslt::getQueryRslt();
	query_rslt->setWithValues(true);
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_an);
	query_context->setStrValue("*");
	query_context->setReverse(false);
	query_context->setBlockSize(100000);
	OmnSleep(5);	
	aos_assert_r(AosQueryColumn(mResultIILName, query_rslt, 0, query_context, mRundata), false);

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
	return true;
}

bool
AosActImportTester::createUpn()
{
	mNumUpns = 5;
	// mNumUpns = OmnRandom::intByRange(
	// 		100, 500, 50,
	// 		501, 5000, 20,
	// 		5001, 100000, 5);

	mUpns.clear();
	for (int i=0; i<mNumUpns; i++)
	{
		mUpns.push_back(OmnRandom::cellPhoneNumber(15, 13, true, true, ' '));
	}
	return true;
}

bool
AosActImportTester::createCpnPrefix()
{
	// mNumCpns = OmnRandom::intByRange(
	// 		10, 50, 50,
	// 		51, 500, 20,
	// 		501, 10000, 5);

	mCompetitorPrefix.clear();
	mCompetitorPrefix.push_back("13611111");
	mCompetitorPrefix.push_back("13622222");
	mCompetitorPrefix.push_back("13633333");
	mCompetitorPrefix.push_back("13644444");
	return true;
}

OmnString
AosActImportTester::pickCallDate()
{
	// YYYYMMDDHHMMSS
	OmnString date = "20120";
	int month = rand() % 6;
	month++;
	date << month;
	int dd = ((rand() % 30) + 1);
	if (dd >= 1 && dd <= 9) 
	{
		date << "0" << dd;
	}
	else
	{
		date << dd;
	}

	date << "112640";
	return date;
}

bool
AosActImportTester::createVoiceFile()
{
	// It creates a number of records:
	// 	random-string + cpn + rs + upn + call-duration + ...

	mNumVoiceCDRs = OmnRandom::intByRange(
			100, 500, 50,
			501, 5000, 20,
			5001, 100000, 5);

	mNumVoiceCDRs = 20;
	OmnString results;
	OmnString records;

	AosStrU64Array data(false);
	for (int i=0; i<mNumVoiceCDRs; i++)
	{
		int cpn_idx = rand() % mNumCpns;
		int upn_idx = rand() % mNumUpns;
		int ccity_idx = rand() % mNumCcity;
		OmnString call_date = pickCallDate();
		OmnString duration = OmnRandom::digitStr(6, 6);
		results << "gg" << OmnRandom::nextLetterDigitStr(39, 39, true)
			<< mUpns[upn_idx] 				// 15
			<< mCpns[cpn_idx]				// 24
			<< call_date					// 14
			<< duration						// 6
			<< OmnRandom::nextLetterDigitStr(181, 181, true)
			<< mCcity[ccity_idx]			// 4
			<< OmnRandom::nextLetterDigitStr(460, 460, true)
			<< "nnnnnnn";
		u64 call_duration = atoll(duration.data());
		data.addValue(mCpns[cpn_idx], call_duration, mRundata);
		records << mCpns[cpn_idx] << ",    "  << duration <<  "\n";
	}

	OmnFile f("/home/felicia/data/voice.txt", OmnFile::eCreate);
	f.put(0, results.data(), results.length(), true);

	OmnFile ff("/home/felicia/data/voice_rawrecords.txt", OmnFile::eCreate);
	ff.put(0, records.data(), records.length(), true);

	records = "";
	data.sort();
	
	mData.clear();
	u32 index = 0;
	filterNum(data, index);
	OmnString phonenum = data.getValue(index);
	u64 docid, sum = data.getU64Value(index);
	index++;
	OmnString number;
	for (; index < data.getNumEntries(); index++)
	{
		filterNum(data, index);
		if (index >= data.getNumEntries()) break;
		number = data.getValue(index);
		docid = data.getU64Value(index);
		if (strncmp(phonenum.data(), number.data(), 24) == 0)
		{
			sum += docid;
		}
		else
		{
			records << phonenum << ",    " << sum << "\n";
			mData.addValue(phonenum.data(), sum, mRundata);
			phonenum = number;
			sum = docid;
		}
	}

	records << phonenum << ",    " << sum << "\n";
	mData.addValue(phonenum.data(), sum, mRundata);

	
	OmnFile fff("/home/felicia/data/voice_records.txt", OmnFile::eCreate);
	fff.put(0, records.data(), records.length(), true);

	return true;
}

void
AosActImportTester::filterNum(AosStrU64Array &data, u32 index)
{
	OmnString phonenum, prefix;
	for(; index < data.getNumEntries(); index++)
	{
		phonenum = data.getValue(index);
		prefix = phonenum.subString(0,3);
		if (strncmp(prefix.data(), "130", 3) == 0 ||
			strncmp(prefix.data(), "131", 3) == 0 ||
			strncmp(prefix.data(), "132", 3) == 0 ||
			strncmp(prefix.data(), "155", 3) == 0 ||
			strncmp(prefix.data(), "156", 3) == 0 ||
			strncmp(prefix.data(), "185", 3) == 0 ||
			strncmp(prefix.data(), "186", 3) == 0)
		{
			continue;
		}

		break;
	}
}

bool
AosActImportTester::createCcity()
{
	mNumCcity = 5;

	mCcity[0] = "0010";
	mCcity[1] = "0020";
	mCcity[2] = "0030";
	mCcity[3] = "0040";
	mCcity[4] = "0050";
	return true;
}

bool
AosActImportTester::createCpn()
{
	createCpnPrefix();

	mNumCpns = 5;
	// mNumCpns = OmnRandom::intByRange(
	// 		10, 50, 50,
	// 		51, 500, 20,
	// 		501, 10000, 5);

	mCpns.clear();
	OmnString cpn;
	u32 num_prefix = mCompetitorPrefix.size();
	for (int i=0; i<mNumCpns; i++)
	{
		cpn = "";
		if (rand() % 4 == 0)
		{
			// Randomly generate a cpn
			mCpns.push_back(OmnRandom::cellPhoneNumber(24, 11, false, true, ' '));
		}
		else
		{
			// Pick a prefix
			cpn = mCompetitorPrefix[rand() % num_prefix];
			cpn << OmnRandom::cellPhoneNumber(16, 3, false, true, ' ');
			mCpns.push_back(cpn);
		}
	}
	return true;
}


bool
AosActImportTester::createData()
{
	createUpn();
	createCpn();
	createCcity();
	createVoiceFile();
	return true;
}


