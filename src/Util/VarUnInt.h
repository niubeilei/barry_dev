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
// 08/15/2014 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_VarUnInt_h
#define AOS_Util_VarUnInt_h

#include "Util/ValueRslt.h"

struct AosVarUnInt
{
	 static bool encode(u8 vv, char *data, const i64 data_len, i64 &idx);
	 static bool encode(u16 vv, char *data, const i64 data_len, i64 &idx);
	 static bool encode(u32 vv, char *data, const i64 data_len, i64 &idx);
	 static bool encode(u64 vv, char *data, const i64 data_len, i64 &idx);
	 static bool encodeDocid(u64 vv, char *data, const i64 data_len, i64 &idx);
	 static bool encode(u8 vv, AosBuff *buff);
	 static bool encode(u16 vv, AosBuff *buff);
	 static bool encode(u32 vv, AosBuff *buff);
	 static bool encode(u64 vv, AosBuff *buff);
	 static bool encodeDocid(u64 vv, AosBuff *buff);
	 static bool decode(const char *data, const i64 len, i64 &idx, AosValueRslt &vv);
	 static bool decodeDocid(char *data, const i64 data_len, i64 &idx, AosValueRslt &vv);
	 static bool encodeNorm(const u64 vv, char *data, const i64 data_len, i64 &idx);
	 static bool decodeNorm(char *data, const i64 data_len, i64 &idx, AosValueRslt &vv);
	
};
#endif
