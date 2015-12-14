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
// 07/08/2010: Created by Tank
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtml.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicHtml();

AosGicHtml::AosGicHtml(const bool flag)
:
AosGic(AOSGIC_HTML, AosGicType::eHtml, flag)
{
}


AosGicHtml::~AosGicHtml()
{
}


bool	
AosGicHtml::generateCode(
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
	// 	4. Flash code
	//convertToJson(vpd,code.mJson);
	//test
	OmnString valbd = vpd->getAttrStr("gic_initDate");
	OmnString gic_initdate;
/*	if(valbd !="" && obj){
		gic_initdate = obj->getAttrStr(valbd,vpd->getAttrStr("gic_D"));
	}
	else
	{gic_initdate = vpd->getAttrStr("gic_D","Html");};	 
	OmnString gic_value = vpd->getAttrStr("gic_value","Html");
	OmnString gic_id = AosGetHtmlElemId();
	vpd->setAttr("gic_id",gic_id);
	return true;
}


bool
AosGicHtml::convertToJson(
		const AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		OmnString &code.mJson)
{*/
	// This function converts the VPD to Json form.
	OmnString gic_type = vpd->getAttrStr("gic_type","gic_html");
	OmnString gic_fsize = vpd->getAttrStr("gic_fsize","12");
	OmnString gic_txtaln = vpd->getAttrStr("gic_txtaln","left");
	OmnString gic_outclr = vpd->getAttrStr("gic_outclr","#000000");
	OmnString gic_outbgc = vpd->getAttrStr("gic_outbgc","transparent");
	OmnString gic_opt = vpd->getAttrStr("gic_opt","0.001");
	OmnString gic_ftstyle = vpd->getAttrStr("gic_ftstyle","normal");
	OmnString gic_clk = vpd->getAttrStr("gic_clk","true");
	OmnString gic_overul = vpd->getAttrStr("gic_overul","false");
	OmnString gic_overclr = vpd->getAttrStr("gic_overclr","#000000");
	OmnString gic_overbgc = vpd->getAttrStr("gic_overbgc","transparent");
	OmnString gic_downclr = vpd->getAttrStr("gic_downclr","#000000");
	OmnString gic_downbclr = vpd->getAttrStr("gic_downbgc","transparent");
	OmnString gic_upclr = vpd->getAttrStr("gic_upclr","#000000");
	OmnString gic_upbclr = vpd->getAttrStr("gic_upbclr","transparent");
	OmnString gic_valbd = vpd->getAttrStr("gic_valbd","@valbd");
	OmnString gic_fixh = vpd->getAttrStr("gic_fixh","true");
	OmnString gic_sticky = vpd->getAttrStr("gic_sticky","true");
	OmnString gic_vertSb = vpd->getAttrStr("gic_vertSb","false");
	OmnString gic_movable = vpd->getAttrStr("gic_movable","false");
	OmnString gic_outbgcbd = vpd->getAttrStr("gic_outbgcbd","@outbgcbd");
	OmnString gic_linespace = vpd->getAttrStr("gic_linespace");
	OmnString gic_cline = vpd->getAttrStr("gic_cline","false");
	OmnString gic_useHtmlcmp = vpd->getAttrStr("gic_useHtmlcmp","false");
	OmnString gic_htmleditor= vpd->getAttrStr("gic_htmleditor","false");
	
	AosXmlTagPtr contentsXml = vpd->getFirstChild("contents");
	OmnString contents = ""; 
	if(contentsXml)
	{
		contents = contentsXml->getNodeText();
		contents.replace("\'", "|&|", true);
		contents.replace("\"", "|&&|", true);
	}

	//code.mJson << ",varbd:\""<<contents<<"\"," ;
	code.mJson << ",varbd:\""<<"\"," ;
	code.mJson << "gic_fsize:\""<<gic_fsize<<"\"," ;
	code.mJson << "gic_txtaln:\""<<gic_txtaln<<"\"," ;
	code.mJson << "gic_outclr:\""<<gic_outclr<<"\"," ;
	code.mJson << "gic_outbgc:\""<<gic_outbgc<<"\"," ;
	code.mJson << "gic_opt:\""<<gic_opt<<"\"," ;
	code.mJson << "gic_ftstyle:\""<<gic_ftstyle<<"\"," ;
	code.mJson << "gic_clk:"<<gic_clk<<"," ;
	code.mJson << "gic_overul:"<<gic_overul<<"," ;
	code.mJson << "gic_overclr:\""<<gic_overclr<<"\"," ;
	code.mJson << "gic_overbgc:\""<<gic_overbgc<<"\"," ;
	code.mJson << "gic_downclr:\""<<gic_downclr<<"\"," ;
	code.mJson << "gic_downbclr:\""<<gic_downbclr<<"\"," ;
	code.mJson << "gic_upclr:\""<<gic_upclr<<"\"," ;
	code.mJson << "gic_upbclr:\""<<gic_upbclr<<"\"," ;
	code.mJson << "gic_valbd:\""<<gic_valbd<<"\"," ;
	code.mJson << "gic_fixh:"<<gic_fixh<<"," ;
	code.mJson << "gic_movable:"<<gic_movable<<"," ;
	code.mJson << "gic_vertSb:"<<gic_vertSb<<"," ;
	code.mJson << "gic_outbgcbd:\""<<gic_outbgcbd<<"\"," ;
	code.mJson << "gic_sticky:\""<<gic_sticky<<"\"," ;
	code.mJson << "gic_linespace:\""<<gic_linespace<<"\"," ;
	code.mJson << "gic_cline:"<<gic_cline<<"," ;
	code.mJson << "gic_htmleditor:"<<gic_htmleditor<<"," ;
	code.mJson << "gic_useHtmlcmp:"<<gic_useHtmlcmp;
	return true;
}

