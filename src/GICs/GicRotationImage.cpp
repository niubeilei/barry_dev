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
#include "GICs/GicRotationImage.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicRotationImage();

AosGicRotationImage::AosGicRotationImage(const bool flag)
:
AosGic(AOSGIC_ROTATIONIMAGE, AosGicType::eRotationImage, flag)
{
}


AosGicRotationImage::~AosGicRotationImage()
{
}


bool	
AosGicRotationImage::generateCode(
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
		<< ",gic_bgcolor: \"" << vpd->getAttrStr("gic_bgcolor", "#333") 
		<< "\",gic_imgsrc: \"" << vpd->getAttrStr("gic_imgsrc", "img101/el9563.png") 
		<< "\",gic_roltime: " << vpd->getAttrStr("gic_roltime", "10") 
		<< ",gic_rolangle: " << vpd->getAttrInt("gic_rolangle", 30); 

	return true;
}

