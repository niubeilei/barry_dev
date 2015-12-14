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
#include "GICs/GicGridApp.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicGridApp();

AosGicGridApp::AosGicGridApp(const bool flag)
:
AosGic(AOSGIC_GRIDAPP, AosGicType::eGridApp, flag)
{
}


AosGicGridApp::~AosGicGridApp()
{
}


bool	
AosGicGridApp::generateCode(
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
	
	OmnString srcbd = vpd->getAttrStr("img_src_bind");
	OmnString bsrc;

	if (srcbd != "" && obj)
		bsrc = obj->getAttrStr(srcbd, vpd->getAttrStr("gic_basesrc"));
	else
		bsrc = vpd->getAttrStr("gic_basesrc", "a2/eo16503.png");

	OmnString valbd = vpd->getAttrStr("img_val_bind", "image");
	OmnString val;
	if (valbd != "" && obj)
		val = obj->getAttrStr(valbd, vpd->getAttrStr("img_value"));
	else
		val = vpd->getAttrStr("img_value", "image");
	
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


	code.mJson  
		<< ",gic_basesrc: \"" << bsrc << "\"," 
			<< "cmp : {cmp_tname : \"" << cmp_tname << "\"," 
			<< "cmp_query : \"" << cmp_query << "\","
			<< "cmp_dftqry : \"" << cmp_dftqry << "\","
			<< "cmp_order : \"" << cmp_order << "\","
			<< "cmp_fnames : \"" << cmp_fnames <<"\","
			<< "cmp_tnamebd : \"" << cmp_tnamebd << "\","
			<< "cmp_psize : \"" << cmp_psize << "\","
			<< "cmp_qrm : \"" << cmp_qrm << "\","
			<< "cmp_reverse : \"" << cmp_reverse << "\","
			<< "cmp_dftqrm : \"" << cmp_dftqrm << "\"},"
		<< "gic_iconsrc: \"" << vpd->getAttrStr("gic_iconsrc", "a2/eo16503.png") 
		<< "\",gic_srcs: \"" << vpd->getAttrStr("gic_srcs", "a2/eo16503.png") 
		<< "\",gic_vpdtips:\"" << vpd->getAttrStr("gic_vpdtips") 
		<< "\",gic_words:\"" << vpd->getAttrStr("gic_words", "image") 
		<< "\",gic_row:" << vpd->getAttrInt("gic_row", 8) 
		<< ",gic_col:" << vpd->getAttrInt("gic_col", 12);

	return true;
}

