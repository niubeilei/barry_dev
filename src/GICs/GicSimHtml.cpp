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
// 2011/02/19 Created by Ken
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicSimHtml.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlModules/DictionaryHandler.h"

AosGicSimHtml::AosGicSimHtml(const bool flag)
:
AosGic(AOSGIC_SIMHTML, AosGicType::eSimHtml, flag)
{
}

AosGicSimHtml::~AosGicSimHtml()
{
}

bool	
AosGicSimHtml::generateCode(
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
	
	//Attribute
	OmnString gic_type = vpd->getAttrStr("gic_type","gic_sim_html");
	OmnString gic_fsize = vpd->getAttrStr("gic_fsize","12");
	OmnString gic_txtaln = vpd->getAttrStr("gic_txtaln","left");
	OmnString gic_outclr = vpd->getAttrStr("gic_outclr","#000000");
	OmnString gic_outbgc = vpd->getAttrStr("gic_outbgc","transparent");
	OmnString gic_ftstyle = vpd->getAttrStr("gic_ftstyle","normal");
	OmnString gic_clk = vpd->getAttrStr("gic_clk","false");
	OmnString gic_overul = vpd->getAttrStr("gic_overul","false");
	OmnString gic_overclr = vpd->getAttrStr("gic_overclr","#000000");
	OmnString gic_overbgc = vpd->getAttrStr("gic_overbgc","transparent");
	OmnString gic_downclr = vpd->getAttrStr("gic_downclr","#000000");
	OmnString gic_downbclr = vpd->getAttrStr("gic_downbgc","transparent");
	OmnString gic_upclr = vpd->getAttrStr("gic_upclr","#000000");
	OmnString gic_upbclr = vpd->getAttrStr("gic_upbclr","transparent");
	OmnString gic_valbd = vpd->getAttrStr("gic_valbd","");
	OmnString gic_fixh = vpd->getAttrStr("gic_fixh","false");
	OmnString gic_sticky = vpd->getAttrStr("gic_sticky","true");
	OmnString gic_vertSb = vpd->getAttrStr("gic_vertSb","false");
	OmnString gic_outbgcbd = vpd->getAttrStr("gic_outbgcbd","@outbgcbd");
	OmnString gic_linespace = vpd->getAttrStr("gic_linespace");
	OmnString gic_paging = vpd->getAttrStr("gic_paging","$");
	OmnString gic_cline = vpd->getAttrStr("gic_cline","true");
	OmnString gic_txtalnh = vpd->getAttrStr("gic_txtalnh", "left");
	OmnString gic_txtalnv = vpd->getAttrStr("gic_txtalnv", "top");
	OmnString gic_href = vpd->getAttrStr("gic_href", "");
	
	//text
	AosXmlTagPtr contents_tag = vpd->getFirstChild("contents");
	OmnString cont;
	if(contents_tag)
	{
		cont = contents_tag->getNodeText();
	}
	int f1 = cont.findSubString("|&&|", 0);
	int f2 = cont.findSubString("|&|", 0);
	if(f1 != -1)
	{
		cont.replace("|&&|", "\"", true);
	}
	if(f2 != -1){
		cont.replace("|&|", "\'", true);
	}
	OmnString eelStyle = vpd->getAttrStr("eelStyle");

	//mutil-language
	OmnString langtype = htmlPtr->getLanguageType(); 
	OmnString langcode = AosDictionaryHandler::matchLangType(langtype);
	if(langcode != "")
	{
		OmnString label = AosDictionaryHandler::getDictionary("contents/_$text", vpd, langcode, htmlPtr->getRundata());
		if(label != "")
		{
			cont= label;
		}
	}
	
	f1 = cont.findSubString("&#xa;", 0);
	if(f1 != -1)
	{
		cont.replace("&#xa;", "<br>", true);
	}
	if(eelStyle == "")
	{
		eelStyle << "word-wrap:break-word;word-break:break-all;";
	}
	code.mHtml << "<div style=\"" << eelStyle << "\">" << cont << "</div>";

	//Json	
	code.mJson << ",gic_fsize:\""<<gic_fsize<<"\"," ;
	code.mJson << "gic_txtaln:\""<<gic_txtaln<<"\"," ;
	code.mJson << "gic_txtalnh:\""<<gic_txtalnh<<"\"," ;
	code.mJson << "gic_txtalnv:\""<<gic_txtalnv<<"\"," ;
	code.mJson << "gic_outclr:\""<<gic_outclr<<"\"," ;
	code.mJson << "gic_outbgc:\""<<gic_outbgc<<"\"," ;
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
	code.mJson << "gic_vertSb:"<<gic_vertSb<<"," ;
	code.mJson << "gic_outbgcbd:\""<<gic_outbgcbd<<"\"," ;
	code.mJson << "gic_sticky:\""<<gic_sticky<<"\"," ;
	code.mJson << "gic_linespace:\""<<gic_linespace<<"\"," ;
	code.mJson << "gic_paging:\""<<gic_paging<<"\"," ;
	code.mJson << "gic_href:\""<<gic_href<<"\"," ;
	code.mJson << "gic_cline:"<<gic_cline;
	
	return true;
}
