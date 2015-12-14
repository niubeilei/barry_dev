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
// 07/25/2010: Created by Michael 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlCmp.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicHtmlCmp();

AosGicHtmlCmp::AosGicHtmlCmp(const bool flag)
:
AosGic(AOSGIC_HTMLCMP, AosGicType::eHtmlCmp, flag)
{
}


AosGicHtmlCmp::~AosGicHtmlCmp()
{
}


bool	
AosGicHtmlCmp::generateCode(
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

	//code.mJson << ",gic_nmmg: \"" << vpd->getAttrStr("gic_nmmg","a3/es1073.jpg") 
    code.mHtml <<"<div id=\"test\" style=\"display:none\"></div>";
		 /*<<"<script type=\"text/javascript\" src=\"/lps-4.7.2/prod/Ext/GICs/ajaxx.js\"></script>"
		 <<"<div id=\"city\" style=\"display:none\">"<<*/
		//"<script language=\"JavaScript\" type=\"text/JavaScript\" src=\"http://api.liqwei.com/location/?js=1\">"<<
		//"</script></div";
//		"</div";
	code.mJson <<",gic_copy:"<< vpd->getAttrStr("gic_copy","true")
			   <<",gic_save:"<< vpd->getAttrStr("gic_save","true")
			   <<",gic_layers:"<< vpd->getAttrStr("gic_layers","false")
			   <<",gic_data:"<< vpd->getAttrStr("gic_data","false")
			   <<",gic_divwidth:"<< vpd->getAttrStr("gic_divwidth","true")
			   <<",gic_divheight:"<< vpd->getAttrStr("gic_divheight","true")
			   <<",gic_overflow:"<< vpd->getAttrStr("gic_overflow","false")
			   //<<",gic_leftimg:"<< vpd->getAttrStr("gic_leftimg","false")
			   //<<",gic_rightimg:"<< vpd->getAttrStr("gic_rightimg","false")
			   <<",gic_imgchk:"<< vpd->getAttrStr("gic_imgchk","false")
			   <<",gic_transparent:"<< vpd->getAttrStr("gic_transparent","true")
			   <<",gic_mouseevt:"<< vpd->getAttrStr("gic_mouseevt","false")
			   <<",gic_background:"<< vpd->getAttrStr("gic_background","true")
			   <<",gic_backgroundc:\""<< vpd->getAttrStr("gic_backgroundc","#aaccff")<<"\""
			   <<",gic_backgroundp:\""<< vpd->getAttrStr("gic_backgroundp","1")<<"\""
			   <<",gic_titlename:\""<< vpd->getAttrStr("gic_titlename","HtmlComposer")<<"\""
			   <<",gic_cancel:"<< vpd->getAttrStr("gic_cancel","false")
			   <<",gic_simulate:"<< vpd->getAttrStr("gic_simulate","true")
			   <<",gic_delete:"<< vpd->getAttrStr("gic_delete","true")
			   <<",gic_weatherpan:"<< vpd->getAttrStr("gic_weatherpan","false")
			   <<",gic_formpan:"<< vpd->getAttrStr("gic_formpan","true")
			   <<",gic_imgpan:"<< vpd->getAttrStr("gic_imgpan","true")
			   <<",gic_editorh:\'"<< vpd->getAttrStr("gic_editorh")<<"\'"
			   <<",gic_editorw:\'"<< vpd->getAttrStr("gic_editorw")<<"\'"
			   <<",gic_autoWH:"<< vpd->getAttrStr("gic_autoWH","true")
			   <<",gic_selall:"<< vpd->getAttrStr("gic_selall","true")
			   <<",gic_property:\'"<< vpd->getAttrStr("gic_property","normal")<<"\'"
			   <<",gic_add:"<< vpd->getAttrStr("gic_add","true")
			   <<",gic_toolshow:"<< vpd->getAttrStr("gic_toolshow","true")
			   <<",gic_paneshow:"<< vpd->getAttrStr("gic_paneshow","true")
			   <<",gic_bgwh:"<< vpd->getAttrStr("gic_bgwh","false")
			   <<",gic_autosize:"<< vpd->getAttrStr("gic_autosize","false")
			   <<",gic_databd:"<< vpd->getAttrStr("gic_databd","false");

	return true;
}

