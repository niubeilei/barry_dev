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
#ifndef Aos_Actions_Testers_ActImportTester_h
#define Aos_Actions_Testers_ActImportTester_h

#include "Actions/Ptrs.h"
#include "IILClient/Ptrs.h"
#include "IILAssembler/Ptrs.h"
#include "IILUtil/IILMatchType.h"
#include "IILUtil/IILEntrySelType.h"
#include "IILUtil/IILValueType.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"
#include <vector>
using namespace std;


class AosActImportTester : virtual public OmnTestPkg
{
private:
	AosRundataPtr			mRundata;
	AosTaskObjPtr			mTask;
	AosIILAssemblerPtr	mIILAssembler;
	vector<OmnString>		mKeys;
	vector<u64>				mDocids;
	OmnString				mResultIILName;
	AosStrU64Array			mData;
	int						mNumVoiceCDRs;
	int						mNumUpns;
	int						mNumCpns;
	int						mNumCcity;
	vector<OmnString>		mUpns;
	vector<OmnString>		mCpns;
	vector<OmnString>		mUnicomRegionCodes;
	vector<OmnString>		mUnicomCityCodes;
	vector<OmnString>		mCompetitorCityCodes;
	vector<OmnString>		mCompetitorPrefix;
	AosXmlTagPtr 			mAction;
	OmnString				mCcity[100];

public:
	AosActImportTester();
	~AosActImportTester();

	virtual bool start();

private:
	bool		basicTest();
	bool		appendEntry(const OmnString &str, const u64 &docid);
	bool		sendData();
	bool		createData();
	bool 		createUpn();
	bool 		createCpn();
	bool		clearData();
	bool 		createCcity();
	OmnString 	pickCallDate();
	bool 		createCpnPrefix();
	bool 		createVoiceFile();
	bool		createLocalResults();
	bool		runAction();
	bool		retrieveResults();
	bool		compareResults();
	void   		filterNum(AosStrU64Array &data, u32 index);
	bool		config();
};
#endif
