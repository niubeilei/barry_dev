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
#include "GICs/GicColorPicker.h"
#include "HtmlServer/HtmlUtil.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicColorPicker();

AosGicColorPicker::AosGicColorPicker(const bool flag)
:
AosGic(AOSGIC_COLORPICKER, AosGicType::eColorPicker, flag)
{
}


AosGicColorPicker::~AosGicColorPicker()
{
}


bool	
AosGicColorPicker::generateCode(
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

	OmnString querybd = vpd->getAttrStr("color_query");
	OmnString gic_value = vpd->getAttrStr("color_value");
/*	if (querybd != ""&& obj)
	{
		gic_value  = obj->getAttrStr(querybd,vpd->getAttrStr("color_value"));
	}
	else
	{
		gic_value = vpd->getAttrStr("color_value");
	}
*/	if(gic_value != "")  gic_value = AosHtmlUtil::getWebColor(gic_value);
	OmnString str;
	str 
		<< ",color_value:\'"<< gic_value <<"\'"
		<< ",color_query:\'" << querybd <<"\'";

	code.mJson << str;
	return true;
}

