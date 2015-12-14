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
// 07/07/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicPieChart.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicPieChart();

AosGicPieChart::AosGicPieChart(const bool flag)
:
AosGic(AOSGIC_PIECHART, AosGicType::ePieChart, flag)
{
}


AosGicPieChart::~AosGicPieChart()
{
}


bool	
AosGicPieChart::generateCode(
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

	code.mJson 
			<<",gic_pie:\'"<<vpd->getAttrStr("gic_pie","30||30%$$20||20%$$10||10%$$15||15%$$25||25%")
			<<"\',gic_piebd:\'"<<vpd->getAttrStr("gic_piebd","")
			<<"\',gic_str:\'"<<vpd->getAttrStr("gic_str","")
			<<"\',gic_point:\'"<<vpd->getAttrStr("gic_point","")
			<<"\',gic_value:\'"<<vpd->getAttrStr("gic_value","")
			<<"\',usedatacol:\'"<<vpd->getAttrStr("usedatacol","")
			<<"\',gic_nmlnclr:\'"<<vpd->getAttrStr("gic_nmlnclr")
			<<"\'";
	return true;
}

