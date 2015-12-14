////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Schema/SchemaDummy.h"


AosSchemaDummy::AosSchemaDummy(const int version)
:
AosSchema(AOSSCHEMA_DUMMY, version)
{
}


AosSchemaDummy::~AosSchemaDummy()
{
}

	
bool
AosSchemaDummy::appendData(
		AosRundata *rdata, 
		const AosBuffDataPtr &buffdata)
{
	return true;
}	
	
bool
AosSchemaDummy::getNextRecordset(
		AosRundata *rdata, 
		AosRecordsetObjPtr &recordset, 
		AosDatasetObj *dataset,
		bool &contents_incomplete)
{
	return true;	
}

