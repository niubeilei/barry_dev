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
// Schemas are AosDataRecord. Schemas are used to interpret data. Almost
// all data (except XML and JSON) need schemas. Schemas are defined by 
// docs.
//
// Modification History:
// 2013/12/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Schema/SchemaMgr.h"

#include "API/AosApi.h"

AosSchemaMgr::AosSchemaMgr()
{
}


AosSchemaMgr::~AosSchemaMgr()
{
}


u64
AosSchemaMgr::getSchemaDocid(AosRundata *rdata, const OmnString &type)
{
	// Schemas are identified by types. 
	if (type == "")
	{
		AosSetErrorUser(rdata, "schemamgr_type_empty") << enderr;
		return 0;
	}

	OmnString objid = AosObjid::composeSchemaObjid(type);
	aos_assert_rr(objid != "", rdata, 0);
	AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
	if (!doc)
	{
		AosSetEntityError(rdata, "schemamgr_schema_not_found", 
				"Schema Type", type) << enderr;
		return 0;
	}

	u64 docid = doc->getAttrU64("current_schema_id", 0);
	if (docid > 0) return docid;
	AosSetEntityError(rdata, "schemamgr_missing_schema_id", 
			"Schema Type", type) << enderr;
	return 0;
}

