////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: EnumInt.h
// Description:
//	Traveling in SuZhou, China. Nothing to do. So here is the 
//  code I wrote.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_FuncTester_h
#define OMN_Tester_FuncTester_h

#include "Tester/TValue.h"

class OmnEnumInt : public OmnTValue<int>
{
private:
	int			*mValues;
	int			 mNumValues;
	int			 mDefault;
	int			 mCrt;

public:
	OmnEnumInt()
		:
	mValues(0),
	mNumValues(0),
	mCrt(0)
	{
	}

	OmnEnumInt(const int *values, const int num_values, const int defaultValue);
	~OmnEnumInt() {}

	virtual void		reset() {mCrt = 0;}
	virtual int			nextValue(bool &status);
};

#endif

