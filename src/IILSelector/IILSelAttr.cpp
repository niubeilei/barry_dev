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
// 08/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "IILSelector/IILSelAttr.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"



AosIILSelAttr::AosIILSelAttr(const bool regflag)
:
AosIILSelector(AOIILSELNAME_ATTR, AosIILSelectorId::eAttr, regflag)
{
}


OmnString 
AosIILSelAttr::getIILName(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// The IIL selector format is:
	// 	<selector type=AOIILSELNAME_ATTR
	// 		container="xxx"
	// 		attrname="xxx"/>
	// where 'container' is optional. If specified, it uses the container's 
	// attribute IIL. Otherwise, it uses the global attribute IIL.
	aos_assert_rr(def, rdata, "");

	OmnString ctnr_objid = def->getAttrStr("container");
	OmnString aname = def->getAttrStr("attrname");
	mIILName = AosIILName::composeAttrIILName(ctnr_objid, aname); 
	return true;
}
#endif
