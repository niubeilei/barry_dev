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
#include "GICs/GicHtmlInput.h"
#include "HtmlUtil/HtmlUtil.h" 
#include "HtmlServer/HtmlUtil.h" 
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicHtmlInput::AosGicHtmlInput(const bool flag)
:
AosGic(AOSGIC_HTMLINPUT, AosGicType::eHtmlInput, flag)
{
}


AosGicHtmlInput::~AosGicHtmlInput()
{
}


bool	
AosGicHtmlInput::generateCode(
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
	// 	ph means placeHolder
	
	OmnString gic_eleid = AosGetHtmlElemId();
	OmnString cls_name, ph_cls_name, styleStr, phStyleStr;
	cls_name << AOSGIC_CLS_INPUT << "-" << gic_eleid;
	ph_cls_name << AOSGIC_CLS_INPUT_PLACEHOLDER << "-" << gic_eleid;
	
	OmnString gic_pmt_color = AosHtmlUtil::getWebColor(vpd->getAttrStr("gic_pmt_color"));
	if (gic_pmt_color != "") phStyleStr << "color:" << gic_pmt_color << ";";

	OmnString edt_fstyle = vpd->getAttrStr("edt_fstyle");
	if (edt_fstyle == "bold") {
		styleStr << "font-weight:bold;";
	} else if (edt_fstyle == "italic") {
		styleStr << "font-style:italic;";
	} else if (edt_fstyle == "bolditalic") {
		styleStr << "font-weight:bold;font-style:italic;";
	}
	
	int edt_fsize = vpd->getAttrInt("edt_fsize", -1);
	if(edt_fsize > 0) styleStr << "font-size:" << edt_fsize << "px;";

	OmnString edt_color = AosHtmlUtil::getWebColor(vpd->getAttrStr("edt_color"));
	if(edt_color != "") styleStr << "color:" << edt_color << ";";
	
	OmnString background_color = AosHtmlUtil::getWebColor(vpd->getAttrStr("background_color"));
	if(background_color != "") styleStr << "background-color:" << background_color << ";";

	OmnString gic_border = vpd->getAttrStr("gic_border");
	if(gic_border != "") styleStr << "border-style:" << gic_border << ";";
	
	int border_width = vpd->getAttrInt("border_width", -1);
	if(border_width >= 0) styleStr << "border-width:" << border_width << "px;";
	else border_width = 0;

	OmnString border_color = AosHtmlUtil::getWebColor(vpd->getAttrStr("border_color"));
	if(border_color != "") styleStr << "border-color:" << border_color << ";";
	
	OmnString css;
	css << "." << cls_name << "{" << styleStr << "}";
	css << "." << ph_cls_name << "{" << phStyleStr << "}";

	OmnString gic_self_Cls = vpd->getAttrStr("gic_self_Cls");
	OmnString gic_self_phCls = vpd->getAttrStr("gic_self_phCls");

	int gic_width = vpd->getAttrInt("gic_width", 200);
	int gic_height = vpd->getAttrInt("gic_height", 26);

	bool edt_multiline = vpd->getAttrBool("edt_multiline", false);
	bool edt_password = vpd->getAttrBool("edt_password", false);
	bool edt_readOnly = vpd->getAttrBool("edt_cond", false);
	OmnString html;
	if(edt_multiline)
	{
		html << "<textarea ";
	}
	else
	{
		OmnString textType = edt_password ? "password" : "text";
		html << "<input type=\"" << textType << "\" ";
	}
	
	OmnString line_height = "line-height:";
	line_height << (gic_height - border_width * 2) << "px;";

	html << " class=\"" << AOSGIC_CLS_INPUT << " " << cls_name << " " << gic_self_Cls << "\" "
		 << (edt_readOnly ? "readOnly" : "")
		 << " style=\"width:" << gic_width << "px;height:" << gic_height << "px;"
		 << (!edt_multiline ? line_height : "") << "\" "
		 << (edt_multiline ? "></textarea>" : "/>");
	
	OmnString edt_value = vpd->getAttrStr("edt_value");
	OmnString edt_query = vpd->getAttrStr("edt_query");
	OmnString gic_pmt = vpd->getAttrStr("gic_pmt");
	OmnString gic_pmtbd = vpd->getAttrStr("gic_pmtbd");

	int gic_common_reg = vpd->getAttrInt("gic_common_reg", -1);
	OmnString gic_reg = vpd->getAttrStr("gic_reg");
	OmnString reg_text = vpd->getAttrStr("reg_text");
	bool check_empty = vpd->getAttrBool("check_empty", false);
	bool gic_obj_serialize = vpd->getAttrBool("gic_obj_serialize", false);
	int edt_maxlength = vpd->getAttrInt("edt_maxlength", -1);

	OmnString json;
	json << ","
		 << "gic_self_Cls:\"" << gic_self_Cls << "\","
		 << "gic_self_phCls:\"" << gic_self_phCls << "\","
		 << "gic_eleid:\"" << gic_eleid << "\","
		 << "edt_value:\"" << edt_value << "\","
		 << "edt_query:\"" << edt_query << "\","
		 << "gic_pmt:\"" << gic_pmt << "\", "
		 << "gic_pmtbd:\"" << gic_pmtbd << "\", "
		 << "gic_pmt_color:\"" << gic_pmt_color << "\","
		 << "edt_fstyle:\"" << edt_fstyle << "\","
		 << "edt_fsize:" << edt_fsize << ","
		 << "edt_color:\"" << edt_color << "\","
		 << "background_color:\"" << background_color << "\","
		 << "gic_border:\"" << gic_border<< "\","
		 << "border_width:" << border_width << ","
		 << "border_color:\"" << border_color << "\","
		 << "edt_multiline:" << edt_multiline << ","
		 << "iedt_readOnly:" << edt_readOnly << ","
		 << "edt_password:" << edt_password << ","
		 << "gic_common_reg:" << gic_common_reg << ","
		 << "gic_reg:\"" << gic_reg << "\","
		 << "reg_text:\"" << reg_text << "\","
		 << "check_empty:" << check_empty << ","
		 << "gic_obj_serialize:" << gic_obj_serialize << ","
		 << "edt_maxlength:"<< edt_maxlength; 
	
	code.mHtml << html;
	code.mCss << css;
	code.mJson << json;
	return true;
}

