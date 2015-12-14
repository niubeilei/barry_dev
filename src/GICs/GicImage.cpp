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
#include "GICs/GicImage.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicImage();

AosGicImage::AosGicImage(const bool flag)
:
AosGic(AOSGIC_IMAGE, AosGicType::eImage, flag)
{
}


AosGicImage::~AosGicImage()
{
}


bool	
AosGicImage::generateCode(
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
	OmnString src;

	if (srcbd != "" && obj)
		src = obj->getAttrStr(srcbd, vpd->getAttrStr("img_src"));
	else
		src = vpd->getAttrStr("img_src", "system/es1073.jpg");

	OmnString valbd = vpd->getAttrStr("img_val_bind", "image");
	OmnString val;
	if (valbd != "" && obj)
		val = obj->getAttrStr(valbd, vpd->getAttrStr("img_value"));
	else
		val = vpd->getAttrStr("img_value", "image");
	
	OmnString gic_id = AosGetHtmlElemId(); 
//	htmlStr << "<div id=\"" << gic_id << "\"></div>"; 
	
	OmnString actions;
	AosXmlTagPtr actions_vpd = vpd->getFirstChild("actions");
	if(actions_vpd)
		actions = actions_vpd->toString();

	code.mJson  
	//	<< "xtype:\"" << vpd->getAttrStr("gic_type")
	//	<< "\",el : \"" << gic_id 
	//	<< "\",id : \"" << gic_id 
		<< ",src: \"" << src 
		<< "\",srcbd: \"" << srcbd 
		<< "\",img_value: \"" << val 
		<< "\",ratio:" << vpd->getAttrStr("img_nkratio", "false") 
		<< ",dressup:\"" << vpd->getAttrStr("img_style", "No") 
		<< "\",gic_iszoom:" << vpd->getAttrStr("gic_iszoom", "false")
		<< ",gic_zoomww:\"" << vpd->getAttrStr("gic_zoomww", "150")
		<< "\",gic_zoomhh:\"" << vpd->getAttrStr("gic_zoomhh", "150")
		<< "\",gic_zoomrad:\"" << vpd->getAttrStr("gic_zoomrad", "75")
		<< "\",gic_zoomsdw:" << vpd->getAttrStr("gic_zoomsdw", "false")
		<< ",opacity:\"" << vpd->getAttrStr("img_opct", "1") 
		<< "\",sft_opc:\"" << vpd->getAttrStr("sft_opc", "1") 
		<< "\",duration:\"" << vpd->getAttrStr("img_anitime", "500") 
		<< "\",img_anitype:\"" << vpd->getAttrStr("img_anitype", "No")
		<< "\",img_fcolor:\"" << vpd->getAttrStr("img_fcolor", "ffff00") 
		<< "\",img_anchor:\"" << vpd->getAttrStr("img_anchor", "t") 
		<< "\",img_easing:\"" << vpd->getAttrStr("img_easing", "easeOut")
		<< "\",img_evt:\"" << vpd->getAttrStr("img_evt", "mouseover")
		<< "\",img_clip:" << vpd->getAttrStr("img_clip", "false")
		<< ",img_tips:\"" << vpd->getAttrStr("img_tips", "")
		<< "\",hor_layout:\"" << vpd->getAttrStr("hor_layout", "center")
		<< "\",var_layout:\"" << vpd->getAttrStr("var_layout", "middle")
		<< "\",img_ncsrc: " << vpd->getAttrStr("img_ncsrc", "false") 
		<< ",img_movable:" << vpd->getAttrStr("img_movable", "false") 
		<< ",img_hfh:" << vpd->getAttrInt("img_hfh", 0)
		<< ",img_hfv:" << vpd->getAttrInt("img_hfv", 0)
		<< ",img_hfx:" << vpd->getAttrInt("img_hfx", 0)
		<< ",img_hfy:" << vpd->getAttrInt("img_hfy", 0)
		<< ",fscreen:" << vpd->getAttrInt("img_w_fscreen", 0)
		<< ",img_fripple:" << vpd->getAttrInt("img_fripple", 1);

	return true;
}

