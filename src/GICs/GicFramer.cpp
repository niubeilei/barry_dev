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
// 07/08/2010: Created by Jackie
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicFramer.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicFramer();

AosGicFramer::AosGicFramer(const bool flag)
:
AosGic(AOSGIC_FRAMER, AosGicType::eFramer, flag)
{
}


AosGicFramer::~AosGicFramer()
{
}


bool	
AosGicFramer::generateCode(
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
	OmnString workvid = AosGetHtmlElemId();
	OmnString gic_framerid = AosGetHtmlElemId();
	OmnString gic_framerel = AosGetHtmlElemId();
	vpd->setAttr("workvid", workvid);
	vpd->setAttr("cmp_id", gic_framerid);
	vpd->setAttr("cmp_el", gic_framerel);
	convertToJson(vpd, obj, code);
	code.mHtml <<"<div id=\"" << gic_framerel << "\"><div id=\"" << workvid << "\"></div></div>";
	return true;
}

bool
AosGicFramer::convertToJson(
		const AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		AosHtmlCode &code)
{
	// This function converts the VPD to Json form.
	OmnString workvid = vpd->getAttrStr("workvid");
	OmnString cmp_id = vpd->getAttrStr("cmp_id");
	OmnString cmp_el = vpd->getAttrStr("cmp_el");
	OmnString gic_type = vpd->getAttrStr("gic_type");
	OmnString srcbd = vpd->getAttrStr("cmp_srcbd");
	OmnString src = obj->getAttrStr(srcbd);
	OmnString sizebd = vpd->getAttrStr("cmp_sizebd");
	OmnString sizedef = obj->getAttrStr(sizebd);
	AosXmlTagPtr framer = vpd->getFirstChild("framer");
	OmnString cmp_type;
	OmnString cmp_src;
	OmnString cmp_sizedef;
	int cmp_xoffset;
	int cmp_yoffset;
	int cmp_width;
	int cmp_height;
	int cmp_hot;
	if (framer)
	{
		cmp_type = framer->getAttrStr("gic_type");
		if (src=="") 
		{
			cmp_src = framer->getAttrStr("cmp_src");
		}
		else
		{
			cmp_src = src;
		}
		if (sizedef=="")
		{
			cmp_sizedef = framer->getAttrStr("cmp_sizedef");
		}
		else
		{
			cmp_sizedef = sizedef;
		}
		cmp_xoffset = framer->getAttrInt("cmp_xoffset", 0);
		cmp_yoffset = framer->getAttrInt("cmp_yoffset", 0);
		cmp_width = framer->getAttrInt("cmp_width", 0);
		cmp_height = framer->getAttrInt("cmp_height", 0);
		cmp_hot = framer->getAttrInt("cmp_hot", 0);
	}
	code.mJson << "," ;
	code.mJson << "cmp_type : \'" << cmp_type << "\'," ;
	code.mJson << "cmp_src : \'" << cmp_src << "\'," ;
	code.mJson << "cmp_srcbd : \'" << srcbd << "\'," ;
	code.mJson << "workvid : \'" << workvid << "\'," ;
	code.mJson << "cmp_id : \'" << cmp_id << "\'," ;
	code.mJson << "cmp_el : \'" << cmp_el << "\'," ;
	code.mJson << "cmp_hot : " << cmp_hot << "," ;
	code.mJson << "cmp_sizedef : \'" << cmp_sizedef << "\'," ;
	code.mJson << "cmp_sizebd : \'" << sizebd << "\'," ;
	code.mJson << "cmp_xoffset : " << cmp_xoffset << "," ;
	code.mJson << "cmp_yoffset : " << cmp_yoffset << "," ;
	code.mJson << "cmp_width : " << cmp_width << "," ;
	code.mJson << "cmp_height : " << cmp_height ;
	return true;
}
