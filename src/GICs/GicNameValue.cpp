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
// 08/07/2010: Created by ketty//////////////////////////////////////////////////////////////////////////
#include "GICs/GicNameValue.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlUtil/HtmlUtil.h"

// static AosGicPtr sgGic = new AosGicNameValue();

AosGicNameValue::AosGicNameValue(const bool flag)
:
AosGic(AOSGIC_NAMEVALUE, AosGicType::eNameValue, flag)
{
}


AosGicNameValue::~AosGicNameValue()
{
}


bool	
AosGicNameValue::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		//const AosGicType &gictype,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	
	OmnString gic_name;
	OmnString gic_value;
	OmnString namebd = vpd->getAttrStr("ptg_bind");
	OmnString valuebd = vpd->getAttrStr("val_bind");

	OmnString gic_x = vpd->getAttrStr("gic_x","100");
	OmnString gic_y = vpd->getAttrStr("gic_y","100");
	

	OmnString gic_labelFontColor = vpd->getAttrStr("ptg_fgcolor");
	OmnString gic_valueFontColor = vpd->getAttrStr("val_fgcolor");
	int gic_labelFontSize = vpd->getAttrInt("ptg_fsize", 12);
	int gic_valueFontSize = vpd->getAttrInt("val_fsize", 12);
	OmnString gic_vertically = "false";
	int gic_space = vpd->getAttrInt("ptg_spacing", 5);
	OmnString gic_labelHalign = vpd->getAttrStr("ptg_valign");
	OmnString gic_labelValign = vpd->getAttrStr("ptg_halign");
	OmnString gic_labelFontStyle = vpd->getAttrStr("ptg_fstyle");
	OmnString gic_valueFontStyle = vpd->getAttrStr("val_fstyle");
	if(gic_valueFontStyle  == "plain"){
		gic_valueFontStyle = "normal";
	}

	if(vpd->getAttrStr("ptg_layout") == "vertical")
	{
		gic_vertically = "true";
	}

	OmnString gic_valueMultiline = vpd->getAttrStr("val_multiline", "true");

	if(namebd != "" && obj)
	{
		gic_name = obj->getAttrStr(namebd, vpd->getAttrStr("ptg_prompt_text","Name"));
	}else{
	
		gic_name = vpd->getAttrStr("ptg_prompt_text","Name");
	}

	if(valuebd !="" && obj)
	{
		gic_value = obj->getAttrStr(valuebd, vpd->getAttrStr("val_text","Value"));
	}else{
	
		gic_value = vpd->getAttrStr("val_text","Value");
	}


	int gic_valueMaxLength = vpd ->getAttrInt("val_maxlength", 0);
	int valueLength = gic_value.length();
	if(gic_valueMaxLength != 0 && gic_valueMaxLength <= valueLength)
	{
		gic_value = gic_value.substr(0, gic_valueMaxLength-1);
	}

	if (gic_labelFontColor.length() >= 2)  // 08/15/2011 lynch
	{
		if(gic_labelFontColor.substr(0,1) == "0x")
		{
			gic_labelFontColor = gic_labelFontColor.substr(2, gic_labelFontColor.length()-1);
		}
	}

	if (gic_valueFontColor.length() >= 2)
	{
		if(gic_valueFontColor.substr(0,1) == "0x")
		{
			gic_valueFontColor = gic_valueFontColor.substr(2, gic_valueFontColor.length()-1);
		}
	}

	code.mHtml << "<p>"	<< gic_name << "</p><p>" << gic_value << "</p>";

	code.mJson << ","
			   << "x:" << gic_x << ","
			   << "y:" << gic_y << ","
			   << "namebd: \"" << namebd << "\","
			   << "valuebd:\"" << valuebd << "\","
			   << "labelText: \"" << gic_name << "\","
			   << "valueText: \"" << gic_value << "\","
	           << "labelFontColor: '" << gic_labelFontColor << "',"
			   << "labelFontSize: " << gic_labelFontSize << ","
			   << "valueFontColor:'" << gic_valueFontColor << "',"
			   << "valueFontSize: " << gic_valueFontSize << ","
			   << "valueMultiline: " << gic_valueMultiline << "," 
			   << "vertically: " << gic_vertically << ",";
	if(gic_labelHalign == "" )
	{
		code.mJson << "labelHalign: 'center',";
	}
	else
	{
		code.mJson << "labelHalign: '" << gic_labelHalign << "',";
	}

	if(gic_labelValign != "")
	{
		code.mJson << "labelValign: '" << gic_labelValign << "',";
	}
	if(gic_labelFontStyle != "") 
	{
		code.mJson << "labelFontStyle: '" << gic_labelFontStyle << "',";
	}
	if(gic_valueFontStyle != "")
	{
		code.mJson <<"valueFontStyle: '" << gic_valueFontStyle << "',";
	}
	code.mJson	<< "space: " << gic_space ;
	

	return true;
}
