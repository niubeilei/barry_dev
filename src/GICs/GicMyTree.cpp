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
#include "GICs/GicMyTree.h"

#include "HtmlModules/Ptrs.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicMyTree::AosGicMyTree(const bool flag)
:
AosGic(AOSGIC_MYTREE, AosGicType::eMyTree, flag)
{
}


AosGicMyTree::~AosGicMyTree()
{
}


bool	
AosGicMyTree::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	/*
	bool gic_iskeepratio = vpd->getAttrBool("gic_iskeepratio");
	bool gic_autoplay = vpd->getAttrBool("gic_autoplay");
	bool gic_random = vpd->getAttrBool("gic_random");
	bool gic_existbutton = vpd->getAttrBool("gic_existbutton");
	bool gic_fullscreen = vpd->getAttrBool("gic_fullscreen");
	int gic_showIdx = vpd->getAttrInt("gic_showIdx", 0);
	OmnString gic_src_bind = vpd->getAttrStr("gic_src_bind"); 
	OmnString gic_anitime = vpd->getAttrStr("gic_anitime");
	OmnString gic_delaytime = vpd->getAttrStr("gic_delaytime");
	OmnString gic_anitype= vpd->getAttrStr("gic_anitype");
	OmnString gic_btnalign = vpd->getAttrStr("gic_btnalign");
	OmnString gic_stretch = vpd->getAttrStr("gic_stretch");
	code.mJson << ",gic_iskeepratio: "<< gic_iskeepratio <<"," 
			<< "gic_fullscreen: "<< gic_fullscreen<<"," 
			<< "gic_showIdx: "<< gic_showIdx<<"," 
			<< "gic_autoplay: "<< gic_autoplay <<"," 
			<< "gic_random: "<< gic_random <<"," 
			<< "gic_src_bind: \'"<< gic_src_bind <<"\'," 
			<< "gic_delaytime: \'"<< gic_delaytime <<"\',"
			<< "gic_anitime: \'"<< gic_anitime <<"\',"
			<< "gic_anitype: \'"<< gic_anitype <<"\',"
			<< "gic_btnalign: \'"<< gic_btnalign <<"\',"
			<< "gic_stretch: \'"<< gic_stretch<<"\'," 
			<< "gic_existbutton: "<< gic_existbutton;
	*/
	return true;
}

