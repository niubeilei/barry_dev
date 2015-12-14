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
// 07/26/2010: Created by Cody
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicListIcon.h"
#include "HtmlModules/DclDb.h"

#include "HtmlServer/HtmlUtil.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicListIcon::AosGicListIcon(const bool flag)
:
AosGic(AOSGIC_LISTICON, AosGicType::eListIcon, flag)
{
}


AosGicListIcon::~AosGicListIcon()
{
}


bool	
AosGicListIcon::generateCode(
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
	OmnString gic_value;
	OmnString gic_id2 = AosGetHtmlElemId();
	
	OmnString objTemplate = vpd->getAttrStr("objTemplate");
	OmnString tip_vpdname = vpd->getAttrStr("tip_vpdname");
	OmnString tip = vpd->getAttrStr("tip", "false");
	OmnString tip_width = vpd->getAttrStr("tip_width", "300");
	OmnString tip_height = vpd->getAttrStr("tip_height", "450");
	OmnString tip_time = vpd->getAttrStr("tip_time", "5000");
	OmnString gic_panelBorderTop = vpd->getAttrStr("gic_panelBorderTop", "none");
	OmnString gic_panelBorderBottom = vpd->getAttrStr("gic_panelBorderBottom", "none");
	OmnString gic_panelBorderLeft = vpd->getAttrStr("gic_panelBorderLeft", "none");
	OmnString gic_panelBorderRight = vpd->getAttrStr("gic_panelBorderRight", "none");
	OmnString gic_dropName = vpd->getAttrStr("gic_dropName");
	OmnString gic_zoneId = vpd->getAttrStr("gic_zoneId", "codyImg");
	OmnString multiSelect = vpd->getAttrStr("multiSelect", "true");
	OmnString singleMove = vpd->getAttrStr("singleMove", "true");
	OmnString gic_vsp = vpd->getAttrStr("gic_vs", "1");
	OmnString gic_hsp = vpd->getAttrStr("gic_hs", "1");
	OmnString trashid = vpd->getAttrStr("trashid");
	OmnString gic_roff = vpd->getAttrStr("gic_rinset", "0");
	OmnString gic_loff = vpd->getAttrStr("gic_linset", "0");
	OmnString gic_boff = vpd->getAttrStr("gic_binset", "0");
	OmnString gic_toff = vpd->getAttrStr("gic_tinset", "0");
	OmnString gic_lstn = vpd->getAttrStr("gic_lstn");
	OmnString oneRoll = vpd->getAttrStr("oneRoll", "false");
	//是否显示滚动条
	OmnString gic_sb1 = vpd->getAttrStr("gic_scrollbar_v", "true");
	OmnString gic_sb = vpd->getAttrStr("gic_scrollBar_v", gic_sb1);
	//是否显示文字
	OmnString gic_icntip = vpd->getAttrStr("gic_icntip", "false");
	//底部的工具条
	OmnString gic_btbar1 = vpd->getAttrStr("gic_pagingToolBar", "true");
	OmnString gic_btbar2 = vpd->getAttrStr("gic_pagebtn", gic_btbar1);
	OmnString gic_btbar = vpd->getAttrStr("gic_pb_def", gic_btbar2);
	OmnString gic_draggroup1 = vpd->getAttrStr("gic_draggroup1");
	OmnString gic_dropgroup1 = vpd->getAttrStr("gic_dropgroup1");
	OmnString gic_draggroup2 = vpd->getAttrStr("gic_draggroup2");
	OmnString gic_dropgroup2 = vpd->getAttrStr("gic_dropgroup2");
	OmnString gic_rolling = vpd->getAttrStr("gic_w_anim", "0");
	OmnString gic_time = vpd->getAttrStr("gic_cgpage_time", "300");
	OmnString bgColor = vpd->getAttrStr("gic_bgColor");
	OmnString withoutBgC = vpd->getAttrStr("withoutBgC");
	//图片的绑定属性
	OmnString gic_iconl;
	//文字的绑定属性
	OmnString gic_labell;
	OmnString gic_iconw;
	OmnString gic_iconh;
	OmnString gic_icnlbpos;
	OmnString gic_icnlbal;
	OmnString gic_itemsmove;
	OmnString keepRatio;
	//<vpd>	
	//	<creators>
	//	  <creator />
	//	</creators>
	//	<datacol/>
	//</vpd>
	//creator中的属性
	AosXmlTagPtr datacol = vpd->getFirstChild("creators");
	if(datacol)
	{
		datacol = datacol->getFirstChild();	
	}
	OmnString gic_borderTop = datacol->getAttrStr("gic_borderTop", "none");
	OmnString gic_borderBottom = datacol->getAttrStr("gic_borderBottom", "none");
	OmnString gic_borderLeft = datacol->getAttrStr("gic_borderLeft", "none");
	OmnString gic_borderRight = datacol->getAttrStr("gic_borderRight", "none");
	OmnString borderCol = datacol->getAttrStr("borderCol", "#999");
	OmnString bgOverColor = datacol->getAttrStr("bgOverColor", "#efefef");
	OmnString bgSelectColor = datacol->getAttrStr("bgSelectColor", "#eff5fb");
	gic_iconw = datacol->getAttrStr("gic_width", "100");
	gic_iconh = datacol->getAttrStr("gic_height", "80");
	gic_iconl = datacol->getAttrStr("img_src_bind", "@zky_tnail");
	gic_labell = vpd->getAttrStr("gic_labell", "@zky_objid");
	gic_icnlbal = datacol->getAttrStr("gic_icnlbal", "center");
	gic_icnlbpos = datacol->getAttrStr("gic_icnlbpos", "bottom");
	gic_itemsmove = vpd->getAttrStr("trackgroups", "");
	if(gic_itemsmove != "" && gic_itemsmove != "false")
	{
		gic_itemsmove = "true";
	}
	else
	{
		gic_itemsmove = "false";
	}
	keepRatio = datacol->getAttrStr("img_nkratio", "true");
	AosDclDb dcl;
	//datacol中的属性
	OmnString css = "";	
	//css
	//有文字
	if(gic_icntip == "true")
	{
		if(gic_icnlbpos == "bottom")
		{
			css << "#a" << gic_id2 << " .item{font-size:12px;display:inline-block;margin-top:"<< gic_hsp
				   << "px;margin-left:" << gic_vsp << "px;padding:3px;}";
			css << "#a" << gic_id2 << " .item{*display:inline;}";
				
			if(gic_icnlbal == "right")
			{
				css << "#a" << gic_id2 << " .item .label{text-align:right;}"; 
			}
			else if(gic_icnlbal == "left")
			{
				css << "#a" << gic_id2 << " .item .label{text-align:left;}";				
			}
			else
			{
				css << "#a" << gic_id2 << " .item .label{text-align:center;}";
				gic_icnlbal = "center";
			}
		}
		else if(gic_icnlbpos == "top")
		{
			css << "#a" << gic_id2 << " .item{font-size:12px;display:inline-block;margin-top:"<< gic_hsp
			<< "px;margin-left:" << gic_vsp << "px;padding:3px;position:relative;}";
			css << "#a" << gic_id2 << " .item .icon{margin-top:10px;}";
			css << "#a" << gic_id2 << " .item{*display:inline;}";
			if(gic_icnlbal == "right")
			{
				css << "#a" << gic_id2 << " .item .label{top:0px;position:absolute;text-align:right}";
			}
			else if(gic_icnlbal == "left")
			{
				//int left = gic_iconh.toInt();
				css << "#a" << gic_id2 << " .item .label{top:0px;text-align:left;position:absolute;}"; 
			}
			else
			{
				css << "#a" << gic_id2 << " .item .label{top:0px;text-align:center;position:absolute;}";
				gic_icnlbal = "center";
			}
		}
		else if(gic_icnlbpos == "left")
		{
			int width = gic_iconw.toInt();
			int width1 = (width + 5 + gic_vsp.toInt() * 2) * 2;
			int height = gic_iconh.toInt();
			int height1 = gic_iconh.toInt() + (gic_hsp.toInt() + 5) * 2;
			css << "#a" << gic_id2 << " .item{font-size:12px;display:inline-block;margin-top:"<< gic_hsp
				   << "px;margin-left:" << gic_vsp << "px;padding:3px;position:relative;width:"
				   << width1 << "px;height:" << height1 << "px;}";
			css << "#a" << gic_id2 << " .item{*display:inline;}";
			css << "#a" << gic_id2 << " .item .icon{dispaly:inline;float:right;}";
			if(gic_icnlbal == "top")
			{
				css << "#a" << gic_id2 << " .item .label{position:absolute;width:" << width 
					   << "height:" << height << "px;top:4px;margin-left:2px;}";
			}
			else if(gic_icnlbal == "bottom")
			{
				css << "#a" << gic_id2 << " .item .label{position:absolute;width:" << width
					   << "height:" << height << "px;top:" << height*3/4 << "px;margin-left:2px;}";
			}
			else
			{
				css << "#a" << gic_id2 << " .item .label{position:absolute;width:" << width
					   << "height:" << height << "px;top:" << height/2 << "px;margin-left:2px;}";
				gic_icnlbal = "center";
			}
		}
		else if(gic_icnlbpos == "right")
		{
			int width = gic_iconw.toInt();
			int width1 = (width + 5 + gic_vsp.toInt() * 2) * 2;
			int height = gic_iconh.toInt();
     	    int height1 = gic_iconh.toInt() + (gic_hsp.toInt() + 5) * 2;
		    css << "#a" << gic_id2 << " .item{font-size:12px;display:inline-block;margin-top:"<< gic_hsp
				   << "px;margin-left:" << gic_vsp << "px;padding:3px;position:relative;width:"
				   << width1 << "px;height:" << height1 << "px;}";
		    css << "#a" << gic_id2 << " .item{*display : inline;}";
			css << "#a" << gic_id2 << " .item .icon{dispaly:inline;}";
			css << "#a" << gic_id2 << " .item .label{position:absolute;width:" << width << "px;left:10xp;";
			if(gic_icnlbal == "top")
			{
				css << "top:4px;left:" << width + 6 << "px;}";
			}
			else if(gic_icnlbal == "bottom")
			{
				css << "top:" << height*3/4 << "px;left:" << width + 6 << "px;}";
			}
			else
			{
				css << "top:" << height/2 << "px;left:" << width + 6 << "px;}";
				gic_icnlbal = "center";
			}
		}

	}
	else
	{
		//没文字
		css << "#a" << gic_id2 << " .item{font-size:12px;";
		//horizontal scroll
		//if(gic_rolling != "1")
		//css	<<"display:inline-block;";
		css	<<"margin-top:" << gic_hsp << "px; margin-left:" << gic_vsp << "px;padding:3px;text-align:center;}";
		//if(gic_rolling != "1")
		//css << "#a" << gic_id2 << " .item{*display : inline;}";
		css << "#a" << gic_id2 << "2 .item{font-size:12px;";
		//css << "display:inline-block;"
		css << "margin-top:" << gic_hsp << "px; margin-left:" << gic_vsp << "px;padding:3px;text-align:center;}";
		//css << "#a" << gic_id2 << "2 .item{*display : inline;}";
				
	}

	css << "#a" << gic_id2 << " .item .icon{height:" << gic_iconh << "px;width:"<< gic_iconw <<"px;}";
	css << "#a" << gic_id2 << "2 .item .icon{height:" << gic_iconh << "px;width:"<< gic_iconw <<"px;}";
	css << "#a" << gic_id2 << "{background-color:#fff;padding-right:"
			  << gic_roff << "px;padding-left:" << gic_loff
			  << "px;padding-top:" << gic_toff << "px;padding-bottom:"
			  << gic_boff <<"px;}";
	css << "#a" << gic_id2 << " div.icon-hover{border:1px solid #dddddd; background:#efefef; padding:2px;}";
	css << "#a" << gic_id2 << " div.x-view-selected{border:1px solid #99bbe8;background-color:#eff5fb;padding:2px;}";
	css << "#a" << gic_id2 << "2{background-color:#fff;padding-right:"
			  << gic_roff << "px;padding-left:" << gic_loff
			  << "px;padding-top:" << gic_toff << "px;padding-bottom:"
			  << gic_boff <<"px;}";
	css << "#a" << gic_id2 << "2 div.icon-hover{border:1px solid #dddddd; background:#efefef; padding:2px;}";
	css << "#a" << gic_id2 << "2 div.x-view-selected{border:1px solid #99bbe8;background-color:#eff5fb;padding:2px;}";

	/*code.mCss << css;*/
	bgColor = AosHtmlUtil::getWebColor(bgColor);
	borderCol = AosHtmlUtil::getWebColor(borderCol);
	bgOverColor = AosHtmlUtil::getWebColor(bgOverColor);
	bgSelectColor = AosHtmlUtil::getWebColor(bgSelectColor);
	//json
 code.mJson << ","
			<< "gic_boff:" << gic_boff << ","
			<< "gic_toff:" << gic_toff << ","
			<< "tip_time:" << tip_time << ","
			<< "trashid:\"" << trashid << "\","
			<< "singleMove:\"" << singleMove << "\","
			<< "id_2:\"a" << gic_id2 << "\","
			<< "objTemplate : \"" << objTemplate << "\","
			<< "gic_iconh : " << gic_iconh << ","
			<< "gic_iconw : " << gic_iconw << ","
			<< "autoScroll : " << gic_sb << ","
			<< "gic_btbar : " << gic_btbar << ","
			<< "gic_roff : " << gic_roff << ","
			<< "gic_loff : " << gic_loff << ","
			<< "multiSelect : \"" << multiSelect << "\","
			<< "keepRatio : \"" << keepRatio << "\","
			<< "gic_icnlbpos : \"" << gic_icnlbpos << "\","
			<< "gic_icntip : " << gic_icntip << ","
			<< "gic_icnlbal : \"" << gic_icnlbal << "\","
			<< "gic_zoneId : \"" << gic_zoneId << "\","
			<< "gic_dropName : \"" << gic_dropName << "\","
			<< "borderCol : \"" << borderCol << "\","
			<< "bgOverColor : \"" << bgOverColor << "\","
			<< "bgSelectColor : \"" << bgSelectColor << "\","
			<< "gic_vsp : " << gic_vsp << ","
			<< "gic_hsp : " << gic_hsp << ","
			<< "gic_panelBorderTop : \"" << gic_panelBorderTop << "\","
			<< "gic_panelBorderLeft : \"" << gic_panelBorderLeft << "\","
			<< "gic_panelBorderRight : \"" << gic_panelBorderRight << "\","
			<< "gic_panelBorderBottom : \"" << gic_panelBorderBottom << "\","
			<< "gic_borderRight : \"" << gic_borderRight << "\","
			<< "gic_borderBottom : \"" << gic_borderBottom << "\","
			<< "gic_borderTop : \"" << gic_borderTop << "\","
			<< "gic_borderLeft : \"" << gic_borderLeft << "\","
			<< "gic_draggroup1 : \"" << gic_draggroup1 << "\","
			<< "gic_draggroup2 : \"" << gic_draggroup2 << "\","
			<< "withoutBgC : \"" << withoutBgC << "\","
			<< "bgColor : \"" << bgColor << "\","
			<< "gic_dropgroup1 : \"" << gic_dropgroup1 << "\","
			<< "gic_dropgroup2 : \"" << gic_dropgroup2 << "\","
			<< "gic_lstn : \"" << gic_lstn << "\","
			<< "gic_iconl : \"" << gic_iconl << "\","
			<< "tip : \"" << tip << "\","
			<< "tip_vpdname : \"" << tip_vpdname << "\","
			<< "tip_width : \"" << tip_width << "\","
			<< "tip_height : \"" << tip_height << "\","
			<< "gic_itemsmove : " << gic_itemsmove << ","
			<< "gic_rolling : " << gic_rolling << ","
			<< "oneRoll : \"" << oneRoll << "\","
			<< "gic_labell : \"" << gic_labell << "\"";
	
	return true;
}

