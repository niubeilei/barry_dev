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
// Modification History:
// 2015/11/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_JEUIA_h
#define AOS_Util_JEUIA_h

#include "aosUtil/Types.h"
#include "Util/Buff.h"

struct AosJEUIA
{
	enum
	{
		eValueSize01 = 235,
		eValueSize02 = (u16)0xffff,
		eValueSize03 = (u32)0xffffff,
		eValueSize04 = (u32)0xffffffff,
		eValueSize05 = (u64)0xffffffffff,
		eValueSize06 = (u64)0xffffffffffff,
		eValueSize07 = (u64)0xffffffffffffff,

		eMark01 = 235,
		eMark02 = 236,
		eMark03 = 237,
		eMark04 = 238,
		eMark05 = 239,
		eMark06 = 240,
		eMark07 = 241,
		eMark08 = 242,

		eMarkNull	= 243,
		eMarkAbsent	= 244,
		eMarkDocid	= 245,

		eMarkUnsignedPair	= 246,
		eMarkSignedPair		= 247
	};

	static bool encode(
					const u64	vv,
					AosBuff *	buff);
	static bool encodeIntoCur(
					const u64	vv,
					AosBuff *	buff);

	static bool decode(
					AosBuff *	buff,
					u64			&vv);

	static u64  decodeU64(
					 const char* const  buff,
					 const u64			def);

	static u32  decodeU32(
					 const char* const 	buff,
					 const u32			def);

	static u16  decodeU16(
					 const char* const 	buff,
					 const u16			def);

	static u8  decodeU8(
					 const char* const 	buff,
					 const u8			def);

	static int getsize(const u64 vv);
};
#endif
