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
// 03/14/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_RandomValueGen_Util_h
#define Omn_RandomValueGen_Util_h

#include "Util/String.h"

class AosCmdKeyType
{
public:
	enum E 
	{
		eUnknown,
		eRead, 
		eModify, 
		eNewOnly, 
		eNewModify, 
		eNoKey,			

		eLastEntry
	};
	static OmnString 		enum2Str(const AosCmdKeyType type);
	static AosCmdKeyType::E str2Enum(const OmnString &name);
};


class AosSelectFromObj
{
public:
	enum E 
	{
		eUnknown,
		eNot,
		eKeyOnly,
		eUniqueOnly,
		eRandom,
		eAll,

		eLastEntry
	};
	static OmnString			enum2Str(AosSelectFromObj::E type);
	static AosSelectFromObj::E	str2Enum(const OmnString &);
};
#endif

