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
#include "SEInterfaces/SchemaPickerObj.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"

AosSchemaPickerObj::AosSchemaPickerObj(const int version)
:
AosJimo(AosJimoType::eSchemaPicker, version)
{
}


AosSchemaPickerObj::~AosSchemaPickerObj()
{
}


AosSchemaPickerObjPtr
AosSchemaPickerObj::createSchemaPickerStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc)
{
	 AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc);
	 if (!jimo) return 0;

	 return dynamic_cast<AosSchemaPickerObj *>(jimo.getPtr());
}

