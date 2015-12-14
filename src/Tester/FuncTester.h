////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FuncTester.h
// Description:
//	Traveling in SuZhou, China. Nothing to do. So here is the 
//  code I wrote.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_EnumInt_h
#define OMN_Tester_EnumInt_h

#include "Tester/Test.h"
#include "Tester/Testcase.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Tester/Ptrs.h"
#include "Tester/TValue.h"
#include "Util/OmnNew.h"

#include <iostream>
using namespace std;



template <class T1, class T2, class T3, unsigned int inputSize>
class OmnFuncTester
{
typedef int (*func)(T1, T2);
	struct TestInput
	{
		T1		mValue1;
		T2		mValue2;
	};

private:
	OmnTValue<T1>  	*mValue1;
	OmnTValue<T2>	*mValue2;
	func			mFunc;
	T3				mReturnValue;
	OmnTestPkg	   *mTester;
	OmnString		mFilename;
	int				mLine;
	TestInput	   *mInput;
	unsigned int	mInputSize;
	int				mInputIdx;	
	bool			mOverflowed;

public:
	OmnFuncTester(OmnTestPkg *tester, 
				  const OmnString &filename, 
				  const int line) 
		:
	mValue1(0),
	mValue2(0),
	mFunc(0),
	mTester(tester),
	mFilename(filename),
	mLine(line), 
	mInput(0),
	mInputSize(inputSize),
	mInputIdx(0), 
	mOverflowed(false)
	{
		if (inputSize)
		{
			mInput = OmnNew TestInput[inputSize];
		}
	}

	~OmnFuncTester()
	{
		OmnDelete mInput;
		mInput = 0;
	}

	bool	setValue(OmnTValue<T1> *v1, OmnTValue<T2> *v2)
	{
		mValue1 = v1;
		mValue2 = v2;
		return true;
	}

	void setReturnValue(T3 v) {mReturnValue = v;}

	bool restart()
	{
		if (!mValue1 || !mValue2)
		{
			return false;
		}

		mValue1->reset();
		mValue2->reset();
		resetInput();

		return start();
	}

	virtual bool start()
	{
		T3 ret;

		bool status1, status2;

		if (!mValue1 || !mValue2)
		{
			return false;
		}

		T1 value1 = (T1) mValue1->nextValue(status1);
		T2 value2 = (T2) mValue2->nextValue(status2);

		if (!status1 || !status2) return false;

		while (status1)
		{
			while (status2)
			{
				mInput[mInputIdx].mValue1 = value1;
				mInput[mInputIdx].mValue2 = value2;
				mInputIdx++;
				if (mInputIdx >= mInputSize)
				{
					mInputIdx = 0;
					mOverflowed = true;
				}

				ret = mFunc(value1, value2);
				OmnNextTc(mTester, OmnExpected<T3>(mReturnValue), OmnActual<T3>(ret), mFilename, mLine)
					<< endtc;

				value2 = (T2) mValue2->nextValue(status2);
			}

			value1 = (T1) mValue1->nextValue(status1);
			mValue2->reset();
			value2 = (T2) mValue2->nextValue(status2);
		}

		return true;
	}

	void printInput() const
	{
		if (mOverflowed)
		{
			for (int i=mInputIdx+1; i<mInputSize; i++)
			{
				cout << "Value1: " << mInput[i].mValue1 << "   Value2: " << mInput[i].mValue2 << endl;
			}
		}

		for (int i=0; i<mInputIdx; i++)
		{
			cout << "Value1: " << mInput[i].mValue1 << "   Value2: " << mInput[i].mValue2 << endl;
		}
	}

	void	resetInput() {mInputIdx = 0; mOverflowed = false;}

	bool	setFunc(func f) {mFunc = f; return true;}
};

#endif
