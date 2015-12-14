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
// 08/17/2010: Created by Cody
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHintButton.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicHintButton();

AosGicHintButton::AosGicHintButton(const bool flag)
:
AosGic(AOSGIC_HINTBUTTON, AosGicType::eHintButton, flag)
{
}


AosGicHintButton::~AosGicHintButton()
{
}


bool	
AosGicHintButton::generateCode(
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
	OmnString gic_htbd = vpd->getAttrStr("gic_htbd");
	OmnString gic_lbbd = vpd->getAttrStr("gic_lbbd");
	OmnString gic_label;

	gic_label = vpd->getAttrStr("gic_label", "");
	OmnString gic_httxt = vpd->getAttrStr("gic_httxt", "");
	OmnString gic_move = vpd->getAttrStr("gic_move", "false");
	OmnString gic_type = vpd->getAttrStr("gic_type", "gic_button");
	OmnString gic_fsize = vpd->getAttrStr("gic_fsize", "12");
	OmnString gic_fstyle = vpd->getAttrStr("gic_fstyle", "plain");
	OmnString gic_fcolor = vpd->getAttrStr("gic_fcolor", "black");
	OmnString gic_disht = vpd->getAttrStr("gic_disht", "false");

	code.mHtml <<"<input type=\"button\" ></input>";
	code.mJson << ","
			<< "gic_lbbd : \"" << gic_lbbd << "\","
			<< "gic_htbd : \"" << gic_htbd << "\","
			<< "gic_httxt : \"" << gic_httxt << "\","
			<< "gic_label : \"" << gic_label << "\","
			<< "gic_fsize : " << gic_fsize << ","
			<< "gic_fstyle : \"" << gic_fstyle << "\","
			<< "gic_fcolor : \"" << gic_fcolor << "\","
			<< "gic_disht : " << gic_disht << "";
				
	return true;
}

