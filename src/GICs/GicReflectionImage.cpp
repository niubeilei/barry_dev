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
#include "GICs/GicReflectionImage.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicReflectionImage();

AosGicReflectionImage::AosGicReflectionImage(const bool flag)
:
AosGic(AOSGIC_REFLECTIONIMAGE, AosGicType::eReflectionImage, flag)
{
}


AosGicReflectionImage::~AosGicReflectionImage()
{
}


bool	
AosGicReflectionImage::generateCode(
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
		<< ",gic_angle: \"" << vpd->getAttrStr("gic_angle", "90") 
		<< "\",gic_bgcolor: \"" << vpd->getAttrStr("gic_bgcolor", "#000") 
		<< "\",gic_gfromcolor: \"" << vpd->getAttrStr("gic_gfromcolor", "#000") 
		<< "\",gic_gtocolor: \"" << vpd->getAttrStr("gic_gtocolor", "#000") 
		<< "\",gic_imgwidth: " << vpd->getAttrInt("gic_imgwidth", 320) 
		<< ",gic_imgheight: " << vpd->getAttrInt("gic_imgheight", 240) 
		<< ",gic_imgsrc: \"" << vpd->getAttrStr("gic_imgsrc", "img101/el9635.png") 
		<< "\",gic_marginleft: " << vpd->getAttrInt("gic_marginleft", 130) 
		<< ",gic_margintop: " << vpd->getAttrInt("gic_margintop", 60) 
		<< ",gic_maskopacity: " << vpd->getAttrInt("gic_maskopacity", 0.5); 

	return true;
}

