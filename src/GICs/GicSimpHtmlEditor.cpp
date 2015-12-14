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
// 07/08/2010: Created by Henry
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicSimpHtmlEditor.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicSimpHtmlEditor();

AosGicSimpHtmlEditor::AosGicSimpHtmlEditor(const bool flag)
:
AosGic(AOSGIC_SIMPHTMLEDITOR, AosGicType::eSimpHtmlEditor, flag)
{
}


AosGicSimpHtmlEditor::~AosGicSimpHtmlEditor()
{
}


bool	
AosGicSimpHtmlEditor::generateCode(
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
	bool gic_enableFormat = vpd->getAttrBool("gic_enableFormat", false);
	bool gic_enableFontSize = vpd->getAttrBool("gic_enableFontSize", false);
	bool gic_enableColors = vpd->getAttrBool("gic_enableColors", false);
	bool gic_enableAlignments = vpd->getAttrBool("gic_enableAlignments", false);
	bool gic_enableLists = vpd->getAttrBool("gic_enableLists", false);
	bool gic_enableSourceEdit = vpd->getAttrBool("gic_enableSourceEdit", false);
	bool gic_enableLinks = vpd->getAttrBool("gic_enableLinks", false);
	bool gic_enableFont = vpd->getAttrBool("gic_enableFont", false);
	bool gic_readOnly = vpd->getAttrBool("gic_readOnly", false);
	bool gic_disabled = vpd->getAttrBool("gic_disabled", false);
	bool gic_emote = vpd->getAttrBool("gic_emote", false);
	bool gic_image = vpd->getAttrBool("gic_image", false);
	bool gic_htmlcmp = vpd->getAttrBool("gic_htmlcmp", false);
	bool gic_selimg = vpd->getAttrBool("gic_selimg", false);
	bool gic_convert = vpd->getAttrBool("gic_convert", true);
	bool gic_addseparator = vpd->getAttrBool("gic_addseparator", false);
	bool gic_menu = vpd->getAttrBool("gic_menu", false);
	OmnString gic_mVpdname = vpd->getAttrStr("gic_mVpdname", "vpd_add_fun");
	
	OmnString edt_value = vpd->getAttrStr("edt_value", "");
	OmnString edt_query = vpd->getAttrStr("edt_query", "");
	OmnString gic_separator = vpd->getAttrStr("gic_separator", "");
	
	OmnString str;
	str << ","
		<< "gic_enableFormat:"<<gic_enableFormat << ", "
		<< "gic_enableFontSize:"<<gic_enableFontSize << ", "
		<< "gic_enableColors:"<<gic_enableColors << ", "
		<< "gic_enableAlignments:"<<gic_enableAlignments << ", "
		<< "gic_enableLists:"<<gic_enableLists << ", "
		<< "gic_enableSourceEdit:"<<gic_enableSourceEdit << ", "
		<< "gic_enableLinks:"<<gic_enableLinks << ", "
		<< "gic_enableFont:"<<gic_enableFont << ", "
		<< "gic_readOnly:"<<gic_readOnly << ", "
		<< "gic_disabled:"<<gic_disabled << ", "
		<< "gic_emote:"<<gic_emote << ", "
		<< "gic_image:"<<gic_image << ", "
		<< "gic_htmlcmp:"<<gic_htmlcmp << ", "
		<< "gic_selimg:"<<gic_selimg << ", "
		<< "gic_convert:"<<gic_convert << ","
		<< "gic_menu:"<<gic_menu << ","
		<< "gic_addseparator:"<<gic_addseparator << ", "
		<< "gic_mVpdname:\"" << gic_mVpdname <<"\", "
		<< "gic_separator:\"" << gic_separator <<"\", "
		<< "edt_query:\"" << edt_query <<"\", "
		<< "edt_value:\"" << edt_value <<"\" ";

	code.mJson << str;
	return true;
}

