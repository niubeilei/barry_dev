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
// 05/44/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_Testers_DatasetTester_h
#define Aos_Dataset_Testers_DatasetTester_h

#include "SEInterfaces/DatasetObj.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"
#include "Dataset/Dataset.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"


class AosDatasetTester : virtual public OmnTestPkg//, public OmnThreadedObj
{

	enum
	{
		eDftNumRcds = 20,
		eDftNumFiles = 2
	};
private:
	AosRundataPtr			mRundata;
	AosXmlTagPtr 			mDatasetConf;
	AosDatasetObjPtr		mDataset;
	int 					mNumRcds;
	int 					mNumFiles;
	OmnString				mFilePath;
	OmnString				mPattern;
	map<OmnString, int> 	mHasRecords;
	OmnString				mRecordType;
	bool					mLastRcdWithNoPattern;
	int 					mErrorWeight;
	OmnString				mFilterType;
	int						mThrds;

	vector<OmnString>		mErrorRecords;
	vector<OmnString>		mFilterRecords;
	vector<OmnString>		mInvalidRecords;
public:
	AosDatasetTester();
	~AosDatasetTester();

	// thread interface
//	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
//	virtual bool    signal(const int threadLogicId){return true;}
//	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{return false;}
	
	virtual bool start();

	class readData: public OmnThrdShellProc       
	{
			friend class AosGicContainer;
			OmnDefineRCObject;

			AosDatasetTester *   mCaller;
	public:
			readData(AosDatasetTester * caller)
			:
			OmnThrdShellProc("readData"),
			mCaller(caller)
			{
			}

			virtual bool    run();          
			virtual bool    procFinished(){return true;}
	};
private:
	bool		config();
	bool		basicTest();

	bool		createFile();
	OmnString	createFileName(const int num);
	OmnString	createVariableRecord(const int indx);
	OmnString   createFixBinRecord(const int indx);
	OmnString	createErrorVariableRecord();
	OmnString	createErrorFixBinRecord();
	bool		readRecords();
	bool		verifyFixBinRecord(AosDataRecordObj *record);
	bool		verifyVariableRecord(AosDataRecordObj *record);
	void		createErrorRecord(
					OmnString &type,
					OmnString &field1,
					OmnString &field2,
					OmnString &field3,
					OmnString &pattern,
					OmnString &subpath);
	bool		checkErrorRcds();
};
#endif
