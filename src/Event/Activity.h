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
// 12/06/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Event_Activity_h
#define Aos_Event_Activity_h

#include "aosUtil/Types.h"
#include <string>
#include <list>


struct AosActivity
{
	u32			mActivityId;
	std::string	mFilename;
	int			mLineno;
	void *		mInst;

	AosActivity()
		:
	mActivityId(0),
	mLineno(-1),
	mInst(0)
	{
	}

	AosActivity(const u32 id, 
				const std::string &fn, 
				const int ln, 
				void *inst)
		:
	mActivityId(id), 
	mFilename(fn),
	mLineno(ln),
	mInst(inst)
	{
	}

	std::string		toString();
	u32				getEventId() const {return mActivityId;}
	void *			getInst() const {return mInst;}
};

typedef std::list<AosActivity>				AosActivityList;
typedef std::list<AosActivity>::iterator	AosActivityItr;

#endif

