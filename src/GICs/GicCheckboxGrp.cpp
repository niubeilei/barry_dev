//////////////////////////////////////////////////////////////////////////// //
//// Copyright (C) 2005 // Packet Engineering, Inc. All rights reserved. 
//// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
// Modification History:
// 07/26/2010: Created by Brian 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicCheckboxGrp.h"

#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "HtmlUtil/HtmlUtil.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"

//static AosGicPtr sgGic = new AosGicCheckbox();

AosGicCheckboxGrp::AosGicCheckboxGrp(const bool flag)
:
AosGic(AOSGIC_CHECKBOXGRP, AosGicType::eCheckBoxGrp, flag)
{
}


AosGicCheckboxGrp::~AosGicCheckboxGrp()
{
}


bool	
AosGicCheckboxGrp::generateCode(
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
	OmnString ptg_prompt_text = vpd->getAttrStr("ptg_prompt_text");
	OmnString ptg_layout = vpd->getAttrStr("ptg_layout","hori");
	OmnString ptg_fgcolor = vpd->getAttrStr("ptg_fgcolor");
	if(ptg_fgcolor != "")
	{
	     ptg_fgcolor = AosHtmlUtil::getWebColor(ptg_fgcolor) ;
	}
	OmnString ptg_fsize = vpd->getAttrStr("ptg_fsize","12");
	OmnString ptg_halign = vpd->getAttrStr("ptg_halign","left");
	OmnString ptg_valign = vpd->getAttrStr("ptg_valign","center");
	OmnString ptg_fstyle = vpd->getAttrStr("ptg_fstyle","normal");
	int ptg_spacing = vpd->getAttrInt("ptg_spacing",5);
	OmnString vpd_value_bind = vpd->getAttrStr("vpd_value_bind");
	int cbg_col_sp = vpd->getAttrInt("cbg_col_sp",0);
	int cbg_row_sp = vpd->getAttrInt("cbg_row_sp",0);
	OmnString cbg_no_cols = vpd->getAttrStr("cbg_no_cols","1");
	OmnString cbg_no_rows = vpd->getAttrStr("cbg_no_rows","4");
	OmnString cbg_hasDataC = vpd->getAttrStr("cbg_hasDataC","vpd");
	OmnString cbg_lm_name = vpd->getAttrStr("cbg_lm_name","lm_wrap_vert");
	OmnString cbg_value_bind = vpd->getAttrStr("cbg_value_bind");
	OmnString chb_value_bind = vpd->getAttrStr("chb_value_bind");
	OmnString chb_name_bind = vpd->getAttrStr("chb_name_bind");
	OmnString box_style = vpd->getAttrStr("box_style","1");
	OmnString img_url = vpd->getAttrStr("img_url");
	OmnString gic_objid_bd = vpd->getAttrStr("gic_objid_bd");
	OmnString gic_dftpath = vpd->getAttrStr("gic_dftpath");
    OmnString cbg_members = "";
	if(cbg_hasDataC == "datacol")
	{ 
		AosXmlTagPtr items = vpd->getFirstChild("items");
	    if(!items)
		{
		    OmnAlarm << " checkboxgrp can't get node 'items'!" << enderr ;
			return false ;
		}
		AosXmlTagPtr content_vpd = vpd->getNextChild("datacol");
	    if(!content_vpd)
		{
		    OmnAlarm << " checkboxgrp can't get node 'datacol'!" << enderr ;
			return false ;
		}

   		OmnString data_type = content_vpd->getAttrStr("gic_type");
  
		if(data_type =="str_datacol")
		{
			 code.mJson << ","
						<< "chb_value_bind:'" << chb_value_bind << "',"
                        << "chb_name_bind: \'"<< chb_name_bind << "\',"
                        << "cbg_value_bind: \'"<< cbg_value_bind << "\',"
                        << "box_style: \'"<< box_style << "\',"
                        << "img_url: \'"<< img_url << "\',"
                        << "cbg_lm_name: \'"<< cbg_lm_name << "\',"
                        << "cbg_hasDataC: \'"<< cbg_hasDataC << "\',"
                        << "cbg_row_sp: "<< cbg_row_sp<< ","
                        << "cbg_col_sp: "<< cbg_col_sp << ","
                        << "cbg_no_rows: '"<< cbg_no_rows << "',"
                        << "cbg_no_cols: '"<< cbg_no_cols << "',"
                        << "cbg_value_bind: \'"<< cbg_value_bind << "\',"
                        << "ptg_fsize: '"<< ptg_fsize << "',"
                        << "ptg_fgcolor:'"<< ptg_fgcolor << "',"
                        << "ptg_fstyle: '"<< ptg_fstyle <<"',"
                        << "ptg_layout:'"<< ptg_layout << "',"
                        << "ptg_spacing:"<< ptg_spacing <<","
                        << "ptg_halign:'"<< ptg_halign <<"',"
                        << "ptg_valign:'"<< ptg_valign <<"',"
						<< "gic_type:'" << data_type <<"',"
                        << "ptg_prompt_text : '" <<ptg_prompt_text << "'";
		}
		else if(data_type == "db_datacol")
		{
             code.mJson << ","
						<< "chb_value_bind:'" << chb_value_bind << "',"
                        << "chb_name_bind: \'"<< chb_name_bind << "\',"
                        << "cbg_value_bind: \'"<< cbg_value_bind << "\',"
                        << "box_style: \'"<< box_style << "\',"
                        << "img_url: \'"<< img_url << "\',"
                        << "cbg_lm_name: \'"<< cbg_lm_name << "\',"
                        << "cbg_hasDataC: \'"<< cbg_hasDataC << "\',"
                        << "cbg_row_sp: "<< cbg_row_sp<< ","
                        << "cbg_col_sp: "<< cbg_col_sp << ","
                        << "cbg_no_rows: '"<< cbg_no_rows << "',"
                        << "cbg_no_cols: '"<< cbg_no_cols << "',"
                        << "cbg_value_bind: \'"<< cbg_value_bind << "\',"
                        << "ptg_fsize: '"<< ptg_fsize << "',"
                        << "ptg_fgcolor:'"<< ptg_fgcolor << "',"
                        << "ptg_fstyle: '"<< ptg_fstyle <<"',"
                        << "ptg_layout:'"<< ptg_layout << "',"
                        << "ptg_spacing:"<< ptg_spacing <<","
                        << "ptg_halign:'"<< ptg_halign <<"',"
                        << "ptg_valign:'"<< ptg_valign <<"',"
						<< "gic_type:'" << data_type <<"',"
						<< "ptg_prompt_text: '" << ptg_prompt_text  << "'";
		   
		}
	}
	else if(cbg_hasDataC == "vpd")
	{
		OmnString members = "" ;
		OmnString flag1 = "|$|" ;
		OmnString flag2 = "|$$|" ;
		AosXmlTagPtr items = vpd->getFirstChild("items");
		if (!items) return true;
		//aos_assert_r(items, false); // 08/15/2011 lynch 
		AosXmlTagPtr item = items->getFirstChild();
		while(item)
		{
			 OmnString name = item->getAttrStr("name") ;
			 OmnString value = item->getAttrStr("value") ;
			 OmnString tip = item->getAttrStr("tip") ;
			 item = items->getNextChild("item");
			 if(item)
			 {
			     members << name << flag1 << value << flag1 << tip << flag2;
			 }
			 else
			 {
			     members << name << flag1 << value << flag1 << tip ;
			 }
		}
	
        code.mJson << ","
				   << "chb_value_bind:'" << chb_value_bind << "',"
                   << "chb_name_bind: \'"<< chb_name_bind << "\',"
                   << "cbg_value_bind: \'"<< cbg_value_bind << "\',"
                   << "box_style: \'"<< box_style << "\',"
                   << "img_url: \'"<< img_url << "\',"
                   << "cbg_lm_name: \'"<< cbg_lm_name << "\',"
                   << "cbg_hasDataC: \'"<< cbg_hasDataC << "\',"
                   << "cbg_row_sp: "<< cbg_row_sp << ","
                   << "cbg_col_sp: "<< cbg_col_sp << ","
                   << "cbg_no_rows: '"<< cbg_no_rows << "',"
                   << "cbg_no_cols: '"<< cbg_no_cols << "',"
                   << "cbg_value_bind: \'"<< cbg_value_bind << "\',"
                   << "ptg_fsize: '"<< ptg_fsize << "',"
                   << "ptg_fgcolor:'"<< ptg_fgcolor << "',"
                   << "ptg_fstyle: '"<< ptg_fstyle <<"',"
                   << "ptg_layout:'"<< ptg_layout << "',"
                   << "ptg_spacing:"<< ptg_spacing <<","
                   << "ptg_halign:'"<< ptg_halign <<"',"
                   << "ptg_valign:'"<< ptg_valign <<"',"
			       << "members:'" << members <<"',"
                   << "ptg_prompt_text : '" << ptg_prompt_text  << "'";
	}
	else if(cbg_hasDataC == "objid")
	{
        code.mJson << ","
				   << "chb_value_bind:'" << chb_value_bind << "',"
                   << "chb_name_bind: \'"<< chb_name_bind << "\',"
                   << "cbg_value_bind: \'"<< cbg_value_bind << "\',"
                   << "box_style: \'"<< box_style << "\',"
                   << "img_url: \'"<< img_url << "\',"
                   << "cbg_lm_name: \'"<< cbg_lm_name << "\',"
                   << "cbg_hasDataC: \'"<< cbg_hasDataC << "\',"
                   << "cbg_row_sp: "<< cbg_row_sp << ","
                   << "cbg_col_sp: "<< cbg_col_sp << ","
                   << "cbg_no_rows: '"<< cbg_no_rows << "',"
                   << "cbg_no_cols: '"<< cbg_no_cols << "',"
                   << "cbg_value_bind: \'"<< cbg_value_bind << "\',"
                   << "ptg_fsize: '"<< ptg_fsize << "',"
                   << "ptg_fgcolor:'"<< ptg_fgcolor << "',"
                   << "ptg_fstyle: '"<< ptg_fstyle <<"',"
                   << "ptg_layout:'"<< ptg_layout << "',"
                   << "ptg_spacing:"<< ptg_spacing <<","
                   << "ptg_halign:'"<< ptg_halign <<"',"
                   << "ptg_valign:'"<< ptg_valign <<"',"
                   << "gic_objid_bd:'"<< gic_objid_bd <<"',"
                   << "gic_dftpath:'"<< gic_dftpath <<"',"
                   << "ptg_prompt_text : '" << ptg_prompt_text  << "'";
	}
    return true;
}


