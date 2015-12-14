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
#ifndef Omn_DataProc_Tester_DataProcComposeSingleTester_h
#define Omn_DataProc_Tester_DataProcComposeSingleTester_h

#include "SEInterfaces/DataProcObj.h"
#include "Tester/TestPkg.h"
#include "DataProc/Ptrs.h"

#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Debug/Debug.h"
#include "Thread/Ptrs.h"
#include <vector>
#include <map>

using namespace std;
class AosDataProcComposeSingleTester : public OmnTestPkg
{
private:
	int									mNumRunners;
	bool								mFinished;
	AosRundataPtr 						mRundata;
	AosDataProcObjPtr		 			mDataProc;
	vector<AosDataRecordObj*>			mInputRcds;
	vector<AosDataRecordObj*>			mOutputRcds;
	map<OmnString, vector<OmnString> >	mInputRcdValues;


public:
	AosDataProcComposeSingleTester();
	~AosDataProcComposeSingleTester();

	virtual bool		start();

	// ThrdShellProc interface
	virtual bool		run();
	bool isFinished() const {return mFinished;}

private:
	bool init();
	bool procData();

	bool initRecord(
			vector<AosDataRecordObj *> &rcds, 
			const AosXmlTagPtr &conf);

	bool setInputRcdValue(const AosXmlTagPtr &conf);

	bool initDataProc(const AosXmlTagPtr &test_conf);

};
#endif

