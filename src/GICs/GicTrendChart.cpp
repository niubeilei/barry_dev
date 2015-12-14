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
#include "GICs/GicTrendChart.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicTrendChart();

AosGicTrendChart::AosGicTrendChart(const bool flag)
:
AosGic(AOSGIC_TRENDCHART, AosGicType::eTrendChart, flag)
{
}


AosGicTrendChart::~AosGicTrendChart()
{
}


bool	
AosGicTrendChart::generateCode(
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
		<< ",gic_retrievetime: \"" << vpd->getAttrStr("gic_retrievetime", "5") 
		<< "\",gic_datatype: \"" << vpd->getAttrStr("gic_datatype", "") 
		<< "\",gic_lstn: \"" << vpd->getAttrStr("gic_lstn", "") 
		<< "\",gic_vunit: \"" << vpd->getAttrStr("gic_vunit", "px") 
		<< "\",gic_grid_lw: \"" << vpd->getAttrStr("gic_grid_lw", "0.5") 
		<< "\",gic_chart_lw: \"" << vpd->getAttrStr("gic_chart_lw", "0.5") 
		<< "\",gic_chart_lcolor: \"" << vpd->getAttrStr("gic_chart_lcolor", "#000000") 
		<< "\",gic_refer_lcolor: \"" << vpd->getAttrStr("gic_refer_lcolor", "#ff0000") 
		<< "\",gic_mode: \"" << vpd->getAttrStr("gic_mode", "") 
		<< "\",gic_highbind: \"" << vpd->getAttrStr("gic_highbind", "") 
		<< "\",gic_lowbind: \"" << vpd->getAttrStr("gic_lowbind", "") 
		<< "\",gic_ybind: \"" << vpd->getAttrStr("gic_ybind", "") 
		<< "\",gic_xlabel_bind: \"" << vpd->getAttrStr("gic_xlabel_bind", "") 
		<< "\",gic_ylabel_bind: \"" << vpd->getAttrStr("gic_ylabel_bind", "") 
		<< "\",gic_numpoint: \"" << vpd->getAttrStr("gic_numpoint", "100") 
		<< "\",gic_collaboration: " << vpd->getAttrBool("gic_collaboration", false)
		<< ",gic_account: \"" << vpd->getAttrStr("gic_account", "") 
		<< "\",gic_role: \"" << vpd->getAttrStr("gic_role", "") 
		<< "\",gic_recve_cid: \"" << vpd->getAttrStr("gic_recve_cid", "") 
		<< "\",gic_grid_lcolor: \"" << vpd->getAttrStr("gic_grid_lcolor", "#000000") << "\""; 
	return true;
}

