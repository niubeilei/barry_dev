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
#include "GICs/GicLabelStr.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicLabelStr();

AosGicLabelStr::AosGicLabelStr(const bool flag)
:
AosGic(AOSGIC_LABELSTR, AosGicType::eLabelStr, flag)
{
}


AosGicLabelStr::~AosGicLabelStr()
{
}


bool	
AosGicLabelStr::generateCode(
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
	

	code.mJson 
		<< ",gic_nmlb: \"" << vpd->getAttrStr("gic_nmlb") 
		<< "\",gic_ovlb: \"" << vpd->getAttrStr("gic_ovlb") 
		<< "\",gic_dwlb: \"" << vpd->getAttrStr("gic_dwlb") 
		<< "\",gic_nmlbbd: \"" << vpd->getAttrStr("gic_nmlbbd") 
		<< "\",gic_ovlbbd: \"" << vpd->getAttrStr("gic_ovlbbd") 
		<< "\",gic_dwlbbd: \"" << vpd->getAttrStr("gic_dwlbbd") 
		<< "\",gic_movbc: \"" << vpd->getAttrStr("gic_movbc","") 
		<< "\",gic_motbc: \"" << vpd->getAttrStr("gic_motbc","") 
		<< "\",gic_mdwbc: \"" << vpd->getAttrStr("gic_mdwbc","") 
		<< "\",gic_movfc: \"" << vpd->getAttrStr("gic_movfc","") 
		<< "\",gic_motfc: \"" << vpd->getAttrStr("gic_motfc","") 
		<< "\",gic_modfc: \"" << vpd->getAttrStr("gic_modfc","") 
		<< "\",gic_movft: \"" << vpd->getAttrStr("gic_movft","Times New Roman") 
		<< "\",gic_motft: \"" << vpd->getAttrStr("gic_motft","") 
		<< "\",gic_modft: \"" << vpd->getAttrStr("gic_modft","") 
		<< "\",gic_modfs: \"" << vpd->getAttrStr("gic_modfs","12") 
		<< "\",gic_movfs: \"" << vpd->getAttrStr("gic_movfs","") 
		<< "\",gic_motfs: \"" << vpd->getAttrStr("gic_motfs","") 
		<< "\",gic_motfy: \"" << vpd->getAttrStr("gic_motfy","normal") 
		<< "\",gic_modfy: \"" << vpd->getAttrStr("gic_modfy","") 
		<< "\",gic_movfy: \"" << vpd->getAttrStr("gic_movfy","") 
		<< "\",gic_labx: \"" << vpd->getAttrStr("gic_labx","0") 
		<< "\",gic_laby: \"" << vpd->getAttrStr("gic_laby","0") 
		<< "\",gic_move: \"" << vpd->getAttrStr("gic_move") 
		<< "\",gic_speed: \"" << vpd->getAttrStr("gic_speed","3000") 
		<< "\",gic_halg: \"" << vpd->getAttrStr("gic_halg","") 
		<< "\",gic_valg: \"" << vpd->getAttrStr("gic_valg","") 
		<< "\"" ;

	return true;
}

