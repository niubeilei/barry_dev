////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemoryCheck.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_MemoryChecker_h
#define Snt_Util_MemoryChecker_h

#include "Util/Obj.h"
#include <iostream.h>


class SntMemoryChecker OmnDeriveFromObj
{
protected:
	enum {FLAG_VALUE = 1234567};
	int		mFlag;

protected:
	SntMemoryChecker()
		:
	mFlag(FLAG_VALUE)
	{
	}

	SntMemoryChecker(const SntMemoryChecker & rhs)
		:
	mFlag(FLAG_VALUE)
	{
		if (rhs.mFlag != FLAG_VALUE)
		{
			cout << "********** Copy from a bad object: " << &rhs << endl;
		}
	}

	SntMemoryChecker & operator = (const SntMemoryChecker &rhs)
	{
		if (this != &rhs)
		{
			if (rhs.mFlag != FLAG_VALUE)
			{
				cout << "********** Assigned from a bad object: " << &rhs << endl;
			}
		}
		return *this;
	}

	virtual ~SntMemoryChecker()
	{
		if (mFlag != FLAG_VALUE)
		{
			cout << "****************** Objects corrupted" << endl;
		}
		else
		{
			mFlag = 0;
		}
	}
};

#endif
