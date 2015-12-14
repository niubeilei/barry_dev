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
// 2014/07/31 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapMgr_Tester_BitmapTester_h
#define Aos_BitmapMgr_Tester_BitmapTester_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"

class OmnTestMgr;

class AosBitmapTester : public OmnTestPkg
{
private:
	AosRundataPtr 	mRundata;
	AosRundata *	mRundataRaw;
	char *			mArray1;
	char *			mArray2;
	AosBitmapObjPtr mBitmap1;
	AosBitmapObjPtr mBitmap2;
	int				mTries;
	u32				mBitmapSize;
	u64				mNumAppends;
	i64				mNumber;
	i64 			mTime;
	i64 			mMaxRoundTime;
	i64 			mMaxMemory;
	i64 			mAlarmFlag;
	u64 			mRound;

	OmnFilePtr		mStateFile;
public:
	AosBitmapTester();
	~AosBitmapTester(); 

public:
	bool 		start();

private:
	bool 		config();
	bool 		basicTest();
	bool		bitmapTorturer();
	bool		setBits(const AosBitmapObjPtr &bitmap, char *array);
	bool		testBits();
	bool		testAND();
	bool		testAppendDocids();
	bool		testAppendDocids2(const AosBitmapObjPtr &bitmap, char *array);
	bool		testOR();
	bool		testSave();
	bool		testSaveNew();
	bool        testIsEmpty();
	bool        testClear();                                                                      
	bool    	testNextandPreDocid(char *array,const int array_size,const AosBitmapObjPtr &bitmap);
	bool	    testCheckandAppend();
	bool        testcountAndAnotB();
	bool        testnextDocid(char *array,const AosBitmapObjPtr &bitmap);
	bool        testincludeBitmap();
	bool        testappendBitmap();
	bool        testremoveBits();
	bool        testexchangeContent();
    bool        testgetNumBits();
	bool        testreset();
	//bool         testgetCrtDocid();//for test CrtDocid
    bool        testSplitBitmap();//给定Bitmap，split平分成若干个bitmap
	bool        testSplitBitmapByValues();//给定Bitmap，split成若干个bitmap,规则是按照给定的边界划分。
	bool        testCloneFrom();//从一个bitmap复制内容到另一个bitmap， 与clone函数的区别是，调用方式是 mBitmap2->cloneFrom(mBitmap1);// mBitmap2是一个已存在的bitmap
//	bool        testGetPage();//从游标处开始取一个page的docids，如果不够page_size,则全取。返回边界值的u64，如果全部取完，则返回0.
	bool 		testGetPage(const u64 &page_size,const u64 &sp, const bool direct);
	u64			randomBetweenAAndB(const u64 &a,const u64 &b);
	u64			randomU64();
	bool		percent(const int i);
	void		changeValue(u64 &v1, u64 &v2);

	// modify at 2015/11/10
    bool testFirstDoc();
    bool testLastDoc();

	// modify at 2015/11/12
	bool testGetCountDocids();
};
#endif
