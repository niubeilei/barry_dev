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
// For the definition of SR00027, please refer to the document.  
//
// Modification History:
// 12/04/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticRules/SR00027.h"

#include "Alarm/Alarm.h"
#include "aosUtil/ReturnCode.h"
#include "Debug/Except.h"
#include "SemanticData/VarContainer.h"
#include "SemanticData/ClassObj.h"
#include "Event/Event.h"


AosSR00027::AosSR00027(const std::string &funcname)
:
AosSemanticRuleCommon(AosRuleId::eSR00027, "SR00027", 0, 0),
mFuncName(funcname)
{
	// 
	// This rule 
}


AosSR00027::~AosSR00027()
{
}


int	
AosSR00027::evaluate(const AosVarContainerPtr &data, 
					 std::string &errmsg)
{
	try
	{
		// 
		// It retrieves the function caller from 'data'. 
		// If not found, whether the call should be allowed
		// is determined by the default policy.
		//
		// AosSemanticDataPtr dd = data->getData(AosSdNames::eProgExeEnv);
		// aos_assert_r(dd, -eAosRc_ProgErr);
		// AosProgExeEnvPtr env = AosVarConverter<AosProgExeEnv>(dd);
		// aos_assert_r(evn, -eAosRc_ProgErr);
		// std::string caller = Aos
		return 0;
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to evaluate rule SR00027: " 
			<< e.getErrmsg() << enderr;
		return -eAosRc_ProgErr;
	}

	return -eAosRc_ProgErr;
}


bool
AosSR00027::procEvent(const AosEventPtr &event, 
					  bool &cont)
{
	return true;
}



