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
// 2014/12/28 Created by Rain
////////////////////////////////////////////////////////////////////////////
#include "Util/VarInt.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"


bool
AosVarInt::encode(i8 vv, char *data, const i64 data_len, i64 &idx)
{
	if (vv == 0)
	{
	    aos_assert_r(idx + 1 <= data_len, false);
	    data[idx++] = 0;
	    return true;
	}
	else
	{
	   u8 value = (vv << 1) ^ (vv >> 31);
	   return AosVarUnInt::encode(value, data, data_len, idx);
	}
}


bool
AosVarInt::encode(i8 vv, AosBuff *buff)
{
	if (vv == 0)
	{
	    buff->append(vv);
	    return true;
	}
	else
	{
	    u8 value = (vv << 1) ^ (vv >> 31);
	    return AosVarUnInt::encode(value, buff);
    }
}


bool
AosVarInt::encode(i16 vv, char *data, const i64 data_len, i64 &idx)
{
	if (vv == 0)
	{
	    aos_assert_r(idx + 1 <= data_len, false);
	    data[idx++] = 0;
	    return true;
	}
	else
	{
	    u16 value = (vv << 1) ^ (vv >> 31);
	    return AosVarUnInt::encode(value, data, data_len, idx);
	}
}

bool
AosVarInt::encode(i16 vv, AosBuff *buff)
{
	if (vv == 0)
	{
	    buff->append(vv);
	    return true;
	}
	else
	{
	    u16 value = (vv << 1) ^ (vv >> 31);
	    return AosVarUnInt::encode(value, buff);
	}
	//return AosVarUnInt(value, data, data_len, idx);
}


bool
AosVarInt::encode(i32 vv, char *data, const i64 data_len, i64 &idx)
{
	if (vv == 0)
	{
	    aos_assert_r(idx + 1 <= data_len, false);
	    data[idx++] = 0;
	    return true;
	}
	else
	{
	    u32 value = (vv << 1) ^ (vv >> 31);
	    //return AosVarUnInt(value, data, data_len, idx);
        return AosVarUnInt::encode(value, data, data_len, idx);
	}
}


bool
AosVarInt::encode(i32 vv, AosBuff *buff)
{
	if (vv == 0)
	{
	    buff->append(vv);
	    return true;
	}
	else
	{
        u32 value = (vv << 1) ^ (vv >> 31);
	    return AosVarUnInt::encode(value, buff);
	}
}

bool
AosVarInt::encode(i64 vv, char *data, const i64 data_len, i64 &idx)
{
	if (vv == 0)
	{
	    aos_assert_r(idx + 1 <= data_len, false);
	    data[idx++] = 0;
	    return true;
	}
	else
	{
	    u64 value = (vv << 1) ^ (vv >> 63);
	    return AosVarUnInt::encode(value, data, data_len, idx);
	}
}

bool
AosVarInt::encode(i64 vv, AosBuff *buff)
{
	if (vv == 0)
	{
	    buff->append(vv);
	    return true;
	}
	else
	{
	    u64 value = (vv << 1) ^ (vv >> 63);
	    return AosVarUnInt::encode(value, buff);
	}
}


bool
AosVarInt::decode(const char *data, const i64 len, i64 &idx, AosValueRslt &vv)
{
	aos_assert_r(len > 0, false);
	u64 v = 0;
	if(data[idx] == 0)
	{
		vv.setU64(v);
	}
	else
	{
	   bool rslt = AosVarUnInt::decode(data, len, idx, vv);
	   aos_assert_r(rslt, false);
	   v = vv.getU64();
	   vv.setU64((v & 0x01) == 1 ? 0-((v+1)>>1) : (v>>1) );
	}
	return true;
}
