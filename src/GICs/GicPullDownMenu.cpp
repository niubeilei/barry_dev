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
// 18/08/2010: Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicPullDownMenu.h"

#include "HtmlModules/Ptrs.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicPullDownMenu();

AosGicPullDownMenu::AosGicPullDownMenu(const bool flag)
:
AosGic(AOSGIC_PULLDOWNMENU, AosGicType::ePullDownMenu, flag)
{
}

AosGicPullDownMenu::~AosGicPullDownMenu()
{
}

bool	
AosGicPullDownMenu::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	AosXmlTagPtr datacol = vpd->getFirstChild("datacol");
	if(!datacol){
		return false;
	}
	OmnString cmp_order = datacol->getAttrStr("cmp_order");
	OmnString cmp_fnames = datacol->getAttrStr("cmp_fnames");
	OmnString cmp_tname = datacol->getAttrStr("cmp_tname");
	OmnString cmp_query = datacol->getAttrStr("cmp_query");
	OmnString cmp_reverse = datacol->getAttrStr("cmp_reverse","false");
	OmnString cmp_psize = datacol->getAttrStr("cmp_pszie","20");
	OmnString cmp_tnamebd = datacol->getAttrStr("cmp_tnamebd");
	OmnString cmp_dftqry = datacol->getAttrStr("cmp_dftqry");
	OmnString cmp_qrm = datacol->getAttrStr("cmp_qrm");
	
	AosXmlTagPtr creators = vpd->getFirstChild("creators");
	AosXmlTagPtr creator = creators->getFirstChild("creator");
	AosXmlTagPtr cc = creator->getFirstChild("contents");
	OmnString gic_data = cc->getNodeText();
	
	OmnString gic_overclr = creator->getAttrStr("gic_overclr");
	if(gic_overclr != "") gic_overclr = AosHtmlUtil::getWebColor(gic_overclr);	
	OmnString gic_outclr = creator->getAttrStr("gic_outclr");
	if(gic_outclr != "") gic_outclr = AosHtmlUtil::getWebColor(gic_outclr);
	OmnString gic_overbgc = creator->getAttrStr("gic_overbgc");
	if(gic_overbgc != "") gic_overbgc = AosHtmlUtil::getWebColor(gic_overbgc);
	OmnString gic_outbgc = creator->getAttrStr("gic_outbgc");
	if(gic_outbgc != "") gic_outbgc = AosHtmlUtil::getWebColor(gic_outbgc);
	
	OmnString gic_classid = AosGetHtmlElemId();
	
	OmnString cssStr;	
	cssStr << ".pulldown-over-" << gic_classid << "{";
	if(gic_overbgc != "") cssStr << "background-color:" << gic_overbgc << " !important;";
	cssStr << "} .pulldown-over-" << gic_classid << " span{";
	if(gic_overclr != "") cssStr << "color:" << gic_overclr << " !important;";
	cssStr << "}";
		
	cssStr << ".pulldown-out-" << gic_classid << "{";
	if(gic_outbgc != "") cssStr << "background-color:" << gic_outbgc << ";";
	cssStr << "} .pulldown-out-" << gic_classid << " span{";
	if(gic_outclr != "") cssStr << "color:" << gic_outclr << ";";
	cssStr << "} .x-menu-list{padding:0}";	
	code.mCss << cssStr;

	OmnString str;
	str	<< ",gic_data:\"" << gic_data << "\","
		<< "gic_overbgc:\"" << gic_overbgc << "\","
		<< "gic_overClass:\"pulldown-over-" << gic_classid << "\","
		<< "gic_outClass:\"pulldown-out-" << gic_classid << "\","
		<< "cmp:{"
			<< "cmp_tname:\"" << cmp_tname << "\","
			<< "cmp_query:\"" << cmp_query << "\","
			<< "cmp_order:\"" << cmp_order << "\","
			<< "cmp_fnames:\"" << cmp_fnames << "\","
			<< "cmp_tnamebd:\"" << cmp_tnamebd << "\","
			<< "cmp_psize:\"" << cmp_psize << "\","
			<< "cmp_qrm:\"" << cmp_qrm << "\","
			<< "cmp_dftqry:\"" << cmp_dftqry << "\","
			<< "cmp_reverse:" << cmp_reverse				
		<< "}";	
	code.mJson << str;
	return true;
}
