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
#include "DataScanner/Testers/FileScannerTester.h"

#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/IILName.h"
#include "IILClient/IILClient.h"
#include "DocClient/DocClient.h"
#include "IILUtil/IILId.h"
#include "IILClient/Ptrs.h"
#include "IILAssembler/IILAssembler.h"
#include "Porting/Sleep.h"
#include "Job/Job.h"
#include "QueryUtil/QueryContext.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/TaskAction.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

AosFileScannerTester::AosFileScannerTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mRundata->setSiteid(100);
	mRundata->setUserid(307);
	mName = "FileScannerTester";
}


AosFileScannerTester::~AosFileScannerTester()
{
}


bool 
AosFileScannerTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosFileScannerTester::basicTest()
{
	aos_assert_r(config(), false);
	//while(1)
	//{
		//clearData();
		//createData();
		//createLocalResults();
		//runAction();
		//compareResults();
	//}
	return true;
}


bool
AosFileScannerTester::config()
{
	//<testers>
	//	<scanner type="file">
	//		<fileinfo>
	//			<entry
	//				zky_physicalid="0"
	//				zky_readsize=""
	//				zky_record_length=""
	//				zky_max_records=""
	//				zky_fullname="~/FileScannerTestData/voice.txt"
	//				zky_startpos="0"
	//				zky_length="-1"/>
	//		</fileinfo>
	//	</scanner>
	//</testers>
	AosRundataPtr rdata = mRundata;
	AosXmlTagPtr app_conf = OmnApp::getAppConfig();
	aos_assert_r(app_conf, false);

	AosXmlTagPtr testers_conf = app_conf->getFirstChild("testers");
	aos_assert_r(testers_conf, false);
	AosXmlTagPtr scanner_conf = testers_conf->getFirstChild("scanner"); 
	mScanner = AosDataScannerObj::getDataScanner(scanner_conf, rdata);
	aos_assert_rr(mScanner, rdata, false);
	return true;
}


bool
AosFileScannerTester::clearData()
{
	return true;
}


bool
AosFileScannerTester::createLocalResults()
{
	return true;
}


bool
AosFileScannerTester::runAction()
{
	return true;
}


bool
AosFileScannerTester::compareResults()
{
	return true;
}

bool
AosFileScannerTester::retrieveResults()
{
	return true;
}

bool
AosFileScannerTester::createData()
{
	createUpn();
	createCpn();
	createCcity();
	createVoiceFile();
	return true;
}

bool
AosFileScannerTester::createUpn()
{
	mNumUpns = OmnRandom::intByRange(
			100, 500, 50,
			501, 5000, 20,
			5001, 100000, 5);

	mUpns.clear();
	for (int i=0; i<mNumUpns; i++)
	{
		mUpns.push_back(OmnRandom::cellPhoneNumber(15, 11, false, true, ' '));
	}
	return true;
}

bool
AosFileScannerTester::createCpn()
{
	createCpnPrefix();

	mNumCpns = OmnRandom::intByRange(
			10, 50, 50,
			51, 500, 20,
			501, 10000, 5);

	mCpns.clear();
	OmnString cpn;
	u32 num_prefix = mCompetitorPrefix.size();
	for (int i=0; i<mNumCpns; i++)
	{
		cpn = "";
		if (rand() % 4 == 0)
		{
			cpn = mCompetitorPrefix[rand() % num_prefix];
			cpn << OmnRandom::cellPhoneNumber(16, 3, false, true, ' ');
			mCpns.push_back(cpn);
		}
		else
		{
			mCpns.push_back(OmnRandom::cellPhoneNumber(24, 11, false, true, ' '));
		}
	}
	return true;
}

bool
AosFileScannerTester::createCpnPrefix()
{
	mCompetitorPrefix.clear();
	mCompetitorPrefix.push_back("13611111");
	mCompetitorPrefix.push_back("13622222");
	mCompetitorPrefix.push_back("13633333");
	mCompetitorPrefix.push_back("13644444");
	return true;
}

bool
AosFileScannerTester::createCcity()
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
AosFileScannerTester::createVoiceFile()
{
	mNumVoiceCDRs = OmnRandom::intByRange(
			100, 500, 50,
			501, 5000, 20,
			5001, 100000, 5);

	mNumVoiceCDRs = 200;
	OmnString results;
	OmnString records;
	OmnString cpn, upn, city;

	OmnString fn = "/home/jozhi/FileScannerTestData/voice.txt";
	OmnFile f(fn.data(), OmnFile::eWriteCreate);
	for (int i=0; i<mNumVoiceCDRs; i++)
	{
		int cpn_idx = rand() % mNumCpns;
		cpn = mCpns[cpn_idx];
		int upn_idx = rand() % mNumUpns;
		upn = mUpns[upn_idx];
		int ccity_idx = rand() % mNumCcity;
		city = mCcity[ccity_idx];
		OmnString call_date = pickCallDate();
		OmnString duration = OmnRandom::digitStr(6, 6);
		results << "gg" << OmnRandom::nextLetterDigitStr(39, 39, true)
				<< upn
				<< cpn
				<< call_date
				<< duration
				<< OmnRandom::nextLetterDigitStr(181, 181, true)
				<< city
				<< OmnRandom::nextLetterDigitStr(460, 460, true)
				<< "nnnnnnn";
		f.append(results.data(), results.length(), false);
	}
	f.flushFileContents();
	return true;
}

bool
AosFileScannerTester::filterNum(const OmnString cpn)
{
	OmnString prefix = cpn.subString(0, 3);
	if (strncmp(prefix.data(), "130", 5) == 0 ||
			strncmp(prefix.data(), "131", 5) == 0 ||
			strncmp(prefix.data(), "132", 5) == 0 ||
			strncmp(prefix.data(), "155", 5) == 0 ||
			strncmp(prefix.data(), "156", 5) == 0 ||
			strncmp(prefix.data(), "185", 5) == 0)
	{
		return false;
	}
	return true;
}

OmnString
AosFileScannerTester::pickCallDate()
{
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
