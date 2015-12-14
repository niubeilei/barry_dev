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
#ifndef Aos_DataScanner_Testers_FileScannerTester_h
#define Aos_DataScanner_Testers_FileScannerTester_h

#include "Actions/Ptrs.h"
#include "DataScanner/Ptrs.h"
#include "IILClient/Ptrs.h"
//#include "IILUtil/IILMatchType.h"
#include "IILUtil/IILEntrySelType.h"
#include "SEInterfaces/DataScannerObj.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"
#include "DataScanner/DataScanner.h"
#include <vector>
using namespace std;


class AosFileScannerTester : virtual public OmnTestPkg
{
private:
	AosRundataPtr		mRundata;
	AosDataScannerObjPtr	mScanner;
	int					mNumVoiceCDRs;
	int					mNumUpns;
	int					mNumCpns;
	int					mNumCcity;


	vector<OmnString>	mUpns;
	vector<OmnString>	mCpns;
	vector<OmnString>	mCompetitorPrefix;

	OmnString			mCcity[100];
public:
	AosFileScannerTester();
	~AosFileScannerTester();

	virtual bool start();

private:
	bool		config();
	bool		basicTest();
	bool		clearData();
	bool		createData();
	bool		createLocalResults();
	bool		runAction();
	bool		retrieveResults();
	bool		compareResults();


	bool		createUpn();
	bool		createCpn();
	bool		createCcity();
	bool		createCpnPrefix();
	bool		createVoiceFile();
	bool		filterNum(const OmnString cpn);
	bool		addCallDuration(const OmnString &phonenum, const u64 &sum);
	OmnString	pickCallDate();

};
#endif
