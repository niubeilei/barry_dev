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
// 07/07/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicPlate.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicPlate();

AosGicPlate::AosGicPlate(const bool flag)
:
AosGic(AOSGIC_PLATE, AosGicType::ePlate, flag)
{
}


AosGicPlate::~AosGicPlate()
{
}


bool	
AosGicPlate::generateCode(
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

	code.mJson 
			<<",gic_path:\'"<<vpd->getAttrStr("gic_path","")
			<<"\',gic_text:\'"<<vpd->getAttrStr("gic_text","")
			<<"\',gic_number:\'"<<vpd->getAttrStr("gic_number","")
			<<"\',gic_defnum:\'"<<vpd->getAttrStr("gic_defnum","")
			<<"\',gic_gicid:\'"<<vpd->getAttrStr("gic_gicid","")
			<<"\',gic_url:\'"<<vpd->getAttrStr("gic_url","")
			<<"\',tip_width:"<<vpd->getAttrInt("tip_width",200)
			<<",tip_x:"<<vpd->getAttrInt("tip_x",0)
			<<",tip_y:"<<vpd->getAttrInt("tip_y",0);
	return true;
}

