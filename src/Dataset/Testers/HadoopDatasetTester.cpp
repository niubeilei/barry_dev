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
#if 0
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


OmnString sgTypes[] = {"ac", "vc", "sg"};// "vb", "wu", "bb", "bj", "bi", "wu"};
int sgTypesLen[] = {10, 15, 10};//, 25, 30, 20, 20, 35, 40};
OmnString sgField1[] = {"aaaa", "bbbb", "cccc"};//, "dddd", "eeee", "ffff", "gggg", "abcd", "efgh"};
OmnString sgField2[] = {"1111", "2222", "3333"};//, "4444", "5555", "1234", "5678", "2345", "6666"};

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
	int tries = 5;
	while(tries--)
	{
		bool rslt = createFile();
		aos_assert_r(rslt, false);

		rslt = readRecords();
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosDatasetTester::config()
{
	//<testers>
	//  <dataset type="data_scanner" zky_objid="xx" num_entries="xx" zky_max_record_sets="xxx">
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
	//  	<data_scanner/>
	//  </dataset>
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
	
	return true;
}

bool
AosDatasetTester::createFile()
{
	// Create a file to read. It is fixed length.
	for (int m=0; m<mNumFiles; m++)
	{
		OmnString fname = createFileName(m);
		OmnFile file(fname.data(), OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(file.isGood(), false);
		for(int i=0; i<mNumRcds; i++)
		{
			OmnString results = createRecord();
			file.append(results.data(), results.length(), false);
		}
		file.flushFileContents();
	}
	return true;
}

OmnString
AosDatasetTester::createFileName(const int num)
{
	OmnString fname = AosGetBaseDirname();
	fname << "dataset_"
	      << num;
	return fname;
}


OmnString
AosDatasetTester::createRecord()
{
	// record format
	// 0~1, types[]
	// 2~5, field1
	// 6~9, field2
	// random 
	//
	// The record is:
	// 	dd222233332222_3333rrrrrrrrrrrrrrrrrrrrrrrr
	// 'dd' is type
	// '2222' is the value for field1
	// '3333' is the value for field2
	// 'rrrrrr...' is a random string
	
	OmnString record;
	int indx = rand() % (sizeof(sgTypes)/sizeof(OmnString));
	record << sgTypes[indx]
		   << sgField1[indx]
		   << sgField2[indx]
//		   << sgField1[indx] << "_" << sgField2[indx]
		   << OmnRandom::nextLetterDigitStr(sgTypesLen[indx], sgTypesLen[indx], true)
	       << mPattern;
	
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
	mDataset = AosDatasetObj::createDatasetStatic(mRundata, 0, mDatasetConf);
	aos_assert_r(mDataset, false);

	while (1)
	{
		AosRecordsetObjPtr recordset;
		bool rslt = mDataset->nextRecordset(mRundata, recordset);
		aos_assert_r(rslt, false);
		if (!recordset) break;
		
		while (1)
		{
			AosDataRecordObj * record = recordset->nextRecord(mRundata);
			if (!record) break;

			aos_assert_r(verifyRecord(record), false);
			aos_assert_r(procRecord(record), false);
		}
	}
	return true;
}


bool
AosDatasetTester::procRecord(const AosDataRecordObj *record)
{
	// This function creates an entry:
	// 	[offset, length] 
	// based on 'record'. 
	// u64 offset = record->getOffset();
	// u32 length = record->getLength();
	static u64 lsDocid = 1000;
	u64 offset = 10000;
	u32 length = 200;
	u64 docid = lsDocid++;

	char data[15];
	*(u64*)data = offset;
	*(u32*)&data[8] = length;

	AosValueRslt value;
	value.setKeyValue(data, 20, true, docid);
	bool rslt = mDocAssembler->appendEntry(value, rdata);
	aos_assert_r(rslt, false);
}


bool
AosDatasetTester::verifyRecord(AosDataRecordObj *record)
{
	AosValueRslt value;
	bool rslt = record->getFieldValue(0, value, mRundata);
	aos_assert_r(rslt, false);
	OmnString type = value.getValueStr1();
	aos_assert_r(type != "", false);

	map<OmnString, int>::iterator itr = mHasRecords.find(type);
	aos_assert_r(itr != mHasRecords.end(), false);

	int index = itr->second;

	rslt = record->getFieldValue(1, value, mRundata);
	aos_assert_r(rslt, false);
	OmnString field1 = value.getValueStr1();
	aos_assert_r(field1 == sgField1[index], false);

	rslt = record->getFieldValue(2, value, mRundata);
	aos_assert_r(rslt, false);
	OmnString field2 = value.getValueStr1();
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
	return true;
}
#endif
