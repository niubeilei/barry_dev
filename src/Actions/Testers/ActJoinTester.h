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
#ifndef Aos_Actions_Testers_ActJoinTester_h
#define Aos_Actions_Testers_ActJoinTester_h

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


class AosActJoinTester : virtual public OmnTestPkg
{
	typedef map<OmnString, u64> OriMAP;
	typedef map<OmnString, u64> MAP;
private:
	enum E
	{
		eTable1,
		eTable2
	};
	
	enum
	{
		eKeyMax = 256
	};

private:
	AosRundataPtr			mRundata;
	AosTaskObjPtr			mTask;
	AosIILAssemblerPtr		mTable1IILAssembler;
	AosIILAssemblerPtr		mTable2IILAssembler;
	vector<OmnString>		mKeys;
	vector<u64>				mDocids;
	OmnString				mResultIILName;
	OmnDataStorePtr 		mStore;

public:
	AosActJoinTester();
	~AosActJoinTester();

	virtual bool start();

private:
	bool		basicTest();
	bool		appendEntry(const E &type, const OmnString &str, const u64 &docid);
	bool		insertData(const E &type, const OmnString &str, const u64 &docid);
	bool		sendData(const E &type);
	bool		createData(const bool big, const int &num_entry);
	bool		createData();
	bool		clearData();
	bool		createLocalResults();
	bool		runAction();
	bool		retrieveResults();
	bool		compareResults();
	bool		config();
};
#endif
