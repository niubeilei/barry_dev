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
#include "GICs/GicLabelImgStr.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicLabelImgStr();

AosGicLabelImgStr::AosGicLabelImgStr(const bool flag)
:
AosGic(AOSGIC_LABELIMGSTR, AosGicType::eLabelImgStr, flag)
{
}


AosGicLabelImgStr::~AosGicLabelImgStr()
{
}


bool	
AosGicLabelImgStr::generateCode(
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
    OmnString gic_movfc = vpd->getAttrStr("gic_movfc","#000000");
	if(gic_movfc!="") gic_movfc = AosHtmlUtil::getWebColor(gic_movfc);
	OmnString gic_motfc = vpd->getAttrStr("gic_motfc","#000000");
	if(gic_motfc!="") gic_motfc = AosHtmlUtil::getWebColor(gic_motfc);
	OmnString gic_modfc = vpd->getAttrStr("gic_modfc","#000000");
	if(gic_modfc!="") gic_modfc = AosHtmlUtil::getWebColor(gic_modfc);
	code.mJson << ",gic_nmlb: \"" << vpd->getAttrStr("gic_nmlb")
		       << "\",gic_ovlb: \"" << vpd->getAttrStr("gic_ovlb")
		       << "\",gic_dwlb: \"" << vpd->getAttrStr("gic_dwlb")
		       << "\",gic_movfc: \"" << gic_movfc
		       << "\",gic_motfc: \"" << gic_motfc
		       << "\",gic_modfc: \"" << gic_modfc
		       << "\",gic_movft: \"" << vpd->getAttrStr("gic_movft","Times New Roman")
		       << "\",gic_motft: \"" << vpd->getAttrStr("gic_motft","Times New Roman")
		       << "\",gic_modft: \"" << vpd->getAttrStr("gic_modft","Times New Roman")
		       << "\",gic_modfs: " << vpd->getAttrInt("gic_modfs",12)
		       << ",gic_movfs: " << vpd->getAttrInt("gic_movfs",12)
		       << ",gic_motfs: " << vpd->getAttrInt("gic_motfs",12)
		       << ",gic_motfy: \"" << vpd->getAttrStr("gic_motfy","normal")
		       << "\",gic_modfy: \"" << vpd->getAttrStr("gic_modfy","normal")
		       << "\",gic_movfy: \"" << vpd->getAttrStr("gic_movfy","normal")
		       << "\",gic_labx: " << vpd->getAttrInt("gic_labx",0)
		       << ",gic_laby: " << vpd->getAttrInt("gic_laby",0)
		       << ",gic_halg: \"" << vpd->getAttrStr("gic_halg")
		       << "\",gic_valg: \"" << vpd->getAttrStr("gic_valg")
			   << "\",gic_lock: \"" << vpd->getAttrStr("gic_lock","2")
			   << "\",gic_nmmwh: \"" << vpd->getAttrStr("gic_nmmwh")<<"\""
			   << ",gic_nmmht: \"" << vpd->getAttrStr("gic_nmmht")<<"\""
			   << ",gic_nmmg: \"" << vpd->getAttrStr("gic_nmmg","img/dt800.png")
			   << "\",gic_ovmg: \"" << vpd->getAttrStr("gic_ovmg")
			   << "\",gic_dwmg: \"" << vpd->getAttrStr("gic_dwmg")
			   << "\",gic_nmmgbd: \"" << vpd->getAttrStr("gic_nmmgbd")
			   << "\",gic_ovmgbd: \"" << vpd->getAttrStr("gic_ovmgbd")
			   << "\",gic_dwmgbd: \"" << vpd->getAttrStr("gic_dwmgbd")
               << "\",gic_nmlbbd: \"" << vpd->getAttrStr("gic_nmlbbd")
               << "\",gic_ovlbbd: \"" << vpd->getAttrStr("gic_ovlbbd")
		       << "\",gic_dwlbbd: \"" << vpd->getAttrStr("gic_dwlbbd")
		       << "\",gic_movable:" << vpd->getAttrStr("gic_movable","false");
	


	return true;
}

