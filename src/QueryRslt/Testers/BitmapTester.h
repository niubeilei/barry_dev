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
// 01/06/2013	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryRslt_Tester_Bitmap_h
#define Aos_QueryRslt_Tester_Bitmap_h

#include "QueryRslt/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxVList.h"


class OmnTestMgr;
struct AosBlock
{
	u64 start;
	u64 end;
	AosBlock(const u64 &s,
			const u64 &e)
		:
		start(s),
		end(e){};
	~AosBlock(){};
};

class AosBitmapTester : public OmnTestPkg 
{
public:
	enum
	{
		eMaxDocid = 0x7fffffffffffffffL
	};

private:
	u64						mMaxBufferSize;
	AosBuffPtr				mA;
	AosBuffPtr				mB;
	AosBuffPtr				mAandB;
	AosBuffPtr				mAorB;
	AosBuffPtr				mAnotB;
	AosBuffPtr				mOrAnd;
	vector<AosBlock>		mBlocks;
	vector<u64>				mNotInRsltVect;
	vector<u64>				mOrAndRsltVect;
	AosBitmapPtr		mAndBitmapRslt;
	AosBitmapPtr		mOrBitmapRslt;
	AosBitmapPtr		mBitmapA;
	AosBitmapPtr		mBitmapB;
	int						mRound;
public:
	AosBitmapTester();
	~AosBitmapTester(); 

public:
	bool 	start();
private:
	bool				config();
	bool 				basicTest();
	bool 				initData();
	bool 				setBitmaps();
	bool 				countData();
	bool 				countAnd();
	bool 				countOr();
	bool 				countNotIn();
	bool 				countOrAnd();
	bool 				checkRslt();
	bool 				initBlocks();
	bool 				initRawData();
	bool 				addBlock(const u64 &max_size, const u64 &min_size);
	bool 				isInBlocks(const u64 &start, const u64 &end);
	bool 				percent(const int &per);
};
#endif
