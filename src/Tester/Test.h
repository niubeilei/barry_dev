////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Test.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_Test_h
#define OMN_Tester_Test_h

#include "Tester/TestAssertMgr.h"
#include "Tester/Testcase.h"
#include "Util/String.h"

class OmnTestcase;

template <class T>
class OmnActual
{
public:
	T	mActual;
	
	OmnActual(const T &t)
		:
	mActual(t)
	{
	}

	OmnString getData() const {OmnString str; str << mActual; return str;}
	T	getValue() const {return mActual;}
};


template <class T>
class OmnExpected
{
public:
	T	mExpected;
	T	mActual;

	OmnExpected(const T &t)
		:
	mExpected(t)
	{
	}

	OmnString getData() const {OmnString str; str << mExpected; return str;}

	OmnExpected<T> & operator == (const OmnActual<T> &value)
	{
		mActual = value.getValue();
		return *this;
	}

	OmnTestcase & operator << (OmnTestcase &tc)
	{
		OmnString expected;
		expected << mExpected;
		tc.setExpected(expected);

		OmnString actual;
		actual << mActual;
		tc.setActual(actual);

		if (mActual == mExpected)
		{
			tc.setSuccess();
		}
		else
		{
			tc.setFailed();
		}

		return tc;
	}
};


#ifndef OmnBeginTest
#define OmnBeginTest mTcSeqno = 0; mTcContextDesc
#endif

#ifndef OmnCreateTc
#define OmnCreateTc mPreTc = mCrtTc; mCrtTc = OmnTestcase(nextTcName(), __FILE__, __LINE__); *this
#endif

#ifndef OmnTestAssert
#define OmnTestAssert(x, c) OmnTestAssertMgr::set(x, c)
#endif

#ifndef OmnCheckTA
#define OmnCheckTA(x) OmnTestAssertMgr::check(x)
#endif

#ifndef OmnTC
#define OmnTC(x, y) mPreTc = mCrtTc; mCrtTc = OmnTestcase(nextTcName(), __FILE__, __LINE__); \
			(x == y) << mCrtTc; *this
#endif

#ifndef OmnTCTrue
#define OmnTCTrue(y) mPreTc = mCrtTc; mCrtTc = OmnTestcase(nextTcName(), __FILE__, __LINE__); \
			(OmnExpected<bool>(true) == OmnActual<bool>(y)) << mCrtTc; *this
#endif

#ifndef AosTC 
#define AosTC(y) mPreTc = mCrtTc; mCrtTc = OmnTestcase(nextTcName(), __FILE__, __LINE__); \
			(OmnExpected<bool>(true) == OmnActual<bool>(y)) << mCrtTc; *this
#endif

#ifndef OmnTest_Assert
#define OmnTest_Assert(y) mPreTc = mCrtTc; mCrtTc = OmnTestcase(nextTcName(), __FILE__, __LINE__); \
			(OmnExpected<bool>(true) == OmnActual<bool>(y)) << mCrtTc; *this
#endif

#ifndef OmnTCTrue_1
#define OmnTCTrue_1(y, z) mPreTc = mCrtTc; mCrtTc = OmnTestcase(nextTcName(), __FILE__, __LINE__); \
			(OmnExpected<bool>(true) == OmnActual<bool>(y)) << mCrtTc; \
			if (!(y)) return z; *this
#endif

#ifndef OmnTCFalse
#define OmnTCFalse(y) mPreTc = mCrtTc; mCrtTc = OmnTestcase(nextTcName(), __FILE__, __LINE__); \
			(OmnExpected<bool>(false) == OmnActual<bool>(y)) << mCrtTc; *this
#endif

#ifndef OmnTCBool
#define OmnTCBool(x, y) mPreTc = mCrtTc; mCrtTc = OmnTestcase(nextTcName(), __FILE__, __LINE__); \
			(OmnExpected<bool>(x) == OmnActual<bool>(y)) << mCrtTc; *this
#endif

#ifndef OmnNextTc
#define OmnNextTc(p, x, y, filename, line) p->nextTc(x, y, filename, line) 
#endif

#ifndef OmnTest
#define OmnTest(cond) mPreTc = mCrtTc; mCrtTc = OmnTestcase(nextTcName(), __FILE__, __LINE__); mCrtTc << (cond); *this
#endif

#endif

