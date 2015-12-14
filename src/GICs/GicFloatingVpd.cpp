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
// 07/08/2010: Created by Tracy
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicFloatingVpd.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicFloatingVpd();

AosGicFloatingVpd::AosGicFloatingVpd(const bool flag)
:
AosGic(AOSGIC_FLOATINGVPD, AosGicType::eFloatingVpd, flag)
{
}


AosGicFloatingVpd::~AosGicFloatingVpd()
{
}


bool	
AosGicFloatingVpd::generateCode(
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
	// Tracy, = Debug
	
	code.mJson  
		<< ",gic_vpdname: \"" << vpd->getAttrStr("gic_vpdname", "vpd_Jimmy") 
		<< "\",gic_iconsrc: \"" << vpd->getAttrStr("gic_iconsrc", "img101/ai7631.jpg") 
		<< "\",gic_moviconsrc: \"" << vpd->getAttrStr("gic_moviconsrc", "a1/ai4215.png") 
		<< "\",gic_iconww: " << vpd->getAttrInt("gic_iconww", 18) 
		<< ",gic_movable:" << vpd->getAttrStr("gic_movable", "true"); 

	return true;
}

