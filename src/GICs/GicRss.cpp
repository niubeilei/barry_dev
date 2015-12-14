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
// 06/28/2010: Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicRss.h"

#include "HtmlModules/Ptrs.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicRss::AosGicRss(const bool flag)
:
AosGic(AOSGIC_RSS, AosGicType::eRss, flag)
{
}


AosGicRss::~AosGicRss()
{
}

bool	
AosGicRss::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	OmnString gic_url= vpd->getAttrStr("gic_url"); 
	code.mJson << ",gic_url: \'" << gic_url <<"\'";
	return true;
}

