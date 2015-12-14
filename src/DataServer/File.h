////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: File.h
// Description:
//	It simulates a normal file.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataServer_File_h
#define Omn_DataServer_File_h


#include "DataServer/DataStore.h"


class OmnFile : public OmnDataStore
{
private:
	OmnTableSchemaPtr		mSchema;

public:
	OmnFile(const OmnTableSchemaPtr &schema);
	~OmnFile();
};
#endif
