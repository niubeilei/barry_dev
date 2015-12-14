////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestPkg.h
// Description:
//	A Test Package is a group of test cases. It is different
//  from a test suite. A test suite is a group of test packages. 
//  Normally, a test package is a group of test cases to test
//  a specific module/class.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_TestPkg_h
#define OMN_Tester_TestPkg_h

#include "aosUtil/Types.h"
#include "Alarm/Alarm.h"
#include "Debug/Rslt.h"
#include "CliUtil/CliWrapper.h"
#include "Tester/Testcase.h"
#include "Tester/Test.h"
#include "Tester/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <string>
#include <vector>
using namespace std;

#ifndef aosRunCli
#define aosRunCli(xxx, zzz) \
	{ \
		OmnString __rslt; \
		bool rt = OmnCliProc::getSelf()->runCliAsClient((xxx), __rslt); \
		if(__rslt.length() >= sizeof(int) - 1) \
		{ \
			__rslt.setLength(__rslt.length() - sizeof(int) - 1); \
		} \
    	OmnTC(OmnExpected<bool>(zzz), OmnActual<bool>(rt)) \
			<< "Location: " << __FILE__ << ":" << __LINE__ \
			<< "\n                Error Message: " << __rslt \
			<< "\n                Command: " << (xxx) << endtc; \
	}
#endif

/*
#ifndef aosRunCli1
#define aosRunCli1(xxx, expectedRslt, actualRslt, rslt, errmsg) \
	{ \
		actualRslt = OmnCliProc::getSelf()->runCliAsClient((xxx), rslt); \
		if((unsigned int)rslt.length() >= sizeof(int) - 1) \
		{ \
			rslt.setLength(rslt.length() - sizeof(int) - 1); \
		} \
    	OmnTC(OmnExpected<bool>(expectedRslt), OmnActual<bool>(actualRslt)) \
			<< "Location: " << __FILE__ << ":" << __LINE__ \
			<< "\n                Rslt: " << rslt \
			<< "\n                Error Message: " << errmsg \
			<< "\n                Command: " << (xxx) << endtc; \
	}
#endif
*/

#ifndef aosRunCli1
#define aosRunCli1(xxx, expectedRslt, actualRslt, rslt, errmsg) \
	{ \
		actualRslt = aos_run_cli((xxx), (rslt)); \
    	OmnTC(OmnExpected<bool>(expectedRslt), OmnActual<bool>(actualRslt)) \
			<< "Location: " << __FILE__ << ":" << __LINE__ \
			<< "\n                Rslt: " << rslt \
			<< "\n                Error Message: " << errmsg \
			<< "\n                Command: " << (xxx) << endtc; \
	}
#endif
/*
static int bool aosRunCli(
			const OmnString &cmd, 
			OmnString &rslt, 
			const bool expected, 
			const OmnString &errmsg, 
			const OmnTestPkg *testPkg, 
			const OmnString &filename, 
			const int lineno)
{
	return OmnCliProc::getSelf()->runCliAndCheck(cmd, rslt, testPkg);
}
*/

#ifndef aosRunCli_getStr
#define aosRunCli_getStr(xxx, zzz, rslt) \
	{ \
		bool rt = OmnCliProc::getSelf()->runCliAsClient((xxx), rslt); \
		if(rslt.length() >= sizeof(int) - 1) \
		{ \
			rslt.setLength(rslt.length() - sizeof(int) - 1); \
		} \
    	OmnTC(OmnExpected<bool>(zzz), OmnActual<bool>(rt)) \
			<< "Location: " << __FILE__ << ":" << __LINE__ \
			<< "\n                Error Message: " << rslt \
			<< "\n                Command: " << (xxx) << endtc; \
	}
#endif


class OmnTestMgr;

class OmnTestPkg : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	OmnMutexPtr		mLock;
	OmnTestcase		mPreTc;
	OmnTestcase		mCrtTc;
	OmnString		mName;
	OmnString		mTcNameRoot;
	OmnString		mTcContextDesc;
	int				mNumTcs;
	int				mNumTcsFailed;
	OmnTestMgrPtr	mTestMgr;
	int				mTcSeqno;

public:
	OmnTestPkg()
		:
	mLock(OmnNew OmnMutex()),
	mNumTcs(0),
	mNumTcsFailed(0),
	mTcSeqno(0)
	{
	}

	~OmnTestPkg() {}

	// 
	// OmnObject interface
	//
	virtual int		objSize() const; 

	// 
	// Chen Ding, 02/11/2007
	//
	// virtual bool		start() = 0;
	virtual bool		start() {return true;}
	OmnString	getPkgName() const {return mName;}
	int			getTotalTcs() const {return mNumTcs;}
	int			getTotalFailedTcs() const {return mNumTcsFailed;}
	void		resetTcCounts() {mNumTcs = 0; mNumTcsFailed = 0;}
//	void		addTcCounts(const bool flag);
	OmnRslt		serializeToDb() const;
	OmnString	nextTcName();
	void		setTestMgr(const OmnTestMgrPtr &testMgr);
	void		setExpected(const OmnString &e) {mCrtTc.setExpected(e);}
	void		setActual(const OmnString &a) {mCrtTc.setActual(a);}
	OmnTestMgrPtr	getTestMgr() const;

	OmnTestPkg & operator << (const OmnTestcase &tc) {mCrtTc = tc; return *this;}
	OmnTestPkg & operator << (const OmnString &rhs) {mCrtTc << rhs; return *this;}
	OmnTestPkg & operator << (const std::string &rhs) {mCrtTc << rhs; return *this;}
	OmnTestPkg & operator << (const char *rhs) {mCrtTc << rhs; return *this;}
	OmnTestPkg & operator << (const int64_t &rhs) {mCrtTc << rhs; return *this;}
	OmnTestPkg & operator << (const int rhs) {mCrtTc << rhs; return *this;}
	OmnTestPkg & operator << (const u64 &rhs) {mCrtTc << rhs; return *this;}
	OmnTestPkg & operator << (const float rhs) {mCrtTc << rhs; return *this;}
	OmnTestPkg & operator << (const bool rhs) {mCrtTc << rhs; return *this;}
	OmnTestPkg & operator << (const OmnTcTitle &title) {mCrtTc.setTitle(title); return *this;}
	void	     operator << (const OmnEndTc &etc);

	template <class T>
	OmnTestPkg & operator << (const OmnExpected<T> &expected)
	{
		setExpected(expected.getData());
		return *this;
	}

	template <class T>
	OmnTestPkg & operator << (const OmnActual<T> &actual)
	{
		setActual(actual.getData());
		return *this;
	}

	template <class T>
	OmnTestPkg & nextTc(
			OmnExpected<T> &expected, 
			OmnActual<T> &actual,
			const OmnString &filename, 
			const int line)
	{
		mPreTc = mCrtTc; 
		mCrtTc = OmnTestcase(nextTcName(), filename, line); 
		(expected == actual) << mCrtTc; 
		return *this;
	}

protected:
	void		updateSeed();

public:
	// Chen Ding, 2013/02/09
	int pickOperation(const vector<int> &weights);
};

#endif

