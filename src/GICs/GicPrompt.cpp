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
// 07/16/2010: Created by lynch yang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicPrompt.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicPrompt::AosGicPrompt(const bool flag)
:
AosGic(AOSGIC_PROMPT, AosGicType::ePrompt, flag)
{
}


AosGicPrompt::~AosGicPrompt()
{
}


bool	
AosGicPrompt::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentId,
		AosHtmlCode &code)
{
	// This function will generate:
	// 	1. HTML code
	// 	2. CSS code
	// 	3. JavaScript Code
	// 	4. Flash code
	code.mCss << ".myclass{}";
	OmnString valbd = vpd->getAttrStr("pmt_query");
	OmnString gic_value;
	if (valbd != "" && obj)
	{
		gic_value = obj->getAttrStr(valbd, vpd->getAttrStr("pmt_text"));
	}
	else
	{
		gic_value = vpd->getAttrStr("pmt_text");
	}
	OmnString div_id = AosGetHtmlElemId();
	OmnString label_id = AosGetHtmlElemId();
	code.mHtml << "<div id=\"" << label_id << "\">"	<< gic_value << "</div>";
	aos_assert_r(convertToJson(vpd, obj, code.mJson, div_id, label_id, gic_value), false);
	return true;
}


bool	
AosGicPrompt::convertToJson(
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		OmnString &jsonStr, 
		const OmnString &div_id,
		const OmnString &label_id,
		const OmnString &gic_value
		)
{
	// This function converts the VPD to Json form. 
	OmnString gic_id = AosGetHtmlElemId();
	OmnString gic_type = vpd->getAttrStr("gic_type"); 
	OmnString clk = vpd->getAttrStr("gic_clickable", "true"); 
	OmnString udrline = vpd->getAttrStr("gic_underlined", "false");
	OmnString fontStyle = vpd->getAttrStr("pmt_fstyle", "normal");
	OmnString fontColor = vpd->getAttrStr("pmt_fgcolor","0x000000");
	OmnString fontSize = vpd->getAttrStr("pmt_fsize", "12");
	OmnString valbd = vpd->getAttrStr("pmt_query","");
	OmnString actions;
	AosXmlTagPtr actions_vpd = vpd->getFirstChild("actions");
	if(actions_vpd)
		actions = actions_vpd->toString();
	//jsonStr << "{";
	//jsonStr << ",el : \'" << div_id <<"\'," ;
	//jsonStr << "id : \'" << gic_id << "\',";
	jsonStr << ",labelId : \'" << label_id << "\',";
	jsonStr << "labelText : \'" << gic_value << "\',";
	jsonStr << "bindingTarget : \'" << valbd << "\',";
	jsonStr << "labelColor : \'" << fontColor << "\',";
	jsonStr << "labelStyle : \'" << fontStyle << "\',";
	jsonStr << "labelSize : \'" << fontSize << "\',";
	//jsonStr << "width : " << gic_width << "," ;
	//jsonStr << "height : " << gic_height << "," ;
	//jsonStr << "xtype : \'"<< gic_type <<"\'," ;
	jsonStr << "underLined : " << udrline << "," ;
	jsonStr << "clickable : " << clk ;
	/*
	if(actions)
		jsonStr << "actions : \'" << actions << "\',"; 
	jsonStr << "x : " << gic_x << "," ;
	jsonStr << "y : " << gic_y;
	*/
	//jsonStr << "}" ;
	return true;
}

