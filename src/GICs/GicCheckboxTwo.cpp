//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved. 
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
// Modification History:
// 29/08/2010: Created by Brian 
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicCheckboxTwo.h"

#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "HtmlUtil/HtmlUtil.h"
#include "Util/String.h"


AosGicCheckboxTwo::AosGicCheckboxTwo(const bool flag)
:
AosGic(AOSGIC_CHECKBOXTWO, AosGicType::eCheckBoxTwo, flag)
{
}


AosGicCheckboxTwo::~AosGicCheckboxTwo()
{
}


bool	
AosGicCheckboxTwo::generateCode(
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
    OmnString cbg_hasDataC = vpd->getAttrStr("cbg_hasDataC","true");
    OmnString chb_value_bind = vpd->getAttrStr("chb_value_bind");
    OmnString chb_name_bind = vpd->getAttrStr("chb_name_bind");
    OmnString src = vpd->getAttrStr("src");
    OmnString gic_column = vpd->getAttrStr("gic_column");
    OmnString gic_ncolumn = vpd->getAttrStr("gic_ncolumn");
	OmnString gic_bgcolor = vpd->getAttrStr("gic_bgcolor");
    OmnString gic_mbwidth = vpd->getAttrStr("gic_mbwidth");
    OmnString gic_nmbwidth = vpd->getAttrStr("gic_nmbwidth");

    OmnString ptg_prompt_text = vpd->getAttrStr("ptg_prompt_text");
    OmnString ptg_fsize = vpd->getAttrStr("ptg_fsize");
    OmnString ptg_fstyle = vpd->getAttrStr("ptg_fstyle");
    OmnString ptg_fgcolor = vpd->getAttrStr("ptg_fgcolor");
	if(gic_bgcolor != "")
	{
	    gic_bgcolor = AosHtmlUtil::getWebColor(gic_bgcolor) ;
	}
	if(ptg_fgcolor != "")
	{
	    ptg_fgcolor = AosHtmlUtil::getWebColor(ptg_fgcolor) ;
	}
	OmnString html = "";
	
	if(cbg_hasDataC == "true")
	{
		AosXmlTagPtr content_vpd = vpd->getFirstChild("datacol");
		OmnString gic_type = content_vpd->getAttrStr("gic_type");
		
		if(gic_type == "str_datacol")
		{
			code.mJson << ","
                       << "cbg_hasDataC: \'"<< cbg_hasDataC << "\'," 
                       << "gic_type: \'"<< gic_type <<"\',"
                       << "chb_value_bind: \'"<< chb_value_bind << "\'," 
                       << "chb_name_bind: \'"<< chb_name_bind << "\'," 
                       << "src: \'"<< src << "\'," 
                       << "gic_column: \'"<< gic_column << "\'," 
                       << "gic_ncolumn: \'"<< gic_ncolumn << "\'," 
                       << "gic_bgcolor: \'"<< gic_bgcolor << "\'," 
                       << "ptg_prompt_text: \'"<< ptg_prompt_text << "\'," 
                       << "ptg_fgcolor: \'"<< ptg_fgcolor << "\'," 
                       << "ptg_fsize: \'"<< ptg_fsize << "\'," 
                       << "ptg_fstyle: \'"<< ptg_fstyle << "\'," 
                       << "gic_mbwidth: \'"<< gic_mbwidth << "\'," 
                       << "gic_nmbwidth: \'"<< gic_nmbwidth << "\'"; 
      }
	  else if(gic_type == "db_datacol")
	  {
		   code.mJson << ","
                       << "cbg_hasDataC: \'"<< cbg_hasDataC << "\'," 
                       << "gic_type: \'"<< gic_type <<"\',"
                       << "chb_value_bind: \'"<< chb_value_bind << "\'," 
                       << "chb_name_bind: \'"<< chb_name_bind << "\'," 
                       << "src: \'"<< src << "\'," 
                       << "gic_column: \'"<< gic_column << "\'," 
                       << "gic_ncolumn: \'"<< gic_ncolumn << "\'," 
                       << "gic_bgcolor: \'"<< gic_bgcolor << "\'," 
                       << "ptg_prompt_text: \'"<< ptg_prompt_text << "\'," 
                       << "ptg_fgcolor: \'"<< ptg_fgcolor << "\'," 
                       << "ptg_fsize: \'"<< ptg_fsize << "\'," 
                       << "ptg_fstyle: \'"<< ptg_fstyle << "\'," 
                       << "gic_mbwidth: \'"<< gic_mbwidth << "\'," 
                       << "gic_nmbwidth: \'"<< gic_nmbwidth << "\'"; 
                   
	  }
	}
    return true;

}


