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
#include "GICs/GicFlash.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


// static AosGicPtr sgGic = new AosGicFlash();

AosGicFlash::AosGicFlash(const bool flag)
:
AosGic(AOSGIC_FLASH, AosGicType::eFlash, flag)
{
}


AosGicFlash::~AosGicFlash()
{
}


bool	
AosGicFlash::generateCode(
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
	
	OmnString srcbd = vpd->getAttrStr("gic_srcbd","");
	OmnString gic_frame = vpd->getAttrStr("gic_frame", "true");
	OmnString flash_type = vpd->getAttrStr("flash_type","swf");
	OmnString gic_src = vpd->getAttrStr("gic_rsc", "**.swf");
	OmnString gic_rpttm = vpd->getAttrStr("gic_rpttm","true");
	OmnString gic_auto = vpd->getAttrStr("gic_auto","true");
	 code.mJson << ",src : \'"<< gic_src <<"\'"
    		    << ",srcbd :\'"<< srcbd <<"\'"
			    << ",frame : " << gic_frame
			    << ",flashType :\'" << flash_type <<"\'"
			    << ",loop : " << gic_rpttm
			    << ",autostart : " << gic_auto;

	return true;
}


