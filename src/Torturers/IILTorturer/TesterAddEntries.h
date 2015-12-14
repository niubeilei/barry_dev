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
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Torturers_IILTorturer_TesterAddEntries_h
#define Aos_Torturers_IILTorturer_TesterAddEntries_h

#include "Rundata/Ptrs.h"
#include "SengTorUtil/SengTester.h"
#include "SengTorUtil/StIILCache.h"
#include "SengTorUtil/StIIL.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "SengTorUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosIILAddEntriesTester : virtual public AosSengTester
{
private:
	enum
	{
		eDftMaxNumIILs = 10000,
		eDftWeightAddEntryByName = 100,
		eDftWeightAddEntryByID = 50,
		eDftWeightCreateIIL = 200,
		eDftWeightCheckIIL = 200
	};

	enum Opr
	{
		eInvalidOpr,

		eAddEntryByName,
		eAddEntryByID,
		eCreateIIL,
		eCheckIIL
	};

	AosStIILCachePtr mIILCache;
	i64				 mMaxNumIILs;
	i64				 mWeightAddEntryByName;
	i64				 mWeightAddEntryByID;
	i64              mWeightCreateIIL;
	i64              mWeightCheckIIL;
	i64              mTotalWeights;

	vector<AosIILAddEntriesTester::Opr > mOperators;
	AosRundataPtr	 mRdata;


public:
	AosIILAddEntriesTester();
	AosIILAddEntriesTester(const AosIILAddEntriesTester &rhs);
	~AosIILAddEntriesTester();

	virtual bool test();
	virtual bool configTester(const AosXmlTagPtr &config);
	virtual AosSengTesterPtr clone();

private:
	bool 	init();
	bool 	calculateWeights();
	Opr 	pickOperation();
	bool 	addEntryByName(OmnString &iilname);
	bool 	addEntryByID(u64 &iilid);
	bool 	pickIILName(bool &str_iil);
	bool 	pickIILID(bool &str_iil);
	bool 	createIIL();
	bool 	checkIIL();
	bool 	login();
	bool 	checkLogin();
};

#endif
