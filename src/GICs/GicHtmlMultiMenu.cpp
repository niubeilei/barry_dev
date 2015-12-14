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
// 2011/12/10: Created by Ken
////////////////////////////////////////////////////////////////////////////

#include "GICs/GicHtmlMultiMenu.h"

#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


AosGicHtmlMultiMenu::AosGicHtmlMultiMenu(const bool flag)
:
AosGic(AOSGIC_HTMLMULTIMENU, AosGicType::eHtmlMultiMenu, flag)
{
}


AosGicHtmlMultiMenu::~AosGicHtmlMultiMenu()
{
}


bool	
AosGicHtmlMultiMenu::generateCode(
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
	OmnString data = vpd->getNodeText("contents");
	AosXmlParser parsers;
	AosXmlTagPtr records = parsers.parse(data, "" AosMemoryCheckerArgs);
	aos_assert_r(records, false);
	records = records->getFirstChild();
	aos_assert_r(records, false);

	int width = vpd->getAttrInt("gic_width", 200);
	int height = vpd->getAttrInt("gic_height", 18);
	
	AosXmlTagPtr record = records->getFirstChild();
	int recNum = records->getNumSubtags();
	int recWidth = (width - 2) / recNum;

	OmnString path;
	OmnString http = "http";

	OmnString bgstr = "background:";
	OmnString gic_menubgColor = vpd->getAttrStr("gic_menubgColor", "#CCCCD0");
	gic_menubgColor = AosHtmlUtil::getWebColor(gic_menubgColor);
	bgstr << gic_menubgColor;
	OmnString gic_menubgImg = vpd->getAttrStr("gic_menubgImg", "");
	if(gic_menubgImg != "")
	{
		if(gic_menubgImg.indexOf(http, 0) == 0) path = "";
		else path = htmlPtr->getImagePath();
		bgstr << " url(" << path << gic_menubgImg << ")";
	}
	bgstr << ";";

	OmnString fontstr = "font:";
	OmnString gic_menuFstyle = vpd->getAttrStr("gic_menuFstyle", "plain");
	if(gic_menuFstyle == "italic" || gic_menuFstyle == "bolditalic") fontstr << "italic ";
	if(gic_menuFstyle == "bold" || gic_menuFstyle == "bolditalic") fontstr << "bold ";
	int gic_menuFsize = vpd->getAttrInt("gic_menuFsize", 12);
	fontstr << gic_menuFsize << "px/" << height << "px Arial;";
	OmnString gic_menuFcolor = vpd->getAttrStr("gic_menuFcolor", "#000000");
	gic_menuFcolor = AosHtmlUtil::getWebColor(gic_menuFcolor);
	fontstr << "color:" << gic_menuFcolor << ";";
	
	OmnString overbgstr = "background:";
	OmnString gic_menuOverbgColor = vpd->getAttrStr("gic_menuOverbgColor", "#C0C0C8");
	gic_menuOverbgColor = AosHtmlUtil::getWebColor(gic_menuOverbgColor);
	overbgstr << gic_menuOverbgColor;
	OmnString gic_menuOverbgImg = vpd->getAttrStr("gic_menuOverbgImg", "");
	if(gic_menuOverbgImg != "")
	{
		if(gic_menuOverbgImg.indexOf(http, 0) == 0) path = "";
		else path = htmlPtr->getImagePath();
		overbgstr << " url(" << path << gic_menuOverbgImg << ")";
	}
	overbgstr << ";";

	OmnString overfontstr = "font:";
	OmnString gic_menuOverFstyle = vpd->getAttrStr("gic_menuOverFstyle", "plain");
	if(gic_menuOverFstyle == "italic" || gic_menuOverFstyle == "bolditalic") overfontstr << "italic ";
	if(gic_menuOverFstyle == "bold" || gic_menuOverFstyle == "bolditalic") overfontstr << "bold ";
	int gic_menuOverFsize = vpd->getAttrInt("gic_menuOverFsize", 12);
	overfontstr << gic_menuOverFsize << "px/" << height << "px Arial;";
	OmnString gic_menuOverFcolor = vpd->getAttrStr("gic_menuOverFcolor", "#CC6600");
	gic_menuOverFcolor = AosHtmlUtil::getWebColor(gic_menuOverFcolor);
	overfontstr << "color:" << gic_menuOverFcolor << ";";

	OmnString selectbgstr = "background:";
	OmnString gic_menuSelectbgColor = vpd->getAttrStr("gic_menuSelectbgColor", "#B0B0B0");
	gic_menuSelectbgColor = AosHtmlUtil::getWebColor(gic_menuSelectbgColor);
	selectbgstr << gic_menuSelectbgColor;
	OmnString gic_menuSelectbgImg = vpd->getAttrStr("gic_menuSelectbgImg", "");
	if(gic_menuSelectbgImg != "")
	{
		if(gic_menuSelectbgImg.indexOf(http, 0) == 0) path = "";
		else path = htmlPtr->getImagePath();
		selectbgstr << " url(" << path << gic_menuSelectbgImg << ")";
	}
	selectbgstr << " !important;";

	OmnString selectfontstr = "font:";
	OmnString gic_menuSelectFstyle = vpd->getAttrStr("gic_menuSelectFstyle", "plain");
	if(gic_menuSelectFstyle == "italic" || gic_menuSelectFstyle == "bolditalic") selectfontstr << "italic ";
	if(gic_menuSelectFstyle == "bold" || gic_menuSelectFstyle == "bolditalic") selectfontstr << "bold ";
	int gic_menuSelectFsize = vpd->getAttrInt("gic_menuSelectFsize", 12);
	selectfontstr << gic_menuSelectFsize << "px/" << height << "px Arial !important;";
	OmnString gic_menuSelectFcolor = vpd->getAttrStr("gic_menuSelectFcolor", "#CC6600");
	gic_menuSelectFcolor = AosHtmlUtil::getWebColor(gic_menuSelectFcolor);
	selectfontstr << "color:" << gic_menuSelectFcolor << " !important;";

	OmnString floatstr = "float:";
	OmnString list_float = vpd->getAttrStr("list_float", "vert");
	if(list_float == "hori") floatstr << "left;";
	else floatstr << "none;";

	OmnString css_name, css;
	css_name << "html_multimenu_" << AosGetHtmlElemId();
	css << "." << css_name << "{display:inline-block;width:auto;border-color:#EEE #555 #222 #FFF;border-style:solid;border-width:1px;}"
		<< "." << css_name << " li{display:block;float:left;position:relative;width:" << recWidth << "px;height:" << height << "px;}"
		<< "." << css_name << " li a{word-wrap:break-word;width:100%;height:100%;position:absolute;left:0px;top:0px;text-align:center;text-decoration:none;" << bgstr << fontstr << "}"
		<< "." << css_name << " li a:hover{" << overbgstr << overfontstr << "}"
		<< "." << css_name << " div{position:absolute;left:" << (recWidth - 10) << "px;width:10px;height:100%}";
	css << "." << css_name << "_level{position:absolute;display:none;z-index:60000;}"
		<< "." << css_name << "_level li{" << floatstr << "}";
	css << "." << css_name << "_selected{"<< selectbgstr << selectfontstr << "}";

	OmnString html;
	html << "<ul class=\"" << css_name << "\">";	
	OmnString zky_name;
	while(record)
	{
		zky_name = record->getAttrStr("zky_name", "");
		html << "<li><a href=\"#\">" << zky_name << "</a><div>&nbsp;</div></li>";
		record = records->getNextChild();
	}
	html << "</ul>";

	code.mHtml << html;
	code.mCss << css;

	OmnString trigger_event = vpd->getAttrStr("trigger_event", "mouse");

	code.mJson << ", "
			   << "css_name:\"" << css_name << "\", "
			   << "list_float:\"" << list_float << "\" ,"
			   << "trigger_event:\"" << trigger_event << "\", "

			   << "gic_menubgColor:\"" << gic_menubgColor << "\", "
			   << "gic_menubgImg:\"" << gic_menubgImg << "\", "
			   << "gic_menuFstyle:\"" << gic_menuFstyle << "\", "
			   << "gic_menuFsize:\"" << gic_menuFsize << "\", "
			   << "gic_menuFcolor:\"" << gic_menuFcolor << "\", "

			   << "gic_menuOverbgColor:\"" << gic_menuOverbgColor << "\", "
			   << "gic_menuOverbgImg:\"" << gic_menuOverbgImg << "\", "
			   << "gic_menuOverFstyle:\"" << gic_menuOverFstyle << "\", "
			   << "gic_menuOverFsize:\"" << gic_menuOverFsize << "\", "
			   << "gic_menuOverFcolor:\"" << gic_menuOverFcolor << "\", "
			   
			   << "gic_menuSelectbgColor:\"" << gic_menuSelectbgColor << "\", "
			   << "gic_menuSelectbgImg:\"" << gic_menuSelectbgImg << "\", "
			   << "gic_menuSelectFstyle:\"" << gic_menuSelectFstyle << "\", "
			   << "gic_menuSelectFsize:\"" << gic_menuSelectFsize << "\", "
			   << "gic_menuSelectFcolor:\"" << gic_menuSelectFcolor << "\" ";
			   
	return true;
}


