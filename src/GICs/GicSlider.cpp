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
// 07/25/2010: Created by Michael 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicSlider.h"

#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicSlider();

AosGicSlider::AosGicSlider(const bool flag)
:
AosGic(AOSGIC_SLIDER, AosGicType::eSlider, flag)
{
}


AosGicSlider::~AosGicSlider()
{
}


bool	
AosGicSlider::generateCode(
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
    OmnString gic_fntcolor = vpd->getAttrStr("gic_fntcolor","#000000");
	if(gic_fntcolor!="") gic_fntcolor = AosHtmlUtil::getWebColor(gic_fntcolor);
	OmnString gic_spnbg = vpd->getAttrStr("gic_spnbg","#ffffff");
	if(gic_spnbg!="") gic_spnbg = AosHtmlUtil::getWebColor(gic_spnbg);
	code.mJson <<",vertical:"<<vpd->getAttrStr("gic_vertical","false") 
		       <<",maxValue:"<<vpd->getAttrInt("gic_maxValue",100)
               <<",minValue:"<<vpd->getAttrInt("gic_minValue",0)
			   <<",value:"<<vpd->getAttrInt("gic_value",50)
			   <<",tips:\""<<vpd->getAttrStr("gic_tips","")<<"\""
			   <<",fntcolor:\""<<gic_fntcolor<<"\""
			   <<",fntfamily:\""<<vpd->getAttrStr("gic_fntfamily","宋体")<<"\""
               <<",fntstyle:\""<<vpd->getAttrStr("gic_fntstyle","normal")<<"\""
			   <<",fntweight:\""<<vpd->getAttrStr("gic_fntweight","normal")<<"\""
			   <<",fntsize:"<<vpd->getAttrInt("gic_fntsize",12)
			   <<",spnw:\""<<vpd->getAttrStr("gic_spnw","100")<<"\""
               <<",spnh:\""<<vpd->getAttrStr("gic_spnh","50")<<"\""
			   <<",spnbg:\""<<gic_spnbg<<"\""
			   <<",increment:\""<<vpd->getAttrStr("gic_increment","1")<<"\""
			   <<",keyincrement:\""<<vpd->getAttrStr("gic_keyincrement","1")<<"\"";


	return true;
}

