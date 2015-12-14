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
// 2011/02/19 Created by Ken
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlPopVpd.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicHtmlPopVpd();

AosGicHtmlPopVpd::AosGicHtmlPopVpd(const bool flag)
:
AosGic(AOSGIC_HTMLPOPVPD, AosGicType::eHtmlPopVpd, flag)
{
}


AosGicHtmlPopVpd::~AosGicHtmlPopVpd()
{
}


bool	
AosGicHtmlPopVpd::generateCode(
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
	OmnString popAlign = vpd->getAttrStr("popAlign", "top");
	OmnString vpdname = vpd->getAttrStr("vpdname", "objid_37401");
	OmnString popBtX= vpd->getAttrStr("popBtX", "100");
	OmnString popBtY= vpd->getAttrStr("popBtY", "0");
	OmnString popVpdW= vpd->getAttrStr("popVpdW", "200");
	OmnString popVpdH= vpd->getAttrStr("popVpdH", "100");
	OmnString popVpdX= vpd->getAttrStr("popVpdX", "0");
	OmnString popVpdY= vpd->getAttrStr("popVpdY", "150");
	OmnString tipPaneW= vpd->getAttrStr("tipPaneW", "200");
	OmnString tipPaneH= vpd->getAttrStr("tipPaneH", "280");
	OmnString autoHideTipNail= vpd->getAttrStr("autoHideTipNail", "false");
	OmnString hideTipNailSrc= vpd->getAttrStr("hideTipNailSrc", "Icons/ControlPanel2.png");
	OmnString popBtImg= vpd->getAttrStr("popBtImg", "Icons/AquaBall2.png");
	OmnString popVpdName= vpd->getAttrStr("popVpdName", "hello world");
	bool isMinBt = vpd->getAttrBool("isMinBt", false);
	OmnString mMinBtSrc = vpd->getAttrStr("mMinBtSrc", "");
	OmnString mMinBtWidth = vpd->getAttrStr("mMinBtWidth", "50");
	OmnString mMinBtHeight = vpd->getAttrStr("mMinBtHeight", "50");

	OmnString str;
	str 
		<< ",popAlign:\'" << popAlign <<"\'"
		<< ",vpdname:\'" << vpdname <<"\'"
		<< ",popVpdName:\'" << popVpdName <<"\'"
		<< ",popBtX:" << popBtX
		<< ",popBtY:" << popBtY 
		<< ",popVpdW:" << popVpdW 
		<< ",popVpdH:" << popVpdH 
		<< ",popVpdX:" << popVpdX 
		<< ",popVpdY:" << popVpdY 
		<< ",tipPaneW:" << tipPaneW 
		<< ",tipPaneH:" << tipPaneH 
		<< ",hideTipNailSrc:\'" << hideTipNailSrc <<"\'" 
		<< ",popBtImg:\'" << popBtImg <<"\'"
		<< ",isMinBt:" << isMinBt
		<< ",mMinBtSrc:\'" << mMinBtSrc <<"\'"
		<< ",mMinBtWidth:\'" << mMinBtWidth <<"\'"
		<< ",mMinBtHeight:\'" << mMinBtHeight <<"\'"
		<< ",autoHideTipNail:" << autoHideTipNail;
	code.mJson << str;
	return true;
}

