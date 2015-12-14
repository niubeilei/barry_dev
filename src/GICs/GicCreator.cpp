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
// 28/07/2010: Created by ketty//////////////////////////////////////////////////////////////////////////
#include "GICs/GicCreator.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicCreator();

AosGicCreator::AosGicCreator(const bool flag)
:
AosGic(AOSGIC_CREATOR, AosGicType::eCreator, flag)
{
}


AosGicCreator::~AosGicCreator()
{
}


bool	
AosGicCreator::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{

	OmnString gic_img_src = vpd->getAttrStr("wgt_icon_src");
	OmnString gic_label = vpd->getAttrStr("textlabel","");
	OmnString cmp_objid_bind = vpd->getAttrStr("cmp_objid_bind","");	
	OmnString wgt_icbd = vpd->getAttrStr("wgt_icbd","");	
	OmnString wgt_lbbd = vpd->getAttrStr("wgt_lbbd","");	
	OmnString gic_creator_vpdName = vpd->getAttrStr("creator_vpdname","");
	
	OmnString path = htmlPtr->getImagePath();

    code.mHtml << "<img style=\"width:50px;height:50px;\" src=\""<< path << gic_img_src << "\" ></img>"
			   <<  "<div>" << gic_label << "</div>"; 

    code.mJson << ",gic_label:\"" << gic_label << "\","
			   << "gic_img_src:\"" << gic_img_src << "\","
			   << "creatorVpdName:\"" << gic_creator_vpdName << "\","
			   << "cmp_objid_bind:\"" << cmp_objid_bind << "\","
			   << "wgt_icbd:\"" << wgt_icbd << "\","
			   << "wgt_lbbd:\"" << wgt_lbbd << "\"";
    return true;
}
