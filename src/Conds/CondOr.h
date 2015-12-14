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
// 12/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conds_CondOr_h
#define Aos_Conds_CondOr_h

#include "Conds/Condition.h"
#include "SEUtil/XmlRandObj.h"
#include "Thread/Ptrs.h"
#include <vector>

class AosCondOr : virtual public AosCondition
{
	OmnMutexPtr				mLock;
	vector<AosConditionObjPtr> mConds;

public:
	AosCondOr(const bool reg);
	~AosCondOr();

	virtual bool evalCond(const AosRundataPtr &rdata);
	virtual bool evalCond(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual bool evalCond(const char *data, const int len, const AosRundataPtr &rdata);
	virtual AosConditionObjPtr clone();
	virtual OmnString generateCommand(
							const AosXmlTagPtr &sdoc,
							const AosRundataPtr &rdata);

	// RandomXml interface. This function should not be in this class.
	// Will move to the torturer.
	virtual OmnString getXmlStr(
					const OmnString &tagname,
					const int level,
					const OmnString &then_part,
					const OmnString &else_part,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);

	virtual bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

