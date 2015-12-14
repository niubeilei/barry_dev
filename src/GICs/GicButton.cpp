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
#include "GICs/GicButton.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlModules/DictionaryHandler.h"


AosGicButton::AosGicButton(const bool flag)
:
AosGic(AOSGIC_BUTTON, AosGicType::eButton, flag)
{
}


AosGicButton::~AosGicButton()
{
}


bool	
AosGicButton::generateCode(
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
	//  	<input type="gic_button" text="gic_value" ></input>
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
	OmnString valbd = vpd->getAttrStr("bt_query");
	OmnString gic_value;

	gic_value = vpd->getAttrStr("bt_label", "Button");
	//lynch
	//mutil-language
	/*
	OmnString label = AosDictionaryHandler::getDictionary(gic_value, 1);
	if(label != "")
	{
		gic_value= label;
	}
	*/
	OmnString gic_drag = vpd->getAttrStr("gic_drag", "false");
	OmnString gic_move = vpd->getAttrStr("gic_move", "false");
	OmnString gic_type = vpd->getAttrStr("gic_type", "gic_button");
	OmnString bt_fsize = vpd->getAttrStr("bt_fsize", "12");
	OmnString bt_fstyle = vpd->getAttrStr("bt_fstyle", "plain");
	OmnString bt_color = vpd->getAttrStr("bt_color", "black");
	OmnString gic_tabindex = vpd->getAttrStr("gic_tabindex", "-1");

	//mutil-language
	OmnString langtype = htmlPtr->getLanguageType(); 
	OmnString langcode = AosDictionaryHandler::matchLangType(langtype);
	if(langcode != "")
	{
		OmnString label = AosDictionaryHandler::getDictionary("bt_label", vpd, langcode, htmlPtr->getRundata());
		if(label != "")
		{
			gic_value= label;
		}
	}

	code.mHtml <<"<input tabindex=\"" << gic_tabindex << "\" type=\"button\" value=\"" << gic_value << "\" ></input>";
	code.mJson << ","
			<< "bt_label : \"" << gic_value << "\","
			<< "bt_fsize : " << bt_fsize << ","
			<< "bt_fstyle : \"" << bt_fstyle << "\","
			<< "bt_color : \"" << bt_color << "\","
			<< "gic_valuedb : \"" << valbd << "\"";
				
	return true;
}

