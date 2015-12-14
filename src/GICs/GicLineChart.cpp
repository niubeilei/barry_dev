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
#include "GICs/GicLineChart.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicLineChart();

AosGicLineChart::AosGicLineChart(const bool flag)
:
AosGic(AOSGIC_LINECHART, AosGicType::eLineChart, flag)
{
}


AosGicLineChart::~AosGicLineChart()
{
}


bool	
AosGicLineChart::generateCode(
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
		<< "\",gic_showstyle: \"" << vpd->getAttrStr("gic_showstyle", "line") 
		<< "\",gic_lgd_fontcolor: \"" << vpd->getAttrStr("gic_lgd_fontcolor", "#444444")//09/16 
		<< "\",gic_lgd_spacing: " << vpd->getAttrInt("gic_lgd_spacing", 2) 
		<< ",gic_lgd_fontsize: " << vpd->getAttrInt("gic_lgd_fontsize", 11) 
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
		<< "\",gic_tip_fontsize: " << vpd->getAttrInt("gic_tip_fontsize", 11) //09/16 
		<< ",gic_xaxis_size: " << vpd->getAttrInt("gic_xaxis_size", 2) 
		<< ",gic_xaxis_gridsize: " << vpd->getAttrInt("gic_xaxis_gridsize", 1) 
		<< ",gic_yaxis_size: " << vpd->getAttrInt("gic_yaxis_size", 2) 
		<< ",gic_yaxis_gridsize: " << vpd->getAttrInt("gic_yaxis_gridsize", 1) 
		<< ",gic_xaxis_color: \"" << vpd->getAttrStr("gic_xaxis_color", "#666666") 
		<< "\",gic_xaxis_gridcolor: \"" << vpd->getAttrStr("gic_xaxis_gridcolor", "#aaaaaa") 
		<< "\",gic_yaxis_color: \"" << vpd->getAttrStr("gic_yaxis_color", "#666666") 
		<< "\",gic_yaxis_gridcolor: \"" << vpd->getAttrStr("gic_yaxis_gridcolor", "#aaaaaa") 
		<< "\",gic_series_cntPoints: " << vpd->getAttrStr("gic_series_cntPoints", "true") 
		<< ",gic_series_pointsize: " << vpd->getAttrInt("gic_series_pointsize", 10);

	return true;
}

