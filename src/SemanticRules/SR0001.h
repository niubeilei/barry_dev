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
// For the definition of SR0001, please refer to the document.  
//
// Modification History:
// 11/26/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_SemanticRules_SR0001_h
#define Aos_SemanticRules_SR0001_h

#include "aosUtil/Types.h"
#include "SemanticRules/SemanticRuleCommon.h"
#include "SemanticData/Types.h"
#include "Util/RCObjImp.h"
#include <string>

extern AosSemanticRulePtr aos_sr0001_creator();

class AosSR0001 : public AosSemanticRuleCommon
{
	OmnDefineRCObject;

private:
	std::string		mObjName;
	u32				mMin;
	u32				mMax;
	u32				mTimeMin;
	u32				mTimeMax;
	AosTimeMeasure::E	mTimeMeasure;

public:
	AosSR0001();
	AosSR0001(const std::string &objName, 
			  const u32 min, 
			  const u32 max, 
			  const u32 time_min, 
			  const u32 time_max);
	~AosSR0001();

	virtual int	evaluate(const AosVarContainerPtr &data, 
						 std::string &errmsg);
	virtual bool procEvent(const AosEventPtr &event, 
						 bool &cont);
};
#endif

