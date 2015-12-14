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
#include "GICs/GicShapeStr.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicShapeStr();

AosGicShapeStr::AosGicShapeStr(const bool flag)
:
AosGic(AOSGIC_SHAPESTR, AosGicType::eShapeStr, flag)
{
	OmnString errmsg;
	AosGicType::addName(AOSGIC_LABELSHAPESTR, mGicType, errmsg);
}


AosGicShapeStr::~AosGicShapeStr()
{
}


bool	
AosGicShapeStr::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentId,
		AosHtmlCode &code)
{
	// This function will generate:
	// 	1. HTML code
	// 	2. CSS code
	// 	3. JavaScript Code
	// 	4. Flash code
	code.mJson << ",gic_nmlnclr:\'"<<vpd->getAttrStr("gic_nmlnclr")<<"\',"
			<< "gic_ovlnclr:\'"<<vpd->getAttrStr("gic_ovlnclr")<<"\',"
			<< "gic_dwlnclr:\'"<<vpd->getAttrStr("gic_dwlnclr")<<"\',"
			<< "gic_nmlnwd:\'"<<vpd->getAttrStr("gic_nmlnwd")<<"\',"
			<< "gic_ovlnwd:\'"<<vpd->getAttrStr("gic_ovlnwd")<<"\',"
			<< "gic_dwlnwd:\'"<<vpd->getAttrStr("gic_dwlnwd")<<"\',"
			<< "gic_nmshp:\'"<<vpd->getAttrStr("gic_nmshp")<<"\',"
			<< "gic_ovshp:\'"<<vpd->getAttrStr("gic_ovshp")<<"\',"
			<< "gic_dwshp:\'"<<vpd->getAttrStr("gic_dwshp")<<"\',"
			<< "gic_nmclr1:\'"<<vpd->getAttrStr("gic_nmclr1")<<"\',"
			<< "gic_ovclr1:\'"<<vpd->getAttrStr("gic_ovclr1")<<"\',"
			<< "gic_dwclr1:\'"<<vpd->getAttrStr("gic_dwclr1")<<"\',"
			<< "gic_nmclr2:\'"<<vpd->getAttrStr("gic_nmclr2")<<"\',"
			<< "gic_ovclr2:\'"<<vpd->getAttrStr("gic_ovclr2")<<"\',"
			<< "gic_dwclr2:\'"<<vpd->getAttrStr("gic_dwclr2")<<"\',"
			<< "gic_nmop:\'"<<vpd->getAttrStr("gic_nmop")<<"\',"
			<< "gic_ovop:\'"<<vpd->getAttrStr("gic_ovop")<<"\',"
			<< "gic_dwop:\'"<<vpd->getAttrStr("gic_dwop")<<"\',"
			<< "gic_dwndfl:\'"<<vpd->getAttrStr("gic_dwndfl")<<"\',"
			<< "gic_ovndfl:\'"<<vpd->getAttrStr("gic_ovndfl")<<"\',"
			<< "gic_nmndfl:\'"<<vpd->getAttrStr("gic_nmndfl")<<"\',"
			<< "gic_fillrt:\'"<<vpd->getAttrStr("gic_fillrt","0")<<"\',"
			<< "gic_fill1:\'"<<vpd->getAttrStr("gic_fill1")<<"\',"
			<< "gic_fill2:\'"<<vpd->getAttrStr("gic_fill2")<<"\'"
			<<",gic_nmlb: \"" << vpd->getAttrStr("gic_nmlb")
		    << "\",gic_ovlb: \"" << vpd->getAttrStr("gic_ovlb")
		    << "\",gic_dwlb: \"" << vpd->getAttrStr("gic_dwlb")
	        << "\",gic_nmlbbd: \"" << vpd->getAttrStr("gic_nmlbbd")
	        << "\",gic_ovlbbd: \"" << vpd->getAttrStr("gic_ovlbbd")
	        << "\",gic_dwlbbd: \"" << vpd->getAttrStr("gic_dwlbbd")
	        << "\",gic_movbc: \"" << vpd->getAttrStr("gic_movbc","")
	        << "\",gic_motbc: \"" << vpd->getAttrStr("gic_motbc","")
	        << "\",gic_modbc: \"" << vpd->getAttrStr("gic_modbc","")
	        << "\",gic_movfc: \"" << vpd->getAttrStr("gic_movfc")
	        << "\",gic_motfc: \"" << vpd->getAttrStr("gic_motfc","#000000")
	        << "\",gic_modfc: \"" << vpd->getAttrStr("gic_modfc")
	        << "\",gic_movft: \"" << vpd->getAttrStr("gic_movft","Times New Roman")
	        << "\",gic_motft: \"" << vpd->getAttrStr("gic_motft","Times New Roman")
	        << "\",gic_modft: \"" << vpd->getAttrStr("gic_modft","Times New Roman")
	        << "\",gic_modfs: \"" << vpd->getAttrStr("gic_modfs","12")
	        << "\",gic_movfs: \"" << vpd->getAttrStr("gic_movfs","")
	        << "\",gic_motfs: \"" << vpd->getAttrStr("gic_motfs","")
	        << "\",gic_motfy: \"" << vpd->getAttrStr("gic_motfy","normal")
	        << "\",gic_modfy: \"" << vpd->getAttrStr("gic_modfy","normal")
	        << "\",gic_movfy: \"" << vpd->getAttrStr("gic_movfy","normal")
	        << "\",gic_halg: \"" << vpd->getAttrStr("gic_halg")
	        << "\",gic_valg: \"" << vpd->getAttrStr("gic_valg")
	        << "\",gic_recrt: \"" << vpd->getAttrStr("gic_recrt")
	        << "\",gic_labx: \"" << vpd->getAttrStr("gic_labx","0")
	        << "\",gic_laby: \"" << vpd->getAttrStr("gic_laby","0")
			<< "\"" ;

	return true;
}

