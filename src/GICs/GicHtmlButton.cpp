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
// 07/08/2010: Created by Cody
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlButton.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlModules/DictionaryHandler.h"


AosGicHtmlButton::AosGicHtmlButton(const bool flag)
:
AosGic(AOSGIC_HTMLBUTTON, AosGicType::eHtmlButton, flag)
{
}


AosGicHtmlButton::~AosGicHtmlButton()
{
}


bool	
AosGicHtmlButton::generateCode(
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
	// 	
	// 	html:
	// 	<div id="div1" style="position">
	//  	<input type="button" text="gic_value" ></input>
	//  </div>
	//	JSOn:
	//	{
	//		el : 'gic_el',
	//		xtype : 'gic_button',
	//		x : gic_x,
	//		y : gic_y, 
	//		text : 'gic_value',
	//		width : gic_width,
	//		height : gic_height,
	//      paneid : paneid,
	//      panelid : panelid
	//	}
	//	code.mCss
	OmnString gic_eleid = AosGetHtmlElemId();
	OmnString cls_name, styleStr;
	cls_name << AOSGIC_CLS_BUTTON << "-" << gic_eleid;

	int bt_fsize = vpd->getAttrInt("bt_fsize", -1);
	if(bt_fsize > 0) styleStr << "font-size:" << bt_fsize << "px;";
	
	OmnString bt_color = AosHtmlUtil::getWebColor(vpd->getAttrStr("bt_color"));
	if(bt_color != "") styleStr << "color:" << bt_color << ";";
	
	OmnString bt_fstyle = vpd->getAttrStr("bt_fstyle");
	if (bt_fstyle == "bold") {
		styleStr << "font-weight:bold;";
	} else if (bt_fstyle == "italic") {
		styleStr << "font-style:italic;";
	} else if (bt_fstyle == "bolditalic") {
		styleStr << "font-weight:bold;font-style:italic;";
	}
	
	OmnString css;
	css << "." << cls_name << "{" << styleStr << "}";

	bool gic_hint = vpd->getAttrBool("gic_hint", false);
	OmnString gic_hintvalue = vpd->getAttrStr("gic_hintvalue");
	OmnString gic_hintbd = vpd->getAttrStr("gic_hintbd");
	
	OmnString bt_query = vpd->getAttrStr("bt_query");
	OmnString bt_label = vpd->getAttrStr("bt_label", "Button");
	
	//mutil-language
	OmnString langtype = htmlPtr->getLanguageType(); 
	OmnString langcode = AosDictionaryHandler::matchLangType(langtype);
	if(langcode != "")
	{
		OmnString label = AosDictionaryHandler::getDictionary("bt_label", vpd, langcode, htmlPtr->getRundata());
		if(label != "") bt_label = label;
	}
	
	int gic_width = vpd->getAttrInt("gic_width", 200);
	int gic_height = vpd->getAttrInt("gic_height", 26);
	
	OmnString gic_self_Cls = vpd->getAttrStr("gic_self_Cls");

	OmnString html;
	html << "<input class=\"" << AOSGIC_CLS_BUTTON << " "
		 << (gic_hint ? AOSGIC_CLS_BUTTON_HINT : "") << " "
		 << cls_name << " " << gic_self_Cls << "\" "
		 << "style=\"width:" << gic_width << "px;height:" << gic_height << "px;\" "
		 << "type=\"button\" value=\"" << bt_label << "\" />";

	OmnString json;
	json << ","
		 << "gic_self_Cls:\"" << gic_self_Cls << "\","
		 << "gic_eleid:\"" << gic_eleid << "\","
		 << "bt_query:\"" << bt_query << "\","
		 << "bt_label:\"" << bt_label << "\","
		 << "bt_fsize:" << bt_fsize << ","
		 << "bt_fstyle:\"" << bt_fstyle << "\","
		 << "bt_color:\"" << bt_color << "\","
		 << "gic_hint:" << gic_hint << ","
		 << "gic_hintvalue:\"" << gic_hintvalue << "\","
		 << "gic_hintbd:\"" << gic_hintbd << "\"";
	
	code.mHtml << html;
	code.mCss << css;
	code.mJson << json;

	return true;
}

