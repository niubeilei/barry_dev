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
// 06/12/2012 Created by Xu Lei
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCollector_Testers_DataCheckTester_h
#define Aos_DataCollector_Testers_DataCheckTester_h

//#include "IILUtil/IILMatchType.h"
//#include "IILUtil/IILEntrySelType.h"
//#include "IILUtil/IILValueType.h"
#include "DataTypes/DataColOpr.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ActionCaller.h"
#include "SEInterfaces/ActionObj.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <vector>
using namespace std;

class AosDataCheckTester : virtual public OmnTestPkg, public AosActionCaller 
{
	enum
	{
		eMaxBuffSize = 100000000
	};
private:
	AosRundataPtr               mRundata;
	AosCompareFunPtr			mComp;
	bool						mStable;
	OmnString					mIILName;
	OmnString					mDataColid;
	vector<u64>					mFileIds;
	int64_t 					mMinSize;
	int64_t						mMaxSize;
	int64_t						mBuffSize;
	AosDataColOpr::E			mOperator;
	OmnFilePtr					mFile;
	u64							mFileSize;
	AosBuffPtr					mFileBuff;
	OmnFilePtr					mProcFile;
	int							mCrtIdx;
	int							mCrtSeqno;
	u64							mJobDocid;
	int							mPerNum;
	AosDataScannerObjPtr		mScanner;
	char*						mPerCrt;

public:
	static int64_t							mProcTotal;
	static int64_t							mAppendTotal;
	static vector<AosActionObjPtr>			mActions;
	static OmnFilePtr						mCrtFile;
	static OmnString						mType;

public:
	AosDataCheckTester();
	~AosDataCheckTester();
	
	virtual bool start();

	virtual void callBack(
			const u64 &reqId, 
			const int64_t &expected_size, 
			const bool &finished);
private:
	bool 		config(const AosXmlTagPtr &app_conf);
	bool 		createIIL();
	void		deleteFiles(const AosRundataPtr &rdata);
	bool 		createFile();
	u32 		getVirtualId();
	bool		getFields(const AosXmlTagPtr &app_conf);
	bool		getFields();
	bool		checkResults(const AosXmlTagPtr &conf);
	bool		checkResults(const OmnString &fname);
};
#endif
