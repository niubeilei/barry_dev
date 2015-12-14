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
// 2014/08/17 Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_BSON_Testers_BsonTester_h
#define Omn_BSON_Testers_BsonTester_h

#include "Rundata/Ptrs.h"
#include "BSON/Ptrs.h"
#include "Tester/TestPkg.h"
#include "BSON/BsonField.h"
#include "BSON/BSON.h"


class AosBsonTester : public OmnTestPkg
{
private:
	map<OmnString, int>								mStrNames;
	map<u32, int>									mU32Names;

	vector<AosBsonField::Type>						mTypes;
	vector<AosValueRslt>							mValues;
	AosBSONPtr										mRecord;

public:
	AosBsonTester();
	~AosBsonTester() {}
	virtual bool		start();


private:
	bool	basicTest();
	bool    createRecord(AosRundata *rdata);
	bool    verifyRecord(AosRundata *rdata);
	OmnString	myPrintableStr(int len, bool with_single_quote);
    bool createVerifySwap(AosRundata *rdata);
};

#endif

