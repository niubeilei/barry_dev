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
// 07/08/2010: Created by Tank
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicCalendar.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicCalendar();

AosGicCalendar::AosGicCalendar(const bool flag)
:
AosGic(AOSGIC_CALENDAR, AosGicType::eCalendar, flag)
{
}


AosGicCalendar::~AosGicCalendar()
{
}


bool	
AosGicCalendar::generateCode(
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
	//convertToJson(vpd,jsonStr);
	//test
	OmnString valbd = vpd->getAttrStr("gic_initDate");
	OmnString gic_initdate;
/*	if(valbd !="" && obj){
		gic_initdate = obj->getAttrStr(valbd,vpd->getAttrStr("gic_D"));
	}
	else
	{gic_initdate = vpd->getAttrStr("gic_D","Calendar");};	 
	OmnString gic_value = vpd->getAttrStr("gic_value","Calendar");
	OmnString gic_id = AosGetHtmlElemId();
	vpd->setAttr("gic_id",gic_id);
	aos_assert_r(convertToJson(vpd, obj, jsonStr),false);
	return true;
}
*/


	// This function converts the VPD to Json form.
	OmnString gic_type = vpd->getAttrStr("gic_type","gic_calendar");
	OmnString gic_selval = vpd->getAttrStr("gic_selval","2008/08/08");
	OmnString gic_selvalbd = vpd->getAttrStr("gic_selvalbd","@selectday");
	OmnString gic_bg_src = vpd->getAttrStr("gic_bg_src");
	OmnString gic_format = vpd->getAttrStr("gic_format","Y-m-d");
	bool gic_selector = vpd->getAttrBool("gic_selector");
	bool gic_permit = vpd->getAttrBool("gic_permit", true);
	code.mJson << ",gic_selval:\'"<<gic_selval<<"\'," ;
	code.mJson << "selval:\'"<<gic_selvalbd<<"\'," ;
	code.mJson << "gic_bg_src:\'"<<gic_bg_src<<"\'," ;
	code.mJson << "gic_selector:"<<gic_selector<<"," ;
	code.mJson << "gic_permit:"<<gic_permit<<"," ;
	code.mJson << "gic_format:\'"<<gic_format<<"\'" ;
	 return true;
}

