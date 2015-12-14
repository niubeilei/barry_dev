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
// 06/14/2013 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataScanner_Testers_DirScannerTester_h
#define Aos_DataScanner_Testers_DirScannerTester_h

#include "Rundata/Rundata.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"
#include "DataScanner/DataScanner.h"
#include "Thread/ThrdShellProc.h"
#include <vector>
#include <map>
using namespace std;


class AosDirScannerTester : virtual public OmnTestPkg
{
	OmnDefineRCObject;
	class createBinaryDocThrd : public OmnThrdShellProc
	{
		friend class AosDirScannerTester;
		OmnDefineRCObject;
	private:
		AosDirScannerTester*			mTester;
		AosRundataPtr					mRundata;
	public:
		createBinaryDocThrd(
				AosDirScannerTester* tester,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("createBinaryDocThrd"),
		mTester(tester),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool    run();          
		virtual bool    procFinished(); 
	};


private:
	AosDataScannerObjPtr	mScanner;
	vector<OmnString>		mDirNames;
	OmnString				mSep;
	int						mPhysicalId;
	bool					mRecursion;
	AosXmlTagPtr			mTemplateDoc;
	OmnString				mContainer;
	bool					mIsFinished;
public:
	AosDirScannerTester();
	~AosDirScannerTester();

	virtual bool start();
	bool isFinished();
	bool setFinished();

private:
	bool			basicTest();
	bool			config(const AosRundataPtr &rdata);
	bool			createDirScanner(const AosRundataPtr &data);
	bool			test(const AosRundataPtr &rdata);
	bool			check(const AosRundataPtr &rdata);
	bool			createBinaryDoc(const AosRundataPtr &rdata);
	bool			puttingTogether(AosBuffPtr &buff, map<OmnString, u64> &docids, const AosRundataPtr &rdata);
	map<OmnString, u64>		getBinaryDocids(const OmnString &source_name, const AosRundataPtr &rdata);
	AosXmlTagPtr	queryBinaryDoc(const int startIdx, const OmnString &source_name, const AosRundataPtr &rdata);
};
#endif
