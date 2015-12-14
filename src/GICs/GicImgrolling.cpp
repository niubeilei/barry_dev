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
// 06/28/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicImgrolling.h"

#include "HtmlModules/Ptrs.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicImgrolling::AosGicImgrolling(const bool flag)
:
AosGic(AOSGIC_IMAROLLING, AosGicType::eImgRolling, flag)
{
}


AosGicImgrolling::~AosGicImgrolling()
{
}


bool	
AosGicImgrolling::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	int gic_boxnum = vpd->getAttrInt("gic_boxnum", 0);
	int gic_rollnum = vpd->getAttrInt("gic_rollnum", 0);
	int gic_space = vpd->getAttrInt("gic_space", 0);
	int gic_bwidth = vpd->getAttrInt("gic_bwidth", 0);
	int gic_bheight= vpd->getAttrInt("gic_bheight", 0);
	bool gic_fullscreen = vpd->getAttrBool("gic_fullscreen");
	bool gic_entryframe = vpd->getAttrBool("gic_entryframe");
	bool gic_iskeepratio = vpd->getAttrBool("gic_iskeepratio");

	OmnString gic_type = vpd->getAttrStr("gic_type"); 
	OmnString gic_src_bind = vpd->getAttrStr("gic_src_bind"); 
	OmnString gic_anitime = vpd->getAttrStr("gic_anitime");
	OmnString gic_boxlayout = vpd->getAttrStr("gic_boxlayout");
	OmnString gic_ratio = vpd->getAttrStr("gic_ratio");
	OmnString gic_position = vpd->getAttrStr("gic_position");
	OmnString gic_entrybodystyle = vpd->getAttrStr("gic_entrybodystyle");
	OmnString gic_desc_bind = vpd->getAttrStr("gic_desc_bind");
	OmnString gic_movetype = vpd->getAttrStr("gic_movetype");

	code.mJson << ",gic_space: " << gic_space << "," 
		<< "gic_boxnum: " << gic_boxnum << "," 
		<< "gic_rollnum: " << gic_rollnum<< "," 
		<< "gic_bwidth: " << gic_bwidth<< "," 
		<< "gic_bheight: " << gic_bheight<< "," 
		<< "gic_entryframe: "<< gic_entryframe <<"," 
		<< "gic_iskeepratio: "<< gic_iskeepratio <<"," 
		<< "gic_src_bind: \'"<< gic_src_bind <<"\'," 
		<< "gic_anitime: \'"<< gic_anitime <<"\'," 
		<< "gic_boxlayout: \'"<< gic_boxlayout <<"\'," 
		<< "gic_position: \'"<< gic_position <<"\'," 
		<< "gic_fullscreen: "<< gic_fullscreen<<"," 
		<< "gic_entrybodystyle: \'"<< gic_entrybodystyle <<"\'," 
		<< "gic_movetype: \'"<< gic_movetype <<"\'," 
		<< "gic_desc_bind: \'"<< gic_desc_bind<<"\'";
	return true;
}

