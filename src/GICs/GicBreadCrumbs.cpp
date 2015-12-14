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
// 07/08/2010: Created by Henry
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicBreadCrumbs.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicBreadCrumbs::AosGicBreadCrumbs(const bool flag)
:
AosGic(AOSGIC_BREADCRUMBS, AosGicType::eBreadCrumbs, flag)
{
}


AosGicBreadCrumbs::~AosGicBreadCrumbs()
{
}


bool	
AosGicBreadCrumbs::generateCode(
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
		//	AosXmlTagPtr thevpd = vpd->getFirstChild("gic_data");
			OmnString gic_width  = vpd->getAttrStr("gic_width","600");
			OmnString gic_height  = vpd->getAttrStr("gic_height","50");
			OmnString gic_data = vpd->getAttrStr("gic_data","");

			OmnString gic_align = vpd->getAttrStr("gic_align","center");
			OmnString gic_ulheight = vpd->getAttrStr("gic_ulheight","32");
			OmnString gic_menu_bgimg = vpd->getAttrStr("gic_menu_bgimg","transparent");
			OmnString gic_menubgcolor = vpd->getAttrStr("gic_menubgcolor","transparent");
			if (gic_menubgcolor != "")
			{
				gic_menubgcolor = AosHtmlUtil::getWebColor(gic_menubgcolor);
			}
			OmnString menu_width_type = vpd->getAttrStr("menu_width_type","false");
			OmnString gic_menuwidth = vpd->getAttrStr("gic_menuwidth","114");
			OmnString menu_padding_width = vpd->getAttrStr("menu_padding_width","30");
			OmnString menu_img_type = vpd->getAttrStr("menu_img_type","false");
			OmnString menu_img = vpd->getAttrStr("menu_img","transparent");
			OmnString gic_imgURL = vpd->getAttrStr("gic_imgURL","transparent");
			OmnString menu_click_img = vpd->getAttrStr("menu_click_img","transparent");
			OmnString menu_bgcolor = vpd->getAttrStr("menu_bgcolor","transparent");
			if(menu_bgcolor != "")
			{
				menu_bgcolor = AosHtmlUtil::getWebColor(menu_bgcolor);
			}
			OmnString menu_mouse_bgcolor = vpd->getAttrStr("menu_mouse_bgcolor","transparent");
			if(menu_mouse_bgcolor != "")
			{
				menu_mouse_bgcolor = AosHtmlUtil::getWebColor(menu_mouse_bgcolor);
			}
			OmnString menu_click_bgcolor = vpd->getAttrStr("menu_click_bgcolor","");
			if(menu_click_bgcolor != "")
			{
				menu_click_bgcolor = AosHtmlUtil::getWebColor(menu_click_bgcolor);
			}
			OmnString gic_menucolor = vpd->getAttrStr("gic_menucolor","#FFFFFF");
			if(gic_menucolor != "")
			{
				gic_menucolor = AosHtmlUtil::getWebColor(gic_menucolor);
			}
			OmnString gic_mousecolor = vpd->getAttrStr("gic_mousecolor","");
			if(gic_mousecolor != "")
			{
				gic_mousecolor = AosHtmlUtil::getWebColor(gic_mousecolor);
			}
			OmnString menu_click_color = vpd->getAttrStr("menu_click_color","");
			if(menu_click_color != "")
			{
				menu_click_color = AosHtmlUtil::getWebColor(menu_click_color);
			}
			OmnString gic_menufsize = vpd->getAttrStr("gic_menufsize","12");
			OmnString menu_mouse_size = vpd->getAttrStr("menu_mouse_size","12");
			OmnString menu_click_size = vpd->getAttrStr("menu_click_size","12");
			OmnString gic_menustyle = vpd->getAttrStr("gic_menustyle","plain");
			OmnString menu_mouse_style = vpd->getAttrStr("menu_mouse_style","plain");
			OmnString menu_click_style = vpd->getAttrStr("menu_click_style","plain");
			OmnString gic_menuSeparated = vpd->getAttrStr("gic_menuSeparated","true");
			OmnString gic_menuSeparatedColor = vpd->getAttrStr("gic_menuSeparatedColor","");
			if(gic_menuSeparatedColor != "")
			{
				gic_menuSeparatedColor = AosHtmlUtil::getWebColor(gic_menuSeparatedColor);
			}
			OmnString menu_separated_width = vpd->getAttrStr("menu_separated_width","3");
			OmnString menu_separated_img = vpd->getAttrStr("menu_separated_img","transparent");

			OmnString gic_itemulheight = vpd->getAttrStr("gic_itemulheight","32");
			OmnString gic_item_bgimg = vpd->getAttrStr("gic_item_bgimg","transparent");
			OmnString gic_itembgcolor = vpd->getAttrStr("gic_itembgcolor","transparent");
			if(gic_itembgcolor != "")
			{
				gic_itembgcolor = AosHtmlUtil::getWebColor(gic_itembgcolor);
			}
			OmnString item_width_type = vpd->getAttrStr("item_width_type","false");
			OmnString gic_itemwidth = vpd->getAttrStr("gic_itemwidth","114");
			OmnString item_padding_width = vpd->getAttrStr("item_padding_width","30");
			OmnString item_img_type = vpd->getAttrStr("item_img_type","false");
			OmnString item_img = vpd->getAttrStr("item_img","transparent");
			OmnString item_mouse_img = vpd->getAttrStr("item_mouse_img","transparent");
			OmnString item_click_img = vpd->getAttrStr("item_click_img","transparent");
			OmnString item_bgcolor = vpd->getAttrStr("item_bgcolor","transparent");
			if(item_bgcolor != "")
			{
				item_bgcolor = AosHtmlUtil::getWebColor(item_bgcolor);
			}
			OmnString item_mouse_bgcolor = vpd->getAttrStr("item_mouse_bgcolor","transparent");
			if(item_mouse_bgcolor != "")
			{
				item_mouse_bgcolor = AosHtmlUtil::getWebColor(item_mouse_bgcolor);
			}
			OmnString item_click_bgcolor = vpd->getAttrStr("item_click_bgcolor","transparent");
			if(item_click_bgcolor != "")
			{
				item_click_bgcolor = AosHtmlUtil::getWebColor(item_click_bgcolor);
			}
			OmnString gic_itemcolor = vpd->getAttrStr("gic_itemcolor","#000000");
			if(gic_itemcolor != "")
			{
				gic_itemcolor = AosHtmlUtil::getWebColor(gic_itemcolor);
			}
			OmnString gic_itemMouseColor = vpd->getAttrStr("gic_itemMouseColor","#8B0000");
			if(gic_itemMouseColor != "")
			{
				gic_itemMouseColor = AosHtmlUtil::getWebColor(gic_itemMouseColor);
			}
			OmnString item_click_color = vpd->getAttrStr("item_click_color","");
			if(item_click_color != "")
			{
				item_click_color = AosHtmlUtil::getWebColor(item_click_color);
			}
			OmnString gic_itemfsize = vpd->getAttrStr("gic_itemfsize","12");
			OmnString item_mouse_size = vpd->getAttrStr("item_mouse_size","12");
			OmnString item_click_size = vpd->getAttrStr("item_click_size","12");
			OmnString gic_itemstyle = vpd->getAttrStr("gic_itemstyle","plain");
			OmnString item_mouse_style = vpd->getAttrStr("item_mouse_style","plain");
			OmnString item_click_style = vpd->getAttrStr("item_click_style","plain");
			OmnString gic_itemSeparated = vpd->getAttrStr("gic_itemSeparated","true");
			OmnString gic_itemSeparatedColor = vpd->getAttrStr("gic_itemSeparatedColor","transparent");
			if(gic_itemSeparatedColor != "")
			{
				gic_itemSeparatedColor = AosHtmlUtil::getWebColor(gic_itemSeparatedColor);
			}
			OmnString item_separated_width = vpd->getAttrStr("item_separated_width","3");
			OmnString item_separated_img = vpd->getAttrStr("item_separated_img","transparent");

	OmnString str;
	str 
		<< ",xtype:\'" << vpd->getAttrStr("gic_type") <<"\'"
		<< ",gic_ulheight:\'" << gic_ulheight <<"\'"
		<< ",gic_align:\'" << gic_align <<"\'"
		<< ",gic_menu_bgimg:\'" << gic_menu_bgimg << "\'"
		<< ",gic_menubgcolor:\'" << gic_menubgcolor <<"\'"
		<< ",menu_width_type:" << menu_width_type << "" 
		<< ",gic_menuwidth:\'" << gic_menuwidth << "\'"
		<< ",menu_padding_width:\'" << menu_padding_width << "\'" 
		<< ",menu_img_type:" << menu_img_type << "" 
		<< ",menu_img:\'" << menu_img << "\'"
		<< ",gic_imgURL:\'" << gic_imgURL << "\'" 
		<< ",menu_click_img:\'" << menu_click_img << "\'" 
		<< ",menu_bgcolor:\'" << menu_bgcolor << "\'" 
		<< ",menu_mouse_bgcolor:\'" << menu_mouse_bgcolor << "\'" 
		<< ",menu_click_bgcolor:\'" << menu_click_bgcolor << "\'" 
		<< ",gic_menucolor:\'" << gic_menucolor << "\'" 
		<< ",gic_mousecolor:\'" << gic_mousecolor <<"\'"
		<< ",menu_click_color:\'" << menu_click_color << "\'" 
		<< ",gic_menufsize:\'" << gic_menufsize << "\'" 
		<< ",menu_mouse_size:\'" << menu_mouse_size << "\'" 
		<< ",menu_click_size:\'" << menu_click_size << "\'" 
		<< ",gic_menustyle:\'" << gic_menustyle << "\'" 
		<< ",menu_mouse_style:\'" << menu_mouse_style << "\'" 
		<< ",menu_click_style:\'" << menu_click_style << "\'" 
		<< ",gic_menuSeparated:" << gic_menuSeparated << "" 
		<< ",gic_menuSeparatedColor:\'" << gic_menuSeparatedColor << "\'" 
		<< ",menu_separated_width:\'" << menu_separated_width << "\'" 
		<< ",menu_separated_img:\'" << menu_separated_img << "\'" 

		<< ",gic_itemulheight:\'" << gic_itemulheight <<"\'"
		<< ",gic_item_bgimg:\'" << gic_item_bgimg << "\'" 
		<< ",gic_itembgcolor:\'" << gic_itembgcolor <<"\'"
		<< ",item_width_type:" << item_width_type << "" 
		<< ",gic_itemwidth:\'" << gic_itemwidth << "\'" 
		<< ",item_padding_width:\'" << item_padding_width << "\'" 
		<< ",item_img_type:" << item_img_type << "" 
		<< ",item_img:\'" << item_img << "\'" 
		<< ",item_mouse_img:\'" << item_mouse_img << "\'" 
		<< ",item_click_img:\'" << item_click_img << "\'" 
		<< ",item_bgcolor:\'" << item_bgcolor << "\'" 
		<< ",item_mouse_bgcolor:\'" << item_mouse_bgcolor << "\'" 
		<< ",item_click_bgcolor:\'" << item_click_bgcolor << "\'" 
		<< ",gic_itemcolor:\'" << gic_itemcolor <<"\'"
		<< ",gic_itemMouseColor:\'" << gic_itemMouseColor <<"\'"
		<< ",item_click_color:\'" << item_click_color << "\'" 
		<< ",gic_itemfsize:\'" << gic_itemfsize << "\'" 
		<< ",item_mouse_size:\'" << item_mouse_size << "\'" 
		<< ",item_click_size:\'" << item_click_size << "\'" 
		<< ",gic_itemstyle:\'" << gic_itemstyle << "\'" 
		<< ",item_mouse_style:\'" << item_mouse_style << "\'" 
		<< ",item_click_style:\'" << item_click_style << "\'" 
		<< ",gic_itemSeparated:" <<  gic_itemSeparated << "" 
		<< ",gic_itemSeparatedColor:\'" << gic_itemSeparatedColor << "\'" 
		<< ",item_separated_width:\'" << item_separated_width << "\'" 
		<< ",item_separated_img:\'" << item_separated_img << "\'" 
		/**********************************************************/
		<< ",gic_data:\'" << gic_data <<"\'";

	code.mJson << str;
	return true;
}

