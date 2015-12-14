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
// A counter name is constructed as:
// 	ddd1:ddd2:...:dddn
//
// This is a utility class. Every time its 'nextName()' is called, it 
// returns the next segment as:
// 	"ddd1:ddd2:ddd3:ddd4:ddd5"
// 		 "ddd2:ddd3:ddd4:ddd5"
// 		 	  "ddd3:ddd4:ddd5"
// 		 	  	   "ddd4:ddd5"
// 					    "ddd5"
//
// Modification History:
// 06/15/211: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "CounterUtil/CounterName.h"

#include "Util/StrSplit.h"


AosCounterName::AosCounterName()
:
mIdx(0),
mCounterIdUsed(false),
mCname("")
{
}


AosCounterName::AosCounterName(const OmnString &name)
:
mIdx(0),
mCname("")
{
	mCname = name;
	AosStrSplit::splitStrBySubstr(name, AOS_COUNTER_SEGMENT_SEP, 
			mContainers, mMembers, eMaxSegments);
}


bool
AosCounterName::setName(const OmnString &name)
{
	mCname = name;
	AosStrSplit::splitStrBySubstr(name, AOS_COUNTER_SEGMENT_SEP, 
			mContainers, mMembers, eMaxSegments);
	return true;
}

bool
AosCounterName::setName2(const OmnString &name)
{
	mCname = name;
	AosStrSplit::splitStrBySubstr(name, AOS_COUNTER_SEGMENT_SEP2,
			mContainers, mMembers, mLevel, eMaxSegments);
	return true;
}
