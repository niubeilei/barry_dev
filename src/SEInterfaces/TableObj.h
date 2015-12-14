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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_TableObj_h
#define Aos_SEInterfaces_TableObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"


class AosTableObj : virtual public AosJimo
{
public:
	virtual OmnString getTablename() const = 0;
	virtual bool dropTable(AosRundata *rdata) = 0;

	virtual AosDataFieldObjPtr getField(
						AosRundata *rdata, 
						const char *field_name) = 0;

	virtual bool alterTable(
						AosRundata *rdata, 
						const AosXmlTagPtr &new_def) = 0;

	virtual bool getSchema(
						AosRundata *rdata, 
						const char *schema_name) = 0;

	virtual bool fieldExist(AosRundata *rdata, const char *field_name) = 0;

};
#endif

