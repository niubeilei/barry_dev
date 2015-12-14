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
#ifndef Aos_QueryCond_CondArith_h
#define Aos_QueryCond_CondArith_h

#include "QueryCond/QueryCond.h"
#include "Util/Opr.h"


class AosCondArith : virtual public AosQueryCond
{
private:
	AosOpr		mOpr;
	OmnString	mValue;

public:
	AosCondArith(const bool regflag);
	AosCondArith(const AosOpr opr, const u64 &value);
	virtual ~AosCondArith();

	virtual bool			parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosQueryCondPtr	clone() const;
	virtual bool			toString(OmnString &str);
};
#endif

