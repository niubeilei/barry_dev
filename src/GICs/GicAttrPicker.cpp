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
#include "GICs/GicAttrPicker.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"


AosGicAttrPicker::AosGicAttrPicker(const bool flag)
:
AosGic(AOSGIC_ATTRPICKER, AosGicType::eAttrPicker, flag)
{
}


AosGicAttrPicker::~AosGicAttrPicker()
{
}


bool	
AosGicAttrPicker::generateCode(
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

	OmnString querybd = vpd->getAttrStr("attr_query");
	OmnString gic_value;
	if (querybd != ""&& obj)
	{
		gic_value  = obj->getAttrStr(querybd,vpd->getAttrStr("attr_value"));
	}
	else
	{
		gic_value = vpd->getAttrStr("attr_value");
	}
	OmnString str;
	str 
		<< ",attr_value:\'"<< gic_value <<"\'"
		<< ",attr_query:\'" << querybd <<"\'";

	code.mJson << str;
	return true;
}

