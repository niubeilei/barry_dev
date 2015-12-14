////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_TestMgr1_h
#define OMN_Tester_TestMgr1_h

#include "Obj/ObjDb.h"
#include "Tester/Ptrs.h"
#include "Tester/TestSuite.h"
#include "Util/String.h"
#include "Util/ValList.h"

class OmnTestcase;

#ifdef AOS_DB_SUPPORT
class OmnTestMgr : public OmnDbObj
#else
class OmnTestMgr : public OmnObject
#endif
{
	OmnDefineRCObject;
public:
	enum SeedType
	{
		eNoInit,
		eRandom,
		eSpecify
	};
private:
	int				mTcs;
	int				mFailedTcs;
	OmnString		mTestId;
	OmnString		mDesc;
	OmnString		mInitiator;
	OmnString		mStartTime;
	OmnString		mEndTime;
	OmnTestSuitePtr	mCrtSuite;
	OmnTestPkgPtr	mCrtPkg;
	bool			mNeedToRecordGoodTcs;
	bool			mSaveToDb;

	OmnVList<OmnTestSuitePtr>	mSuites;
	
	SeedType		mSeedType;
	unsigned int	mOrigSeed;
	unsigned int	mCrtSeed;
	int				mTries;

public:
	OmnTestMgr(const OmnString &testId, 
			   const OmnString &desc, 
			   const OmnString &initiator);
	~OmnTestMgr();

	OmnClassId::E	getClassId() const {return OmnClassId::eOmnTestMgr;}

	// 
	// OmnObj Interface
	//
	virtual OmnString	toString() const;
	virtual int			objSize() const;

	// 
	// OmnDbObj Interface
	//
#ifdef AOS_DB_SUPPORT
	virtual OmnDbObjPtr	clone() const;
	virtual OmnRslt		serializeFromDb();
	virtual OmnDbObjPtr	getNotInDbObj() const;
	virtual bool		hasSameObjId(const OmnDbObjPtr &rhs) const;
	virtual OmnString	insertStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	existStmt() const;
	virtual OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);
#endif

	bool		start();
	bool		finish();
	void		setTries(const int t) {mTries = t;}
	int			getTries() const {return mTries;}

	int			getTcs() const {return mTcs;}
	int			getFailedTcs() const {return mFailedTcs;}
	void		addFailedTc(const OmnTestcase &tc, const OmnTestcase &preTc);
	void		addSuite(const OmnTestSuitePtr &suite);
	void		tcFinished(OmnTestcase &tc);
	OmnRslt		testPkgFinised(const OmnTestPkgPtr &pkg);
	void		saveTcRslt(OmnTestcase &tc);
	void		setCrtPkg(const OmnTestPkgPtr &pkg);
	void		setCrtSuite(const OmnTestSuitePtr &suite);
	OmnString	getStat() const;
//	void		addRslt(const int totalTc, const int totalFailed)
//	{
//		mTcs += totalTc;
//		mFailedTcs += totalFailed;
//	}
	void		setLoggingFlag(const bool f) {mNeedToRecordGoodTcs = f;}

	void					setSeedType(const OmnTestMgr::SeedType seedType){mSeedType = seedType;}
	OmnTestMgr::SeedType	getSeedType()const {return mSeedType;}
	void					setSeed(const int seed){mOrigSeed = seed;}
	void					setSeed(int &argc, char** &argv);
	void					updateSeed();
	OmnTestSuitePtr			getCrtSuite() const;

private:
	void					initSeed();
};

#endif

