////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FormatedFile.h
// Description:
//	A formated file simulates a SQL table. Such a file contains a list
//  of entries. Each entry is one line. Entry has format, which is 
//  defined by OmnSchema.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataServer_FormatedFile_h
#define Omn_DataServer_FormatedFile_h


#include "DataServer/DataStore.h"


class OmnFormatedFile : public OmnDataStore
{
private:
	OmnTableSchemaPtr		mSchema;

public:
	OmnFormatedFile(const OmnTableSchemaPtr &schema);
	~OmnFormatedFile();
};
#endif
