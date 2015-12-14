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
// 06/28/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicSchedule.h"

#include "HtmlModules/Ptrs.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicSchedule::AosGicSchedule(const bool flag)
:
AosGic(AOSGIC_SCHEDULE, AosGicType::eSchedule, flag)
{
}


AosGicSchedule::~AosGicSchedule()
{
}


bool	
AosGicSchedule::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	OmnString gic_ctnr = vpd->getAttrStr("gic_ctnr"); 
	OmnString gic_sdstyle = vpd->getAttrStr("gic_sdstyle"); 
	OmnString gic_sestyle = vpd->getAttrStr("gic_sestyle"); 
	OmnString gic_datestyle = vpd->getAttrStr("gic_datestyle"); 
	OmnString gic_weekstyle = vpd->getAttrStr("gic_weekstyle"); 
	OmnString gic_ctnid = vpd->getAttrStr("gic_ctnid"); 
	bool gic_isudata= vpd->getAttrBool("gic_isudata");
	code.mJson << ",gic_ctnr: \'"<< gic_ctnr <<"\',"
			<< "gic_sdstyle: \'"<< gic_sdstyle<<"\',"
			<< "gic_sestyle: \'"<< gic_sestyle<<"\',"
			<< "gic_datestyle: \'"<< gic_datestyle<<"\',"
			<< "gic_weekstyle: \'"<< gic_weekstyle<<"\',"
			<< "gic_isudata: "<< gic_isudata<<","
			<< "gic_ctnid: \'"<< gic_ctnid <<"\'";
	return true;
}

