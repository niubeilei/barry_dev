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
// 2011/02/19 Created by Ken
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlTimePicker.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlModules/DictionaryHandler.h"
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace boost::gregorian;
using namespace boost::posix_time;

AosGicHtmlTimePicker::AosGicHtmlTimePicker(const bool flag)
:
AosGic(AOSGIC_HTMLTIMEPICKER, AosGicType::eHtmlTimePicker, flag)
{
}


AosGicHtmlTimePicker::~AosGicHtmlTimePicker()
{
}


bool	
AosGicHtmlTimePicker::generateCode(
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
	
	//for display
	OmnString gic_showdate = vpd->getAttrStr("gic_showdate", "inline");
	OmnString gic_showweek = vpd->getAttrStr("gic_showweek", "inline");
	OmnString gic_showhour = vpd->getAttrStr("gic_showhour", "inline");
	OmnString gic_showminue = vpd->getAttrStr("gic_showminue", "inline");
	OmnString gic_showsecond = vpd->getAttrStr("gic_showsecond", "inline");
	
	//height, width
	OmnString gic_height = vpd->getAttrStr("gic_height", "20px");
	OmnString gic_datewidth = vpd->getAttrStr("gic_datewidth", "80px");
	OmnString gic_weekwidth = vpd->getAttrStr("gic_weekwidth", "50px");
	OmnString gic_hourwidth = vpd->getAttrStr("gic_hourwidth", "20px");
	OmnString gic_minuewidth = vpd->getAttrStr("gic_minuewidth", "20px");
	OmnString gic_secondwidth = vpd->getAttrStr("gic_secondwidth", "20px");
	OmnString gic_hoursepwidth = vpd->getAttrStr("gic_hoursepwidth", "20px");
	OmnString gic_minuesepwidth = vpd->getAttrStr("gic_minuesepwidth", "20px");
	OmnString gic_secondsepwidth = vpd->getAttrStr("gic_secondsepwidth", "20px");

	
	//whether to show border
	OmnString gic_dateborder = vpd->getAttrStr("gic_dateborder", "true");
	OmnString gic_weekborder = vpd->getAttrStr("gic_weekborder", "true");
	OmnString gic_hmsborder = vpd->getAttrStr("gic_hmsborder", "true");
	
	//time border
	OmnString gic_timeborderw = vpd->getAttrStr("gic_timeborderw", "1px");
	OmnString gic_timeborderc = vpd->getAttrStr("gic_timeborderc", "#000000");
	OmnString gic_timeborders = vpd->getAttrStr("gic_timeborders", "solid");

	//separate
	OmnString gic_hoursep = vpd->getAttrStr("gic_hoursep", ":");
	OmnString gic_minuesep = vpd->getAttrStr("gic_minuesep", ":");
	OmnString gic_secondsep = vpd->getAttrStr("gic_secondsep", "");

	OmnString gic_loadobj = vpd->getAttrStr("gic_loadobj", "true");
	OmnString gic_language = vpd->getAttrStr("gic_language", "chinese");
	OmnString gic_weekindx = vpd->getAttrStr("gic_weekindx", "");
	OmnString gic_timebd = vpd->getAttrStr("gic_timebd");
	//get value
	OmnString gic_date, gic_week, gic_hour, gic_minue, gic_second;
	OmnString gic_timevalue = vpd->getAttrStr("gic_timevalue", "");
	date_facet *facet = OmnNew time_facet("%Y-%m-%d");
	ostringstream os;
	os.imbue(locale(cout.getloc(), facet));
	ptime p;
	if (gic_timevalue == "")
	{
		p = second_clock::local_time();
	}
	else
	{
		time_t tt = atoi(gic_timevalue.data());
		tm* tm2 = localtime(&tt);
		p = ptime_from_tm(*tm2);
	}	
	os << p.date();
	gic_date << os.str();

	int week = p.date().day_of_week();
	changeWeek(week, gic_week, gic_weekindx, gic_language);

	os.str("");
	os << p.time_of_day().hours();
	gic_hour << os.str();
	if (atoi(gic_hour.data()) < 10)
	{
		gic_hour = "0";
		gic_hour << os.str();
	}

	os.str("");
	os << p.time_of_day().minutes();
	gic_minue << os.str();
	if (atoi(gic_minue.data()) < 10)
	{
		gic_minue = "0";
		gic_minue << os.str();
	}

	os.str("");
	os << p.time_of_day().seconds();
	gic_second << os.str();
	if (atoi(gic_second.data()) < 10)
	{
		gic_second = "0";
		gic_second << os.str();
	}

	if (gic_timevalue == "")
	{
		int time = OmnGetSecond();
		gic_timevalue << time;
	}
	OmnString html = "";

	OmnString gicid = AosGetHtmlElemId();
	OmnString dateid = "date_";
	dateid << gicid;
	OmnString weekid = "week_";
	weekid << gicid;
	OmnString hourid = "hour_";
	hourid << gicid;
	OmnString hoursepid = "hoursep_";
	hoursepid << gicid;
	OmnString minueid = "minue_";
	minueid << gicid;
	OmnString minuesepid = "minuesep_";
	minuesepid << gicid;
	OmnString secondid = "second_";
	secondid << gicid;
	OmnString secondsepid = "secondsep_";
	secondsepid << gicid;

	// date input
	OmnString date_dftcss;
	date_dftcss << "height:20px; width:80px; display:inline; border:1px #000000 solid;";
	OmnString date_css = vpd->getAttrStr("gic_datecss", date_dftcss);
	html << "<input name=\"" << dateid << "\" type=\"text\" value=\"" << gic_date << "\" readonly=\"readonly\" " << 
			"style=\"" << date_css << "\" /> ";

	// week input
	OmnString week_dftcss;
	week_dftcss << "height:20px; width:50px; display:inline; border:1px #000000 solid;";
	OmnString week_css = vpd->getAttrStr("gic_weekcss", week_dftcss);
	html << "<input name=\"" << weekid << "\" type=\"text\" value=\"" << gic_week << "\" readonly=\"readonly\" " << 
			"style=\"" << week_css << "\" /> ";

	OmnString borderstr = "";
	borderstr << "height:20px; width:20px; border-color:#000000; border-style:solid; " <<
		"border-top-width:1px; border-bottom-width:1px;";

	//hour input
	OmnString hour_dftcss;
	hour_dftcss << borderstr << "display:inline; border-right-width:0px; border-left-width:1px;";
	OmnString hour_css = vpd->getAttrStr("gic_hourcss", hour_dftcss);
	html << "<input name=\"" << hourid << "\" type=\"text\" value =\"" << gic_hour << "\" " <<
		    "style=\"" << hour_css << "\" />";

	//hour separate input
	OmnString hoursep_dftcss;
	hoursep_dftcss << borderstr << "display:inline; border-left-width:0px; border-right-width:0px;";
	OmnString hoursep_css = vpd->getAttrStr("gic_hoursepcss", hoursep_dftcss);
	html << "<input name=\"" << hoursepid << "\" type=\"text\" value=\"" << gic_hoursep << "\" readonly=\"readonly\" " <<
		 "style=\"" << hoursep_css << "\" />";

	//minue input
	OmnString minue_css = vpd->getAttrStr("gic_minuecss", hoursep_dftcss);
	html << "<input name=\"" << minueid << "\" type=\"text\" value =\"" << gic_minue << "\" " <<
		    "style=\"" << minue_css << "\" />";

	//minue separate input
	OmnString minuesep_css = vpd->getAttrStr("gic_minuesepcss", hoursep_dftcss);
	html << "<input name=\"" << minuesepid << "\" type=\"text\" value=\"" << gic_minuesep << "\" readonly=\"readonly\" " <<
		 "style=\"" << minuesep_css << "\" />";

	//second input
	OmnString second_dftcss;
	second_dftcss << borderstr << "display:inline; border-left-width:0px; border-right-width:1px;";
	OmnString second_css = vpd->getAttrStr("gic_secondcss", second_dftcss);
	html << "<input name=\"" << secondid << "\" type=\"text\" value =\"" << gic_second << "\" " <<
		    "style=\"" << second_css << "\" />";

	//second separate input
	OmnString secondsep_dftcss;
	secondsep_dftcss << borderstr << "display:none; border-left-width:0px; border-right-width:1px;";
	OmnString secondsep_css = vpd->getAttrStr("gic_secondsepcss", secondsep_dftcss);
	html << "<input name=\"" << secondsepid << "\" type=\"text\" value=\"" << gic_secondsep << "\" readonly=\"readonly\" " <<
		 "style=\"" << secondsep_css << "\" />";

	code.mHtml << html;

	OmnString str = ",";
	str << "gic_showdate:\'" << gic_showdate << "\', "
		<< "gic_showweek:\'" << gic_showweek << "\', "
		<< "gic_showhour:\'" << gic_showhour << "\', "
		<< "gic_showminue:\'" << gic_showminue << "\', "
		<< "gic_showsecond:\'" << gic_showsecond << "\', "
		<< "gic_timevalue:\'" << gic_timevalue << "\', "
		<< "gic_datewidth:\'" << gic_datewidth << "\', "
		<< "gic_weekwidth:\'" << gic_weekwidth << "\', "
		<< "gic_hourwidth:\'" << gic_hourwidth << "\', "
		<< "gic_minuewidth:\'" << gic_minuewidth << "\', "
		<< "gic_secondwidth:\'" << gic_secondwidth << "\', "
		<< "gic_hoursepwidth:\'" << gic_hoursepwidth << "\', "
		<< "gic_minuesepwidth:\'" << gic_minuesepwidth << "\', "
		<< "gic_secondsepwidth:\'" << gic_secondsepwidth << "\', "
		<< "gic_dateborder:\'" << gic_dateborder << "\', "
		<< "gic_weekborder:\'" << gic_weekborder << "\', "
		<< "gic_hmsborder:\'" << gic_hmsborder << "\', "
		<< "gic_timeborderw:\'" << gic_timeborderw << "\', "
		<< "gic_timeborderc:\'" << gic_timeborderc << "\', "
		<< "gic_timeborders:\'" << gic_timeborders << "\', "
		<< "gic_hoursep:\'" << gic_hoursep << "\', "
		<< "gic_minuesep:\'" << gic_minuesep << "\', "
		<< "gic_secondsep:\'" << gic_secondsep << "\', "
		<< "gic_dateid:\'" << dateid << "\', "
		<< "gic_weekid:\'" << weekid << "\', "
		<< "gic_hourid:\'" << hourid << "\', "
		<< "gic_minueid:\'" << minueid << "\', "
		<< "gic_secondid:\'" << secondid << "\', "
		<< "gic_hoursepid:\'" << hoursepid << "\', "
		<< "gic_minuesepid:\'" << minuesepid << "\', "
		<< "gic_language:\'" << gic_language << "\', "
		<< "gic_weekindx:\'" << gic_weekindx << "\', "
		<< "gic_timebd:\'" << gic_timebd << "\', "
		<< "gic_loadobj:" << gic_loadobj << ","
		<< "gic_secondsepid:\'" << secondsepid << "\'";


	code.mJson << str;
	return true;
}


bool	
AosGicHtmlTimePicker::changeWeek(
		const int &week,
		OmnString &gic_week,
		OmnString &gic_weekindx,
		const OmnString &gic_language)
{
	switch(week)
	{
		case 0:
			if(gic_language == "chinese")
			{
				gic_week = "星期日";
			}
			else
			{
				gic_week = "Sun";
			}
			gic_weekindx = "0";
			break;
		case 1:
			if(gic_language == "chinese")
			{
				gic_week = "星期一";
			}
			else
			{
				gic_week = "Mon";
			}
			gic_weekindx = "1";
			break;
		case 2:
			if(gic_language == "chinese")
			{
				gic_week = "星期二";
			}
			else
			{
				gic_week = "Tues";
			}
			gic_weekindx = "2";
			break;
		case 3:
			if(gic_language == "chinese")
			{
				gic_week = "星期三";
			}
			else
			{
				gic_week = "Wed";
			}
			gic_weekindx = "3";
			break;
		case 4:
			if(gic_language == "chinese")
			{
				gic_week = "星期四";
			}
			else
			{
				gic_week = "Thurs";
			}
			gic_weekindx = "4";
			break;
		case 5:
			if(gic_language == "chinese")
			{
				gic_week = "星期五";
			}
			else
			{
				gic_week = "Fri";
			}
			gic_weekindx = "5";
			break;
		case 6:
			if(gic_language == "chinese")
			{
				gic_week = "星期六";
			}
			else
			{
				gic_week = "Sat";
			}
			gic_weekindx = "6";
			break;
		default:
			break;
	}
	return true;
}
