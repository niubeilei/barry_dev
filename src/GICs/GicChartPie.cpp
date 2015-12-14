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
// 07/08/2010: Created by Tracy
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicChartPie.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicChartPie();

AosGicChartPie::AosGicChartPie(const bool flag)
:
AosGic(AOSGIC_CHARTPIE, AosGicType::eChartPie, flag)
{
}


AosGicChartPie::~AosGicChartPie()
{
}


bool	
AosGicChartPie::generateCode(
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
	// Tracy, = Debug
	
	code.mJson  
		<< ",gic_lgd_display: \"" << vpd->getAttrStr("gic_lgd_display", "none") 
		<< "\",gic_lgd_fontcolor: \"" << vpd->getAttrStr("gic_lgd_fontcolor", "#444444")//09/16 
		<< "\",gic_lgd_spacing: " << vpd->getAttrInt("gic_lgd_spacing", 2) 
		<< ",gic_lgd_fontsize: " << vpd->getAttrInt("gic_lgd_fontsize", 12) 
		<< ",gic_font_size: " << vpd->getAttrInt("gic_font_size", 11) // 09/16
		<< ",gic_font_color: \"" << vpd->getAttrStr("gic_font_color", "#444444") // 09/16
		<< "\",gic_font_bold: " << vpd->getAttrStr("gic_font_bold", "false") // 09/16
		<< ",gic_font_italic: " << vpd->getAttrStr("gic_font_italic", "false") // 09/16
		<< ",gic_font_underline: " << vpd->getAttrStr("gic_font_underline", "false") // 09/16
		<< ",gic_bg_color:\"" << vpd->getAttrStr("gic_bg_color", "null") // 09/16
		<< "\",gic_border_color:\"" << vpd->getAttrStr("gic_border_color", "null") // 09/16
		<< "\",gic_border_size:" << vpd->getAttrStr("gic_border_size", "0") // 09/16
		<< ",gic_tip_bordersize:" << vpd->getAttrInt("gic_tip_bordersize", 1) // 09/16
		<< ",gic_tip_bordercolor:\"" << vpd->getAttrStr("gic_tip_bordercolor", "null") //09/16
		<< "\",gic_tip_bgcolor: \"" << vpd->getAttrStr("gic_tip_bgcolor", "null") //09/16
		<< "\",gic_tip_fontcolor: \"" << vpd->getAttrStr("gic_tip_fontcolor", "#444444") //09/16
		<< "\",gic_tip_fontsize: " << vpd->getAttrInt("gic_tip_fontsize", 11); //09/16 

	return true;
}

