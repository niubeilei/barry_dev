////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
// Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_GroupDocOpr_h
#define Aos_StorageEngine_GroupDocOpr_h

#include "Util/String.h"

//#define AOSTAG_GROUPDOC_INSERT						"insert"
#define AOSTAG_GROUPDOC_BATCHINSERT						"batch_insert"
//#define AOSTAG_GROUPDOC_DELETE						"delete"
#define AOSTAG_GROUPDOC_BATCHDELETE						"batch_delete"
#define AOSTAG_GROUPDOC_UPDATE							"update"
#define AOSTAG_GROUPDOC_INVALID							"invalid"

class AosGroupDocOpr
{
public:
	enum E
	{
		eInvalid,
		eBatchInsert,
		//eInsert,
		//eDelete,
		eBatchDelete,
		eUpdate,

		eMax
	};

	static bool isValid(const E type)
	{
		return type > eInvalid && type < eMax; 
	}
	static E toEnum(const OmnString &str)
	{
		//if (str == AOSTAG_GROUPDOC_INSERT) return eInsert;
		//if (str == AOSTAG_GROUPDOC_DELETE) return eDelete;
		if (str == AOSTAG_GROUPDOC_UPDATE) return eUpdate;
		else if (str == AOSTAG_GROUPDOC_BATCHINSERT) return eBatchInsert;
		else if (str == AOSTAG_GROUPDOC_BATCHDELETE) return eBatchDelete;
		else return eInvalid;
	}
	static OmnString toStr(const E type)
	{
		//if (type == eInsert) return AOSTAG_GROUPDOC_INSERT;
		//if (type == eDelete) return AOSTAG_GROUPDOC_DELETE;
		if (type == eUpdate) return AOSTAG_GROUPDOC_UPDATE;
		else if (type == eBatchInsert) return AOSTAG_GROUPDOC_BATCHINSERT;
		else if (type == eBatchDelete) return AOSTAG_GROUPDOC_BATCHDELETE;
		else return AOSTAG_GROUPDOC_INVALID;
	}
};
#endif

