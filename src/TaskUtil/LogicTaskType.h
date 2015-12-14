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
#ifndef Aos_JobAndTask_LogicTaskType_h
#define Aos_JobAndTask_LogicTaskType_h

#include "Util/String.h"

#define AOSTAG_LOGICTASKTYPE_VER1						"ver1"
#define AOSTAG_LOGICTASKTYPE_MAP						"map"
#define AOSTAG_LOGICTASKTYPE_REDUCE						"reduce"
#define AOSTAG_LOGICTASKTYPE_INVALID					"invalid"

class AosLogicTaskType
{
public:
	enum E
	{
		eInvalid,
		eVer1,
		eMap,
		eReduce,

		eMax
	};

	static bool isValid(const E type)
	{
		return type > eInvalid && type < eMax; 
	}
	static E toEnum(const OmnString &str)
	{
		if (str == AOSTAG_LOGICTASKTYPE_VER1) return eVer1;
		else if (str == AOSTAG_LOGICTASKTYPE_MAP) return eMap;
		else if (str == AOSTAG_LOGICTASKTYPE_REDUCE) return eReduce;
		else return eInvalid;
	}
	static OmnString toStr(const E type)
	{
		if (type == eVer1) return AOSTAG_LOGICTASKTYPE_VER1;
		else if (type == eMap) return AOSTAG_LOGICTASKTYPE_MAP;
		else if (type == eReduce) return AOSTAG_LOGICTASKTYPE_REDUCE;
		else return AOSTAG_LOGICTASKTYPE_INVALID;
	}
};
#endif

