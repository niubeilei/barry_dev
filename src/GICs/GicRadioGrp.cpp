//////////////////////////////////////////////////////////////////////////// //
//// Copyright (C) 2005 // Packet Engineering, Inc. All rights reserved. 
//// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
// Modification History:
// 07/26/2010: Created by Brian 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicRadioGrp.h"

#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "HtmlUtil/HtmlUtil.h"
#include "Util/String.h"

//static AosGicPtr sgGic = new AosGicCheckbox();

AosGicRadioGrp::AosGicRadioGrp(const bool flag)
:
AosGic(AOSGIC_RADIOGRP, AosGicType::eRadioGrp, flag)
{
}


AosGicRadioGrp::~AosGicRadioGrp()
{
}


bool	
AosGicRadioGrp::generateCode(
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
	OmnString ptg_fsize = vpd->getAttrStr("ptg_fsize", "12");
	OmnString ptg_halign = vpd->getAttrStr("ptg_halign","left");
	OmnString ptg_valign = vpd->getAttrStr("ptg_valign","center");
	OmnString ptg_fstyle = vpd->getAttrStr("ptg_fstyle");
	int ptg_spacing = vpd->getAttrInt("ptg_spacing",0);
	int rgp_col_sp = vpd->getAttrInt("rgp_col_sp",0);
	int rgp_row_sp = vpd->getAttrInt("rgp_row_sp",0);
	OmnString rgp_no_cols = vpd->getAttrStr("rgp_no_cols","1");
	OmnString rgp_no_rows = vpd->getAttrStr("rgp_no_rows","2");
	OmnString rgp_hasDataC = vpd->getAttrStr("rgp_hasDataC","false");
	OmnString rgp_query = vpd->getAttrStr("rgp_query");
	OmnString rgp_name_bind = vpd->getAttrStr("rgp_name_bind");
	OmnString rgp_value_bind = vpd->getAttrStr("rgp_value_bind");
	OmnString rgp_value = vpd->getAttrStr("rgp_value");
	OmnString rgp_lout = vpd->getAttrStr("rgp_lout","vert");
    OmnString gic_members = "";
	if(rgp_hasDataC == "true")
	{
		AosXmlTagPtr content_vpd = vpd->getFirstChild("datacol");
		OmnString data_type = content_vpd->getAttrStr("gic_type");
		
		if(data_type =="str_datacol")
		{
			 code.mJson << ","
                        << "ptg_prompt_text: \'"<< ptg_prompt_text << "\',"
                        << "ptg_layout: \'"<< ptg_layout << "\',"
                        << "ptg_fgcolor:\'"<< ptg_fgcolor << "\',"
                        << "ptg_fsize: \'"<< ptg_fsize << "\',"
                        << "ptg_halign: \'"<< ptg_halign << "\',"
                        << "ptg_valign: \'"<< ptg_valign << "\',"
                        << "ptg_fstyle: \'"<< ptg_fstyle << "\',"
                        << "ptg_spacing: \'"<< ptg_spacing << "\',"
                        << "rgp_col_sp: "<< rgp_col_sp<< ","
                        << "rgp_row_sp: "<< rgp_row_sp << ","
                        << "rgp_hasDataC: '"<< rgp_hasDataC << "',"
                        << "rgp_query: '"<< rgp_query << "',"
                        << "rgp_name_bind: \'"<< rgp_name_bind << "\',"
                        << "rgp_value_bind: '"<< rgp_value_bind << "',"
                        << "rgp_value:'"<< rgp_value << "',"
                        << "rgp_no_cols: '"<< rgp_no_cols <<"',"
                        << "rgp_no_rows:'"<< rgp_no_rows << "',"
                        << "rgp_lout:'"<< rgp_lout <<"',"
						<< "data_type:'" << data_type <<"'";
		}
		else if(data_type == "db_datacol")
		{
             code.mJson << ","
                        << "ptg_prompt_text: \'"<< ptg_prompt_text << "\',"
                        << "ptg_layout: \'"<< ptg_layout << "\',"
                        << "ptg_fgcolor:\'"<< ptg_fgcolor << "\',"
                        << "ptg_fsize: \'"<< ptg_fsize << "\',"
                        << "ptg_halign: \'"<< ptg_halign << "\',"
                        << "ptg_valign: \'"<< ptg_valign << "\',"
                        << "ptg_fstyle: \'"<< ptg_fstyle << "\',"
                        << "ptg_spacing: \'"<< ptg_spacing << "\',"
                        << "rgp_col_sp: "<< rgp_col_sp<< ","
                        << "rgp_row_sp: "<< rgp_row_sp << ","
                        << "rgp_hasDataC: '"<< rgp_hasDataC << "',"
                        << "rgp_query: '"<< rgp_query << "',"
                        << "rgp_name_bind: \'"<< rgp_name_bind << "\',"
                        << "rgp_value_bind: '"<< rgp_value_bind << "',"
                        << "rgp_value:'"<< rgp_value << "',"
                        << "rgp_no_cols: '"<< rgp_no_cols <<"',"
                        << "rgp_no_rows:'"<< rgp_no_rows << "',"
                        << "rgp_lout:'"<< rgp_lout <<"',"
						<< "data_type:'" << data_type <<"'";
		}
	}
	else
	{
   		gic_members = vpd->getAttrStr("rgp_members");
	
        code.mJson << ","
                   << "ptg_prompt_text: \'"<< ptg_prompt_text << "\',"
                   << "ptg_layout: \'"<< ptg_layout << "\',"
                   << "ptg_fgcolor:\'"<< ptg_fgcolor << "\',"
                   << "ptg_fsize: \'"<< ptg_fsize << "\',"
                   << "ptg_halign: \'"<< ptg_halign << "\',"
                   << "ptg_valign: \'"<< ptg_valign << "\',"
                   << "ptg_fstyle: \'"<< ptg_fstyle << "\',"
                   << "ptg_spacing: \'"<< ptg_spacing << "\',"
                   << "rgp_col_sp: "<< rgp_col_sp<< ","
                   << "rgp_row_sp: "<< rgp_row_sp << ","
                   << "rgp_hasDataC: '"<< rgp_hasDataC << "',"
                   << "rgp_query: '"<< rgp_query << "',"
                   << "rgp_name_bind: \'"<< rgp_name_bind << "\',"
                   << "rgp_value_bind: '"<< rgp_value_bind << "',"
                   << "rgp_value:'"<< rgp_value << "',"
                   << "rgp_no_cols: '"<< rgp_no_cols <<"',"
                   << "rgp_no_rows:'"<< rgp_no_rows << "',"
                   << "rgp_lout:'"<< rgp_lout <<"',"
                   << "rgp_members:'"<< gic_members <<"'";
	}
    return true;
}


