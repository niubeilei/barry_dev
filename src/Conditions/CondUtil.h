////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 12/02/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conditions_CondUtil_h
#define Aos_Conditions_CondUtil_h

#include "Conditions/Ptrs.h"
#include <list>
#include <string>

typedef std::list<AosConditionPtr>	AosConditionList;

class AosConditionType
{
public:
	enum E 
	{
		eUnknown = 0,
		eInField,
		eNotInField,
		eEqual,
		eNotEqual,
		eGreater,
		eGreaterEqual,
		eLess,
		eLessEqual,

		eLastEntry
	};
	static std::string enum2Str(const AosConditionType::E type);
	static AosConditionType::E str2Enum(const std::string& name);
};


class AosCondDataType
{
public:
	enum E 
	{
		eUnknown = 0,
		eConstant,
		eVariable,
		eParmValue,
		eTableCell,
		eTable,

		eLastEntry
	};
	static std::string enum2Str(const AosCondDataType::E type);
	static AosCondDataType::E str2Enum(const std::string& name);
};

#endif

