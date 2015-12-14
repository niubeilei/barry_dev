////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestSuite.h
// Description:
//	A TestSuite is a collection of Tester.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_TestSuite_h
#define OMN_Tester_TestSuite_h

#include "Database/Ptrs.h"
#include "Obj/ObjDb.h"
#include "Tester/Ptrs.h"
#include "Util/ValList.h"
#include "Util/RCObjImp.h"


class OmnTestMgr;

#ifdef AOS_DB_SUPPORT
class OmnTestSuite : virtual public OmnDbObj
#else
class OmnTestSuite : virtual public OmnObject
#endif
{
	OmnDefineRCObject;

	enum
	{
		eMaxFailed = 1000
	};

protected:
	OmnString					mTestId;
	OmnString					mSuiteName;
	OmnString					mDesc;
	OmnVList<OmnTestPkgPtr>		mPackages;
	int							mTotalTc;
	int							mTotalFailed;

public:
	OmnTestSuite(const OmnString &suiteId, 
				 const OmnString &desc)
		:
	OmnObject(OmnClassId::eOmnTestSuite),
#ifdef AOS_DB_SUPPORT
	OmnDbObj(OmnClassId::eOmnTestSuite, OmnStoreId::eTest),
#endif
	mSuiteName(suiteId),
	mDesc(desc),
	mTotalTc(0),
	mTotalFailed(0)
	{
	}

	virtual ~OmnTestSuite()
	{
	}

	OmnClassId::E	getClassId() const {return OmnClassId::eOmnTestSuite;}

	// 
	// OmnObj Interface
	//
	virtual OmnString	toString() const;
	virtual int			objSize() const;

	// 
	// OmnDbObj interface
	//
#ifdef AOS_DB_SUPPORT
    virtual OmnRslt		serializeFromDb();
	virtual OmnDbObjPtr	clone() const;
	virtual OmnDbObjPtr	getNotInDbObj() const;
	virtual bool		hasSameObjId(const OmnDbObjPtr &rhs) const;
	OmnRslt	serializeFromRecord(const OmnDbRecordPtr &record);
#endif

	bool	addTestPkg(const OmnTestPkgPtr &package);
	void	setTestId(const OmnString &testId) {mTestId = testId;}
	virtual int		start(const OmnTestMgrPtr &testMgr);
	OmnString		getSuiteName() const {return mSuiteName;}

protected:
#ifdef AOS_DB_SUPPORT
	OmnString	insertStmt() const;
	OmnString	removeStmt() const;
	OmnString	updateStmt() const;
	OmnString	existStmt() const;
	OmnRslt		insertAllSuiteEntries() const;
	OmnString	sqlAllFields() const;
#endif
};

#endif
