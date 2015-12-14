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
#include "GICs/GicScrollMenu.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicScrollMenu();

AosGicScrollMenu::AosGicScrollMenu(const bool flag)
:
AosGic(AOSGIC_SCROLLMENU, AosGicType::eScrollMenu, flag)
{
}


AosGicScrollMenu::~AosGicScrollMenu()
{
}


bool	
AosGicScrollMenu::generateCode(
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
			OmnString gic_height  = vpd->getAttrStr("gic_height","35");


			OmnString gic_def = vpd->getAttrStr("gic_def","-1");
			OmnString gic_menubgimg = vpd->getAttrStr("gic_menubgimg","");
			OmnString gic_mousebgimg = vpd->getAttrStr("gic_mousebgimg","");
			OmnString gic_clickbgimg = vpd->getAttrStr("gic_clickbgimg","");
			OmnString gic_menubgColor = vpd->getAttrStr("gic_menubgColor","transparent");
			if(gic_menubgColor != "")
			{
				gic_menubgColor = AosHtmlUtil::getWebColor(gic_menubgColor);
			}
			OmnString gic_mousebgColor = vpd->getAttrStr("gic_mousebgColor","transparent");
			if(gic_mousebgColor != "")
			{
				gic_mousebgColor = AosHtmlUtil::getWebColor(gic_mousebgColor);
			}
			OmnString gic_clickbgColor = vpd->getAttrStr("gic_clickbgColor","transparent");
			if(gic_clickbgColor != "")
			{
				gic_clickbgColor = AosHtmlUtil::getWebColor(gic_clickbgColor);
			}
			OmnString gic_menuColor = vpd->getAttrStr("gic_menuColor","#000000");
			if(gic_menuColor != "")
			{
				gic_menuColor = AosHtmlUtil::getWebColor(gic_menuColor);
			}
			OmnString gic_menuMouseColor = vpd->getAttrStr("gic_menuMouseColor","#000000");
			if(gic_menuMouseColor != "")
			{
				gic_menuMouseColor = AosHtmlUtil::getWebColor(gic_menuMouseColor);
			}
			OmnString gic_menuClickColor = vpd->getAttrStr("gic_menuClickColor","#000000");
			if(gic_menuClickColor != "")
			{
				gic_menuClickColor = AosHtmlUtil::getWebColor(gic_menuClickColor);
			}
			OmnString gic_menuFsize = vpd->getAttrStr("gic_menuFsize","12");
			OmnString gic_menuMouseFsize = vpd->getAttrStr("gic_menuMouseFsize","12");
			OmnString gic_menuClickFsize = vpd->getAttrStr("gic_menuClickFsize","12");
			OmnString gic_menuFstyle = vpd->getAttrStr("gic_menuFstyle","plain");
			OmnString gic_menuMouseFstyle = vpd->getAttrStr("gic_menuMouseFstyle","plain");
			OmnString gic_menuClickFstyle = vpd->getAttrStr("gic_menuClickFstyle","plain");
			OmnString gic_menufixedwidth = vpd->getAttrStr("gic_menufixedwidth","false");
			OmnString gic_menuwidth = vpd->getAttrStr("gic_menuwidth","150");
			OmnString gic_textalign = vpd->getAttrStr("gic_textalign","center");
			OmnString gic_leftSpace = vpd->getAttrStr("gic_leftSpace","");
			OmnString gic_rightSpace = vpd->getAttrStr("gic_rightSpace","");
			OmnString gic_menuSeparated = vpd->getAttrStr("gic_menuSeparated","true");
			OmnString gic_menuSeparatedwidth = vpd->getAttrStr("gic_menuSeparatedwidth","2");
			OmnString gic_menuSeparatedColor = vpd->getAttrStr("gic_menuSeparatedColor","#FFFFFF");
			if(gic_menuSeparatedColor != "")
			{
				gic_menuSeparatedColor = AosHtmlUtil::getWebColor(gic_menuSeparatedColor);
			}
			OmnString gic_menuSeparatedimg = vpd->getAttrStr("gic_menuSeparatedimg","");
			OmnString gic_scrollType = vpd->getAttrStr("gic_scrollType","up");
			OmnString gic_data = vpd->getAttrStr("gic_data","");
			
	OmnString str;
	str 
		<< ",xtype:\'" << vpd->getAttrStr("gic_type") <<"\'"
		<< ",gic_def:\'" << gic_def<<"\'" 
		<< ",gic_menubgimg:\'" << gic_menubgimg<<"\'" 
		<< ",gic_mousebgimg:\'" << gic_mousebgimg<<"\'"
		<< ",gic_clickbgimg:\'" << gic_clickbgimg<<"\'"
		<< ",gic_menubgColor:\'" << gic_menubgColor<<"\'"
		<< ",gic_mousebgColor:\'" << gic_mousebgColor<<"\'"
		<< ",gic_clickbgColor:\'" << gic_clickbgColor<<"\'"
		<< ",gic_menuColor:\'" << gic_menuColor<<"\'"
		<< ",gic_menuMouseColor:\'" << gic_menuMouseColor<<"\'"
		<< ",gic_menuClickColor:\'" << gic_menuClickColor<<"\'"
		<< ",gic_menuFsize:\'" << gic_menuFsize<<"\'"
		<< ",gic_menuMouseFsize:\'" << gic_menuMouseFsize<<"\'"
		<< ",gic_menuClickFsize:\'" << gic_menuClickFsize<<"\'"
		<< ",gic_menuFstyle:\'" << gic_menuFstyle<<"\'"
		<< ",gic_menuMouseFstyle:\'" << gic_menuMouseFstyle<<"\'"
		<< ",gic_menuClickFstyle:\'" << gic_menuClickFstyle<<"\'"
		<< ",gic_menufixedwidth:\'" << gic_menufixedwidth<<"\'"
		<< ",gic_menuwidth:\'" << gic_menuwidth<<"\'"
		<< ",gic_textalign:\'" << gic_textalign<<"\'"
		<< ",gic_leftSpace:\'" << gic_leftSpace<<"\'"
		<< ",gic_rightSpace:\'" << gic_rightSpace<<"\'"
		<< ",gic_menuSeparated:\'" << gic_menuSeparated<<"\'"
		<< ",gic_menuSeparatedwidth:\'" << gic_menuSeparatedwidth<<"\'"
		<< ",gic_menuSeparatedColor:\'" << gic_menuSeparatedColor<<"\'"
		<< ",gic_menuSeparatedimg:\'" << gic_menuSeparatedimg<<"\'"
		<< ",gic_scrollType:\'" << gic_scrollType<<"\'"
		<< ",gic_data:\'" << gic_data <<"\'";

	code.mJson << str;
	return true;
}

