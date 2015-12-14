//////////////////////////////////////////////////////////////////////////
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
// 07/08/2010: Created by Phnix 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicCombox.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlModules/DataCol.h"
#include "HtmlModules/Ptrs.h"
#include "HtmlServer/HtmlUtil.h"

static const OmnString strNull = "";
//static bool bTrue = true;

AosGicCombox::AosGicCombox(const bool flag)
:
AosGic(AOSGIC_COMBOX, AosGicType::eCombox, flag)
{
}


AosGicCombox::~AosGicCombox()
{
}

bool	
AosGicCombox::generateCode(
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

	//get every record's value:
	OmnString cmb_text_bind = vpd->getAttrStr("cmb_text_bind", "");
	OmnString cmb_value_query_dcl = vpd->getAttrStr("cmb_value_query_dcl", "");

	AosXmlTagPtr tag = 0;//doc->getFirstChild();
	OmnString gic_id = AosGetHtmlElemId();


	// ptg_color
	OmnString fgcolorbd= vpd->getAttrStr("gic_fgcolorbd", "");
	OmnString gic_labelFontColor;
	if (fgcolorbd != "" && obj)
		gic_labelFontColor = obj->getAttrStr(fgcolorbd, vpd->getAttrStr("ptg_fgcolor", ""));
	else
	{
		gic_labelFontColor = vpd->getAttrStr("ptg_fgcolor", "black");
		if (gic_labelFontColor.substr(0,1) == "0x" || gic_labelFontColor.substr(0,1) == "0X")
		{
			gic_labelFontColor = gic_labelFontColor.substr(2,gic_labelFontColor.length()-1);
		}
	}
	if(gic_labelFontColor.data())
	{
		 gic_labelFontColor = AosHtmlUtil::getWebColor(gic_labelFontColor);
	}

	OmnString gic_width = vpd->getAttrStr("gic_width","0");
	OmnString gic_height = vpd->getAttrStr("gic_height","0");

	OmnString gic_labelFontStyle = vpd->getAttrStr("ptg_fstyle", "plain");
	OmnString gic_labelFontSize  = vpd->getAttrStr("ptg_fsize", "12");
	OmnString gic_labelText = vpd->getAttrStr("ptg_prompt_text", "");
	OmnString gic_layout = vpd->getAttrStr("ptg_layout", "hori");
	OmnString gic_halign = vpd->getAttrStr("ptg_halign", "left");
	OmnString gic_valign = vpd->getAttrStr("ptg_valign", "center");
	OmnString slc_dataC = vpd->getAttrStr("slc_dataC", "false");
	OmnString cmb_entries = vpd->getAttrStr("cmb_entries", "");		//when don't use dataCol, cmb_entries will be effect
	OmnString cmb_editable = vpd->getAttrStr("cmb_editable", "");
	OmnString cmb_dft_value = vpd->getAttrStr("cmb_dft_value", "");		// default bind the display 
	OmnString cmb_dft_text = vpd->getAttrStr("cmb_dft_text", "");		// default text
	OmnString gic_dftvbd = vpd->getAttrStr("gic_dftvbd", "");			// default bind the value
	OmnString cmb_value_query = vpd->getAttrStr("cmb_value_query", "");		
	OmnString cmb_entry_type = vpd->getAttrStr("cmb_entry_type", "nv");		
	OmnString cmb_newAdd = vpd->getAttrStr("cmb_newadd", "false");
	OmnString ptg_spacing = vpd->getAttrStr("ptg_spacing", "5");

	code.mHtml <<"<span></span><span></span>";
	code.mJson << ",";

	code.mJson << "labelFontColor:\"" <<gic_labelFontColor 
		<< "\",labelFontSize:\"" <<gic_labelFontSize 
		<< "\",labelText:\"" <<gic_labelText
		<< "\",slc_dataC:\"" <<slc_dataC	//use the backend data or not
		<< "\",cmb_entries:\"" <<cmb_entries	//data which don't use backend datacol
		<< "\",labelFontStyle:\"" <<gic_labelFontStyle
		<< "\",gic_layout:\"" <<gic_layout 
		<< "\",halign:\"" <<gic_halign
		<< "\",valign:\"" <<gic_valign
		<< "\",ptg_spacing:\"" <<ptg_spacing
		<< "\",cmb_editable:\"" <<cmb_editable		// combox can edit or not
		<< "\",cmb_entry_type:\"" <<cmb_entry_type	// "nv":name and value, "vo":nameonly, means name == value
		<< "\",cmb_dft_value:\"" <<cmb_dft_value	// default value, which will display when combobox appear 默认数值
		<< "\",cmb_dft_text:\"" <<cmb_dft_text		// default text to display
		<< "\",gic_dftvbd:\"" <<gic_dftvbd			// gic_dftvbd 13  默认数值绑定
		<< "\",cmb_value_query:\"" <<cmb_value_query // data bind, like '@zky_gaga'
		<< "\",cmb_text_bind:\"" <<cmb_text_bind // bind dataCol name, to show
		<< "\",cmb_value_query_dcl:\"" <<cmb_value_query_dcl	 // bind dataCol value in backend 
		<< "\",cmb_newAdd:\"" <<cmb_newAdd		// can add a new value or not
		<<"\"";
	return true;
}
