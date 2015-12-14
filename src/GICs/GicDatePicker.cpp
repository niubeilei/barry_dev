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
// 07/07/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicDatePicker.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicDatePicker();

AosGicDatePicker::AosGicDatePicker(const bool flag)
:
AosGic(AOSGIC_DATEPICKER, AosGicType::eDatePicker, flag)
{
}


AosGicDatePicker::~AosGicDatePicker()
{
}


bool	
AosGicDatePicker::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	// This function will generate:
	// 	1. HTML code
	// 	2. CSS code
	// 	3. JavaScript Code

	OmnString querybd = vpd->getAttrStr("date_query");
	OmnString formatvalue = vpd->getAttrStr("formatvalue");
	OmnString gic_datetype = vpd->getAttrStr("gic_datetype");
	bool gic_ntoday = vpd->getAttrBool("gic_ntoday", false);
	OmnString gic_his = vpd->getAttrStr("gic_his");
	OmnString gic_value;
	if (querybd != ""&& obj)
	{
		gic_value  = obj->getAttrStr(querybd,vpd->getAttrStr("date_value"));
	}
	else
	{
		gic_value = vpd->getAttrStr("date_value");
	}
	OmnString str;
	str 
		<< ",date_value:\'"<< gic_value <<"\'"
		<< ",formatvalue:\'"<< formatvalue <<"\'"
		<< ",gic_datetype:\'"<< gic_datetype <<"\'"
		<< ",gic_his:\'"<< gic_his <<"\'"
		<< ",gic_ntoday:"<< gic_ntoday
		<< ",date_query:\'" << querybd <<"\'";

	code.mJson << str;
	return true;
}

