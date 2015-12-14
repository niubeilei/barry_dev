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
// 28/07/2010: Created by ketty//////////////////////////////////////////////////////////////////////////
#include "GICs/GicClock.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicClock();

AosGicClock::AosGicClock(const bool flag)
:
AosGic(AOSGIC_CLOCK, AosGicType::eClock, flag)
{
}


AosGicClock::~AosGicClock()
{
}


bool	
AosGicClock::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	OmnString gic_id = AosGetHtmlElemId();
    OmnString gic_number_display = vpd->getAttrStr("gic_data_display","true");
	OmnString gic_week_display = vpd->getAttrStr("gic_data_display2","true");

	OmnString gic_img_src = vpd->getAttrStr("gic_img_src","Clock1");

	OmnString gic_bg_src;
	OmnString gic_hour_src;
	OmnString gic_min_src;
	OmnString gic_sec_src;
	gic_bg_src << "http://218.64.170.28/lps-4.7.2/images/Clock/" << gic_img_src << "/clock_normal.png";
	gic_hour_src << "http://218.64.170.28/lps-4.7.2/images/Clock/" << gic_img_src << "/clock_hour.png" ;
	gic_min_src << "http://218.64.170.28/lps-4.7.2/images/Clock/" << gic_img_src << "/clock_minute.png";
	gic_sec_src << "http://218.64.170.28/lps-4.7.2/images/Clock/" << gic_img_src << "/clock_second.png";


   code.mHtml << "<img style=\"display:none; position:absolute\" src=\"" << gic_bg_src << "\" id=\"" << gic_id << "-bg\" onload=\"AosClock_Bg_Onready = true; \"></img>"
			  << "<img style=\"display:none; position:absolute\" src=\"" << gic_hour_src << "\" id=\"" << gic_id << "-hour\" onload = \"AosClock_Hour_Onready = true; \"></img>"
		      << "<img style=\"display:none; position:absolute\" src=\"" << gic_min_src << "\" id=\"" << gic_id << "-min\" onload = \" AosClock_Min_Onready = true;\"></img>"
			  << "<img style=\"display:none; position:absolute\" src=\"" << gic_sec_src << "\" id=\"" << gic_id << "-sec\" onload=\" AosClock_Sec_Onready = true;\"></img>";

   code.mJson << ",numberDisplay: " << gic_number_display << ","
			  << "weekDisplay: " << gic_week_display << ","
			  << "gicImgSrc:\"" << gic_img_src << "\","
			  << "imgBgSrc:\"" << gic_bg_src << "\","
			  << "imgHourSrc:\"" << gic_hour_src << "\","
			  << "imgMinSrc:\"" << gic_min_src << "\","
			  << "imgSecSrc:\"" << gic_sec_src << "\","
			  << "imgBgId:\"" << gic_id << "-bg\"," 
			  << "imgHourId:\"" << gic_id << "-hour\"," 
			  << "imgMinId:\"" << gic_id << "-min\"," 
			  << "imgSecId:\"" << gic_id << "-sec\""; 

	return true;
}
