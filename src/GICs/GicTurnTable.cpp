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
#include "GICs/GicTurnTable.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicTurnTable();

AosGicTurnTable::AosGicTurnTable(const bool flag)
:
AosGic(AOSGIC_TURNTABLE, AosGicType::eTurnTable, flag)
{
}


AosGicTurnTable::~AosGicTurnTable()
{
}


bool	
AosGicTurnTable::generateCode(
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
		<< "\",gic_bgimgsrc: \"" << vpd->getAttrStr("gic_bgimgsrc", "img101/el39061823.png") 
		<< "\",gic_gifts: \"" << vpd->getAttrStr("gic_gifts", "img101/el39061823.png") 
		<< "\",gic_israndom: " << vpd->getAttrStr("gic_israndom", "false") 
		<< ",gic_ptsrc: \"" << vpd->getAttrStr("gic_ptsrc", "Clock/Clock2/clock_second.png") 
		<< "\",gic_rstyle: \"" << vpd->getAttrStr("gic_rstyle", "1") 
		<< "\",gic_rtime: " << vpd->getAttrInt("gic_rtime", 3) 
		<< ",gic_sliderww: " << vpd->getAttrInt("gic_sliderww", 200) 
		<< ",gic_slidermin: " << vpd->getAttrInt("gic_slidermin", 0) 
		<< ",gic_slidermax: " << vpd->getAttrInt("gic_slidermax", 100) 
		<< ",gic_turnang: " << vpd->getAttrInt("gic_turnang", 10); 

	return true;
}

