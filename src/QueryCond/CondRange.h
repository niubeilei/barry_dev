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
// 08/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryCond_CondRange_h
#define Aos_QueryCond_CondRange_h

#include "QueryCond/QueryCond.h"
#include "Util/Opr.h"


class AosCondRange : virtual public AosQueryCond
{
private:
	AosOpr		mOpr;
	OmnString	mMin;
	OmnString	mMax;
	bool		mLeftInclusive;
	bool		mRightInclusive;

public:
	AosCondRange(const bool regflag);
	virtual ~AosCondRange();

	virtual bool			parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosQueryCondPtr	clone() const;
	virtual bool			toString(OmnString &str);
};
#endif

