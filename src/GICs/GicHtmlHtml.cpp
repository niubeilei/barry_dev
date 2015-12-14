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
#include "GICs/GicHtmlHtml.h"

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "HtmlModules/DictionaryHandler.h"

AosGicHtmlHtml::AosGicHtmlHtml(const bool flag)
:
AosGic(AOSGIC_HTMLHTML, AosGicType::eHtmlHtml, flag)
{
}


AosGicHtmlHtml::~AosGicHtmlHtml()
{
}


bool	
AosGicHtmlHtml::generateCode(
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
	//convertToJson(vpd,code.mJson);
	//test
	AosXmlTagPtr vpdConts = vpd->getFirstChild("contents");
	OmnString cont = "";
	if(vpdConts)
	{
		cont = vpdConts->getNodeText();
	}

	//lynch
	//mutil-language
	OmnString langtype = htmlPtr->getLanguageType(); 
	OmnString langcode = AosDictionaryHandler::matchLangType(langtype);
	if(langcode != "")
	{
		OmnString label = AosDictionaryHandler::getDictionary("contents/_$text", vpd, langcode, htmlPtr->getRundata());
		if(label != "")
		{
			cont= label;
		}
	}
    //int findHandle = cont.findSubString("gAosHtmlHandler",0);
	//if(findHandle != -1)
	//{
	//	htmlPtr->addGic("gAosHtmlHandler");//js file id;
	//}
	int f1 = cont.findSubString("|&&|", 0);
	int f2 = cont.findSubString("|&|", 0);
	if(f1 != -1)
	{
		cont.replace("|&&|", "\"", true);
	}
	if(f2 != -1){
		cont.replace("|&|", "\'", true);
	}
	if(!htmlPtr->isHomePage())
	{
		bool t = true;
		cont.replace("\'", "\\'", t);
	}

	//felicia,2011/09/06, for border
	OmnStrParser1 parser("");
	parser.convertBorder(cont, "IE");

	code.mHtml << cont;
	
	OmnString gic_cursor = vpd->getAttrStr("gic_cursor", "default");
	code.mJson << ",gic_cursor:\""<<gic_cursor<<"\"" ;
	return true;
}

