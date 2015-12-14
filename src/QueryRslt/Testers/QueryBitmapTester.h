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
// 09/21/2011	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryRslt_Tester_BitmapTester_h
#define Aos_QueryRslt_Tester_BitmapTester_h

#include "QueryRslt/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxVList.h"


class OmnTestMgr;


class AosBitmapTester : public OmnTestPkg
{
	enum
	{
		eBuffInitSize = 1000, 
		eBuffIncSize = 1000

	};

public:
	AosBitmapObjPtr 	mBitmap1;
	AosBitmapObjPtr 	mBitmap2;
	AosBitmapObjPtr 	mBitmap3;
	AosBitmapObjPtr 	mBitmap4;	
	AosBitmapObjPtr    mBitmapNegate;
	AosBitmapObjPtr 	mBitmapAnd;
	AosBitmapObjPtr 	mBitmapOr;	
    
	u64*				mBits;
	bool*				mBitExist;
	int					mNumBits;


	u64*				mBits2;
	bool*				mBitExist2;
	int					mNumBits2;
	
	u64*				mBits3;
	bool*				mBitExist3;
	int					mNumBits3;


	u64*				mBits4;
	bool*				mBitExist4;
	int					mNumBits4;

	u64                 find_count;

	AosBitmapTester();
	~AosBitmapTester(); 

	bool 	start();

private:
	bool 	basicTest(const bool show_info);
	bool	testOneBitmap(const bool show_info);
	bool	testTwoBitmap(const bool show_info);	
	bool    testCtrlBitmap(const bool show_info);  //Navy:only to compare the new AND function with old one
	bool	addBitmap(const int seq,const bool show_info);
	bool	removeBitmap(const int seq,const bool show_info);
	bool	checkBitmap(const int seq,const int &index,const bool show_info);
	bool	checkBitmapRandom(const int seq,const bool show_info);
	bool	checkAllBitmap(const int seq,const bool show_info);
	bool 	addDeleteBitmap(const int seq,const bool show_info);
	bool	cleanResource();
	bool	checkSaveLoad(const int seq,const bool show_info);

	bool	checkAndOr(const bool show_info);
	bool	checkAnd(const bool show_info);
	bool	checkAndWithCtrlRslt(const bool show_info);
	bool	checkOr(const bool show_info);
	bool	checkNegate(const bool show_info);

	bool	checkRslt(const u64 &value,const bool a, const bool b);

	bool	checkNextDocid(const int seq,const bool show_info);
	
	u64     getFindCount() { return find_count;};
    void    setFindCount() { find_count++; return;};	
	void    printBitmapFilter(AosBitmapObjPtr a, bool is_move);
};
#endif














