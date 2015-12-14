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
#include "GICs/GicBarChart.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"


AosGicBarChart::AosGicBarChart(const bool flag)
:
AosGic(AOSGIC_BARCHART, AosGicType::eBarChart, flag)
{
}


AosGicBarChart::~AosGicBarChart()
{
}


bool	
AosGicBarChart::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentId,
		AosHtmlCode &code)
{
	// This function will generate:
	// 	1. HTML code
	code.mJson 
			<<",gic_value:\'"<<vpd->getAttrStr("gic_value")
			<<"\',gic_value1:\'"<<vpd->getAttrStr("gic_value1")
			<<"\',gic_value2:\'"<<vpd->getAttrStr("gic_value2")
			<<"\',gic_value3:\'"<<vpd->getAttrStr("gic_value3")
			<<"\',gic_color1:\'"<<vpd->getAttrStr("gic_color1","#ff0000")
			<<"\',gic_color2:\'"<<vpd->getAttrStr("gic_color2","#ffff00")
			<<"\',gic_color3:\'"<<vpd->getAttrStr("gic_color3","#00ffff")
			<<"\',usedatacol:\'"<<vpd->getAttrStr("usedatacol")
			<<"\',gic_space:\'"<<vpd->getAttrStr("gic_space")
			<<"\',gic_str:\'"<<vpd->getAttrStr("gic_str","30|50|20|80$35|20|55|66")
			<<"\'";
	return true;
}


OmnString 
AosGicBarChart::generateGicVpd()
{
    return "";
}
