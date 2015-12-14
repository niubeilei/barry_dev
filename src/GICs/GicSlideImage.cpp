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
// 2011/03/20: Created by Tracy 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicSlideImage.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicSlideImage::AosGicSlideImage(const bool flag)
:
AosGic(AOSGIC_SLIDEIMAGE, AosGicType::eSlideImage, flag)
{
}


AosGicSlideImage::~AosGicSlideImage()
{
}


bool	
AosGicSlideImage::generateCode(
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
	
	OmnString srcs = vpd->getAttrStr("gic_srcs", "");

	OmnString imageHtmlCode;
	OmnString path = htmlPtr->getImagePath();
//	OmnString path = "http://218.64.170.28:8080/lps-4.7.2/images/";

	// Only show First Image.
	// srcs = tb|$|image_2/el9524.jpg|$$|lr|$|image_2/el9 ... ...
	AosStrSplit entry_parser(srcs, "|$$|");
	OmnString item;
	OmnString src;

	imageHtmlCode << "<div style=\"position:absolute;overflow:hidden;width:100%;height:100%\">";
	int flag = 0;	
	while((item = entry_parser.nextWord()) != "")
	{
		AosStrSplit field_parser(item, "|$|");
		OmnString type = field_parser.nextWord();
		src = field_parser.nextWord();
		if(flag == 0)
		{
			imageHtmlCode << "<img style=\"position: absolute;\" src=\""
			<< path << src << "\"/>";
			flag = 1;
		}
		else
		{
			imageHtmlCode << "<img style=\"position: absolute; display: none;\" src=\""
			<< path << src << "\"/>";
		}
	}
	imageHtmlCode << "</div>";

	code.mHtml << imageHtmlCode ;
	// End

	code.mJson  
		<< ",gic_srcs: \"" << srcs 
		<< "\",gic_animatetime:" << vpd->getAttrStr("gic_animatetime", "6") 
		<< ",gic_oheight:" << vpd->getAttrStr("gic_oheight", "200") 
		<< ",gic_owidth:" << vpd->getAttrStr("gic_owidth", "400"); 

	return true;
}

