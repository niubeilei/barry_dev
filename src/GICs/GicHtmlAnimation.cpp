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
// 2011/02/19 Created by Michael Yang 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlAnimation.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicHtmlAnimation();

AosGicHtmlAnimation::AosGicHtmlAnimation(const bool flag)
:
AosGic(AOSGIC_HTMLANIMATION, AosGicType::eHtmlAnimation, flag)
{
}


AosGicHtmlAnimation::~AosGicHtmlAnimation()
{
}


bool	
AosGicHtmlAnimation::generateCode(
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
	//convertToJson(vpd,code.mJson);
	//test
	//OmnString htmlcontents = vpd->getAttr(); 
	AosXmlTagPtr vv = vpd->getFirstChild("contents");
	OmnString cont = "";
	if(vv)
	{
		cont = vv->getNodeText();
		cont.replace("|&|","\'", true);
		cont.replace("|&&|","\"", true);
		cont.replace("\'","\\'", true);
	}	
	int pos = code.mHtml.findSubString("style",4);
	code.mHtml.insert("overflow: hidden;",pos+7);
	code.mHtml << cont << cont;
	code.mJson << ",gic_moveflag:\"" 
			   << vpd->getAttrStr("gic_moveflag", "vert") 
			   << "\",gic_speed : "<<vpd->getAttrInt("gic_speed",10);
	return true;
}

