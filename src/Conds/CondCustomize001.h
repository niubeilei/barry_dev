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
// 05/30/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conds_CondCustomize001_h
#define Aos_Conds_CondCustomize001_h

#include "Conds/Condition.h"
#include <vector>
#include "SEUtil/XmlRandObj.h"

class AosCondCustomize001 : virtual public AosCondition
{

public:
	AosCondCustomize001(const bool reg);
	~AosCondCustomize001();

	// Condition interface
	virtual bool evalCond(const char *record, const int len, const AosRundataPtr &rdata);
	virtual bool evalCond(const AosXmlTagPtr &cfg, const AosRundataPtr &rdata);
	virtual AosConditionObjPtr clone();

private:
};
#endif
