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
// vpd:
// <repeater itemwidth="xxxx" itemheight="xxxx" tplname="xxxx">
//  <actions>
//  	<action ..../>
//  	<action ..../>
//  	...
//  </actions>
// </repeater>
//
//
//   
//
// Modification History:
// 05/11/2012: Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlRepeater.h"

#include "HtmlLayoutMgr/Layout.h"
#include "HtmlLayoutMgr/AllLayout.h"
#include "DbQuery/Query.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

const int sgContainerMemberPsize = 200;

AosGicHtmlRepeater::AosGicHtmlRepeater(const bool flag)
:
AosGic(AOSGIC_HTMLREPEATER, AosGicType::eHtmlRepeater, flag),
mItemHeight(0),
mColor0(""),
mColor1(""),
mTplName("")
{
}


AosGicHtmlRepeater::~AosGicHtmlRepeater()
{
}


bool	
AosGicHtmlRepeater::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	mItemHeight = vpd->getAttrInt("gic_itemheight", 0);
	mColor0 = vpd->getAttrStr("gic_color0", "transparent");
	mColor1 = vpd->getAttrStr("gic_color1", "transparent");
	mTplName = vpd->getAttrStr("gic_tplname", "");
	aos_assert_r(mTplName != "", false);
	
	code.mJson << ",gic_itemheight: " << mItemHeight
			   << ",gic_color0: \"" << mColor0 << "\""
			   << ",gic_color1: \"" << mColor1 << "\""
			   << ",gic_tplname: \"" << mTplName << "\"";

	AosXmlTagPtr datacol = vpd->getFirstChild("datacol");
	aos_assert_r(datacol, false);
	int pagesize = datacol->getAttrInt("cmp_psize", 20);
	OmnString htmlcode = "";
	OmnString tpljsons = "[";
	OmnString tplhtmls = "[";
	OmnString tplcsss = "[";
	OmnString tplvpds = "[";
	for(int i=0; i<pagesize; i++)
	{
		AosXmlTagPtr tplvpd, vpdroot, tplobj;
		AosHtmlCode vpdCode;
		bool rs = htmlPtr->getVpd(mTplName, tplvpd, vpdroot);
		if (rs)
		{
			AosLayout::createLayoutVpd(htmlPtr, tplvpd, tplobj, 
					parentid, vpdCode, 200, 200);
			tpljsons << vpdCode.mJson << ",";
			tplhtmls << "\'" << vpdCode.mHtml << "\',";
			tplcsss << "\'" <<vpdCode.mCss << "\',";
			tplvpds << "\'" << tplvpd->toString() << "\',";
		}

		if (i%2 == 0)
		{
			htmlcode << "<div style=\"background: " << mColor0 << "\"></div>";
		}
		else
		{
			htmlcode << "<div style=\"background: " << mColor1 << "\"></div>";
		}
	}
	code.mHtml << htmlcode;
	code.mJson << ",mTplJsons: " << tpljsons << "]"
		<< ",mTplHtmls: " << tplhtmls << "]"
		<< ",mTplCsss: " << tplcsss << "]"
		<< ",mTplVpds: " << tplvpds << "]";


	AosDataColPtr dcl = AosDataCol::getInstance(vpd);
	if (dcl)
	{
		AosXmlTagPtr doc = dcl->retrieveData(htmlPtr, vpd, obj, "");
		if (doc)
		{
			AosXmlTagPtr datas = doc->getFirstChild("Contents");
			if (datas)
			{
				code.mJson << ",gic_data: '" << datas->toString() << "'";
			}
		}
	}

	return true;
}
