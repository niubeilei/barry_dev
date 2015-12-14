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
#ifndef Aos_Actions_Testers_ActCreateDocByIILTester_h
#define Aos_Actions_Testers_ActCreateDocByIILTester_h

#include "Actions/Ptrs.h"
#include "IILClient/Ptrs.h"
#include "IILUtil/IILMatchType.h"
#include "IILUtil/IILEntrySelType.h"
#include "IILUtil/IILValueType.h"
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
#include <vector>
using namespace std;


class AosActCreateDocByIILTester : virtual public OmnTestPkg
{
private:
	typedef hash_map<OmnString, map<OmnString, OmnString>, Omn_Str_hash, compare_str> OmnRecordMap;
	typedef hash_map<OmnString, map<OmnString, OmnString>, Omn_Str_hash, compare_str>::iterator OmnRecordMapItr;
	vector< AosXmlTagPtr>			mIILInfo;
	vector<OmnString>				mCompetitorPrefix;
	vector<OmnString>				mKeys;
	map<OmnString, int>				mDistinctCpns;
	AosRundataPtr					mRundata;
	OmnString						mResultIILName;
	vector<u64>						mDocids;
	AosXmlTagPtr 					mAction;
	OmnString 						mKeyName;
	OmnRecordMap					mRecords;
	int								mNumCpns;
	bool							mFirstRun;
	static AosIILClientObjPtr		smIILClient;

public:
	AosActCreateDocByIILTester();
	~AosActCreateDocByIILTester();

	virtual bool start();

private:
	bool		basicTest();
	bool		createData();
	bool 		createCpn();
	bool		clearData();
	bool 		createCompetitorPrefix();
	bool		createLocalResults();
	bool		runAction();
	bool		retrieveResults();
	bool		retrieveResults1();
	bool		compareResults();
	bool		config();
};
#endif
