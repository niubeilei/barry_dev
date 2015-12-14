////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
//
// Modification History:
// 07/04/2011: Created by Michael Yang
//
////////////////////////////////////////////////////////////////////////////
#include "GICs/GicMsg.h"

#include "HtmlModules/Ptrs.h"
#include "HtmlModules/DclDb.h"
#include "HtmlUtil/HtmlUtil.h"
#include "SEUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


AosGicMsg::AosGicMsg(const bool flag)
:
AosGic(AOSGIC_MSG, AosGicType::eMsg, flag)
{
}


AosGicMsg::~AosGicMsg()
{
}


bool	
AosGicMsg::generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code)
{
	return true;
}

