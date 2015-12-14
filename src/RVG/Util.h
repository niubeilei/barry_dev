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
// 12/14/2007: Modified by Allen Xu
// 01/02/2008: Modified by Chen Ding on AosCmdKeyType:
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RVG_Util_h
#define Aos_RVG_Util_h

#include <string>


class AosSequenceOrder
{
public:
	enum E
	{
		eSeqOrder_Random = 0,
		eSeqOrder_ASC,
		eSeqOrder_DEC,

		eLastEntry
	};

	static OmnString enum2Str(const AosSequenceOrder::E type);
	static AosSequenceOrder::E str2Enum(const OmnString & name);
};


class AosValueType
{
public:
	enum E 
	{
		eUnknown = 0,
		eBool,
		eU8,
		eU16,
		eU32,
		eU64,
		eInt8,
		eInt16,
		eInt32,
		eInt64,
		eFloat,
		eDouble,
		eString,
		eByteStream,
		eUserDefined,
		eTable,
		eRecord,

		eLastEntry
	};
	static OmnString enum2Str(const AosValueType::E type);
	static AosValueType::E str2Enum(const OmnString &name);
};


class AosRVGType
{
public:
	enum E 
	{
		eUnknown = 0, 
		eCharsetRSG,
		eEnumRSG,
		eEnumSeqRSG,
		eU8RIG,
		eU16RIG, 
		eU32RIG, 
		eU64RIG, 
		eInt8RIG,
		eInt16RIG, 
		eInt32RIG, 
		eInt64RIG, 
		eFloat, 
		eDouble,
		eAndRSG,
		eOrRSG,

		eLastEntry
	};
	static OmnString enum2Str(const AosRVGType::E type);
	static AosRVGType::E	str2Enum(const OmnString &name);
};


class AosRVGUsageType
{
public:
	enum E 
	{
		eUnknown = 0,
		eInputOnly,
		eInputOutputThrRef,
		eInputOutputThrAddr,
		eOutputThrRef,
		eOutputThrAddr,
	
		eLastEntry
	};
	static OmnString enum2Str(const AosRVGUsageType::E type);
	static AosRVGUsageType::E 	str2Enum(const OmnString &name);
};


class AosCorCtrl
{
public:
	enum E
	{
		eInvalid = 0,

		eRandom,
		eCorrectIfPossible,
		eIncorrectIfPossible,
		eCorrectOnly,
		eIncorrectOnly,

		eLastEntry
	};

	static OmnString enum2Str(const AosCorCtrl::E type);
	static AosCorCtrl::E	str2Enum(const OmnString &name);
};


#endif

