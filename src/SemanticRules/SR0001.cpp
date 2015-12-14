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
#include "SemanticRules/SR0001.h"

#include "Alarm/Alarm.h"
#include "aosUtil/ReturnCode.h"
#include "Debug/Except.h"
#include "Event/Event.h"
#include "SemanticData/VarContainer.h"
#include "SemanticData/ClassObj.h"
#include "Util/OmnNew.h"


AosSemanticRulePtr aos_sr0001_creator()
{
	return OmnNew AosSR0001();
}

AosSR0001::AosSR0001()
:
AosSemanticRuleCommon(AosRuleId::eSR0001, "SR0001", 0, 0), 
mMin(0), 
mMax(0),
mTimeMin(0),
mTimeMax(0)
{
}


AosSR0001::AosSR0001(const std::string &objname, 
			  const u32 min, 
			  const u32 max, 
			  const u32 time_min, 
			  const u32 time_max)
:
AosSemanticRuleCommon(AosRuleId::eSR0001, "SR0001", 0, 0),
mObjName(objname),
mMin(min),
mMax(max),
mTimeMin(time_min),
mTimeMax(time_max)
{
}


AosSR0001::~AosSR0001()
{
}


int	
AosSR0001::evaluate(const AosVarContainerPtr &data, 
					std::string &errmsg)
{
	try
	{
		AosClassObjPtr obj = data->getClassObj(mObjName);
		u32 num = obj->getInstCreated(
				mTimeMin, mTimeMax, mTimeMeasure);

		if (num < mMin || mMax > 0 && num > mMax) 
		{
			errmsg = "The number of instances the object: ";
			errmsg += obj->getName() 
				   += " created is out of the expectation: ";
			errmsg += AosTimeMeasure::getErrMsg(
						   mTimeMin, mTimeMax, mTimeMeasure);
			return -eAosRc_RuleEvalFalse;
		}
		return 0;
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to evaluate rule: " 
			<< e.getErrmsg() << enderr;
		return -eAosRc_ProgErr;
	}

	return -eAosRc_ProgErr;
}


bool 
AosSR0001::procEvent(const AosEventPtr &event, 
						 bool &cont)
{
	return true;
}


