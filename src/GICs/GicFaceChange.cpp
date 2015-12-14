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
// 2011/03/23: Created by Tracy
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicFaceChange.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicFaceChange();

AosGicFaceChange::AosGicFaceChange(const bool flag)
:
AosGic(AOSGIC_FACECHANGE, AosGicType::eFaceChange, flag)
{
}


AosGicFaceChange::~AosGicFaceChange()
{
}


bool	
AosGicFaceChange::generateCode(
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
	
	OmnString srcbd = vpd->getAttrStr("gic_bgsrcbd");
	OmnString src;
	if (srcbd != "" && obj)
		src = obj->getAttrStr(srcbd, vpd->getAttrStr("gic_bgsrc"));
	else
		src = vpd->getAttrStr("gic_bgsrc", "a3/es1073.jpg");
	

	OmnString imageHtmlCode;
	OmnString path = htmlPtr->getImagePath();

	//Only show background image, The reason is don't how many face.
	imageHtmlCode << "<div style=\"position: absolute; width: 100%; height: 100%;\">"  
		<< "<img style=\"position: absolute; width:100%; height:100%;\" src=\"" 
		<< path << src << "\"/></div>";
	code.mHtml << imageHtmlCode ;

	code.mJson  
		<< ",gic_bgsrc: \"" << src 
		<< "\",gic_bgsrcbd: \"" << srcbd 
		<< "\",gic_facesrc: \"" << vpd->getAttrStr("gic_facesrc", "") 
		<< "\",gic_facesrcbd: \"" << vpd->getAttrStr("gic_facesrcbd", "")
		<< "\",gic_mfacesrc: \"" << vpd->getAttrStr("gic_mfacesrc", "")
		<< "\",gic_mfacesrcbd: \"" << vpd->getAttrStr("gic_facesrcbd", "")
		<< "\",gic_isshowman: " << vpd->getAttrStr("gic_isshowman", "false");
	return true;
}

