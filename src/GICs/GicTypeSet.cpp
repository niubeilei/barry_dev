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
#include "GICs/GicTypeSet.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicTypeSet();

AosGicTypeSet::AosGicTypeSet(const bool flag)
:
AosGic(AOSGIC_TYPESET, AosGicType::eTypeSet, flag)
{
}


AosGicTypeSet::~AosGicTypeSet()
{
}


bool	
AosGicTypeSet::generateCode(
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

	//set CSS
	OmnString cssStr;
	OmnString cellww;
	OmnString cellwwbd = vpd->getAttrStr("gic_cellwwbd");
	if (cellwwbd != "" && obj)
		cellww = obj->getAttrStr(cellwwbd, vpd->getAttrStr("gic_cellww"));
	else
		cellww = vpd->getAttrStr("gic_cellww", "60");

	// binding
	OmnString ctn, col, itemstl;
	OmnString ctnbd = vpd->getAttrStr("gic_ctnbd");
	if (ctnbd != "" && obj)
		ctn = obj->getAttrStr(ctnbd, vpd->getAttrStr("gic_ctn"));
	else
		ctn = vpd->getAttrStr("gic_ctn");

	OmnString disordctnbd = vpd->getAttrStr("gic_disordctnbd");

	OmnString colbd = vpd->getAttrStr("gic_colbd");
	if (colbd != "" && obj)
		col = obj->getAttrStr(colbd, vpd->getAttrStr("gic_col"));
	else
		col = vpd->getAttrStr("gic_col");

	OmnString itemstlbd = vpd->getAttrStr("gic_itemstlbd");
	if (itemstlbd != "" && obj)
		itemstl = obj->getAttrStr(itemstlbd, vpd->getAttrStr("gic_itemstl"));
	else
		itemstl = vpd->getAttrStr("gic_itemstl");
	// end
	
	OmnString gic_ctn, content;
	AosXmlTagPtr cnt = vpd->getFirstChild("contents");
	if (cnt) content = cnt->getNodeText();

	if (ctn!="") 
		gic_ctn = ctn;
	else if (content!="") 
		gic_ctn = content;
	else gic_ctn = "ABCDE";

	code.mJson  
		<< ",gic_ctn: \"" << gic_ctn 
		<< "\",gic_containerbd: \"" << vpd->getAttrStr("gic_containerbd", "typeset")
		<< "\",gic_ctnbd: \"" << ctnbd 
		<< "\",gic_disordctnbd: \"" << disordctnbd 
		<< "\",gic_itemstl: \"" << vpd->getAttrStr("gic_itemstl", "word") 
		<< "\",gic_flashsrc: \"" << vpd->getAttrStr("gic_flashsrc", "") 
		<< "\",gic_colbd: \"" << colbd 
		<< "\",gic_cellwwbd: \"" << cellwwbd 
		<< "\",gic_itemstlbd: \"" << itemstlbd 
		<< "\",gic_col:" << vpd->getAttrInt("gic_col", 6)
		<< ",gic_cellww:" << cellww;

	return true;
}

