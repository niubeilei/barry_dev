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
#include "GICs/GicInput.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

// static AosGicPtr sgGic = new AosGicInput();

AosGicInput::AosGicInput(const bool flag)
:
AosGic(AOSGIC_INPUT, AosGicType::eInput, flag)
{
}


AosGicInput::~AosGicInput()
{
}


bool	
AosGicInput::generateCode(
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
			 OmnString gic_id = AosGetHtmlElemId();
			 /*value bd*/
			 OmnString querybd = vpd->getAttrStr("edt_query");
			 OmnString gic_value;
			 if (querybd != ""&& obj)
			 {
				 gic_value  = obj->getAttrStr(querybd,vpd->getAttrStr("edt_value"));
			 }
			 else
			 {
				 gic_value = vpd->getAttrStr("edt_value");
			 }
			/*pmt bd*/
			OmnString pmtbd = vpd->getAttrStr("gic_pmtbd");
			OmnString gic_pmt;
			if(pmtbd != ""&& obj)
			{
				gic_pmt  = obj->getAttrStr(pmtbd,vpd->getAttrStr("gic_pmt",""));
			}
			else
			{
				gic_pmt  = vpd->getAttrStr("gic_pmt","");
			}
			
			OmnString gic_width  = vpd->getAttrStr("gic_width","200");
			OmnString gic_height = vpd->getAttrStr("gic_height","25");
			OmnString gic_labelText = vpd->getAttrStr("ptg_prompt_text");
			OmnString gic_fstyle = vpd->getAttrStr("edt_fstyle","plain");
			OmnString gic_fgstyle = vpd->getAttrStr("ptg_fstyle","plain");
			OmnString gic_textType = vpd->getAttrStr("edt_password","false");
			OmnString gic_redOnly = vpd->getAttrStr("edt_cond","false");
			OmnString gic_textFsize = vpd->getAttrStr("edt_fsize","12");
			OmnString gic_obj_serialize = vpd->getAttrStr("gic_obj_serialize");
			OmnString gic_valueColor = vpd->getAttrStr("edt_color","#000000");
			if(gic_valueColor != "")
			{
				gic_valueColor = AosHtmlUtil::getWebColor(gic_valueColor);
			}
			OmnString gic_Multiline = vpd->getAttrStr("edt_multiline","false");
			OmnString gic_transparent = vpd->getAttrStr("gic_transparent","false");
			OmnString gic_MaxLength = vpd->getAttrStr("edt_maxlength","");			
			OmnString gic_layout = vpd->getAttrStr("ptg_layout","hori");
			OmnString gic_halign = vpd->getAttrStr("ptg_halign","left");
			OmnString gic_valign = vpd->getAttrStr("ptg_valign","center" );
			OmnString gic_spacing = vpd->getAttrStr("ptg_spacing","5");
			OmnString gic_labelcolor = vpd->getAttrStr("ptg_fgcolor","#000000");
			if(gic_labelcolor != "")
			{
				gic_labelcolor = AosHtmlUtil::getWebColor(gic_labelcolor);
			}
			OmnString gic_fsize = vpd->getAttrStr("ptg_fsize","12");
			OmnString gic_pmt_color = vpd->getAttrStr("gic_pmt_color","#808080");
			if(gic_pmt_color != "")
			{
				gic_pmt_color = AosHtmlUtil::getWebColor(gic_pmt_color);
			}
			OmnString gic_reg = vpd->getAttrStr("gic_reg","");
			OmnString reg_text = vpd->getAttrStr("reg_text","");
			OmnString gic_common_reg = vpd->getAttrStr("gic_common_reg","-1");
			OmnString gic_masked = vpd->getAttrStr("gic_masked","-1");
		//	code.mHtml <<">";

	OmnString str;
	str 
		<< ",xtype:\'" << vpd->getAttrStr("gic_type") <<"\'"
	//	<< ",el:\'" <<gic_id <<"\'"
		<< ",ptg_prompt_text:\'" << gic_labelText <<"\'"
		<< ",edt_fstyle:\'" << gic_fstyle <<"\'"
		<< ",edt_password:\'" << gic_textType <<"\'"
		<< ",edt_condRead:\'" << gic_redOnly <<"\'"
		<< ",edt_value:\'"<< gic_value <<"\'"
		<< ",edt_fsize:\'" << gic_textFsize <<"\'" 
		<< ",edt_color:\'" <<gic_valueColor<<"\'"
		<< ",edt_multiline:\'"<< gic_Multiline <<"\'"
		<< ",edt_maxlength:\'"<< gic_MaxLength <<"\'" 
		<< ",edt_query:\'" << querybd <<"\'"
		<< ",ptg_layout:\'" << gic_layout <<"\'"
		<< ",ptg_halign:\'" << gic_halign <<"\'"
		<< ",ptg_valign :\'" << gic_valign <<"\'"
		<< ",ptg_spacing:\'" << gic_spacing <<"\'"
		<< ",ptg_fsize:\'" << gic_fsize <<"\'" 
		<< ",ptg_fstyle:\'" << gic_fgstyle <<"\'"
		<< ",ptg_fgcolor:\'" << gic_labelcolor <<"\'"
		<< ",gic_pmt:\'" << gic_pmt <<"\'"
		<< ",gic_obj_serialize:\'" << gic_obj_serialize <<"\'"
		<< ",gic_pmtbd:\'" << pmtbd <<"\'"
		<< ",gic_transparent:\'" << gic_transparent <<"\'"
		<< ",gic_reg:\'" << gic_reg <<"\'"
		<< ",reg_text:\'" << reg_text <<"\'"
		<< ",gic_common_reg:\'" << gic_common_reg <<"\'"
		<< ",gic_masked:\'" << gic_masked <<"\'"
		<< ",gic_pmt_color:\'" << gic_pmt_color <<"\'";

	code.mJson << str;
	return true;
}

