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

#include "GICs/GicHtmlTwoLevelNav.h"

#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


AosGicHtmlTwoLevelNav::AosGicHtmlTwoLevelNav(const bool flag)
:
AosGic(AOSGIC_HTMLTWOLEVELNAV, AosGicType::eHtmlTwoLevelNav, flag)
{
}


AosGicHtmlTwoLevelNav::~AosGicHtmlTwoLevelNav()
{
}


bool	
AosGicHtmlTwoLevelNav::generateCode(
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

	int width = vpd->getAttrInt("gic_width", 600);
	int height = vpd->getAttrInt("gic_height", 70);
	int first_li_width = vpd->getAttrInt("gic_first_li_width", 90);
	int second_li_width = vpd->getAttrInt("gic_second_li_width", 90);
	int second_height = vpd->getAttrInt("gic_second_height", 35);
	int first_height = height - second_height;
	if(first_height < 1) first_height = 1;
	
	OmnString imgpath = htmlPtr->getImagePath();;

	OmnString first_bgstr = "background:";
	OmnString first_bgColor = vpd->getAttrStr("gic_first_bgColor", "#FF6400");
	first_bgColor = AosHtmlUtil::getWebColor(first_bgColor);
	first_bgstr << first_bgColor;
	OmnString first_bgImg = vpd->getAttrStr("gic_first_bgImg", "");
	first_bgstr << parseImgPath(first_bgImg, imgpath) << ";";
	
	OmnString second_bgstr = "background:";
	OmnString second_bgColor = vpd->getAttrStr("gic_second_bgColor", "#F7F7F7");
	second_bgColor = AosHtmlUtil::getWebColor(second_bgColor);
	second_bgstr << second_bgColor;
	OmnString second_bgImg = vpd->getAttrStr("gic_second_bgImg", "");
	second_bgstr << parseImgPath(second_bgImg, imgpath) << ";";
	
	OmnString first_li_bgstr = "background:";
	OmnString first_li_bgColor = vpd->getAttrStr("gic_first_li_bgColor", "#FF6400");
	first_li_bgColor = AosHtmlUtil::getWebColor(first_li_bgColor);
	first_li_bgstr << first_li_bgColor;
	OmnString first_li_bgImg = vpd->getAttrStr("gic_first_li_bgImg", "");
	first_li_bgstr << parseImgPath(first_li_bgImg, imgpath) << ";";

	OmnString first_li_fontstr = "font:";
	OmnString first_li_Fstyle = vpd->getAttrStr("gic_first_li_Fstyle", "plain");
	if(first_li_Fstyle == "italic" || first_li_Fstyle == "bolditalic") first_li_fontstr << "italic ";
	if(first_li_Fstyle == "bold" || first_li_Fstyle == "bolditalic") first_li_fontstr << "bold ";
	int first_li_Fsize = vpd->getAttrInt("gic_first_li_Fsize", 12);
	first_li_fontstr << first_li_Fsize << "px/" << first_height << "px Arial;";
	OmnString first_li_Fcolor = vpd->getAttrStr("gic_first_li_Fcolor", "#FFFFFF");
	first_li_Fcolor = AosHtmlUtil::getWebColor(first_li_Fcolor);
	first_li_fontstr << "color:" << first_li_Fcolor << ";";
	
	OmnString first_li_over_bgstr = "background:";
	OmnString first_li_over_bgColor = vpd->getAttrStr("gic_first_li_over_bgColor", "#A0A0A0");
	first_li_over_bgColor = AosHtmlUtil::getWebColor(first_li_over_bgColor);
	first_li_over_bgstr << first_li_over_bgColor;
	OmnString first_li_over_bgImg = vpd->getAttrStr("gic_first_li_over_bgImg", "");
	first_li_over_bgstr << parseImgPath(first_li_over_bgImg, imgpath) << ";";

	OmnString first_li_over_fontstr = "font:";
	OmnString first_li_over_Fstyle = vpd->getAttrStr("gic_first_li_over_Fstyle", "plain");
	if(first_li_over_Fstyle == "italic" || first_li_over_Fstyle == "bolditalic") first_li_over_fontstr << "italic ";
	if(first_li_over_Fstyle == "bold" || first_li_over_Fstyle == "bolditalic") first_li_over_fontstr << "bold ";
	int first_li_over_Fsize = vpd->getAttrInt("gic_first_li_over_Fsize", 12);
	first_li_over_fontstr << first_li_over_Fsize << "px/" << first_height << "px Arial;";
	OmnString first_li_over_Fcolor = vpd->getAttrStr("gic_first_li_over_Fcolor", "#000000");
	first_li_over_Fcolor = AosHtmlUtil::getWebColor(first_li_over_Fcolor);
	first_li_over_fontstr << "color:" << first_li_over_Fcolor << ";";
	
	OmnString second_li_bgstr = "background:";
	OmnString second_li_bgColor = vpd->getAttrStr("gic_second_li_bgColor", "#F7F7F7");
	second_li_bgColor = AosHtmlUtil::getWebColor(second_li_bgColor);
	second_li_bgstr << second_li_bgColor;
	OmnString second_li_bgImg = vpd->getAttrStr("gic_second_li_bgImg", "");
	second_li_bgstr << parseImgPath(second_li_bgImg, imgpath) << ";";

	OmnString second_li_fontstr = "font:";
	OmnString second_li_Fstyle = vpd->getAttrStr("gic_second_li_Fstyle", "plain");
	if(second_li_Fstyle == "italic" || second_li_Fstyle == "bolditalic") second_li_fontstr << "italic ";
	if(second_li_Fstyle == "bold" || second_li_Fstyle == "bolditalic") second_li_fontstr << "bold ";
	int second_li_Fsize = vpd->getAttrInt("gic_second_li_Fsize", 12);
	second_li_fontstr << second_li_Fsize << "px/" << second_height << "px Arial;";
	OmnString second_li_Fcolor = vpd->getAttrStr("gic_second_li_Fcolor", "#000000");
	second_li_Fcolor = AosHtmlUtil::getWebColor(second_li_Fcolor);
	second_li_fontstr << "color:" << second_li_Fcolor << ";";
	
	OmnString second_li_over_bgstr = "background:";
	OmnString second_li_over_bgColor = vpd->getAttrStr("gic_second_li_over_bgColor", "#B0B0B0");
	second_li_over_bgColor = AosHtmlUtil::getWebColor(second_li_over_bgColor);
	second_li_over_bgstr << second_li_over_bgColor;
	OmnString second_li_over_bgImg = vpd->getAttrStr("gic_second_li_over_bgImg", "");
	second_li_over_bgstr << parseImgPath(second_li_over_bgImg, imgpath) << ";";

	OmnString second_li_over_fontstr = "font:";
	OmnString second_li_over_Fstyle = vpd->getAttrStr("gic_second_li_over_Fstyle", "plain");
	if(second_li_over_Fstyle == "italic" || second_li_over_Fstyle == "bolditalic") second_li_over_fontstr << "italic ";
	if(second_li_over_Fstyle == "bold" || second_li_over_Fstyle == "bolditalic") second_li_over_fontstr << "bold ";
	int second_li_over_Fsize = vpd->getAttrInt("gic_second_li_over_Fsize", 12);
	second_li_over_fontstr << second_li_over_Fsize << "px/" << second_height << "px Arial;";
	OmnString second_li_over_Fcolor = vpd->getAttrStr("gic_second_li_over_Fcolor", "#FFFFFF");
	second_li_over_Fcolor = AosHtmlUtil::getWebColor(second_li_over_Fcolor);
	second_li_over_fontstr << "color:" << second_li_over_Fcolor << ";";
	
	OmnString second_li_select_bgstr = "background:";
	OmnString second_li_select_bgColor = vpd->getAttrStr("gic_second_li_select_bgColor", "#B0B0B0");
	second_li_select_bgColor = AosHtmlUtil::getWebColor(second_li_select_bgColor);
	second_li_select_bgstr << second_li_select_bgColor;
	OmnString second_li_select_bgImg = vpd->getAttrStr("gic_second_li_select_bgImg", "");
	second_li_select_bgstr << parseImgPath(second_li_select_bgImg, imgpath) << " !important;";

	OmnString second_li_select_fontstr = "font:";
	OmnString second_li_select_Fstyle = vpd->getAttrStr("gic_second_li_select_Fstyle", "plain");
	if(second_li_select_Fstyle == "italic" || second_li_select_Fstyle == "bolditalic") second_li_select_fontstr << "italic ";
	if(second_li_select_Fstyle == "bold" || second_li_select_Fstyle == "bolditalic") second_li_select_fontstr << "bold ";
	int second_li_select_Fsize = vpd->getAttrInt("gic_second_li_select_Fsize", 12);
	second_li_select_fontstr << second_li_select_Fsize << "px/" << second_height << "px Arial;";
	OmnString second_li_select_Fcolor = vpd->getAttrStr("gic_second_li_select_Fcolor", "#CC6600");
	second_li_select_Fcolor = AosHtmlUtil::getWebColor(second_li_select_Fcolor);
	second_li_select_fontstr << "color:" << second_li_select_Fcolor << " !important;";
	
	int first_split_width = vpd->getAttrInt("gic_first_split_width", 5);
	OmnString first_split_bgstr = "background:";
	OmnString first_split_bgColor = vpd->getAttrStr("gic_first_split_bgColor", "#FF6400");
	first_split_bgColor = AosHtmlUtil::getWebColor(first_split_bgColor);
	first_split_bgstr << first_split_bgColor;
	OmnString first_split_bgImg = vpd->getAttrStr("gic_first_split_bgImg", "");
	first_split_bgstr << parseImgPath(first_split_bgImg, imgpath) << ";";
	
	int second_split_width = vpd->getAttrInt("gic_second_split_width", 5);
	OmnString second_split_bgstr = "background:";
	OmnString second_split_bgColor = vpd->getAttrStr("gic_second_split_bgColor", "#F7F7F7");
	second_split_bgColor = AosHtmlUtil::getWebColor(second_split_bgColor);
	second_split_bgstr << second_split_bgColor;
	OmnString second_split_bgImg = vpd->getAttrStr("gic_second_split_bgImg", "");
	second_split_bgstr << parseImgPath(second_split_bgImg, imgpath) << ";";
		
	int first_padding_left = vpd->getAttrInt("gic_first_padding_left", 20);
	int second_padding_left = vpd->getAttrInt("gic_second_padding_left", 30);
	
	OmnString css_name, first_css_name, second_css_name, css;
	css_name << "html_twolevelnav_" << AosGetHtmlElemId();
	first_css_name << css_name << "_first";
	second_css_name << css_name << "_second";
	
	css	<< "." << first_css_name << "{display:inline-block;width:auto;position:relative;left:" << first_padding_left << "px;}"
		<< "." << first_css_name << " li{display:block;float:left;}"
		<< "." << first_css_name << "_split{width:" << first_split_width << "px;height:" << first_height << "px;" << first_split_bgstr << "}"
		<< "." << first_css_name << "_a{width:" << first_li_width << "px;height:" << first_height << "px;cursor:pointer;display:block;float:left;word-wrap:break-word;text-align:center;text-decoration:none;" << first_li_bgstr << first_li_fontstr << "}"
		<< "." << first_css_name << "_over {" << first_li_over_bgstr << first_li_over_fontstr << "}";
	
	css	<< "." << second_css_name << "{display:inline-block;width:auto;position:relative;left:" << second_padding_left << "px;}"
		<< "." << second_css_name << " li{display:block;float:left;}"
		<< "." << second_css_name << "_split{width:" << second_split_width << "px;height:" << second_height << "px;" << second_split_bgstr << "}"
		<< "." << second_css_name << "_a{width:" << second_li_width << "px;height:" << second_height << "px;cursor:pointer;display:block;float:left;word-wrap:break-word;text-align:center;text-decoration:none;" << second_li_bgstr << second_li_fontstr << "}"
		<< "." << second_css_name << "_over {" << second_li_over_bgstr << second_li_over_fontstr << "}"
		<< "." << second_css_name << "_select {" << second_li_select_bgstr << second_li_select_fontstr << "}";


	OmnString first_div, second_div;
	first_div << "<div style=\"width:" << width << "px;height:" << first_height << "px;" << first_bgstr << "\">"
		 	  << "<ul class=\"" << first_css_name << "\">";
	second_div << "<div style=\"width:" << width << "px;height:" << second_height << "px;" << second_bgstr << "\">";
	
	int recNum = records->getNumSubtags();
	if(recNum == 0) return true;

	AosXmlTagPtr record = records->getFirstChild();
	OmnString zky_name;
	
	zky_name = record->getAttrStr("zky_name", "");
	first_div << "<li><a class=\"" << first_css_name << "_a " << first_css_name << "_over\" href=\"javascript:void(0);\">" << zky_name << "</a></li>";
	
	OmnString sec_ul;
	sec_ul << "<ul class=\"" << second_css_name << "\">";
	AosXmlTagPtr subRec = record->getFirstChild("record");
	while(subRec)
	{
		zky_name = subRec->getAttrStr("zky_name", "");
		sec_ul << "<li><a class=\"" << second_css_name << "_a " << second_css_name << "\" href=\"javascript:void(0);\">" << zky_name << "</a></li>";
		subRec = record->getNextChild("record");
		if(subRec) sec_ul << "<li class=\"" << second_css_name << "_split\"></li>"; 
	}
	sec_ul << "</ul>";
	second_div << sec_ul;
	
	record = records->getNextChild();
	for(int i=1; i<recNum; i++)
	{
		if(record) first_div << "<li class=\"" << first_css_name << "_split\"></li>"; 
		zky_name = record->getAttrStr("zky_name", "");
		first_div << "<li><a class=\"" << first_css_name << "_a\" href=\"javascript:void(0);\">" << zky_name << "</a></li>";
		
		sec_ul = "";
		sec_ul << "<ul class=\"" << second_css_name << "\" style=\"display:none;\">";
		subRec = record->getFirstChild("record");
		while(subRec)
		{
			zky_name = subRec->getAttrStr("zky_name", "");
			sec_ul << "<li><a class=\"" << second_css_name << "_a " << second_css_name << "\" href=\"javascript:void(0);\">" << zky_name << "</a></li>";
			subRec = record->getNextChild("record");
			if(subRec) sec_ul << "<li class=\"" << second_css_name << "_split\"></li>"; 
		}
		sec_ul << "</ul>";
		second_div << sec_ul;
		
		record = records->getNextChild();
	}
	first_div << "</ul></div>";
	second_div << "</div>";

	code.mHtml << first_div << second_div;
	code.mCss << css;
	
	OmnString jsonStr = ", ";
	jsonStr << "css_name:\"" << css_name << "\" ";
	code.mJson << jsonStr;
	return true;
}


OmnString
AosGicHtmlTwoLevelNav::parseImgPath(const OmnString &img, const OmnString &path)
{
	OmnString src = "";
	if(img == "") return src;
	src << " url(";
	OmnString http = "http://";
	if(img.indexOf(http, 0) != 0) src << path;
	src << img << ")";
	return src;
}
