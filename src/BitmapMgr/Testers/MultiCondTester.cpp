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
// 01/06/2013       Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "BitmapMgr/Testers/MultiCondTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "BitmapMgr/Ptrs.h"
#include "BitmapMgr/BitmapMgr.h"
#include "Debug/Debug.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "Util/HashUtil.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"

AosMultiCondTester::AosMultiCondTester()
{
}


AosMultiCondTester::~AosMultiCondTester()
{
}

bool 
AosMultiCondTester::start()
{
	aos_assert_r(basicTest(), false);
	return true;
}


bool
AosMultiCondTester::basicTest()
{
	//	1. create blocks, global attr list
	//
	//	2. get each block to create doc
	//		2.1 how many attr this block has?
	//		2.2 for each doc, determine the 
	//			value of each attr
	//		2.3 for each doc, for each conditions
	//			group, check the doc is in this conds? 
	//			if it is in, list the [docid, order value] into this group*
	//
	//	3. create query conditions group
	//		3.1 how many groups we should have
	//		3.2 for each group, how many conditions 
	//			we should have
	//		3.3 for each condition, which attr it is using? 
	//			how about the operation? the max/min/equal/ne value?
	//
	//
	//	4. how to list [.....] to a group?
	//		4.1 add to list
	//		4.2 if it is over 1,000,000 entries
	//			4.2.1 sort
	//			4.2.2 save to a file(fixed length)
	//			4.2.3 start using another file
	//
	//	5. call query functions to get result
	//
	//	6. check from result to expect
	//		6.1 get 1000 entries from each file to vectors
	//		6.2 for loop: get each docid from result
	//		6.3 check if it is in the bottom of the vectors, 
	//			if it is in, remove the entry(change cursor 
	//			from that vector)** (if it is not in any of 
	//			the bottom, alarm!!!!!!!)
	//		6.4 check the [order value] from the entry, see 
	//			if it is increasing/decresing 
	//		6.5 if remove the entry cause no entry in the 
	//			vectors any more, get another 1000 entries
	//		6.6 if no entries in the file, this vector is finished
	//		6.7 whenever we stop checking, check each vectors remained. 
	//			the bottom values should > (<) the last value we found
	//		6.8 if we run to the end, all the vectors should be removed.
	//  		(finish the condition group 1, start group 2)---- 
	//  		query another time with group 2 conditions, check result.
	
	int tries = 1000000;
	for (int i=0; i<tries; i++)
	{
		aos_assert_r(clear(), false);
		aos_assert_r(init(), false);
	}

	return true;
}

#define MemeberID(docID) \
((u16)(((docID)&0xFFFFFF0000000000ULL>>30) \
	| ((docID)&0x0000000F00000000ULL>>26)   \
	| ((docID)&0x000000000F000000ULL>>22)   \
	|((docID)&0x000000000000C000ULL>>14)))  

#define SectionID(docID) \
	((u16)(((docID)&0x000000f000000000ULL>>28) \
		| ((docID)&0x00000000f0000000ULL>>24) \
		| ((docID)&0x0000000000ff0000ULL>>16)))

#endif
