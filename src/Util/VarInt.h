////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 12/28/2014 Created by Rain
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_VarInt_h
#define AOS_Util_VarInt_h

#include "Util/ValueRslt.h"
#include "Util/VarUnInt.h"
struct AosVarInt
{
	 static bool encode(i8 vv, char *data, const i64 data_len, i64 &idx);
	 static bool encode(i16 vv, char *data, const i64 data_len, i64 &idx);
	 static bool encode(i32 vv, char *data, const i64 data_len, i64 &idx);
	 static bool encode(i64 vv, char *data, const i64 data_len, i64 &idx);
	 static bool encode(i8 vv, AosBuff *buff);
	 static bool encode(i16 vv, AosBuff *buff);
	 static bool encode(i32 vv, AosBuff *buff);
	 static bool encode(i64 vv, AosBuff *buff);
	 static bool decode(const char *data, const i64 len, i64 &idx, AosValueRslt &vv);
	 static bool encodeNorm(const i64 vv, char *data, const i64 data_len, i64 &idx);
	 static bool decodeNorm(char *data, const i64 data_len, i64 &idx, AosValueRslt &vv);
};
#endif
