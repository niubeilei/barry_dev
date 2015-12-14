////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/SchemaObj.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"

AosSchemaObj::AosSchemaObj(const int version)
:
AosJimo(AosJimoType::eSchema, version)
{
}


AosSchemaObj::~AosSchemaObj()
{
}

AosSchemaObjPtr
AosSchemaObj::createSchemaStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc)
{
	if (!worker_doc)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc);
	if (!jimo) return 0;
	if (jimo->getJimoType() != AosJimoType::eSchema)
	{
		AosSetErrorUser3(rdata, "syntax_error", "schemaobj_not_schema") 
			<< worker_doc << enderr;
		return 0;
	}

	AosSchemaObjPtr schema = dynamic_cast<AosSchemaObj*>(jimo.getPtr());
	if (!schema)
	{
		AosSetError(rdata, "internal_error") << worker_doc << enderr;
		return 0;
	}

	return schema;
}

