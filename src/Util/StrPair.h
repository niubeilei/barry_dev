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
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_StrPair_h
#define Omn_Util_StrPair_h

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <vector>
using namespace std;

class AosStrPair : public OmnRCObject
{
public:
	enum
	{
		eMaxRanges = 100000,
		FieldSep = '-',
		RangeSep = ','
	};

private:
	u32					mCrtIdx;
	vector<OmnString>	mPairs;
	char				mRangeSep;

public:
	AosStrPair(const OmnString &str, 
			const char field_sep = FieldSep,
			const char range_sep = RangeSep);
	~AosStrPair(); 

	bool nextPair(OmnString &left, OmnString &right, AosRundata *rdata);
};
#endif

