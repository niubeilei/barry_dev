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
#include "GICs/GicGrowingChart.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicGrowingChart();

AosGicGrowingChart::AosGicGrowingChart(const bool flag)
:
AosGic(AOSGIC_GROWINGCHART, AosGicType::eGrowingChart, flag)
{
}


AosGicGrowingChart::~AosGicGrowingChart()
{
}


bool	
AosGicGrowingChart::generateCode(
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
		<< ",gic_bgcolor: \"" << vpd->getAttrStr("gic_bgcolor", "#444") 
		<< "\",gic_circlecolor: \"" << vpd->getAttrStr("gic_circlecolor", "#fff") 
		<< "\",gic_borderspan: " << vpd->getAttrInt("gic_borderspan", 50) 
		<< ",gic_segbordercolor: \"" << vpd->getAttrStr("gic_segbordercolor", "#fff") 
		<< "\",gic_title: \"" << vpd->getAttrStr("gic_title", "I am Chart") 
		<< "\",gic_titcolor: \"" << vpd->getAttrStr("gic_titcolor", "#ff00ff")<<"\""; 

	return true;
}

