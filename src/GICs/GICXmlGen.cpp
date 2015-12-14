//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved. 
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
// Modification History:
// 05/19/2011: Created by Brian 
////////////////////////////////////////////////////////////////////////////

#include "GICs/GICXmlGen.h"

#include "GICs/GIC.h"
#include "GICs/GicTypes.h"
#include "GICs/Ptrs.h"
#include "XmlUtil/XmlTag.h"


AosGicXmlGen::AosGicXmlGen()
{
}


AosGicXmlGen::~AosGicXmlGen()
{
}


bool
AosGicXmlGen::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentId,
		AosHtmlCode &code)
{
	return true;
}


OmnString
AosGicXmlGen::getVpdXml(
		const OmnString &gictypeStr, 
		const AosRundataPtr &rdata)
{
	AosGicType::E gictype = AosGicType::toEnum(gictypeStr);
	aos_assert_r(AosGicType::isValid(gictype), "");

	AosGicPtr gic = AosGic::getGic(gictype);
	if(gic)
	{
    	AosRandomRulePtr rule = OmnNew AosRandomRule(gictypeStr);
		return gic->getXmlStr(rule, rdata);
	}
	//never be here
	return "";
}

	
OmnString
AosGicXmlGen::getVpdXml(const AosRundataPtr &rdata)
{
	OmnString gictype = getRandGicType();
	AosGicType::E gtype = AosGicType::toEnum(gictype);
	aos_assert_r(AosGicType::isValid(gtype), "");

    AosRandomRulePtr rule = OmnNew AosRandomRule(gictype);
	AosGicPtr gic = AosGic::getGic(gtype);
	aos_assert_r(gic, "");
	return gic->getXmlStr(rule, rdata);
}


OmnString 
AosGicXmlGen::getRandGicType()
{
	while(1)
	{
		AosGicType::E gictype = (AosGicType::E)(rand() % AosGicType::eMax);
		AosGicPtr gic = AosGic::getGic(gictype);
    	if (gic)
    	{
			return gic->getGicName();
		}
	}
}
