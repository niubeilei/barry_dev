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
//   
//
// Modification History:
// 2013/06/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_DataProc_Tester_DataProcWordParserTester_h
#define Omn_DataProc_Tester_DataProcWordParserTester_h

#include "SEInterfaces/DataProcObj.h"
#include "Tester/TestPkg.h"
#include "DataProc/Ptrs.h"

#include "SEInterfaces/DataRecordObj.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Debug/Debug.h"
#include "Thread/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Tester/TestPkg.h"
#include <vector>
#include <map>

using namespace std;
class AosDataProcWordParserTester : public OmnTestPkg
{
	 struct RunTest : public OmnThrdShellProc                                                             
     {
		 OmnDefineRCObject;

		 int64_t								mTestNum;
		 AosDataProcObjPtr		 			mDataProc;
		 vector<AosDataRecordObjPtr>			mRcds;
		 vector<AosDataRecordObj*>			mInputRcds;
		 vector<AosDataRecordObj*>			mOutputRcds;
		 map<OmnString, vector<OmnString> >	mInputRcdValues;
		 AosDataProcWordParserTester *  	mCaller;
		 AosRundata*						mRundata;
		 AosXmlTagPtr						mConf;

		 RunTest(
				 AosDataProcWordParserTester* caller,
				 AosRundata * rdata,
				 const AosXmlTagPtr &conf)
		 :
		 OmnThrdShellProc("AosDataProcWordParserTester"), 
		 mCaller(caller),
		 mRundata(rdata),
		 mConf(conf)
		 {
		 }

		 ~RunTest(){}

		 bool run();
		 bool procFinished()
		 {
			 return true;
		 }
		 
		 bool procData();
		 bool init();

		 bool initRecord(
				 vector<AosDataRecordObj *> &rcds, 
				 const AosXmlTagPtr &conf);

		 bool setInputRcdValue(const AosXmlTagPtr &conf);

		 bool initDataProc(const AosXmlTagPtr &test_conf);


	 };

private:
	bool								mFinished;
	AosXmlTagPtr						mConf;
	AosRundataPtr 						mRundata;
	int64_t								mThrdNum;
	int64_t								mTestNum;
/*
	int									mNumRunners;
	AosDataProcObjPtr		 			mDataProc;
	vector<AosDataRecordObjPtr>			mRcds;
	vector<AosDataRecordObj*>			mInputRcds;
	vector<AosDataRecordObj*>			mOutputRcds;
	map<OmnString, vector<OmnString> >	mInputRcdValues;
*/

public:
	AosDataProcWordParserTester();
	~AosDataProcWordParserTester();

	virtual bool		start();

	// ThrdShellProc interface
	virtual bool		run(){};
	bool isFinished() const {return mFinished;}

private:
	bool config();
/*
	bool init();
	bool procData();

	bool initRecord(
			vector<AosDataRecordObj *> &rcds, 
			const AosXmlTagPtr &conf);

	bool setInputRcdValue(const AosXmlTagPtr &conf);

	bool initDataProc(const AosXmlTagPtr &test_conf);
*/
};
#endif

