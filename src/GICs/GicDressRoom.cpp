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
#include "GICs/GicDressRoom.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicDressRoom();

AosGicDressRoom::AosGicDressRoom(const bool flag)
:
AosGic(AOSGIC_DRESSROOM, AosGicType::eDressRoom, flag)
{
}


AosGicDressRoom::~AosGicDressRoom()
{
}


bool	
AosGicDressRoom::generateCode(
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
		<< ",gic_bgcolor: \"" << vpd->getAttrStr("gic_bgcolor", "#99ff99") 
		<< "\",gic_coatsrc: \"" << vpd->getAttrStr("gic_coatsrc", "null") 
		<< "\",gic_containerbd: \"" << vpd->getAttrStr("gic_containerbd", "") 
		<< "\",gic_iscloudid: " << vpd->getAttrStr("gic_iscloudid", "true") 
		<< ",gic_dressratio: " << vpd->getAttrStr("gic_dressratio", "100") 
		<< ",gic_movable:" << vpd->getAttrStr("gic_movable", "true") 
		<< ",gic_hatsrc: \"" << vpd->getAttrStr("gic_hatsrc", "null") 
		<< "\",gic_isdressup: " << vpd->getAttrStr("gic_isdressup", "true") 
		<< ",gic_bmodelsrc: \"" << vpd->getAttrStr("gic_bmodelsrc", "system/do51996.png") 
		<< "\",gic_modelsrc: \"" << vpd->getAttrStr("gic_modelsrc", "system/do51979.png") 
	//	<< ",gic_bmodelsrc: \"" << vpd->getAttrStr("gic_bmodelsrc", "img101/do51996.png") 
	//	<< "\",gic_modelsrc: \"" << vpd->getAttrStr("gic_modelsrc", "img101/do51979.png") 
		<< "\",gic_rmsrc: \"" << vpd->getAttrStr("gic_rmsrc", "a1/el13294.jpg") 
		<< "\",gic_shoessrc: \"" << vpd->getAttrStr("gic_shoessrc", "null") 
		<< "\",gic_suitsrc: \"" << vpd->getAttrStr("gic_suitsrc", "null") 
		<< "\",gic_trouserssrc: \"" << vpd->getAttrStr("gic_trouserssrc", "null")<< "\"";

	return true;
}

