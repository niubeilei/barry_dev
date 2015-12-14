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
#include "GICs/GicVerticalMenu.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicVerticalMenu();

AosGicVerticalMenu::AosGicVerticalMenu(const bool flag)
:
AosGic(AOSGIC_VERTICALMENU, AosGicType::eVerticalMenu, flag)
{
}


AosGicVerticalMenu::~AosGicVerticalMenu()
{
}


bool	
AosGicVerticalMenu::generateCode(
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
			OmnString gic_data = vpd->getAttrStr("gic_data","");
			OmnString gic_cenimg = vpd->getAttrStr("gic_cenimg","");
			OmnString gic_mouseimg = vpd->getAttrStr("gic_mouseimg","");
			OmnString gic_clickimg = vpd->getAttrStr("gic_clickimg","");
			OmnString gic_clickColor = vpd->getAttrStr("gic_clickColor","");
			OmnString gic_menuMouseColor = vpd->getAttrStr("gic_menuMouseColor","");
			OmnString gic_menuClickColor = vpd->getAttrStr("gic_menuClickColor","");
			OmnString gic_menuMouseFsize = vpd->getAttrStr("gic_menuMouseFsize","");
			OmnString gic_menuClickFsize = vpd->getAttrStr("gic_menuClickFsize","");
			OmnString gic_menuMouseFstyle = vpd->getAttrStr("gic_menuMouseFstyle","");
			OmnString gic_menuClickFstyle = vpd->getAttrStr("gic_menuClickFstyle","");
			OmnString gic_menuHeight = vpd->getAttrStr("gic_menuHeight","25");
			OmnString gic_menuFstyle = vpd->getAttrStr("gic_menuFstyle","plain");
			OmnString gic_menuFsize = vpd->getAttrStr("gic_menuFsize","12");
			OmnString gic_menuColor = vpd->getAttrStr("gic_menuColor","#000000");
			if(gic_menuColor != "")
			{
				gic_menuColor =  AosHtmlUtil::getWebColor(gic_menuColor);
			}
			OmnString gic_separated = vpd->getAttrStr("gic_separated","true");
			OmnString gic_separated_height = vpd->getAttrStr("gic_separated_height","3");
			OmnString gic_separated_img = vpd->getAttrStr("gic_separated_img","");
			OmnString gic_separatedColor = vpd->getAttrStr("gic_separatedColor","");
			if(gic_separatedColor != "")
			{
				gic_separatedColor = AosHtmlUtil::getWebColor(gic_separatedColor);
			}
			OmnString gic_mouseColor = vpd->getAttrStr("gic_mouseColor","");
			OmnString gic_cenColor = vpd->getAttrStr("gic_cenColor","");
			OmnString gic_textalign = vpd->getAttrStr("gic_textalign","center");
			OmnString gic_leftSpace = vpd->getAttrStr("gic_leftSpace","0");
			OmnString gic_rightSpace = vpd->getAttrStr("gic_rightSpace","0");
			OmnString gic_def = vpd->getAttrStr("gic_def","-1");

	OmnString str;
	str 
		<< ",xtype:\'" << vpd->getAttrStr("gic_type") <<"\'"
		<< ",gic_cenimg:\'" << gic_cenimg <<"\'"
		<< ",gic_mouseimg:\'" << gic_mouseimg <<"\'"
		<< ",gic_menuHeight:\'" << gic_menuHeight <<"\'"
		<< ",gic_menuFstyle:\'" << gic_menuFstyle <<"\'"
		<< ",gic_menuFsize:\'" << gic_menuFsize <<"\'"
		<< ",gic_menuColor:\'" << gic_menuColor <<"\'"
		<< ",gic_separated:\'" <<  gic_separated <<"\'"
		<< ",gic_separated_height:\'" << gic_separated_height <<"\'"
		<< ",gic_separated_img:\'" << gic_separated_img <<"\'"
		<< ",gic_separatedColor:\'" << gic_separatedColor <<"\'"
		<< ",gic_mouseColor:\'" << gic_mouseColor <<"\'"
		<< ",gic_cenColor:\'" << gic_cenColor <<"\'"
		<< ",gic_textalign:\'" << gic_textalign <<"\'"
		<< ",gic_leftSpace:\'" << gic_leftSpace <<"\'"
		<< ",gic_rightSpace:\'" << gic_rightSpace <<"\'"
		<< ",gic_clickimg:\'" << gic_clickimg <<"\'"
		<< ",gic_clickColor:\'" << gic_clickColor <<"\'"
		<< ",gic_menuMouseColor:\'" << gic_menuMouseColor <<"\'"
		<< ",gic_menuClickColor:\'" << gic_menuClickColor <<"\'"
		<< ",gic_menuMouseFsize:\'" << gic_menuMouseFsize <<"\'"
		<< ",gic_menuClickFsize:\'" << gic_menuClickFsize <<"\'"
		<< ",gic_menuMouseFstyle:\'" << gic_menuMouseFstyle <<"\'"
		<< ",gic_menuClickFstyle:\'" << gic_menuClickFstyle <<"\'"
		<< ",gic_def:\'" << gic_def <<"\'"
		<< ",gic_data:\'" << gic_data <<"\'";

	code.mJson << str;
	return true;
}

