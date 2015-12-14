////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 11/07/2013 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskMgr_TaskErrorType_h
#define AOS_TaskMgr_TaskErrorType_h

#include "Util/String.h"

#define AOSTASKERRORTYPE_INVALID			"invalid"
#define AOSTASKERRORTYPE_OUTPUT_ERROR		"output_error"
#define AOSTASKERRORTYPE_INPUT_ERROR		"input_error"
#define AOSTASKERRORTYPE_CODE_ERROR			"code_error"
#define AOSTASKERRORTYPE_SERVER_ERROR		"server_error"
#define AOSTASKERRORTYPE_COREDUMP_ERROR		"coredump_error"
#define AOSTASKERRORTYPE_STORAGE_ERROR		"storage_error"

class AosTaskErrorType
{
public:
	enum E
	{
		eInvalid,

		eOutPutError,
		eInPutError, 
		eCodeError,
		eServerError,
		eCoreDumpError,
		eStorageError,

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}

	static E toEnum(const OmnString &str)
	{
		if (str == AOSTASKERRORTYPE_INVALID) return eInvalid;
		if (str == AOSTASKERRORTYPE_OUTPUT_ERROR) return eOutPutError;
		if (str == AOSTASKERRORTYPE_INPUT_ERROR) return eInPutError;
		if (str == AOSTASKERRORTYPE_CODE_ERROR) return eCodeError;
		if (str == AOSTASKERRORTYPE_SERVER_ERROR) return eServerError;
		if (str == AOSTASKERRORTYPE_COREDUMP_ERROR) return eCoreDumpError;
		if (str == AOSTASKERRORTYPE_STORAGE_ERROR) return eStorageError;
		return eInvalid;
	}

	static OmnString toStr(const E code)
	{
		switch (code)
		{
		case eOutPutError:		return AOSTASKERRORTYPE_OUTPUT_ERROR;
		case eInPutError:		return AOSTASKERRORTYPE_INPUT_ERROR;
		case eCodeError:		return AOSTASKERRORTYPE_CODE_ERROR;
		case eServerError:		return AOSTASKERRORTYPE_SERVER_ERROR;
		case eCoreDumpError:	return AOSTASKERRORTYPE_COREDUMP_ERROR;
		case eStorageError:		return AOSTASKERRORTYPE_STORAGE_ERROR;
		case eInvalid:			return AOSTASKERRORTYPE_INVALID;
		default:			break;
		}
		return AOSTASKERRORTYPE_INVALID;
	}
};

#endif
