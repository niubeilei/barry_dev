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
//
// Modification History:
// 02/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RemoteBackupUtil_RemoteBkType_h
#define Aos_RemoteBackupUtil_RemoteBkType_h

#include "Util/String.h"

#define AOSREMOTEBACK_INVALID			"invalid"
#define AOSREMOTEBACK_CREATEDOC			"doc_created"
#define AOSREMOTEBACK_DELETEDOC			"doc_deleted"
#define AOSREMOTEBACK_MODIFYDOC			"doc_modified"
#define AOSREMOTEBACK_SAVEIILS			"iil_save"
#define AOSREMOTEBACK_ADDWORD			"iil_addword"

class AosRemoteBkType
{
public:
	enum E
	{
		eInvalid,

		eDocCreated,
		eDocDeleted,
		eDocModified,
		eIILs,
		eAddWord,

		eMax
	};

	static inline bool isValid(const E type)
	{
		return type > eInvalid && type < eMax;
	}

	static E toEnum(const OmnString &str);
	static OmnString toString(const E code);
	static bool addName(const OmnString &name, const E eid);
};
#endif



