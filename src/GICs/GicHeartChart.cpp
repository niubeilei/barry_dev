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
#include "GICs/GicHeartChart.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicHeartChart();

AosGicHeartChart::AosGicHeartChart(const bool flag)
:
AosGic(AOSGIC_HEARTCHART, AosGicType::eHeartChart, flag)
{
}


AosGicHeartChart::~AosGicHeartChart()
{
}


bool	
AosGicHeartChart::generateCode(
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
		<< ",gic_hheight: " << vpd->getAttrInt("gic_hheight", 0) 
		<< ",gic_hdelt: " << vpd->getAttrInt("gic_hdelt", 0) 
		<< ",gic_numpoint: " << vpd->getAttrInt("gic_numpoint", 0) 
		<< ",gic_chart_lw: " << vpd->getAttrInt("gic_chart_lw", 1) 
		<< ",gic_collaboration: " << vpd->getAttrBool("gic_collaboration", false) 
		<< ",gic_chart_lcolor: \"" << vpd->getAttrStr("gic_chart_lcolor", "#00ff00") << "\""
		<< ",gic_account: \"" << vpd->getAttrStr("gic_account", "") << "\""
		<< ",gic_role: \"" << vpd->getAttrStr("gic_role", "sender") << "\""
		<< ",gic_recve_cid: \"" << vpd->getAttrStr("gic_recve_cid", "") << "\""
		<< ",gic_vunit: \"" << vpd->getAttrStr("gic_vunit", "px") << "\""
		<< ",gic_datatypes: \"" << vpd->getAttrStr("gic_datatypes", "") << "\""; 
	return true;
}

