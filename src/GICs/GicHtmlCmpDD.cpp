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
#include "GICs/GicHtmlCmpDD.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicHtmlCmpDD();

AosGicHtmlCmpDD::AosGicHtmlCmpDD(const bool flag)
:
AosGic(AOSGIC_HTMLCMPDD, AosGicType::eHtmlCmpDD, flag)
{
}


AosGicHtmlCmpDD::~AosGicHtmlCmpDD()
{
}


bool	
AosGicHtmlCmpDD::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{

	OmnString gic_img_src;
	OmnString path = htmlPtr->getImagePath();
	gic_img_src << path<< vpd->getAttrStr("wgt_icon_src");
	
	OmnString gic_label = vpd->getAttrStr("textlabel","");
	OmnString gic_width = vpd->getAttrStr("gic_width","50");	
	OmnString gic_creator_vpdName = vpd->getAttrStr("creator_vpdname","");
    OmnString gic_information = vpd->getAttrStr("information");
    code.mHtml << "<img style=\"width:"<< gic_width << ";height:" << gic_width << "\" src=\"" << gic_img_src << "\" ></img>"
	   		  <<  "<div>" << gic_label << "</div>"; 
	if(gic_information != "")
    	code.mJson << ",information:\"" << gic_information << "\"";
	if(gic_creator_vpdName != "")
    	code.mJson << ",creatorVpdName:\"" << gic_creator_vpdName << "\"";

    return true;
}
