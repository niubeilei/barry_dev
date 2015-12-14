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
#include "Dataset/Testers/DatasetTester.h"

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
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/DatasetObj.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/ThreadPool.h"
#include <vector>

static int sgNumTypes = 3;
static OmnString sgTypes[] = {"ac", "vc", "sg"};//, "vb", "wu", "bb", "bj", "bi", "wu"};
static int sgTypesLen[] = {10, 15, 10};//, 25, 30, 20, 20, 35, 40};
static OmnString sgField1[] = {"aaaa", "bbbb", "cccc"};//, "dddd", "eeee", "ffff", "gggg", "abcd", "efgh"};
static OmnString sgField2[] = {"1111", "2222", "3333"};//, "4444", "5555", "1234", "5678", "2345", "6666"};
static OmnString sgErrorType = "bb";

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("dataset_test", __FILE__, __LINE__);

AosDatasetTester::AosDatasetTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mRundata->setSiteid(100);
	mRundata->setUserid(307);
	mName = "DatasetTester";
}


AosDatasetTester::~AosDatasetTester()
{
}



bool 
AosDatasetTester::start()
{
	cout << "Start Dataset Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosDatasetTester::basicTest()
{
	aos_assert_r(config(), false);

//	int tries = 1;
//	while(tries--)
//	{
	bool rslt = createFile();
	aos_assert_r(rslt, false);
	
	mDataset = AosCreateDataset(mRundata, mDatasetConf);
	aos_assert_r(mDataset, false);

	mDataset->sendStart(mRundata);

//	OmnThreadedObjPtr thisPtr(this, false);
	vector<OmnThrdShellProcPtr> runners;
	for (int i=0; i<mThrds; i++)
	{
		OmnThrdShellProcPtr runner = OmnNew readData(this);
		runners.push_back(runner);
//		OmnThreadPtr thread  = OmnNew OmnThread(thisPtr, "Thrd", i, true, true, __FILE__, __LINE__);
//		mThreads.push_back(thread);
//		thread->start();
	}

	sgThreadPool->procSync(runners);
OmnScreen << " ############ test finished ######### " << endl;
//	rslt = readRecords();
//	aos_assert_r(rslt, false);
//	}

	return true;
}

bool
AosDatasetTester::readData::run()
{
	return mCaller->readRecords();
}
/*
bool
AosDatasetTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	//aos_assert_r(basicTest(thread->getLogicId()/iilnums, thread->getLogicId()), false);
	bool rslt = readRecords();
	aos_assert_r(rslt, false);
	state = OmnThrdStatus::eStop;
	return true;
}
*/

bool
AosDatasetTester::config()
{
	//<testers>
	//  <mDataset type="data_scanner" zky_objid="xx" num_entries="xx" zky_max_record_sets="xxx">
	//  	<recordset recordset_size="xx"/>
	// 		<schema type="fixed_len" last_entry_with_nopattern="false" pattern="xxx" max_record_length="xx" skip_invalid_records="true">
	// 			<records>
	// 				<record record_len="xxx" record_name="xxx"/>
	// 				<record record_len="xxx" record_name="xxx"/>
	// 			</records>
	//      	<schema_picker start_pos="xxx" len="xxx">
	//  	        <schemas>
	//  				<schema cont="xx" shcema_name="xxx"/>
	//  				<schema cont="xx" shcema_name="xxx"/>
	//  			</schemas>
	//  		</schema_picker>
	//  	</schema>
	//  	<data_scanner start_pos="xx" read_block_size="xx">
	//  		<metadata physical_id="xx" charaset="xx">filename</metadata>
	//  	</data_scanner>
	//  </mDataset>
	//</testers>
	AosRundataPtr rdata = mRundata;
	AosXmlTagPtr app_conf = OmnApp::getAppConfig();
	aos_assert_r(app_conf, false);

	AosXmlTagPtr testers_conf = app_conf->getFirstChild("testers");
	aos_assert_r(testers_conf, false);

	mNumRcds = testers_conf->getAttrInt("num_records", eDftNumRcds);

	mNumFiles = testers_conf->getAttrInt("num_files", eDftNumFiles);

//	mFilePath = testers_conf->getAttrStr("file_path", "");
//	aos_assert_r(mFilePath != "", false);
	
	mPattern = testers_conf->getAttrStr("pattern", "\n\r");

	mDatasetConf = testers_conf->getFirstChild("dataset"); 
	aos_assert_rr(mDatasetConf, rdata, false);

	mRecordType = testers_conf->getAttrStr("record_type", "");
	aos_assert_r(mRecordType != "", false);

	mFilterType = testers_conf->getAttrStr("filter_type", "");

	mLastRcdWithNoPattern = false;
	
	mErrorWeight = testers_conf->getAttrInt("error_weight", 10);
	
	mThrds = testers_conf->getAttrInt("threads", 2);
	return true;
}

bool
AosDatasetTester::createFile()
{
	// Create a file to read. It is fixed length.
	//for (int m=0; m<mNumFiles; m++)
	//{
		//OmnString fname = createFileName(m);
		OmnString fname = "/home/ice/AOS/Data/dataset_file.txt";
		OmnFile file(fname.data(), OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(file.isGood(), false);
		for(int i=0; i<mNumRcds; i++)
		{
			OmnString results = "";
			if (mRecordType == "fixbin")
			{
				results = createFixBinRecord(i);
			}
			else if (mRecordType == "variable")
			{
				results = createVariableRecord(i);
			}
			aos_assert_r(results != "", false);
//OmnScreen << " @@@@@@ create record " << results << endl;
			file.append(results.data(), results.length(), false);
		}
		file.flushFileContents();
	//}
	return true;
}

OmnString
AosDatasetTester::createFileName(const int num)
{
	OmnString fname = AosGetBaseDirname();
	fname << "mDataset_"
	      << num;
	return fname;
}


OmnString
AosDatasetTester::createErrorVariableRecord()
{
	// This function creates an invalid variable length record. 
	// 1. Invalid pattern, incomplete pattern
	// 2. has sub pattern
	// 3. sgTypes error, the results will be not find schema
	// 4. field error
	
	OmnString record, type, field1, field2, field3, pattern;
	OmnString subpath = "";
	createErrorRecord(type, field1, field2, field3, pattern, subpath);
	record << type << "|";
	if (field1 != "")
	{
		record << field1 << "|";
	}
	record << field2 << "|"
		   << field3 
	       << pattern;
/*
	if (type == mFilterType)
	{
		mFilterRecords.push_back(record);
	}
	else if (subpath == "type")
	{
		mErrorRecords.push_back(record);	
	}
	else if (subpath == "field")
	{
		mInvalidRecords.push_back(record);
	}
*/
	return record;
}

OmnString
AosDatasetTester::createErrorFixBinRecord()
{
	// This function creates an invalid variable length record. 
	// 1. Invalid pattern, incomplete pattern
	// 2. has sub pattern
	// 3. sgTypes error, the results will be not find schema
	// 4. field error
	
	OmnString record, type, field1, field2, field3, pattern;
	OmnString subpath = "";
	createErrorRecord(type, field1, field2, field3, pattern, subpath);
	record << type;
	if (field1 == "")
	{
		record << field2 << "_" << field2;
	}
	else
	{
		record << field1;
	}
	record << field2
	       << field3
		   << pattern;
	
/*	
	if (subpath != "subpath")
	{
		mErrorRecords.push_back(record);	
//OmnScreen << " ###### the " << mErrorRecords.size() << " error : " << record << endl;
	}
	else if (type == mFilterType)
	{
		mFilterRecords.push_back(record);
	}
*/
	return record;

}


void
AosDatasetTester::createErrorRecord(
		OmnString &type,
		OmnString &field1,
		OmnString &field2,
		OmnString &field3,
		OmnString &pattern,
		OmnString &subpath)
{
	int indx = rand() % sgNumTypes;
	
	type = sgTypes[indx];
	field1 = sgField1[indx];
	field2 = sgField2[indx];
	field3 = OmnRandom::nextLetterDigitStr(sgTypesLen[indx], sgTypesLen[indx], true);
	pattern = mPattern;
	map<OmnString, int>::iterator itr = mHasRecords.find(sgTypes[indx]);
	if (itr == mHasRecords.end())
	{
		mHasRecords[sgTypes[indx]] = indx;
	}
	
	if (rand() % 100 < 25)
	{
		// error type
		type = sgErrorType;
		subpath = "type";
	}
	else if (rand() % 100 < 75)
	{
		// field error
		field1 = "";
		subpath = "field";
	}
	else
	{
		// sub pattern
		aos_assert(mPattern.length() > 1);
		int num = rand() % (mPattern.length()-1);
		if (num == 0) num++;
		if (sgTypesLen[indx] < num)
		{
			num = sgTypesLen[indx];
		}
		field3 = mPattern.subString(0, num);
		int rand_num = sgTypesLen[indx] - num;
		if (rand_num > 0)
		{
			field3 << OmnRandom::nextLetterDigitStr(rand_num, rand_num, true);
		}
		subpath = "subpath";
	}
}

OmnString
AosDatasetTester::createVariableRecord(const int idx)
{
	// It creates a new variable length record. The record format is:
	// 	0~1, types[]
	// 	2~5, field1
	// 	6~9, field2
	// 	random 
	// 	pattern(line separator)
	//
	// The record is:
	// 	dd222233332222_3333rrrrrrrrrrrrrrrrrrrrrrrr
	// 'dd' is type
	// '2222' is the value for field1
	// '3333' is the value for field2
	// 'rrrrrr...' is a random string
	
//	if (rand()% mErrorWeight == 2)
//	{
//		return createErrorVariableRecord();
//	}

	OmnString record;
	int indx = 0;//rand() % sgNumTypes;
	record << sgTypes[indx] << "|"
		   << sgField1[indx] << "|" 
		   << sgField2[indx] << "|"
		   << OmnRandom::nextLetterDigitStr(sgTypesLen[indx], sgTypesLen[indx], true)
	       << mPattern;
	/*
	if (idx == mNumRcds-1)
	{
		// the last record wheather to has pattern
		if (rand()%100 >= 20)
		{
			record << mPattern;
		}
		else
		{
			mLastRcdWithNoPattern = true;
		}
	}
	else
	{
		record << mPattern;
	}
	*/
/*
	if(sgTypes[indx] == mFilterType)
	{
		mFilterRecords.push_back(record);
	}
*/
	map<OmnString, int>::iterator itr = mHasRecords.find(sgTypes[indx]);
	if (itr == mHasRecords.end())
	{
		mHasRecords[sgTypes[indx]] = indx;
	}
	return record;
}

OmnString
AosDatasetTester::createFixBinRecord(const int idx)
{
	// It creates a new variable length record. The record format is:
	// 	0~1, types[]
	// 	2~5, field1
	// 	6~9, field2
	// 	random 
	// 	pattern(line separator)
	//
	// The record is:
	// 	dd222233332222_3333rrrrrrrrrrrrrrrrrrrrrrrr
	// 'dd' is type
	// '2222' is the value for field1
	// '3333' is the value for field2
	// 'rrrrrr...' is a random string
	
//	if (rand()%mErrorWeight== 1)
//	{
//		return createErrorFixBinRecord();
//	}

	OmnString record;
	int indx = 0;//rand() % sgNumTypes;
	record << sgTypes[indx] 
		   << sgField1[indx]  
		   << sgField2[indx] 
		   << OmnRandom::nextLetterDigitStr(sgTypesLen[indx], sgTypesLen[indx], true);
//	       << mPattern;
	
	if (idx == mNumRcds-1)
	{
		// the last record wheather to has pattern
		if (rand()%100 >= 20)
		{
			record << mPattern;
		}
		else
		{
			mLastRcdWithNoPattern = true;
		}
	}
	else
	{
		record << mPattern;
	}
/*
	if(sgTypes[indx] == mFilterType)
	{
		mFilterRecords.push_back(record);
	}
*/
	map<OmnString, int>::iterator itr = mHasRecords.find(sgTypes[indx]);
	if (itr == mHasRecords.end())
	{
		mHasRecords[sgTypes[indx]] = indx;
	}
	return record;
}


bool
AosDatasetTester::readRecords()
{
	//mDataset = AosDatasetObj::createDatasetStatic(mRundata, 0, mDatasetConf);
	//aos_assert_r(mDataset, false);

	int num_rcdset = 0;
	AosRecordsetObjPtr recordset;
	while (1)
	{
		bool rslt = mDataset->nextRecordset(mRundata, recordset);
		aos_assert_r(rslt, false);
		if (!recordset) break;
		if (recordset->size() <= 0) break;
		
		while (1)
		{
			AosDataRecordObj * record;
			bool rslt = recordset->nextRecord(mRundata, record);
			aos_assert_r(rslt, false);
			if (!record) break;

			if (mRecordType == "fixbin")
			{
				aos_assert_r(verifyFixBinRecord(record), false);
			}
			else if (mRecordType == "variable")
			{
				aos_assert_r(verifyVariableRecord(record), false);
			}
		}
		num_rcdset++;
		//OmnScreen << " @@@@@@ the " << num_rcdset << " recordset " << endl;
	}

	aos_assert_r(checkErrorRcds(), false);
	return true;
}

bool
AosDatasetTester::checkErrorRcds()
{
	/*
	vector<OmnString> error_rcds = mDataset->getErrorRecords();
	aos_assert_r(mErrorRecords.size() == error_rcds.size(), false);

	for(int i=0; i<mErrorRecords.size(); i++)
	{
		aos_assert_r(mErrorRecords[i] == error_rcds[i], false);
	}

	vector<OmnString> filter_rcds = mDataset->getFilterRecords();
	aos_assert_r(mFilterRecords.size() == filter_rcds.size(), false);
	for(int i=0; i<mFilterRecords.size(); i++)
	{
		OmnString rcd_str = filter_rcds[i];
		if (mRecordType == "variable" && mFilterRecords[i].length() > rcd_str.length())
		{
			rcd_str << mPattern;
		}
		aos_assert_r(mFilterRecords[i] == rcd_str, false);
	}
	
	vector<OmnString> invalid_rcds = mDataset->getInvalidRecords();
	aos_assert_r(mInvalidRecords.size() == invalid_rcds.size(), false);
	for(int i=0; i<mInvalidRecords.size(); i++)
	{
		OmnString rcd_str = invalid_rcds[i];
		if (mRecordType == "variable" && mInvalidRecords[i].length() > rcd_str.length())
		{
			rcd_str << mPattern;
		}
		aos_assert_r(mInvalidRecords[i] == rcd_str, false);
	}
	*/
	return true;
}

bool
AosDatasetTester::verifyVariableRecord(AosDataRecordObj *record)
{
	AosValueRslt value;
	bool rslt = record->getFieldValue(0, value, mRundata);
//	if (!rslt)
//	{
//		mDataset->addInvalidContents(mRundata, record);
//		return true;
//	}
	aos_assert_r(rslt, false);
	OmnString type = value.getValueStr1();
	aos_assert_r(type != "", false);

	map<OmnString, int>::iterator itr = mHasRecords.find(type);
	aos_assert_r(itr != mHasRecords.end(), false);

	int index = itr->second;

	rslt = record->getFieldValue(1, value, mRundata);
///	if (!rslt)
//	{
//		mDataset->addInvalidContents(mRundata, record);
//		return true;
//	}
	aos_assert_r(rslt, false);
	OmnString field1 = value.getValueStr1();
//	aos_assert_r(field1.length() == sgField1[index].length(), false);
//	aos_assert_r(field1 == sgField1[index], false);
	if (field1 != sgField1[index])
	{
		mDataset->addInvalidContents(mRundata, record);
		return true;
	}

	rslt = record->getFieldValue(2, value, mRundata);
//	if (!rslt)
//	{
//		mDataset->addInvalidContents(mRundata, record);
//		return true;
//	}
	aos_assert_r(rslt, false);
	OmnString field2 = value.getValueStr1();
//	aos_assert_r(field2.length() == sgField2[index].length(), false);
	aos_assert_r(field2 == sgField2[index], false);
//	if (field2 != sgField2[index])
//	{
//		mDataset->addInvalidContents(mRundata, record);
//		return true;
//	}

	rslt = record->getFieldValue(3, value, mRundata);
//	if (!rslt)
//	{
//		mDataset->addInvalidContents(mRundata, record);
//		return true;
//	}
	aos_assert_r(rslt, false);
	OmnString field3 = value.getValueStr1();
//	if (field3.length() != sgTypesLen[index])
//	{
//		mDataset->addInvalidContents(mRundata, record);
//		return true;
//	}
	aos_assert_r(field3.length() == sgTypesLen[index], false);

//	rslt = record->getFieldValue(4, value, mRundata);
//	aos_assert_r(rslt, false);
//	OmnString field4 = value.getValueStr1();
//	aos_assert_r(field4 == mPattern, false);

//	int rcdlen = record->getRecordLen();
//	aos_assert_r(rcdlen == sgTypesLen[index]+10+mPattern.length(), false);

//	OmnString str = type;
//	str << field1 << field2 << field3 << field4;
	//str << field1 << "|"  << field2 << "|" << field3 << mPattern;
//OmnScreen << " @@@@@@  verify record : " << str << endl;
	return true;
}


bool
AosDatasetTester::verifyFixBinRecord(AosDataRecordObj *record)
{
	AosValueRslt value;
	bool rslt = record->getFieldValue(0, value, mRundata);
//	if (!rslt)
//	{
//		mDataset->addInvalidContents(mRundata, record);
//		mInvalidRecord++;
//	}
	aos_assert_r(rslt, false);
	OmnString type = value.getValueStr1();
	aos_assert_r(type != "", false);

	map<OmnString, int>::iterator itr = mHasRecords.find(type);
	aos_assert_r(itr != mHasRecords.end(), false);

	int index = itr->second;

	rslt = record->getFieldValue(1, value, mRundata);
	aos_assert_r(rslt, false);
	OmnString field1 = value.getValueStr1();
//	aos_assert_r(field1.length() == sgField1[index].length(), false);
	aos_assert_r(field1 == sgField1[index], false);

	rslt = record->getFieldValue(2, value, mRundata);
	aos_assert_r(rslt, false);
	OmnString field2 = value.getValueStr1();
//	aos_assert_r(field2.length() == sgField2[index].length(), false);
	aos_assert_r(field2 == sgField2[index], false);

	rslt = record->getFieldValue(3, value, mRundata);
	aos_assert_r(rslt, false);
	OmnString field3 = value.getValueStr1();
	aos_assert_r(field3.length() == sgTypesLen[index], false);

	rslt = record->getFieldValue(4, value, mRundata);
	aos_assert_r(rslt, false);
	OmnString field4 = value.getValueStr1();
	aos_assert_r(field4 == mPattern, false);

	int rcdlen = record->getRecordLen();
	aos_assert_r(rcdlen == sgTypesLen[index]+10+mPattern.length(), false);

//	OmnString str = type;
//	str << field1 << field2 << field3 << field4;
	//str << field1 << "|"  << field2 << "|" << field3 << mPattern;
//OmnScreen << " @@@@@@  verify record : " << str << endl;
	return true;
}
