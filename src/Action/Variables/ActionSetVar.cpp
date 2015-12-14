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
// This class implements the action to set a variable.  
//
// Modification History:
// 11/29/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Action/ActionSetVar.h"

#include "aos/aosReturnCode.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "SemanticData/Var.h"
#include "SemanticData/VarAtomic.h"
#include "SemanticData/VarContainer.h"


AosActionSetVar::AosActionSetVar(const std::string &varname, 
								 const std::string &valuename)
:
mVarName(varname),
mValueName(valuename)
{
}


AosActionSetVar::~AosActionSetVar()
{
}


// 
// It sets the variable identified by mVarName stored in 
// 'semanticData' to the value identified by 'mValueName'
// stored in 'actionData'. If the variable identified by 'mVarName'
// or 'mValueName' is not found, it is an error. If the type
// is not compatible, it is an error.
//
int 
AosActionSetVar::doAction(const AosVarContainerPtr &semanticData, 
						  const AosVarContainerPtr &actionData,
						  std::string &errmsg)
{
	/* Temperarily commented out by Chen Ding, 01/28/2008
	try
	{
		AosSdAtomicPtr var = semanticData->getAtomic(mVarName);
		AosSdAtomicPtr value = actionData->getAtomic(mValueName);
		var->setValue(value);
		return true;
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to set var: " << e.getErrmsg() << enderr;
		return -eAosRc_ProgErr;
	}
	*/

	return -eAosRc_ProgErr;
}

