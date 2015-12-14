////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/01/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Datalet_RecordUtil_h
#define Aos_Datalet_RecordUtil_h

#include "Util/Buff.h"


extern int gAosEndian;

inline u16 AosConvertU16(char *data)
{
	u16 value;
	u8 *buff = (u8*)&value;
	buff[0] = data[1];
	buff[1] = data[0];
	return value;
}

inline bool AosEncodeU16(AosBuff *buff, const int endian, const u16 value)
{
	char *data = (char*)&value;
	if (endian != gAosEndian)
	{
		char data1[2];
		data1[0] = data[1];
		data1[1] = data[0];
		buff->setBuff(data1, 2);
		return true;
	}

	buff->setBuff(data, 2);
	return true;
		
};

inline bool AosEncodeInt16(AosBuff *buff, const int endian, const int16_t value)
{
	return AosEncodeU16(buff, endian, (u16)value);
}

inline bool AosEncodeU32(AosBuff *buff, const int endian, const u32 value)
{
	char *data = (char*)&value;
	if (endian != gAosEndian)
	{
		char data1[4];
		data1[0] = data[3];
		data1[1] = data[2];
		data1[2] = data[1];
		data1[3] = data[0];
		buff->setBuff(data1, 4);
		return true;
	}

	buff->setBuff(data, 4);
	return true;
		
};

inline bool AosEncodeU64(AosBuff *buff, const int endian, const u64 value)
{
	char *data = (char*)&value;
	if (endian != gAosEndian)
	{
		char data1[8];
		data1[0] = data[7];
		data1[1] = data[6];
		data1[2] = data[5];
		data1[3] = data[4];
		data1[4] = data[3];
		data1[5] = data[2];
		data1[6] = data[1];
		data1[7] = data[0];
		buff->setBuff(data1, 8);
		return true;
	}

	buff->setBuff(data, 8);
	return true;
};

#endif

