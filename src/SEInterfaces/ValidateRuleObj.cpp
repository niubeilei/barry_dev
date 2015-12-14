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
// 2014/09/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ValidateRuleObj.h"

#include "API/AosApi.h"




AosValidateRuleObj::AosValidateRuleObj(const int version)
:
AosJimo(AosJimoType::eValidateRule, version)
{
}


AosValidateRuleObj::~AosValidateRuleObj()
{
}


AosValidateRuleObjPtr 
AosValidateRuleObj::createValidateRule(
		AosRundata *rdata, 
		const OmnString &classname, 
		const int version)
{
	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, classname, version);
	if (!jimo) return 0;

	if (jimo->getJimoType() != AosJimoType::eValidateRule)
	{
		AosLogError(rdata, false, "internal_error: ")
			<< "Classname: " << classname
			<< ", Jimo Type: " << jimo->getJimoType() << enderr;
		return 0;
	}

	AosValidateRuleObjPtr rule = dynamic_cast<AosValidateRuleObj*>(jimo.getPtr());
	if (!rule)
	{
		AosLogError(rdata, false, "internal_error: ")
			<< "Classname: " << classname << enderr;
		return 0;
	}

	return rule;
}


