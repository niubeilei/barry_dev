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
// 2011/02/19: Created by Ken
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlTree.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicHtmlTree();

AosGicHtmlTree::AosGicHtmlTree(const bool flag)
:
AosGic(AOSGIC_HTMLTREE, AosGicType::eHtmlTree, flag)
{
}


AosGicHtmlTree::~AosGicHtmlTree()
{
}


bool	
AosGicHtmlTree::generateCode(
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
	//
	//////////////////////////////////////
	//datacol中的属性
	AosXmlTagPtr datacol =  vpd->getFirstChild("datacol");
	OmnString cmp_tname = datacol->getAttrStr("cmp_tname", "");
	OmnString cmp_query = datacol->getAttrStr("cmp_query", "");
	OmnString cmp_dftqry = datacol->getAttrStr("cmp_dftqry","");
	OmnString cmp_order = datacol->getAttrStr("cmp_order", "");
	OmnString cmp_fnames = datacol->getAttrStr("cmp_fnames","");
	OmnString cmp_tnamebd = datacol->getAttrStr("cmp_tnamebd","");
	OmnString cmp_psize = datacol->getAttrStr("cmp_psize","");
	OmnString cmp_qrm = datacol->getAttrStr("cmp_qrm","");
	OmnString cmp_dftqrm = datacol->getAttrStr("cmp_dftqrm","");
	OmnString cmp_reverse = datacol->getAttrStr("cmp_reverse","false");


	OmnString path = htmlPtr->getImagePath();
	OmnString src = vpd->getAttrStr("gic_src", "a3/es1073.jpg");


	code.mJson  
		<< ",gic_src:\"" << src 
		<< "\",gic_typeaname:\"" << vpd->getAttrStr("gic_typeaname", "zky_otype") 
		<< "\",gic_selcolor:\"" << vpd->getAttrStr("gic_selcolor", "#67BF7F") 
		<< "\",gic_isedit: " << vpd->getAttrStr("gic_isedit", "false") 
		<< ",img_movable:" << vpd->getAttrStr("img_movable", "false"); 

	return true;
}

