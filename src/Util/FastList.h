////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FastList.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_FastList_h
#define Snt_Util_FastList_h

#include "Util/Obj.h"

template <class C>
class SntFastList OmnDeriveFromObj
{
	C *			mPrimary;
	C **		mSecondary;
	int			mCount;

public:
	SntFastList(const int initSize = 5, 
				const int maxSize = -1,
				const bool overflowHandling = true)
				:
	mPrimary(0),
	mCount(0)
	{
		mSecondary = 0;
	}
	
	~SntFastList()
	{
		//OmnDelete mPrimary; 		OmnDelete(mPrimary);		mPrimary = 0;
		//OmnDelete [] mSecondary; 		OmnDeleteArrary(mSecondary)		mSecondary = 0;
	}


};
#endif
