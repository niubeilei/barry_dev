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
// 06/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_Testers_ActUnicomTester_h
#define Aos_Actions_Testers_ActUnicomTester_h

#include "Actions/Ptrs.h"
#include "IILAssembler/Ptrs.h"
#include "IILClient/Ptrs.h"
#include "IILUtil/IILMatchType.h"
#include "IILUtil/IILEntrySelType.h"
#include "IILUtil/IILValueType.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"
#include <vector>
using namespace std;


class AosActUnicomTester : virtual public OmnTestPkg
{
private:
	struct Record
	{
		OmnString 	upn;
		OmnString	cpn;

	};

	typedef hash_map<const OmnString, Record, Omn_Str_hash, compare_str> RsltMap_t;
	typedef hash_map<const OmnString, Record, Omn_Str_hash, compare_str>::iterator RsltMapItr_t;

	AosRundataPtr			mRundata;
	AosTaskObjPtr			mTask;
	AosIILAssemblerPtr		mIILAssembler;
	vector<OmnString>		mKeys;
	vector<u64>				mDocids;
	OmnString				mResultIILName;
	int						mNumVoiceCDRs;
	vector<OmnString>		mUpns;
	vector<OmnString>		mCpns;
	vector<OmnString>		mUTowns;
	vector<OmnString>		mUCities;
	vector<OmnString>		mCCities;
	vector<OmnString>		mCPNPrefix;
	AosStr2U64_t			mUpnMap;
	RsltMap_t				mResults;	

public:
	AosActUnicomTester();
	~AosActUnicomTester();

	virtual bool start();

private:
	bool		basicTest();
	bool		appendEntry(const OmnString &str, const u64 &docid);
	bool		sendData();
	bool		createData();
	bool		clearData();
	bool		createLocalResults();
	bool		runAction();
	bool		retrieveResults();
	bool		compareResults();
	bool		config();
	bool 		createUpn();
	bool 		createCpn();
	bool 		createUTown();
	bool 		createUPNPrefixFile();
	bool 		createCpnPrefix();
	bool 		createCpnPrefixFile();
	bool 		createCpnCityCodes();
	bool 		createUCity();
	bool 		createCCity();
	OmnString 	pickCallDate();
	bool 		createVoiceFile();
	bool 		createShortMsgFile();
	bool		convertData();
};
#endif

