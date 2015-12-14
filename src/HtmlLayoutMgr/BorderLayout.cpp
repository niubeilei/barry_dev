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
// 07/16/2010: Created by lynch yang
////////////////////////////////////////////////////////////////////////////
#include "HtmlLayoutMgr/BorderLayout.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

//static AosLayoutPtr sgGic = OmnNew AosBorderLayout();

AosBorderLayout::AosBorderLayout()
{
	AosLayoutPtr ptr(this, false);
	registerLayout(eAosHtmlLayoutType_border, ptr);
}


AosBorderLayout::~AosBorderLayout()
{
}


bool	
AosBorderLayout::generateLayoutCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		AosXmlTagPtr &obj,
		const OmnString parentId,
		AosHtmlCode &code,
		const int parentWidth,
		const int parentHeight)
{
	// This function will generate:
	// 	1. HTML code
	// 	2. CSS code
	// 	3. JavaScript Code
	// 	4. Flash code
	OmnString temp = "layout:'border', xtype:'aos_cpane', ";
	code.mJson.insert(temp, 1);
	
	AosXmlTagPtr panes = vpd->getFirstChild("panes");
	AosXmlTagPtr child, parent;
	if (panes)
	{
		parent = panes;
	}
	else
	{
		parent = vpd;
	}
	
	bool hasCenter = false;
	int i = 0;
	OmnString region, regionStr;
	AosHtmlCode regionCode;
	child = parent->getFirstChild("pane");
	while (child && i < 5)
	{
		regionStr = "";
		region = child->getAttrStr("ctn_region");
		if ( region == "north" ||
			 region == "south" ||
			 region == "west" ||
			 region == "east" ||
			 region == "center" )
		{
			if (region == "center")
				hasCenter = true;
			if (i != 0)
				code.mJson << ",";
			htmlPtr->createPane(regionCode, child, obj, parentId, parentWidth, parentHeight);
			regionStr << "region:'" << region << "', "; 
			regionCode.mJson.insert(regionStr, 1);
			code.append(regionCode);
			regionCode.reset();
		}
		child = parent->getNextChild("pane");
		i++;
	}
	return true;
}
