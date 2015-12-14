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
#include "GICs/GicShape.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"


AosGicShape::AosGicShape(const bool flag)
:
AosGic(AOSGIC_SHAPE, AosGicType::eShape, flag)
{
	OmnString errmsg;
	AosGicType::addName(AOSGIC_LABELSHAPE, mGicType, errmsg);
}


AosGicShape::~AosGicShape()
{
}


bool	
AosGicShape::generateCode(
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
	int width = vpd->getAttrInt("gic_width", 100);	
	int height = vpd->getAttrInt("gic_height", 100);	
	OmnString nmshape = vpd->getAttrStr("gic_nmshp");	
	int nmlnwd = vpd->getAttrInt("gic_nmlnwd", 2);	
	OmnString recrt = vpd->getAttrStr("gic_recrt");	
	OmnString nmclr1 = vpd->getAttrStr("gic_nmclr1");
	OmnString nmclr2 = vpd->getAttrStr("gic_nmclr2");
	OmnString motbc= vpd->getAttrStr("gic_motbc");
	OmnString nmop = vpd->getAttrStr("gic_nmop","1");	
	OmnString nmndfl = vpd->getAttrStr("gic_nmndfl");
	OmnString nmlnclr= vpd->getAttrStr("gic_nmlnclr");
	OmnString fillrt= vpd->getAttrStr("gic_fillrt","0");

	code.mHtml << "<svg pwidth=\""<< width 
	<<"\" pheight=\""<< height 
	<<"\" nmshape=\""<< nmshape 
	<<"\" nmlnwd=\""<< nmlnwd 
	<<"\" recrt=\""<< recrt 
	<<"\" nmclr1=\""<< nmclr1 
	<<"\" nmclr2=\""<< nmclr2 
	<<"\" nmlnclr=\""<< nmlnclr 
	<<"\" nmop=\""<<nmop 
	<<"\" nmndfl=\""<<nmndfl 
	<<"\" fillrt=\""<<fillrt
	<<"\"/>"; 

	code.mJson << ",gic_nmlnclr:\""<<vpd->getAttrStr("gic_nmlnclr")<<"\","
			<< "gic_ovlnclr:\""<<vpd->getAttrStr("gic_ovlnclr")<<"\","
			<< "gic_dwlnclr:\""<<vpd->getAttrStr("gic_dwlnclr")<<"\","
			<< "gic_nmlnwd:\""<<vpd->getAttrStr("gic_nmlnwd")<<"\","
			<< "gic_ovlnwd:\""<<vpd->getAttrStr("gic_ovlnwd")<<"\","
			<< "gic_dwlnwd:\""<<vpd->getAttrStr("gic_dwlnwd")<<"\","
			<< "gic_nmshp:\""<<vpd->getAttrStr("gic_nmshp")<<"\","
			<< "gic_ovshp:\""<<vpd->getAttrStr("gic_ovshp")<<"\","
			<< "gic_dwshp:\""<<vpd->getAttrStr("gic_dwshp")<<"\","
			<< "gic_nmclr1:\""<<vpd->getAttrStr("gic_nmclr1")<<"\","
			<< "gic_ovclr1:\""<<vpd->getAttrStr("gic_ovclr1")<<"\","
			<< "gic_dwclr1:\""<<vpd->getAttrStr("gic_dwclr1")<<"\","
			<< "gic_nmclr2:\""<<vpd->getAttrStr("gic_nmclr2")<<"\","
			<< "gic_ovclr2:\""<<vpd->getAttrStr("gic_ovclr2")<<"\","
			<< "gic_dwclr2:\""<<vpd->getAttrStr("gic_dwclr2")<<"\","
			<< "gic_nmop:\""<<vpd->getAttrStr("gic_nmop")<<"\","
			<< "gic_ovop:\""<<vpd->getAttrStr("gic_ovop")<<"\","
			<< "gic_dwop:\""<<vpd->getAttrStr("gic_dwop")<<"\","
			<< "gic_movbc:\""<<vpd->getAttrStr("gic_movbc")<<"\","
			<< "gic_motbc:\""<<vpd->getAttrStr("gic_motbc")<<"\","
			<< "gic_mdwbc:\""<<vpd->getAttrStr("gic_mdwbc")<<"\","
			<< "gic_dwndfl:\""<<vpd->getAttrStr("gic_dwndfl")<<"\","
			<< "gic_ovndfl:\""<<vpd->getAttrStr("gic_ovndfl")<<"\","
			<< "gic_fillrt:\""<<vpd->getAttrStr("gic_fillrt","0")<<"\","
			<< "gic_fill1:\""<<vpd->getAttrStr("gic_fill1")<<"\","
			<< "gic_fill2:\""<<vpd->getAttrStr("gic_fill2")<<"\","
			<< "gic_recrt:\""<<vpd->getAttrStr("gic_recrt")<<"\","
			<< "gic_lock:\""<<vpd->getAttrStr("gic_lock")<<"\","
			<< "gic_noevent:\""<<vpd->getAttrStr("gic_noevent")<<"\","
			<< "gic_nmndfl:\""<<vpd->getAttrStr("gic_nmndfl")<<"\"";

	return true;
}

