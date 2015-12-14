////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 01/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/SdocActionObj.h"

#include "Actions/SdocAction.h"
#include "Actions/Torturer/ActionRandUtil.h"


AosSdocActionObj::AosSdocActionObj()
:
AosActionObj(0)
{
}


AosSdocActionObj::~AosSdocActionObj()
{
}


bool 
AosSdocActionObj::runActions(
		const AosXmlTagPtr &def, 
		const OmnString &tagname,
		const AosRundataPtr &rdata)
{
	return AosSdocAction::runActions(def, tagname, rdata);
}


bool 
AosSdocActionObj::runActions(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return AosSdocAction::runActions(def, rdata);
}


bool 
AosSdocActionObj::runAction(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return AosSdocAction::runAction(def, rdata);
}


OmnString 
AosSdocActionObj::getActionId(const AosXmlTagPtr &action)
{
	return AosSdocAction::getActionId(action);
}


OmnString 
AosSdocActionObj::pickActions(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	return AosActionRandUtil::pickActions(tagname, level, rule, rdata);
}


