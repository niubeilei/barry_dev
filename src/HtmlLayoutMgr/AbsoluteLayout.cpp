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
#include "HtmlLayoutMgr/AbsoluteLayout.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosAbsoluteLayout::AosAbsoluteLayout()
{
	AosLayoutPtr ptr(this, false);
	registerLayout(eAosHtmlLayoutType_absolute, ptr);
	registerLayout(eAosHtmlLayoutType_lm_freestyle, ptr);
	registerLayout(eAosHtmlLayoutType_ad_vert, ptr);
}


AosAbsoluteLayout::~AosAbsoluteLayout()
{
}


bool	
AosAbsoluteLayout::generateLayoutCode(
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
	//OmnString temp = "xtype:'aos_panel', layout:'absolute',";
	OmnString panel_type = vpd->getAttrStr("panel_type",vpd->getAttrStr("container_type"));
	OmnString temp = "mlLayoutType:'lm_fixed', layout:'absolute', xtype:'";
	temp << (panel_type == "xmleditor_panel" ? "aos_xmleditor_panel" : "aos_panel") << "', ";
	code.mJson.insert(temp, 1);
	htmlPtr->createGic(code, vpd, obj, parentId, parentWidth, parentHeight);
	return true;
}
