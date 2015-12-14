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
// 05/11/2012: Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicHtmlProgressBar.h"

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

AosGicHtmlProgressBar::AosGicHtmlProgressBar(const bool flag)
:
AosGic(AOSGIC_HTMLPROGRESSBAR, AosGicType::eHtmlProgressBar, flag)
{
}


AosGicHtmlProgressBar::~AosGicHtmlProgressBar()
{
}


bool	
AosGicHtmlProgressBar::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	code.mJson << ",gic_bartype: \"" << vpd->getAttrStr("gic_bartype", "default") << "\""
			   << ",gic_unint: \"" << vpd->getAttrStr("gic_unint", "percent") << "\""
			   << ",gic_imgsrc: \"" << vpd->getAttrStr("gic_imgsrc", "") << "\""
			   << ",gic_valuebd: \"" << vpd->getAttrStr("gic_valuebd", "") << "\""
			   << ",gic_bstyle: \"" << vpd->getAttrStr("gic_bstyle", "1px solid #000000") << "\""
			   << ",gic_color1: \"" << vpd->getAttrStr("gic_color1", "#ff0000") << "\""
			   << ",gic_color2: \"" << vpd->getAttrStr("gic_color2", "#ffffff") << "\"";
	return true;
}
