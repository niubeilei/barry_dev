////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RandomInt.h
// Description:
//	Traveling in SuZhou, China. Nothing to do. So here is the 
//  code I wrote.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_RandomInt_h
#define OMN_Tester_RandomInt_h

#include "Tester/TValue.h"


class OmnRandomInt : public OmnTValue<int>
{
private:
	int			mMin;
	int			mMax;

public:
	OmnRandomInt(int min, int max)
		:
	mMin(min),
	mMax(max)
	{
	}

	~OmnRandomInt();

	virtual int			nextValue(bool &status);
};

#endif

