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
// 07/07/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicQuestionAnswer.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


// static AosGicPtr sgGic = new AosGicQuestionAnswer();

AosGicQuestionAnswer::AosGicQuestionAnswer(const bool flag)
:
AosGic(AOSGIC_QUESTIONANSWER, AosGicType::eQuestionAnswer, flag)
{
}


AosGicQuestionAnswer::~AosGicQuestionAnswer()
{
}


bool	
AosGicQuestionAnswer::generateCode(
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
	OmnString gcn_overcolor = vpd->getAttrStr("gcn_overcolor", "transparent");
	OmnString gcn_downcolor = vpd->getAttrStr("gcn_downcolor", "transparent");
	OmnString gcn_bgcolor = vpd->getAttrStr("gcn_bgcolor", "transparent");
	OmnString gcn_owngics = vpd->getAttrStr("gcn_owngics", "true");
	OmnString gcn_valign = vpd->getAttrStr("gcn_valign", "top");
	OmnString gcn_halign = vpd->getAttrStr("gcn_halign", "left");
	OmnString gcn_dsize = vpd->getAttrStr("gcn_dsize", "true");
	OmnString gcn_cvpd = vpd->getAttrStr("gcn_cvpd", "null");
		
	OmnString gic_isflash = vpd->getAttrStr("gic_isflash", "false");
	OmnString gic_notown_mouevt = vpd->getAttrStr("gic_notown_mouevt", "false");
	//OmnString gic_layout = vpd->getAttrStr("gic_layout", "lm_freestyle");
	OmnString gic_layer = vpd->getAttrStr("gic_layer", "0");
	//layout mgr

	code.mJson <<",gic_layer:" << gic_layer
			<<",gic_isflash:" << gic_isflash 
			<<",gic_notown_mouevt:" << gic_notown_mouevt 
			<<",gcn_owngics:" << gcn_owngics 
			<<",gcn_dsize:" << gcn_dsize 
			<<",gcn_cvpd:\"" << gcn_cvpd << "\""
			<<",gcn_overcolor:\"" << gcn_overcolor << "\""
			<<",gcn_downcolor:\"" << gcn_downcolor << "\""
			<<",gcn_bgcolor:\"" << gcn_bgcolor << "\""
			<<",gic_qtype:\"" << vpd->getAttrStr("gic_qtype", "choice") << "\""
			<<",gic_rightanswer:\"" << vpd->getAttrStr("gic_rightanswer", "choice") << "\""
			<<",gic_score:" << vpd->getAttrStr("gic_score", "2")  
			<<",flag:" << "true"
			<<",isGroup: " << "true"
			<<",comps: ["; 
	AosXmlTagPtr thevpd = vpd->getFirstChild("gic_creators");
	if(thevpd)
	{
		AosXmlTagPtr nChild = thevpd->getFirstChild();
		//aos_assert_r(nChild,false);
		while (nChild)
		{
			//AosGic::createGic(htmlPtr, nChild, obj, parentid, code);
			AosGic::createGic(htmlPtr, nChild, obj, parentid, code, -1, -1);	//Ketty 2011/09/22
			nChild = thevpd->getNextChild();
			if (nChild){
				code.mJson<<",";
			}
		}
	}

	code.mJson << "]";

	return true;
}


