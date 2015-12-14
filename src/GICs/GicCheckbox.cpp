//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved. 
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
// Modification History:
// 07/08/2010: Created by Brian 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicCheckbox.h"

#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "HtmlUtil/HtmlUtil.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

#include <vector>

AosGicCheckbox::AosGicCheckbox(const bool flag)
:
AosGic(AOSGIC_CHECKBOX, AosGicType::eCheckBox, flag)
{
}


AosGicCheckbox::~AosGicCheckbox()
{
}


bool	
AosGicCheckbox::generateCode(
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
	
   
 
	//This function converts the VPD to json form
	OmnString gic_text = vpd->getAttrStr("ptg_prompt_text");
	OmnString gic_checkbox_ly = vpd->getAttrStr("gic_checkbox_ly");
	OmnString gic_layout = vpd->getAttrStr("ptg_layout");
	OmnString gic_labelColor = vpd->getAttrStr("ptg_fgcolor");
	if(gic_labelColor != "")
	{
	    gic_labelColor = AosHtmlUtil::getWebColor(gic_labelColor) ;
	}
	int gic_labelFontSize = vpd->getAttrInt("ptg_fsize",12);
	OmnString gic_halign = vpd->getAttrStr("ptg_halign","left");
	OmnString gic_valign = vpd->getAttrStr("ptg_valign","center");
	OmnString gic_labelStyle = vpd->getAttrStr("ptg_fstyle","normal");
	OmnString cbx_query = vpd->getAttrStr("cbx_query");
	OmnString cbxTvbd = vpd->getAttrStr("cbx_tvbd");
	OmnString cbxFvbd = vpd->getAttrStr("cbx_fvbd");
	OmnString cbxTv = vpd->getAttrStr("cbx_tv");
	OmnString cbxFv = vpd->getAttrStr("cbx_fv");
	OmnString cbxLbbd = vpd->getAttrStr("cbx_lbbd");
	OmnString boxStyle = vpd->getAttrStr("box_style","1");
	OmnString imgUrl = vpd->getAttrStr("img_url");
	int gic_space = vpd->getAttrInt("ptg_spacing",5);
    
	if(gic_layout == "stack_hori")
	{
	     gic_layout = "hori";
	}
	if(gic_layout == "")
	{
	     gic_layout = "hori";	
	}
	// get value of gic_checked	
	OmnString gic_checked = vpd->getAttrStr("cbx_dft_status");
  
    code.mJson << ","
               << "labelFontColor: \""<< gic_labelColor << "\","
               << "labelFontSize: "<< gic_labelFontSize << ","
               << "labelStyle: \""<< gic_labelStyle <<"\","
               << "checked: \""<< gic_checked << "\","
               << "boxStyle:\""<< boxStyle << "\","
               << "imgUrl: \""<< imgUrl << "\","
               << "cbxQuery: \""<< cbx_query << "\","
               << "cbxTvbd: \""<< cbxTvbd << "\","
               << "cbxFvbd: \""<< cbxFvbd << "\","
               << "cbxTv: \""<< cbxTv << "\","
               << "cbxFv: \""<< cbxFv << "\","
               << "cbxLbbd: \""<< cbxLbbd << "\","
               << "layoutCfg:\""<< gic_layout<<"\","
               << "gicCheckboxLy:\""<< gic_checkbox_ly<<"\","
               << "space:"<< gic_space <<","
               << "halign:\""<< gic_halign <<"\","
               << "valign:\""<< gic_valign <<"\","
               << "labelText : \"" <<gic_text << "\"";
               return true;
}


OmnString 
AosGicCheckbox::getXml(const AosRandomRulePtr &rule)
{
	mRule = rule;
    OmnString xml = "<";
	xml << rule->mNodeName << " ";

	XmlGenRuleValueIter itr;
	for (itr = rule->mAttrs_r.begin(); itr != rule->mAttrs_r.end(); ++itr)
	{
		xml << itr->first <<"=\"" << (itr->second).getRandomValue() << "\" ";
	}
	for (itr = rule->mAttrs_o.begin(); itr != rule->mAttrs_o.end(); ++itr)
	{
		if (rand()%10 < 8)
			xml << itr->first <<"=\"" << (itr->second).getRandomValue() << "\" ";
	}
	
	xml << "/>";
	
	return xml;
}
