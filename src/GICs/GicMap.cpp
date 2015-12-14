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
// 07/25/2010: Created by Michael 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicMap.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicMap();

AosGicMap::AosGicMap(const bool flag)
:
AosGic(AOSGIC_MAP, AosGicType::eMap, flag)
{
}


AosGicMap::~AosGicMap()
{
}


bool	
AosGicMap::generateCode(
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
    
	code.mJson << ",scale:" << vpd->getAttrStr("gic_scale","true")
		       << ",address:\""<<vpd->getAttrStr("gic_address","苏州国际科技园")<<"\""
			   << ",marker:\""<<vpd->getAttrStr("gic_marker","苏州国际科技园")<<"\""
			   << ",maptype:\""<<vpd->getAttrStr("gic_maptype","GMapTypeControl")<<"\""
			   << ",GNavLabelControl:"<<vpd->getAttrStr("gic_gnavlabel","true")
			   << ",enableGoogleBar:"<<vpd->getAttrStr("gic_googlebar","true")
			   << ",enableScrollWheelZoom:"<<vpd->getAttrStr("gic_scrollz","true")
			   << ",bgmap:\""<<vpd->getAttrStr("gic_bgmap","GLargeMapControl3D")<<"\""
               << ",zoom:"<<vpd->getAttrInt("gic_zoom",2)
			   << ",zoomlevel:"<<vpd->getAttrInt("gic_zoomlevel",14)
			   << ",pitch:"<<vpd->getAttrInt("gic_pitch",100)
			   << ",yaw:"<<vpd->getAttrInt("gic_yaw",40)
			   << ",dragcursor:\""<<vpd->getAttrStr("gic_dragcursor","pointer")<<"\""
			   << ",cursor:\""<<vpd->getAttrStr("gic_cursor","pointer")<<"\"";
	


	return true;
}

