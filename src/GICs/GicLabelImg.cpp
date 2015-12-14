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
#include "GICs/GicLabelImg.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicLabelImg();

AosGicLabelImg::AosGicLabelImg(const bool flag)
:
AosGic(AOSGIC_LABELIMG, AosGicType::eLabelImg, flag)
{
}


AosGicLabelImg::~AosGicLabelImg()
{
}


bool	
AosGicLabelImg::generateCode(
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

	code.mJson << ",gic_nmmg: \"" << vpd->getAttrStr("gic_nmmg","a3/es1073.jpg") 
			   << "\",gic_ovmg: \"" << vpd->getAttrStr("gic_ovmg") 
			   << "\",gic_dwmg: \"" << vpd->getAttrStr("gic_dwmg") 
			   << "\",gic_nmmwh: \"" << vpd->getAttrStr("gic_nmmwh","100") 
			   << "\",gic_ovmwh: \"" << vpd->getAttrStr("gic_ovmwh","110") 
			   << "\",gic_dwmwh: \"" << vpd->getAttrStr("gic_dwmwh") 
		       << "\",gic_dwmht: \"" << vpd->getAttrStr("gic_dwmht") 
		       << "\",gic_ovmht: \"" << vpd->getAttrStr("gic_ovmht","50") 
		       << "\",gic_nmmht: \"" << vpd->getAttrStr("gic_nmmht","40") 
		       << "\",gic_movy: \"" << vpd->getAttrStr("gic_movy","0") 
		       << "\",gic_mdwy: \"" << vpd->getAttrStr("gic_mdwy","0") 
		       << "\",gic_nmop: \"" << vpd->getAttrStr("gic_nmop","1") 
			   << "\",gic_ovop: \"" << vpd->getAttrStr("gic_ovop","1") 
			   << "\",gic_dwop: \"" << vpd->getAttrStr("gic_dwop","1")
			   << "\",gic_motbc: \"" << vpd->getAttrStr("gic_motbc")
			   << "\",gic_movbc: \"" << vpd->getAttrStr("gic_movbc")
			   << "\",gic_mdwbc: \"" << vpd->getAttrStr("gic_mdwbc")
			   <<"\"";

	return true;
}

