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
// This is a utility to select docs.
//
// Modification History:
// 01/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocSelector/Torturer/DocSelRandUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocSelector/Ptrs.h"
#include "DocSelector/DocSelector.h"
#include "Random/RandomRule.h"
#include "XmlUtil/XmlTag.h"

AosDocSelRandUtil::AosDocSelRandUtil()
{
}


AosDocSelRandUtil::~AosDocSelRandUtil()
{
}


OmnString 
AosDocSelRandUtil::pickDocSelector(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	AosDocSelectorType::E type = pickSelector();
	AosDocSelectorPtr selector = AosDocSelector::getSelector(type, rdata);
	aos_assert_r(selector, "");
	return selector->getXmlStr(tagname, level, rule, rdata);
}

AosDocSelectorType::E 
AosDocSelRandUtil::pickSelector()
{
	return (AosDocSelectorType::E)OmnRandom::nextInt(
			(int)AosDocSelectorType::getFirst(), 
			(int)AosDocSelectorType::getLast());
}

