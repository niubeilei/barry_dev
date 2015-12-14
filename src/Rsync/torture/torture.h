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
// 2015/03/10 Created by Koala Ren
//////////////////////////////////////////////////////////////////////////////
#ifndef TORTURE_H
#define TORTURE_H
#define JoinNum 5

#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <string>
#include <vector>
using namespace std;

class AosRundata;

class AOSTorture
{
private:
	int							mMaxTables;
    OmnString					mJoinList[JoinNum];///The type of join
    OmnString					mSQLStr; // the test case generate in this program

    int 						mNumTables; // the number of table we will use
    vector<int> 				mTableList; // which table we will use among 50 tables
    vector<int>					mJoinSequence; //which join in each place
    vector<vector<int> >		mBracket; 	// if there is a bracket in each place, and
                            				//value == 0, represent no bracket
                            				//value == 1, represent left bracket
                           					//value == 2, represent right bracket

public:
    AOSTorture();
    virtual ~AOSTorture();

	bool initBracketList();
	bool config(AosRundata *rdata, const AosXmlTagPtr &conf);
    bool generateTableSequence();
    bool generateJoinSequence();
    bool generateBracketSequence(int low, int high);
    bool generateSQLCase();
    bool printSQLCase();
};

#endif // TORTURE_H
