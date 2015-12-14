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
// 2015/11/17 Created by tracy
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_JESIA_h
#define AOS_Util_JESIA_h

#include "aosUtil/Types.h"
#include "Util/Buff.h"

struct AosJESIA
{
	enum
	{
		eValueSize01 = 127,
		eValueSize02 = (u16)0x7fff,
		eValueSize03 = (u32)0x7fffff,
		eValueSize04 = (u32)0x7fffffff,
		eValueSize05 = (u64)0x7fffffffffULL,

		eMark01 = -110,
		eMark02 = -111,
		eMark03 = -112,
		eMark04 = -113,
		eMark05 = -114,
		eMark06 = -115,
		eMark07 = -116,
		eMark08 = -117,

		eMarkNull	= -118,
		eMarkAbsent	= -119,
	};

	static bool encode(
					const i64	vv,
					AosBuff *	buff);

	static bool decode(
					AosBuff *	buff,
					i64 		&vv);

	static i64	decodeI64(
					const char*		buff,
					const i64  		def);

	static i32	decodeI32(
					const char*		buff,
					const i32  		def);

	static i16	decodeI16(
					const char*		buff,
					const i16  		def);

	static i8	decodeI8(
					const char*		buff,
					const i8  		def);

	static int	getsize(const i64	vv);
};
#endif
