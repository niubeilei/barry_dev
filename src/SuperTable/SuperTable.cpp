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
// 2013/10/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SuperTable/SuperTable.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosSuperTable::AosSuperTable(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &doc)
{
	if (doc)
	{
		mObjid = doc->getAttrStr(AOSTAG_OBJID, "");
		if (mObjid == "")
		{
			AosSetErrorUser(rdata, "invalid_super_table_doc") << doc->toString() << enderr;
			OmnThrowException("invald_super_table_doc");
		}
	}
}


AosSuperTable::~AosSuperTable()
{
}


AosXmlTagPtr
AosSuperTable::retrieveTableDoc(const AosRundataPtr &rdata)
{
	aos_assert_rr(mObjid != "", rdata, 0);
	AosXmlTagPtr doc = AosGetDocByObjid(mObjid, rdata);
	if (!doc)
	{
		if (rdata->getArg1(AOSARG_ACCESS_DENIED) == "true") return 0;
		AosSetErrorUser(rdata, "super_table_not_defined") << mObjid << enderr;
		return 0;
	}
	return doc;
}


