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
#include "GICs/GicAnimatedChart.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicAnimatedChart::AosGicAnimatedChart(const bool flag)
:
AosGic(AOSGIC_ANIMATEDCHART, AosGicType::eAnimatedChart, flag)
{
}


AosGicAnimatedChart::~AosGicAnimatedChart()
{
}


bool	
AosGicAnimatedChart::generateCode(
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
		<< ",gic_arrowcolor: \"" << vpd->getAttrStr("gic_arrowcolor", "#000") 
		<< "\",gic_bgcolor: \"" << vpd->getAttrStr("gic_bgcolor", "#333333") 
		<< "\",gic_circlecolor: \"" << vpd->getAttrStr("gic_circlecolor", "#fff") 
		<< "\",gic_fontcolor: \"" << vpd->getAttrStr("gic_fontcolor", "#fff") 
		<< "\",gic_fontxx: " << vpd->getAttrInt("gic_fontxx", 310) 
		<< ",gic_titwidth: " << vpd->getAttrInt("gic_titwidth", 134) 
		<< ",gic_titbgcolor: \"" << vpd->getAttrStr("gic_titbgcolor", "#666666") << "\""; 

	return true;
}

