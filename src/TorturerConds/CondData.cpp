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
// 06/08/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TorturerConds/CondData.h"

#include "Parms/RVG.h"
#include "Util/VarList.h"

bool
AosCondData::getValue(const OmnString &argName, 
					  OmnString &fieldName,
					  OmnString &value, 
					  AosRVGType &type) const
{
	/* 
	 * Temp commented out by Chen Ding, 12/02/2007
	 *
	for (int i=0; i<mArgs.entries(); i++)
	{
		if (mArgs[i]->getName() == argName)
		{
			type = mArgs[i]->getRVGType();
			fieldName = mArgs[i]->getFieldName();
			if (!mArgs[i]->getCrtValue(value))
			{
				OmnAlarm << "Failed to retrieve parm value: " 
					<< argName << enderr;
				return false;
			}

			return true;
		}
	}
	 */

	OmnAlarm << "Parm not found: " << argName << enderr;
	return false;
}
	

bool
AosCondData::getVar(const OmnString &name, OmnString &value) const
{
    if (!mVariables)
	{
		OmnAlarm << "mVariables is null: " << name << ":" << value << enderr;
		return false;
	}

	return mVariables->getValue(name, value);
}


bool		
AosCondData::setArgs(const OmnDynArray<AosRVGPtr> &args)
{
	mArgs = args;
	return true;
}


void		
AosCondData::setVars(const AosVarListPtr &vars)
{
	mVariables = vars;
}


