////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This smart doc assumes:
// <sdoc doctype="xxx" event="">
// <cond>
// 		<lhs confirm_code="xxx"/>
// 		<rhs msg="xxx"/>
// 	</cond>
// 	<true_actions>
// 		<action attr_name="xxx" attr_value="xxx"/>
// 	</true_actions>
// 	<false_actions>
// 		<action attr_name="xxx" attr_value="xxx" />
// 	</false_actions>
// </sdoc>
//
// If the conditions in <conds> are true, it runs actions in <true_actions>.
// Otherwise, it runs the actions in <false_actions>. 
//
// Modification History:
// 06/24/2011	Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocCondActions.h"

#include "Conds/Condition.h"
#include "Debug/Error.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "Security/Session.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"
#include "XmlInterface/WebRequest.h"


AosSdocCondActions::AosSdocCondActions(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_CONDACTIONS, AosSdocId::eCondActions, flag)
{
}


AosSdocCondActions::~AosSdocCondActions()
{
}


bool
AosSdocCondActions::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	if(siteid == 0)
	{
		rdata->setError() << "Missing Siteid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	// 1. Retrieve the smart doc
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 1. Retrieve the created doc
	AosXmlTagPtr conds = sdoc->getFirstChild("condition");
	if (!conds)
	{
		rdata->setError() << "Failed to retrieve the created doc ";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = AosCondition::evalCondStatic(conds, rdata);
	AosXmlTagPtr actions;
	if (rslt)
	{
		actions = sdoc->getFirstChild("true_actions");
	}
	else
	{
		actions = sdoc->getFirstChild("false_actions");
	}

	if (actions)
	{
		return AosSdocAction::runActions(actions, rdata);
	}

	return true;
}

