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
// 07/20/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HtmlModules/DclStr.h"

#include "HtmlUtil/HtmlUtil.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/String.h"
#include "Util/StrParser.h"

static OmnString sgDefaultDatacolName = "datacol";

AosDclStr::AosDclStr()
{
}


OmnString
AosDclStr::getJsonConfig(const AosXmlTagPtr &vpd)
{
	return "";
}


AosXmlTagPtr 
AosDclStr::retrieveData(
		const u32 siteid,
		const AosXmlTagPtr &vpd,		//where the vpd from? 
		const AosXmlTagPtr &obj,
		const OmnString &ssid,
		const OmnString &tagname)
{
	// This function retrieves the data based on 'vpd'.
	// 'vpd' should be in the format: 
	// 	<...>
	// 		...
	// 		<datacol  ...>
	// 		</datacol>
	// 		...
	// 	</...>
	//
	// where '<datacol ...>' defines the data collector. The name of 
	// this subtag may be specified by the parameter 'tagname'. If 
	// 'tagname' is empty, it defaults to sgDefaultDatacolName. 
	//
	// The function returns an XML tag with the following
	// format:
	// 	<records>
	// 		<record .../>
	// 		<record .../>
	// 		...
	// 	</records>
	aos_assert_r(vpd, 0);

	OmnString tname = tagname;
	if (tname == "") tname = sgDefaultDatacolName;
	AosXmlTagPtr datacol_vpd = vpd->getFirstChild(tname);
	if (!datacol_vpd)
	{
		return 0;
	}

	OmnString contents = datacol_vpd->getAttrStr("cmp_contents","");
	OmnString anames = datacol_vpd->getAttrStr("cmp_anames","");
	OmnString attr_sep = datacol_vpd->getAttrStr("cmp_attr_sep", "|^|");
	OmnString filed= datacol_vpd->getAttrStr("cmp_field_sep", "|$|");
	OmnString entry_sep = datacol_vpd->getAttrStr("cmp_entry_sep", "|$$|");

	AosStrSplit entry_parser(contents, entry_sep.data());
	OmnString entry;
	OmnString results = "<response><Contents>";
	OmnStrParser1Ptr aname_parser;
	while ((entry = entry_parser.nextWord()) != "")
	{
		results << "<record ";
		AosStrSplit field_parser(entry, filed.data());
		OmnString field;
		if(anames != "")
		{
			aname_parser = OmnNew OmnStrParser1(anames, ",", false ,false);
		}
		while ((field = field_parser.nextWord())!= "")
		{
			OmnString aname="";
			if (aname_parser)
			{
				if((aname= aname_parser->nextWord())!= "")
				{
					results << aname << "=\"" << field <<"\" ";
				}
			}
			else
			{
				AosStrSplit attr_parser(field, attr_sep.data());
				OmnString lhs = attr_parser.nextWord();
				OmnString rhs = attr_parser.nextWord();
				results << lhs << "=\"" << rhs <<"\" ";
			}
		}
		results << "/>";
	}

	results << "</Contents></response>";
	AosXmlParser xmlparser;
	AosXmlTagPtr xml = xmlparser.parse(results, "" AosMemoryCheckerArgs);
	return xml;
}

