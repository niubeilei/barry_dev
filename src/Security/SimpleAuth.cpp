////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SimpleAuth.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

/*
#include "Security/SimpleAuth.h"

#include "Alarm/Alarm.h"
#include "Porting/Random.h"
#include "Util1/Time.h"



OmnSimpleAuth::OmnSimpleAuth()
:
mChallengeLen(eMaxChallengeLen)
{
	OmnInitRandom();
}


OmnSimpleAuth::~OmnSimpleAuth()
{
}


void
OmnSimpleAuth::setChallengeLen(const int l)
{
	mChallengeLen = l;
	if (mChallengeLen > eMaxChallengeLen || mChallengeLen <= 0)
	{
		mChallengeLen = eMaxChallengeLen;
	}
}


OmnString	
OmnSimpleAuth::getChallenge()
{
	//
	// A challenge is a string generated based on certain pattern. 
	// Currently, we use random() to generate an index and use the
	// index to determine a character.
	//
	char challenge[eMaxChallengeLen+1];
	int index = 0;
	while (index < mChallengeLen)
	{
		char c = (OmnRandom() & 0xff);
		if (c >= 33 && c <= 126)
		{
			challenge[index++] = c;
		}
	}

	challenge[index] = 0;

	OmnTrace << "Challenge: " << challenge << endl;
	return challenge;
}
*/

