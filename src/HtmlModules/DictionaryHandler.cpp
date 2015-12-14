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
// 07/16/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HtmlModules/DictionaryHandler.h"

#include "HtmlUtil/HtmlRc.h"
#include "HtmlUtil/HtmlUtil.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/String.h"
#include "Dictionary/DictionaryMgr.h"

AosDictionaryHandler::AosDictionaryHandler()
{
}

const OmnString
AosDictionaryHandler::getDictionary(
		const OmnString &attr, 
		const AosXmlTagPtr &gic_vpd,  
		const OmnString &langcode, 
		const AosRundataPtr &rdata)
{
	// Each GIC has a subtag that contains all the dictionary entries. 
	// The tag name is "dictionaryobjs", which is in the format:
	// 	<gic ...>
	// 		<dictionaryobjs>
	// 			<entry dictattrbd="xxx" dictid="xxx"/>
	// 			<entry dictattrbd="xxx" dictid="xxx"/>
	//			...
	//		</dictionaryobjs>
	//	</gic>
	// This function checks whether the <dictionaryobjs> contains the
	// entry whose 'dictattrbd' value is 'attr'. If yes, it uses the
	// dictionary to translate the attribute.
	OmnString dictId = "";
	AosXmlTagPtr dictobjs = gic_vpd->getFirstChild("dictionaryobjs");
	if(!dictobjs)
	{
		return "";
	}
	AosXmlTagPtr child = dictobjs->getFirstChild();
	while(child)
	{
		OmnString dictAttr = child->getAttrStr("dictattrbd");
		if(attr == dictAttr)
		{
			dictId = child->getAttrStr("dictid");
			break;
		}
		child = dictobjs->getNextChild();
	}

/*	int position1 = initalStr.findSubString("@{",0);
	int position2 = initalStr.findSubString("}@",position1);
	if(position1 != -1 && position2 !=-1)
	{
		dictId = initalStr.substr(position1+2,position2-1);
		OmnScreen << "dictId:" << dictId << endl;
	}
*/	if(dictId != "")
	{
		int dictid = atoi(dictId.data());
		return AosDictionaryMgr::getSelf()->resolveTerm(dictid, langcode, rdata);
	}
	return "";
}

const OmnString 
AosDictionaryHandler::matchLangType(const OmnString &language)
{
	if (language == "")
	{
		return NULL;
	}
	OmnString langtype;
	if(language == "Chinese")
	{
		langtype = "zh-cn";
		return langtype;
	}
	else if(language == "English")
	{
		langtype = "en";
		return langtype;
	}
	else if(language == "France")
	{
		langtype = "fr";
		return langtype;
	}
	else if(language == "Germany")
	{
		langtype = "de";
		return langtype;
	}
	return NULL;

}

