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
#include "GICs/GicAnalyticsChart.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicAnalyticsChart();

AosGicAnalyticsChart::AosGicAnalyticsChart(const bool flag)
:
AosGic(AOSGIC_ANALYTICSCHART, AosGicType::eAnalyticsChart, flag)
{
}


AosGicAnalyticsChart::~AosGicAnalyticsChart()
{
}


bool	
AosGicAnalyticsChart::generateCode(
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
		<< ",gic_bgcolor: \"" << vpd->getAttrStr("gic_bgcolor", "#000") 
		<< "\",gic_chartcolor: \"" << vpd->getAttrStr("gic_chartcolor", "#ff00ff") 
		<< "\",gic_point: \"" << vpd->getAttrStr("gic_point") 
		<< "\",gic_value: \"" << vpd->getAttrStr("gic_value") 
		<< "\",usedatacol: \"" << vpd->getAttrStr("usedatacol") 
		<< "\",chart_data: \"" << vpd->getAttrStr("chart_data", "") 
		<< "\",gic_gridcolor: \"" << vpd->getAttrStr("gic_gridcolor", "#ffff00") << "\""; 

	return true;
}

