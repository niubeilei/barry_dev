//////////////////////////////////////////////////////////////////////////// //
//// Copyright (C) 2005 // Packet Engineering, Inc. All rights reserved. 
//// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
// Modification History:
// 07/26/2010: Created by Brian 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlCount.h"

#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "HtmlUtil/HtmlUtil.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "HtmlModules/DclDb.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "XmlUtil/Ptrs.h"

#include <string.h>
#include <stdio.h>
#include <map>

AosGicHtmlCount::AosGicHtmlCount(const bool flag)
:
AosGic(AOSGIC_HTMLCOUNT, AosGicType::eHtmlCount, flag)
{
}


AosGicHtmlCount::~AosGicHtmlCount()
{
}


bool	
AosGicHtmlCount::generateCode(
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
	//This function converts the VPD to json form
	aos_assert_r(init(vpd), false);

	// Create Html code
	createHtmlCode(code);
	
	// Create Json code
	bool rslt = createJsonCode(code);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosGicHtmlCount::init(
		const AosXmlTagPtr &vpd)
{
	// This function init all member data.
	// vpd should be this format:
	// <vpd ....../>
	aos_assert_r(vpd, false);

	//Init color of pre_text, next_text and countdown
	mCountColor = vpd->getAttrStr(AOSTAG_COUNTDOWN_COLOR,"black");

	//Init font-size of pre_text, next_text and countdown
	mCountFontSize = vpd->getAttrStr(AOSTAG_COUNTDOWN_SIZE,"12");
	
	//Init style of pre_text, next_text and countdown
	mCountStyle = vpd->getAttrStr(AOSTAG_COUNTDOWN_STYLE,"normal");

	//Init countdown or sumtime
	mCountWay = vpd->getAttrStr(AOSTAG_COUNT_WAY,"countdown");
	aos_assert_r(mCountWay != "", false);
	
	// init time format
	mCountShowFormat = vpd->getAttrStr(AOSTAG_COUNTSHOWFORMAT,"1");
	aos_assert_r(mCountShowFormat != "", false);
	
	// init time format
	mCountTimeFormat = vpd->getAttrStr(AOSTAG_COUNTTIMEFORMAT,"1");
	aos_assert_r(mCountTimeFormat != "", false);
	
	//Init interval
	mInterval = vpd->getAttrInt(AOSTAG_INTERVAL,1000);
	aos_assert_r(mInterval != 0, false);

	//Init databind 
	mCountDB = vpd->getAttrStr(AOSTAG_DATABIND,"@src");
	aos_assert_r(mCountDB != "", false);
	
	mCountTimeFormatDB = vpd->getAttrStr(AOSTAG_COUNTTIMEFORMATDB,"@time_src");
	aos_assert_r(mCountTimeFormatDB != "", false);
	
	// Init deadline time
	mTimePoint = vpd->getAttrStr(AOSTAG_TIMEPOINT, "3000/01/01/00/00/00");
	aos_assert_r(mTimePoint != "", false);
	
	return true;
}


bool 
AosGicHtmlCount::createHtmlCode(AosHtmlCode &code)
{
	// This function create html code
	// <div>
	// 		<div> pre </div>
	// 	 	<div> countdown </div>
	// 	 	<div> next </div>
	// </div>
	code.mHtml << "<div class=\"gic_count_bk\" style=\"position:absolute;"
		<< "font-size:" << mCountFontSize << "px;"
		<< "color:" << mCountColor << ";"
		<< setStyle(mCountStyle) << "\">" 
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin-right:10px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin-right:10px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin-right:10px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin-right:10px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin-right:10px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "<div style=\"float:left;margin:1px;width:15px;height:22px;\"></div>"
		<< "</div>";
	return true;
}


bool	
AosGicHtmlCount::createJsonCode(AosHtmlCode &code)
{
	// This function create json code
	// if count_way is "count_down"
	// json code should have 
	// year,month,day,hour,minute,second.
	code.mJson << ","
		<< "mCountWay:\"" << mCountWay << "\","
		<< "mCountFontSize:\"" << mCountFontSize << "\","
		<< "mCountColor:\"" << mCountColor << "\","
		<< "mCountShowFormat:\"" << mCountShowFormat << "\","
		<< "mCountTimeFormat:\"" << mCountTimeFormat << "\","
		<< "mCountDB:\"" << mCountDB << "\","
		<< "mCountTimeFormatDB:\"" << mCountTimeFormatDB << "\","
		<< "mTimePoint:\"" << mTimePoint << "\","
		<< "mInterval:" << mInterval << ","
	    << "mCountStyle:\"" << mCountStyle << "\"";
	
	return true ;
}


OmnString
AosGicHtmlCount::setStyle(const OmnString &style)
{
	OmnString str ;
	if(style == "normal")
	{
	     str << "font-weight:normal;";
		 str << "font-style:normal;";
	}
	else if(style == "plain")
	{
	     str << "font-weight:normal;";
		 str << "font-style:normal;";
	}
	else if(style == "bold")
	{
	     str << "font-weight:bold;";
		 str << "font-style:normal;";
	}
	else if(style == "italic")
	{
	     str << "font-weight:normal;";
		 str << "font-style:italic;";
	}
	else if(style == "bolditalic")
	{
	     str << "font-weight:bold;";
		 str << "font-style:italic;";
	}
	return str ;
}

/*	
OmnString 
AosGicHtmlCount::getXml(const AosRandomRulePtr &rule)
{
	mRule = rule;

    OmnString xml, name, value,
			  nodename = rule->mNodeName;

	xml << "<" << nodename << " ";
	XmlGenRuleValueIter itr;
	for (itr = rule->mAttrs_r.begin(); itr != rule->mAttrs_r.end(); ++itr)
	{
		name = itr->first;
		value = (itr->second).getRandomValue();

		xml << name << "=\"" << value << "\" ";
	}
	for (itr = rule->mAttrs_o.begin(); itr != rule->mAttrs_o.end(); ++itr)
	{
		name = itr->first;
		value = (itr->second).getRandomValue();

		if (rand()%10 < 8)
		{
			xml << name << "=\"" << value << "\" ";
		}
	}
	xml << ">";
	map<OmnString, AosRandomRulePtr>::iterator nit;
	for(nit = rule->mNodes_r.begin();nit!= rule->mNodes_r.end();nit++)
	{
		xml << "<items>";
		int r = rand()%5;
		for(int i=0; i<r; i++)
		{
			xml << getXml(nit->second);
		}
		xml << "</items>";
	}
	xml << "</" << nodename << ">";
	return xml;
}

*/
