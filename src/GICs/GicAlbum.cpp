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
// 07/26/2010: Created by Cody
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicAlbum.h"
#include "HtmlModules/DclDb.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicAlbum::AosGicAlbum(const bool flag)
:
AosGic(AOSGIC_ALBUM, AosGicType::eAlbum, flag)
{
}


AosGicAlbum::~AosGicAlbum()
{
}


bool	
AosGicAlbum::generateCode(
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
	// 	html:
	OmnString gic_tree = "gic_tree";
	OmnString gic_listicon = "gic_iconlist";
	OmnString gic_x = vpd->getAttrStr("gic_x");
	OmnString gic_y = vpd->getAttrStr("gic_y");
	OmnString paneName = vpd->getAttrStr("paneName");
	OmnString vpd_name = vpd->getAttrStr("vpd_name");
	OmnString listJson, treeJson, strHtml;

	//<vpd>
	// <vpd gic_type='gic_listicon' />
	// <vpd gic_type='gic_tree' />
	//</vpd>
	AosXmlTagPtr gic = vpd->getFirstChild();
	while(gic)
	{
		AosHtmlCode creatorCode;
		AosXmlTagPtr mobj;
		//AosGic::createGic(htmlPtr, gic, mobj, parentid, creatorCode);
		AosGic::createGic(htmlPtr, gic, mobj, parentid, creatorCode, -1, -1); //Ketty 2011/09/22
		if(creatorCode.mJson.findSubString(AOSGIC_TREE, 0) != -1)
		{
			treeJson << creatorCode.mJson; 
		}
		else if(creatorCode.mJson.findSubString(AOSGIC_LISTICON, 0) != -1)
		{
			listJson << creatorCode.mJson;
		}
		gic = vpd->getNextChild();
	}

	//json
 	code.mJson << ","
 			   << "listJson : " << listJson << ","
 			   << "vpd_name : \"" << vpd_name << "\","
 			   << "paneName : \"" << paneName << "\","
 			   << "treeJson : " << treeJson << "";
	return true;
}

