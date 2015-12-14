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
#ifndef Aos_QueryRslt_Tester_BitmapSectionNew_h
#define Aos_QueryRslt_Tester_BitmapSectionNew_h

#include "QueryRslt/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxVList.h"
#include "BitmapMgr/QueryNode.h"
#include <vector>

class OmnTestMgr;

/*
struct AosEntry
{
	u64 start;
	u64 end;
	AosEntry(const u64 &s,
			const u64 &e)
		:
		start(s),
		end(e){};
	~AosEntry(){};
};
*/
class AosBitmapSectionNew : public OmnTestPkg 
{
public:
	enum
	{
		eMaxDocid = 0x7fffffffffffffffULL
	};

private:
	AosBitmapObjPtr    mBitmap;
	vector<u64 >            mRsltDocids;
	vector<u64 >            mOrDocids;
	vector<u64 >            mAndDocids;
	map_t                   bitmaps;
 
  

public:
	AosBitmapSectionNew();
	~AosBitmapSectionNew(); 

public:
	bool 	start();
private:
	bool 	config();
	bool 	basicTest();

	bool 	init();
 	bool 	createBlock();
 	bool 	createBitmap();
 	bool 	addBit();
 	bool 	removeBit();
 	bool 	orBlock();
 	bool 	andBlock();
 	bool    checkSections();
 	
 	bool 	checkAndAdd(vector<u64> &docid_vector, const u64 &docid);
 	bool    checkAndRemove(vector<u64> &docid_vector, const u64 &docid);
 	bool    vector_and(vector<u64> &a, const vector<u64> &b);
 	
 	//void    debug_show(const<u64> &vect, const char *msg);
};
#endif
