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
// The super class for all semantics data. 
//
// Modification History:
// 11/27/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticData_Types_h
#define Aos_SemanticData_Types_h

#include "aosUtil/Types.h"
#include <string>


class AosTimeMeasure
{
public:
	enum E
	{
		eInvalid, 
		eOffsetOfCrtTime,
		eLastInstCreated,
		eLastInstDeleted,
		eLastInstRead,
		eLastInstModified,
		eLastReset,
		eLastMarker,
		eLastCounter,
		eLastPat
	};

	static std::string	toStr(const AosTimeMeasure::E code);
	static AosTimeMeasure::E toEnum(const std::string &name);
	static std::string	getErrMsg(const u32, const u32, 
							const AosTimeMeasure::E);
};

class AosVarType
{
public:
	enum E
	{
		eBool,
		eInt8,
		eInt16,
		eInt32,
		eInt64,
		eUint8,
		eUint16,
		eUint32,
		eUint64,
		eString,
		eFloat,
		eDouble,
		eContainer
	};

	static std::string	toStr(const AosVarType::E code);
	static AosVarType::E toEnum(const std::string &name);
};



#endif

