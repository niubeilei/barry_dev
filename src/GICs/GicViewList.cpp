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
// 08/21/2010: Created by Cody
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicViewList.h"
#include "HtmlModules/DclDb.h"

#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicViewList();

AosGicViewList::AosGicViewList(const bool flag)
:
AosGic(AOSGIC_VIEWLIST, AosGicType::eViewList, flag)
{
}


AosGicViewList::~AosGicViewList()
{
}


bool	
AosGicViewList::generateCode(
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
	OmnString valbd = vpd->getAttrStr("gic_valued");
	OmnString oneRoll = vpd->getAttrStr("oneRoll", "false");
	OmnString multiSelect = vpd->getAttrStr("multiSelect", "true");
	OmnString tip_time = vpd->getAttrStr("tip_time", "5000");
	OmnString singleMove = vpd->getAttrStr("singleMove", "true");
	OmnString trashid = vpd->getAttrStr("trashid");
	OmnString gic_selattr = vpd->getAttrStr("gic_selattr");
	OmnString gic_selval = vpd->getAttrStr("gic_selval", "true");
	OmnString show_num = vpd->getAttrStr("showNum");
	OmnString use_control = vpd->getAttrStr("useControl", "false");
	OmnString gic_value;
	const OmnString str_datacol = "str_datacol";
	const OmnString db_datacol = "db_datacol"; 
	OmnString gic_id2 = AosGetHtmlElemId();
	OmnString objTemplate = vpd->getAttrStr("objTemplate");
	//OmnString gic_borderTop = vpd->getAttrStr("gic_borderTop", "none");
	//OmnString gic_borderBottom = vpd->getAttrStr("gic_borderBottom", "none");
	//OmnString gic_borderLeft = vpd->getAttrStr("gic_borderLeft", "none");
	//OmnString gic_borderRight = vpd->getAttrStr("gic_borderRight", "none");
	OmnString gic_itemsmove = vpd->getAttrStr("trackgroups", "");
	if(gic_itemsmove != "" && gic_itemsmove != "false")
	{
		gic_itemsmove = "true";
	}
	else
	{
		gic_itemsmove = "false";
	}
	OmnString gic_panelBorderTop = vpd->getAttrStr("gic_panelBorderTop", "none");
	OmnString gic_panelBorderBottom = vpd->getAttrStr("gic_panelBorderBottom", "none");
	OmnString gic_panelBorderLeft = vpd->getAttrStr("gic_panelBorderLeft", "none");
	OmnString gic_panelBorderRight = vpd->getAttrStr("gic_panelBorderRight", "none");
	OmnString tip_vpdname = vpd->getAttrStr("tip_vpdname");
	OmnString tip = vpd->getAttrStr("tip", "false");
	OmnString tip_width = vpd->getAttrStr("tip_width", "300");
	OmnString tip_height = vpd->getAttrStr("tip_height", "450");
	OmnString gic_draggroup1 = vpd->getAttrStr("gic_draggroup1");
	OmnString gic_dropgroup1 = vpd->getAttrStr("gic_dropgroup1");
	OmnString gic_draggroup2 = vpd->getAttrStr("gic_draggroup2");
	OmnString gic_dropgroup2 = vpd->getAttrStr("gic_dropgroup2");
	OmnString gic_vsp = vpd->getAttrStr("gic_vs", "2");
	OmnString gic_hsp = vpd->getAttrStr("gic_hs", "2");
	OmnString gic_roff = vpd->getAttrStr("gic_rinset", "2");
	OmnString gic_loff = vpd->getAttrStr("gic_linset", "2");
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn");
	OmnString gic_boff = vpd->getAttrStr("gic_binset", "2");
	OmnString gic_toff = vpd->getAttrStr("gic_tinset", "2");
	OmnString withoutBgC = vpd->getAttrStr("withoutBgC", "false");
	OmnString gic_time = vpd->getAttrStr("gic_cgpage_time", "false");
	OmnString gic_isVertical = vpd->getAttrStr("gic_isVertical","false");
	//是否显示滚动条
	OmnString gic_sb = vpd->getAttrStr("gic_scrollBar_v", "false");
	//底部的工具条
	OmnString gic_btbar = vpd->getAttrStr("gic_pb_def", "false");
	OmnString gic_rolling = vpd->getAttrStr("gic_w_anim", "0");
	//it doesn't word now
	gic_rolling = "0";
	OmnString cmp_fgcbd;
	OmnString cmp_value_bind;
	OmnString gic_html, field;

	AosXmlTagPtr datacol = vpd->getFirstChild("creators");
	AosXmlTagPtr celloverride = datacol->getFirstChild("celloverride");
	AosXmlTagPtr templates = datacol->getFirstChild("templates");
	OmnString celloverrideStr, templatesStr;
	if(celloverride)
	{
		celloverrideStr = (OmnString)celloverride->toString();
		int ftc = celloverrideStr.findSubString("\n", 0);
		int dbquot = celloverrideStr.findSubString("\"", 0);
		int quot = celloverrideStr.findSubString("\'", 0);
		if(ftc != -1)
		{
			celloverrideStr.replace("\n", "", true);
		}
		if(dbquot != -1)
		{
			celloverrideStr.replace("\"", "|&&|", true);
		}
		if(quot != -1)
		{
			celloverrideStr.replace("\'", "|&|", true);
		}
	}
	if(templates)
	{
		templatesStr = (OmnString)templates->toString();
		int ftc = templatesStr.findSubString("\n", 0);
		int dbquot = templatesStr.findSubString("\"", 0);
		int quot = templatesStr.findSubString("\'", 0);
		if(ftc != -1)
		{
			templatesStr.replace("\n", "", true);
		}
		if(dbquot != -1)
		{
			templatesStr.replace("\"", "|&&|", true);
		}
		if(quot != -1)
		{
			templatesStr.replace("\'", "|&|", true);
		}
	}
	OmnString colType, cmp_tname, cmp_query, cmp_order
		, cmp_fnames, cmp_tnamebd, cmp_psize, cmp_qrm, cmp_dftqry
		, cmp_anames, cmp_entry_sep, cmp_field_sep, cmp_contents, cmp_reverse;
	if(datacol)
	{
		datacol = datacol->getFirstChild();	
	}
	OmnString allowEditor =  datacol->getAttrStr("allowEditor", "false");
	OmnString gic_borderTop = datacol->getAttrStr("gic_borderTop", "none");
	OmnString gic_borderBottom = datacol->getAttrStr("gic_borderBottom", "none");
	OmnString gic_borderLeft = datacol->getAttrStr("gic_borderLeft", "none");
	OmnString gic_borderRight = datacol->getAttrStr("gic_borderRight", "none");
	OmnString bgColor = datacol->getAttrStr("gic_outbgc");
	OmnString fontColor = datacol->getAttrStr("gic_outclr");
	OmnString bgOverColor = datacol->getAttrStr("gic_overbgc");
	OmnString bgSelectColor = datacol->getAttrStr("gic_downbgc");
	OmnString fontOverColor = datacol->getAttrStr("gic_overclr");
	OmnString fontSelectColor = datacol->getAttrStr("gic_downclr");
	OmnString fontSelectedColor = datacol->getAttrStr("gic_itemclr");
	OmnString bgSelectedColor = datacol->getAttrStr("gic_itembgc");
	OmnString fontSize = datacol->getAttrStr("gic_fsize", "12");
	OmnString gic_height = datacol->getAttrStr("gic_height");
	OmnString gic_width = datacol->getAttrStr("gic_width");
	OmnString cmp_width = datacol->getAttrStr("cmp_width", gic_width);
	OmnString cmp_height = datacol->getAttrStr("cmp_height", gic_height);
	OmnString borderCol = datacol->getAttrStr("borderCol", "#999");
	cmp_value_bind = datacol->getAttrStr("cmp_value_bind", "@classname");
	cmp_fgcbd = datacol->getAttrStr("cmp_fgcbd", "@color");
	datacol = datacol->getFirstChild("contents");
	OmnString content = "";
	if(datacol)
	{
		content = datacol->getNodeText();
		int dbquot = content.findSubString("\"", 0);
		int quot = content.findSubString("\'", 0);
		if(dbquot != -1)
		{
			content.replace("\"", "|&&|", true);
		}
		if(quot != -1)
		{
			content.replace("\'", "|&|", true);
		}
	}
	//在课表中有待完善,field在js中配置
	field << "[{name : \"name\", mapping : \""<<cmp_value_bind
		  <<"\"}, {name : \"color\", mapping : \""<<cmp_fgcbd<<"\"}]";
	gic_html << "<div class=\'item\' style=\'width:"<<cmp_width<<"px;height:"<<cmp_height
		     << "px;\'><span style=\'color:{color};\'>{name}</span></div>";
	gic_html = vpd->getAttrStr("gic_html", gic_html);

	//line-height:"<<cmp_height<<"px;
	/*code.mCss << "#a" << gic_id2 << " .item{display:inline-block;margin-left:"<< gic_vsp 
			  << "px;margin-top:" << gic_hsp << "px;";
	if(gic_isVertical == "true")
		code.mCss << "line-height:"<<cmp_height<<"px;";
	code.mCss << "text-align : center;width:"<<cmp_width
			  << "px;height:"<<cmp_height<<"px;color:"
			  <<fontColor<<";font-size:"<<fontSize<<"px;background-color:"<< bgColor <<"}";
	code.mCss << "#a"<<gic_id2<<" .item{*display:inline;}";
	//background-color:transparent;
	code.mCss << "#a" << gic_id2 << "{padding-right:"
			  << gic_roff << "px;padding-left:" << gic_loff
			  << "px;padding-top:" << gic_toff << "px;padding-bottom:"
			  << gic_boff <<"px;}";
	code.mCss << "#a" << gic_id2 << " div.icon-hover{color:"<<fontOverColor<<";background-color:"<< bgOverColor <<"; cursor:pointer;}";
	code.mCss << "#a" << gic_id2 << " div.x-view-selected{color:"<<fontSelectColor<<";background-color:"<< bgSelectColor <<";}";*/
	bgColor = AosHtmlUtil::getWebColor(bgColor);
	fontColor = AosHtmlUtil::getWebColor(fontColor);
	fontSelectColor = AosHtmlUtil::getWebColor(fontSelectColor);
	fontOverColor = AosHtmlUtil::getWebColor(fontOverColor);
	bgOverColor = AosHtmlUtil::getWebColor(bgOverColor);
	bgSelectColor = AosHtmlUtil::getWebColor(bgSelectColor);
	borderCol = AosHtmlUtil::getWebColor(borderCol);
	//json
 code.mJson << ","
			<< "id_2:\"a" << gic_id2 << "\","
			<< "trashid:\"" << trashid << "\","
			<< "singleMove:\"" << singleMove << "\","
			<< "show_num:\"" << show_num << "\","
			<< "use_control:\"" << use_control << "\","
			<< "allowEditor : \"" << allowEditor << "\","
			<< "bgColor : \"" << bgColor << "\","
			<< "fontColor : \"" << fontColor << "\","
			<< "borderCol : \"" << borderCol << "\","
			<< "bgOverColor : \"" << bgOverColor << "\","
			<< "bgSelectColor : \"" << bgSelectColor << "\","
			<< "fontSelectColor : \"" << fontSelectColor << "\","
			<< "fontOverColor : \"" << fontOverColor << "\","
			<< "fontSelectedColor : \"" << fontSelectedColor << "\","
			<< "bgSelectedColor : \"" << bgSelectedColor << "\","
			<< "objTemplate : \"" << objTemplate << "\","
			<< "multiSelect : \"" << multiSelect << "\","
			<< "fontSize : \"" << fontSize << "\","
			<< "gic_vsp : \"" << gic_vsp << "\","
			<< "celloverride : \"" << celloverrideStr << "\","
			<< "templates : \"" << templatesStr << "\","
			<< "gic_itemsmove : \"" << gic_itemsmove << "\","
			<< "gic_hsp : \"" << gic_hsp << "\","
			<< "gic_roff : \"" << gic_roff << "\","
			<< "gic_loff : \"" << gic_loff << "\","
			<< "gic_toff : \"" << gic_toff << "\","
			<< "gic_boff : \"" << gic_boff << "\","
			<< "cmp_width : \"" << cmp_width << "\","
			<< "cmp_height : \"" << cmp_height << "\","
			<< "gic_isVertical : " << gic_isVertical << ","
			<< "gic_boff : \"" << gic_boff << "\","
			<< "gic_toff : \"" << gic_toff << "\","
			<< "gic_html : \"" << gic_html << "\","
			<< "gic_draggroup1 : \"" << gic_draggroup1 << "\","
			<< "gic_draggroup2 : \"" << gic_draggroup2 << "\","
			<< "gic_dropgroup1 : \"" << gic_dropgroup1 << "\","
			<< "gic_dropgroup2 : \"" << gic_dropgroup2 << "\","
			<< "gic_borderTop : \"" << gic_borderTop << "\","
			<< "gic_borderLeft : \"" << gic_borderLeft << "\","
			<< "gic_borderBottom : \"" << gic_borderBottom << "\","
			<< "gic_borderRight : \"" << gic_borderRight << "\","
			<< "gic_panelBorderTop : \"" << gic_panelBorderTop << "\","
			<< "gic_panelBorderLeft : \"" << gic_panelBorderLeft << "\","
			<< "gic_panelBorderBottom : \"" << gic_panelBorderBottom << "\","
			<< "gic_panelBorderRight : \"" << gic_panelBorderRight << "\","
			<< "gic_lstn : \"" << gic_lstn << "\","
			<< "field : " << field << ","
			<< "tip_time : " << tip_time << ","
			<< "gic_rolling : " << gic_rolling << ","
			<< "autoScroll : " << gic_sb << ","
			<< "tip : \"" << tip << "\","
			<< "tip_vpdname : \"" << tip_vpdname << "\","
			<< "tip_width : \"" << tip_width << "\","
			<< "tip_height : \"" << tip_height << "\","
			<< "gic_btbar : " << gic_btbar << ","
			<< "withoutBgC : \"" << withoutBgC << "\","
			<< "oneRoll : \"" << oneRoll << "\","
			 << "content : \"" << content << "\"";
	
	return true;
}

