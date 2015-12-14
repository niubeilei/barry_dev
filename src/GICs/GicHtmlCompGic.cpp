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
#include "GICs/GicHtmlCompGic.h"
#include "HtmlUtil/HtmlUtil.h"
#include "HtmlServer/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

AosGicHtmlCompGic::AosGicHtmlCompGic(const bool flag)
:
AosGic(AOSGIC_HTMLCOMPGIC, AosGicType::eHtmlCompGic, flag)
{
}


AosGicHtmlCompGic::~AosGicHtmlCompGic()
{
}


bool	
AosGicHtmlCompGic::generateCode(
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
	aos_assert_r(vpd, false);
	
	AosXmlTagPtr columns = vpd->getFirstChild("columns");
	aos_assert_r(columns, false);
	
	typedef map<OmnString, OmnString> mStrMap;
	typedef map<OmnString, OmnString>::iterator mStrMapItr;

	mStrMap mMap;
	mStrMapItr itr;
	OmnString mGicsStr, name, vpdname;
	AosXmlTagPtr column = columns->getFirstChild();
	while(column)
	{
		name = column->getAttrStr("name");
		vpdname = column->getAttrStr("vpdname");
		column = columns->getNextChild();
		if(name != "" && vpdname != "")
		{
			mMap.insert(make_pair(name, vpdname));
			mGicsStr << name << ":\"" << vpdname << "\"";
			if(column) mGicsStr << ", ";
		}
	}
	
	OmnString gic_bind = vpd->getAttrStr("gic_bind");
	
	OmnString str = ", ";
	str << "gic_bind:\"" << gic_bind << "\", "
		<< "mGicNames:{" << mGicsStr << "} ";
	
	code.mJson << str;
	return true;
}

