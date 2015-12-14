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
#include "HtmlLayoutMgr/AutoLayout.h"

#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosAutoLayout::AosAutoLayout()
{
	AosLayoutPtr ptr(this, false);
	registerLayout(eAosHtmlLayoutType_auto, ptr);
}


AosAutoLayout::~AosAutoLayout()
{
}


bool	
AosAutoLayout::generateLayoutCode(
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
	aos_assert_r(vpd, false);

	OmnString temp;
	temp << "xtype:'aos_npane', ";
	temp << "mlLayoutType:'lm_npane', "; 
	OmnString vpdname = vpd->getAttrStr("panel_vpd_name", "");
	OmnString objname = vpd->getAttrStr("panel_obj_name", "");
	bool status = false;

	if (vpdname != "")
	{
		status = htmlPtr->checkVpdPath(vpdname);
		if(!status)
		{
			vpd->removeAttr("panel_vpd_name");
		}
		else
		{
			temp << "vpdname:'" << vpdname << "', ";
			AosXmlTagPtr embedVpd, embedVpdroot, embedObj, embedObjroot;
			status = htmlPtr->getVpd(vpdname, embedVpd, embedVpdroot);
			if(status)
			{
				temp << "vpdLoaded:true, ";

				if(objname != "")
				{
					bool rest = htmlPtr->getVpd(objname, embedObj, embedObjroot);
					if(!rest)
					{
						OmnAlarm << "faile to get the obj,objname:" << objname << enderr;
					}
				}
				OmnString vpdPath = htmlPtr->createVpdPath(vpdname);
			
				OmnString flag = htmlPtr->getInsertFlag();
				htmlPtr->appendVpdArray(embedVpd, obj, parentId, flag, vpdPath, parentWidth, parentHeight);
				code.mHtml << flag; 
				code.mJson << flag;
			}
		}
	}

	code.mJson.insert(temp, 1);
	if(!status)
	{
		AosXmlTagPtr child = vpd->getFirstChild("panel");
		if (child)
		{
			htmlPtr->createPane(code, child, obj, parentId, parentWidth, parentHeight);
		}
	}

	return true;
}
