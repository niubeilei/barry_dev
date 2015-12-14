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
// Modification History:
// 04/25/2014 Created by Young
////////////////////////////////////////////////////////////////////////////

#include "AosConf/DataFieldWithSubkeys.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/DataTypes.h"
#include <boost/make_shared.hpp>

using AosConf::DataFieldWithSubkeys;
using boost::make_shared;

DataFieldWithSubkeys::DataFieldWithSubkeys(const AosXmlTagPtr &xml)
{
	aos_assert(xml);
	vector<pair<OmnString ,OmnString> > v = xml->getAttributes();
	for (size_t i=0; i<v.size(); i++) 
	{
		setAttribute(v[i].first, v[i].second);
	}
	AosXmlTagPtr node = xml->getFirstChild(true);
	while (node)
	{
		setSubkeyDef(node->toString());
		node = xml->getNextChild();
	}
}


DataFieldWithSubkeys::DataFieldWithSubkeys()
{
}


DataFieldWithSubkeys::~DataFieldWithSubkeys()
{
}

		
string
DataFieldWithSubkeys::getConfig() 
{
	string conf = "<datafield ";
	for (map<string, string>::iterator itr=mAttrs.begin();
			itr!=mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";
	
	for (size_t i=0; i<mSubkeysDef.size(); i++)
	{
		conf += mSubkeysDef[i];	
	}

	conf += "</datafield>";

	return conf;
}


