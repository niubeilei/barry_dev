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
// 11/29/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Action/ActionDef/ActionUtil.h"

#include "Action/ActionDef/Action.h"

/* Commented out by Chen Ding, 03/24/2009
// 
// Return 0 if successful. Otherwise, it returns error code.
//
int AosAction_doActions(AosActionList &actions, 
						const AosVarContainerPtr &semanticData, 
						const AosVarContainerPtr &actionData, 
						std::string &errmsg)
{
	std::list<AosActionPtr>::iterator itr;
	for (itr = actions.begin(); itr != actions.end(); itr++)
	{
		int rslt = (*itr)->doAction(semanticData, actionData, errmsg);
		if (rslt != 0)
		{
			// 
			// Failed performing the action. 
			//
			return rslt;
		}
	}

	// 
	// All actions were performed. 
	//
	return 0;
}
*/

