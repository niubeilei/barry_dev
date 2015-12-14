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
// Modification History:
// 10/02/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_MemoryCheckerObj_h
#define AOS_Util_MemoryCheckerObj_h

#include "Util/ClassNames.h"
#include "Util/String.h"


class AosMemoryCheckerObj
{
protected:
	OmnString 		mMemoryCheckerFname;
	int				mMemoryCheckerLineno;
	AosClassName::E mClassName;

public:
	AosMemoryCheckerObj() {}
	~AosMemoryCheckerObj() {}

	void setFileLine(const char *file, const int line)
	{
		mMemoryCheckerFname = file;
		mMemoryCheckerLineno = line;
	}
	OmnString getMemoryCheckerFname() const {return mMemoryCheckerFname;}
	int getMemoryCheckerLineno() const {return mMemoryCheckerLineno;}

	// Chen Ding, 2013/05/13
	virtual int64_t getMemorySize() const {return 0;}
	void setMemoryCheckerClassName(const AosClassName::E c) {mClassName = c;}
	AosClassName::E getMemoryCheckerClassName() {return mClassName;}
};
#endif

				
