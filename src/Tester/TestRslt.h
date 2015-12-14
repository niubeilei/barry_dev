////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestRslt.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_TestRslt_h
#define OMN_Tester_TestRslt_h

#include "Database/Ptrs.h"
#include "Obj/ObjDb.h"
#include "Porting/GetTime.h"
#include "Util/String.h"


class OmnTestRslt : public OmnDbObj
{
private:
	enum
	{
		eStatus,
		eFile,
		eLine,
		eMsg
	};

	OmnString		mTestId;
	OmnString		mSuiteName;
	OmnString		mPackageName;
	int				mDuration;
	int				mSuccessTcs;
	int				mFailedTcs;

public:
	OmnTestRslt(const OmnString &testId,
				const OmnString &suiteName,
				const OmnString &packageName, 
				const int duration,
				const int successTcs,
				const int failedTcs)
		:
	OmnObject(OmnClassId::eOmnTestRslt),
	OmnDbObj(OmnClassId::eOmnTestRslt, OmnStoreId::eTest),
	mTestId(testId),
	mSuiteName(suiteName),
	mPackageName(packageName),
	mDuration(duration),
	mSuccessTcs(successTcs),
	mFailedTcs(failedTcs)
	{
	}

	~OmnTestRslt() {}

	// 
	// OmnDbObj interface
	//
    virtual OmnRslt    serializeFromDb();

	OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);

private:
	OmnString	insertStmt() const;
	OmnString	removeStmt() const;
	OmnString	sqlAllFields() const;
};

#endif

