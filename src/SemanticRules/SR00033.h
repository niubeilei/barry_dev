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
// 12/08/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticRules_SR00033_h
#define Aos_SemanticRules_SR00033_h

#include "aosUtil/Types.h"
#include "Event/Ptrs.h"
#include "Event/ActivityTracker.h"
#include "SemanticRules/SemanticRuleCommon.h"
#include "SemanticData/Types.h"
#include "SemanticObj/Ptrs.h"
#include "Util/RCObjImp.h"
#include <string>
#include <list>

class AosSR00033 : public AosSemanticRuleCommon
{
	OmnDefineRCObject;

private:
	u32			mMax;

public:
	AosSR00033(const u32 max);
	~AosSR00033();

	virtual int	evaluate(const AosVarContainerPtr &data, 
						 std::string &errmsg);
	virtual bool procEvent(const AosEventPtr &event, 
						 bool &cont);

private:
	bool 	checkElemAdded(const AosEventPtr &event);
};
#endif

