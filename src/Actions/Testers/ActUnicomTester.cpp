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
// 06/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/Testers/ActUnicomTester.h"

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
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosActUnicomTester::AosActUnicomTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mName = "ActUnicomTester";
}


AosActUnicomTester::~AosActUnicomTester()
{
}


bool 
AosActUnicomTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	// basicTest();
	convertData();
	return true;
}


bool
AosActUnicomTester::convertData()
{
	OmnFile file1("UpnPrefix.txt", OmnFile::eReadOnly);
	aos_assert_r(file1.isGood(), false);
	u64 length = file1.getLength();
	char *buff = OmnNew char[length+10];
	bool rslt = file1.readToBuff(0, length, buff);
	aos_assert_r(rslt, false);
	return true;
}

#if 0
bool 
AosActUnicomTester::basicTest()
{
	// aos_assert_r(config(), false);
	while (1)
	{
		clearData();
		createData();
		createLocalResults();
		runAction();
		retrieveResults();
		compareResults();
		return true;
	}
	return true;
}


bool
AosActUnicomTester::config()
{
	AosXmlTagPtr app_conf = OmnApp::getAppConfig();
	aos_assert_r(app_conf, false);
	AosXmlTagPtr conf = app_conf->getFirstChild("tester_conf");
	aos_assert_r(conf, false);

	// CreateJob
	AosXmlTagPtr jobtag = app_conf->getFirstChild("job_conf");
	AosJobPtr job = AosJob::staticCreateJob(jobtag, mRundata);
	aos_assert_r(job, false);

	// Create a task
	AosXmlTagPtr task_def = conf->getFirstChild("task_conf");
	aos_assert_r(task_def, false);
	mTask = AosTask::getTask(job, task_def, mRundata);
	aos_assert_r(mTask, false);

	// Create IIL assember
	AosXmlTagPtr assembler_def = conf->getFirstChild("iil_assembler");
	aos_assert_r(assembler_def, false);
	mIILAssembler = AosIILTransAssembler::createIILAssembler(mTask, assembler_def, mRundata);
	aos_assert_r(mIILAssembler, false);

	mResultIILName = conf->getAttrStr("result_iilname");
	aos_assert_r(mResultIILName != "", false);

	return true;
}


bool
AosActUnicomTester::clearData()
{
	return true;
}


bool
AosActUnicomTester::appendVoiceRecord(Record &record)
{
	// It adds a voice record to its internal data. When this function
	// is called, it is guaranteed that its cpn is truly a cpn.

	// 1. Update the mNumCallsMap.
	OmnString key = record.upn;
	key << "$$" << record.cpn;
	AosStr2U64_t numcall_itr = mNumCalls.find(key);
	u64 num_calls = 0;
	if (numcall_itr == mNumCalls.end())
	{
		// It is not in the map yet.
		mNumCalls[key] = 1;
	}
	else
	{
		numcall_itr->second++;
	}
	num_calls = numcall_itr->second;
	return true;
}


bool
AosActUnicomTester::createResults()
{
	// For each cpn, it checks what calls it made. For all the calls it
	// made, it checks whether it is in the same city. If yes, whether
	// it is the max. If yes, it gets its utown.
	//
	// It then creates the following table:
	// 	upn
	// 	cpn
	// 	call_duration
	// 	num_short_messages
	// 	earlist-call-time-in-90-days
	// 	utown
	// 	status
	RsltMapItr_t rslt_itr = mResults.find(record.cpn);
	if (rslt_itr == mResults.end())
	{
		AosStr2U64Itr_t itr = mUpnMap.find(record.upn);
		aos_assert_r(itr != mUpnMap.end(), false);

		u64 docid = itr->second;
		record.ucity = docid >> 32;
		record.utown = docid;

		mResults[record.cpn] = record;
		return true;
	}
}


bool
AosActUnicomTester::runAction()
{
	OmnNotImplementedYet;
	return false;
}


bool
AosActUnicomTester::compareResults()
{
	OmnNotImplementedYet;
	return false;
}


bool
AosActUnicomTester::retrieveResults()
{
	AosQueryRsltPtr query_rslt = AosQueryRslt::getQueryRslt();
	query_rslt->setWithValues(true);
	AosQueryContextPtr query_context = AosQueryContext::getQueryContext();
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
AosActUnicomTester::pickUpnPrefix()
{
	int idx = rand() % sgUpnPrefix.size();
	return sgUpnPrefix[idx];
}


bool
AosActUnicomTester::createUpn()
{
	// UPNs are in the form:
	// 	86 + upn-prefix + digit string
	mNumUpns = OmnRandom::intByRange(
	 		100, 500, 50,
	 		501, 5000, 20,
	 		5001, 100000, 5);

	mUpns.clear();
	AosStr2U64Itr_t itr;
	for (int i=0; i<mNumUpns; i++)
	{
		OmnString upn = "86";
		upn << pickUpnPrefix()
			<< OmnRandom::digitStr(8, 8);
		itr = mUpnMap.find(upn);
		if (itr == mUpnMap.end())
		{
			u32 ucity = pickUCity();
			u32 utown = pickUTown();
			u64 docid = (ucity << 32) + utown;
			mUpnMap[upn] = docid;
			mUpns.push_back(upn);
		}
	}
	return true;
}


bool
AosActUnicomTester::createCpn()
{
	createCpnPrefix();

	// mNumCpns = OmnRandom::intByRange(
	// 		10, 50, 50,
	// 		51, 500, 20,
	// 		501, 10000, 5);

	int num_cpns = 5;
	mCpns.clear();
	OmnString cpn;
	u32 num_prefix = mCPNPrefix.size();
	for (int i=0; i<num_cpns; i++)
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
			cpn = mCPNPrefix[rand() % num_prefix];
			cpn << OmnRandom::cellPhoneNumber(16, 3, false, true, ' ');
			mCpns.push_back(cpn);
		}
	}
	return true;
}


bool
AosActUnicomTester::createCpnPrefix()
{
	// mNumCpns = OmnRandom::intByRange(
	// 		10, 50, 50,
	// 		51, 500, 20,
	// 		501, 10000, 5);

	mCPNPrefix.clear();
	mCPNPrefix.push_back("86136111");
	mCPNPrefix.push_back("86136222");
	mCPNPrefix.push_back("86136333");
	mCPNPrefix.push_back("86136444");
	return true;
}


bool
AosActUnicomTester::createUTown()
{
	mUTowns.push_back("11");
	mUTowns.push_back("22");
	return true;
}


bool
AosActUnicomTester::createUCity()
{
	mUCities.push_back("3333");
	mUCities.push_back("4444");
	mUCities.push_back("5555");
	return true;
}


bool
AosActUnicomTester::createCCity()
{
	mCCities.push_back("0010");
	mCCities.push_back("0020");
	mCCities.push_back("0030");
	mCCities.push_back("0040");
	mCCities.push_back("0050");
	return true;
}

bool
AosActUnicomTester::createData()
{
	// It creates two input files, one for void and one for short messages.
	createUpn();
	createCpn();
	createCCity();
	createUTown();
	createUCity();
	createCCity();
	createVoiceCDRs();
	createShortMsgFile();
	createUPNPrefixFile();
	createCpnPrefixFile();
	return true;
}


OmnString
AosActUnicomTester::pickCallDate()
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
AosActUnicomTester::createVoiceCDRs()
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

	Record record;
	for (int i=0; i<mNumVoiceCDRs; i++)
	{
		int cpn_idx = rand() % mCpns.size();
		int upn_idx = rand() % mUpns.size();
		int ccity_idx = rand() % mCCities.size();
		OmnString call_date = pickCallDate();
		OmnString duration = OmnRandom::digitStr(6, 6);
		results << "vc" << OmnRandom::nextLetterDigitStr(39, 39, true)
			<< mUpns[upn_idx] 				// 15
			<< mCpns[cpn_idx]				// 24
			<< call_date					// 14
			<< duration						// 6
			<< OmnRandom::nextLetterDigitStr(181, 181, true)
			<< mCCities[ccity_idx]			// 4
			<< OmnRandom::nextLetterDigitStr(460, 460, true)
			<< "nnnnnnn";
		records << mUpns[upn_idx] << ", " << mCpns[cpn_idx] << ", " << call_date
			<< ", " << duration << ", " << mCCities[ccity_idx] << "\n";

		record.type = 0;
		record.upn = mUpns[upn_idx];
		record.cpn = mCpns[cpn_idx];
		record.duration = atoi(duration.data());
		record.call_date = call_date;
		record.ccity = mCCities[ccity_idx];

		appendVoiceRecord(record);
	}

	OmnFile f("data/voice.txt", OmnFile::eCreate);
	f.put(0, results.data(), results.length(), true);

	OmnFile ff("data/voice_records.txt", OmnFile::eCreate);
	ff.put(0, records.data(), records.length(), true);

	return true;
}


bool
AosActUnicomTester::createShortMsgFile()
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

	for (int i=0; i<mNumVoiceCDRs; i++)
	{
		int cpn_idx = rand() % mCpns.size();
		int upn_idx = rand() % mUpns.size();
		int ccity_idx = rand() % mCCities.size();
		OmnString call_date = pickCallDate();
		results << "ss" << OmnRandom::nextLetterDigitStr(67, 67, true)
			<< mUpns[upn_idx] 				// 15
			<< mCpns[cpn_idx]				// 24
			<< call_date					// 14
			<< OmnRandom::nextLetterDigitStr(108, 108, true)
			<< "01"
			<< OmnRandom::nextLetterDigitStr(347, 347, true)
			<< "nnnnn";
		records << mUpns[upn_idx] << ", " << mCpns[cpn_idx] << ", " << call_date
			<< ", " << mCCities[ccity_idx] << "\n";
	}

	OmnFile f("data/shortmsg.txt", OmnFile::eCreate);
	f.put(0, results.data(), results.length(), true);

	OmnFile ff("data/shortmsg_records.txt", OmnFile::eCreate);
	ff.put(0, records.data(), records.length(), true);

	return true;
}


bool
AosActUnicomTester::createUPNPrefixFile()
{
	OmnString results;

	for (u32 i=0; i<mUpns.size(); i++)
	{
		int region_idx = rand() % mUTowns.size();
		int city_idx = rand() % mUCities.size();
		results 
			<< mUpns[i] 					// 15
			<< mUTowns[region_idx]			// 2
			<< mUCities[city_idx];			// 4
	}

	OmnFile f("data/unicom_prefix.txt", OmnFile::eCreate);
	f.put(0, results.data(), results.length(), true);

	return true;
}


bool
AosActUnicomTester::createCpnPrefixFile()
{
	OmnString results;
	OmnString records;

	for (u32 i=0; i<mCPNPrefix.size(); i++)
	{
		int city_idx = rand() % mCCities.size();
		results
			<< mCPNPrefix[i] 						// 8
			<< OmnRandom::digitStr(4, 4)			// 4		LONG_CODE
			<< mCCities[city_idx]					// 4		LONG_CODE_LOC
			<< "t"									// type (1)
			<< "20120609081232"						// Timestamp, 14
			<< "6666"								// LONG_CODE_NEW (4)
			<< "7777"								// LONG_CODE_LOC_NEW (4)
			<< "a"									// Type_New (1)
			<< "20120101000000"						// Begin Date	(14)
			<< "20121231000000"						// End Date	(14)
			<< "b"									// Time Type
			<< OmnRandom::nextLetterDigitStr(50, 50, true)		// Note
			<< "c";									// Audit flag

		records << mCPNPrefix[i] << ", " << mCCities[city_idx] << "\n";
	}

	OmnFile f("data/competitor_prefix.txt", OmnFile::eCreate);
	f.put(0, results.data(), results.length(), true);

	OmnFile ff("data/competitor_prefix_records.txt", OmnFile::eCreate);
	ff.put(0, records.data(), records.length(), true);

	return true;
}


bool
AosActUnicomTester::appendEntry(const OmnString &str, const u64 &docid)
{
	return mIILAssembler->appendEntry(str, docid, mRundata);
}


bool
AosActUnicomTester::sendData()
{
	return mIILAssembler->sendFinish(mRundata);
}

#endif
