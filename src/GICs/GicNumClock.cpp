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
// 17/02/201: Created by Ketty//////////////////////////////////////////////////////////////////////////
#include "GICs/GicNumClock.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicNumClock();

AosGicNumClock::AosGicNumClock(const bool flag)
:
AosGic(AOSGIC_NUMCLOCK, AosGicType::eNumClock, flag)
{
}


AosGicNumClock::~AosGicNumClock()
{
}


bool	
AosGicNumClock::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	OmnString gic_id = AosGetHtmlElemId();
	
	OmnString imgPath = "http://218.64.170.28/lps-4.7.2/images/";

	OmnString gic_bg_src = vpd->getAttrStr("gic_bg_src","image_2/ct9500.jpg");
	OmnString gic_time_left = vpd->getAttrStr("gic_time_left", "35");
	OmnString gic_time_top = vpd->getAttrStr("gic_time_top", "30");
	OmnString gic_time_size = vpd->getAttrStr("gic_time_size", "32");
	OmnString gic_time_color = vpd->getAttrStr("gic_time_color","8C867E");
	OmnString gic_time_style = vpd->getAttrStr("gic_time_style","Kenia");
	OmnString gic_sec_display = vpd->getAttrStr("gic_sec_display","true");

	OmnString gic_date_left = vpd->getAttrStr("gic_date_left", "50");
	OmnString gic_date_top = vpd->getAttrStr("gic_date_top", "0");
	OmnString gic_date_size = vpd->getAttrStr("gic_date_size", "16");
	OmnString gic_date_color = vpd->getAttrStr("gic_date_color","8C867E");
	OmnString gic_date_style = vpd->getAttrStr("gic_date_style","Gruppo");
	OmnString gic_date_display = vpd->getAttrStr("gic_date_display","true");
    
	OmnString gic_week_left = vpd->getAttrStr("gic_week_left", "120");
	OmnString gic_week_top = vpd->getAttrStr("gic_week_top", "0");
	OmnString gic_week_size = vpd->getAttrStr("gic_week_size", "16");
	OmnString gic_week_color = vpd->getAttrStr("gic_week_color","8C867E");
	OmnString gic_week_style = vpd->getAttrStr("gic_week_style","Geo");
	OmnString gic_week_display = vpd->getAttrStr("gic_week_display","true");

	OmnString gic_period_left = vpd->getAttrStr("gic_period_left", "120");
	OmnString gic_period_top = vpd->getAttrStr("gic_period_top", "50");
	OmnString gic_period_size = vpd->getAttrStr("gic_period_size", "16");
	OmnString gic_period_color = vpd->getAttrStr("gicperiode_color","8C867E");
	OmnString gic_period_style = vpd->getAttrStr("gic_period_style","Radley");
	OmnString gic_period_display = vpd->getAttrStr("gic_period_display","true");


	code.mJson << ",imgPath:\"" << imgPath << "\","
			  <<  "mTimeLeft:" << gic_time_left << ","
			  <<  "mTimeTop:" << gic_time_top << ","
			  <<  "mTimeSize:" << gic_time_size << ","
			  <<  "mTimeColor:\"" << gic_time_color << "\","
			  <<  "mTimeFStyle:\"" << gic_time_style << "\","
			  <<  "showSec:" << gic_sec_display << ","
			  <<  "mDateLeft:" << gic_date_left << ","
			  <<  "mDateTop:" << gic_date_top << ","
			  <<  "mDateSize:" << gic_date_size << ","
			  <<  "mDateColor:\"" << gic_date_color << "\","
			  <<  "mDateFStyle:\"" << gic_date_style << "\","
			  <<  "showDate:" << gic_date_display << ","
			  <<  "mWeekLeft:" << gic_week_left << ","
			  <<  "mWeekTop:" << gic_week_top << ","
			  <<  "mWeekSize:" << gic_week_size << ","
			  <<  "mWeekColor:\"" << gic_week_color << "\","
			  <<  "mWeekFStyle:\"" << gic_week_style << "\","
			  <<  "showWeek:" << gic_week_display << ","
			  <<  "mPeriodLeft:" << gic_period_left << ","
			  <<  "mPeriodTop:" << gic_period_top << ","
			  <<  "mPeriodSize:" << gic_period_size << ","
			  <<  "mPeriodColor:\"" << gic_period_color << "\","
			  <<  "mPeriodFStyle:\"" << gic_period_style << "\","
			  <<  "showPeriod:" << gic_period_display << ","


			  << "bgSrc:\"" << gic_bg_src << "\"";

	return true;
}
