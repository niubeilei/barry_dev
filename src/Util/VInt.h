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
// VInt is encoded as:
// 		0ddddddd		[0, 127]
// 		10dddddd dddddddd	[2*7, (2*14)-1]
// 		110ddddd dddddddd dddddddd [2*14, (2*21)-1]
// 		...
//
// Modification History:
// 2014/01/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_VInt_h
#define Omn_Util_VInt_h


inline u64 AosParseVInt(const char *data, int &bytes_used)
{
	int nn = 0;
	u8 c = data[nn++];
	if (c < 0x80)
	{
		bytes_used = 1;
		return c;
	}

	if (c < 0xc0)
	{
		// 10dddddd dddddddd
		u64 vv = ((u64)(c & 0x3f) << 8) + (u8)data[nn++];
		bytes_used = 2;
		return vv;
	}

	if (c < 0xe0)
	{
		// 110ddddd dddddddd dddddddd
		u64 vv = ((u64)(c & 0x1f) << 16) + 
				 ((u64)((u8)data[nn+1]) << 8) + 
				 ((u8)data[nn+2]);
		bytes_used = nn+2;
		return vv;
	}

	if (c < 0xf0)
	{
		// 1110dddd dddddddd dddddddd dddddddd
		u64 vv = ((u64)(c & 0x0f) << 24) + 
				 ((u64)((u8)data[nn+1]) << 16) + 
				 ((u64)((u8)data[nn+2]) << 8)  + 
				 ((u8)data[nn+3]);
		bytes_used = nn+3;
		return vv;
	}

	if (c < 0xf8)
	{
		// 11110ddd dddddddd dddddddd dddddddd dddddddd
		u64 vv = ((u64)(c & 0x07) << 32) + 
				 ((u64)((u8)data[nn+1]) << 24) + 
				 ((u64)((u8)data[nn+2]) << 16) + 
				 ((u64)((u8)data[nn+3]) << 8)  + 
				 ((u8)data[nn+4]);
		bytes_used = nn+4;
		return vv;
	}

	if (c < 0xfc)
	{
		// 111110dd dddddddd dddddddd dddddddd dddddddd dddddddd
		u64 vv = ((u64)(c & 0x03) << 40) + 
				 ((u64)((u8)data[nn+1]) << 32) + 
				 ((u64)((u8)data[nn+2]) << 24) + 
				 ((u64)((u8)data[nn+3]) << 16) + 
				 ((u64)((u8)data[nn+4]) << 8)  + 
				 ((u8)data[nn+5]);
		bytes_used = nn+5;
		return vv;
	}

	if (c < 0xfe)
	{
		// 1111110d dddddddd dddddddd dddddddd dddddddd dddddddd
		// dddddddd
		u64 vv = ((u64)(c & 0x01) << 48) + 
				 ((u64)((u8)data[nn+1]) << 40) + 
				 ((u64)((u8)data[nn+2]) << 32) + 
				 ((u64)((u8)data[nn+3]) << 24) + 
				 ((u64)((u8)data[nn+4]) << 16) + 
				 ((u64)((u8)data[nn+5]) << 8)  + 
				 ((u8)data[nn+6]);
		bytes_used = nn+6;
		return vv;
	}

	if (c < 0xff)
	{
		// 11111110 dddddddd dddddddd dddddddd dddddddd dddddddd
		// dddddddd dddddddd
		u64 vv = ((u64)((u8)data[nn+1]) << 48) + 
				 ((u64)((u8)data[nn+2]) << 40) + 
				 ((u64)((u8)data[nn+3]) << 32) + 
				 ((u64)((u8)data[nn+4]) << 24) + 
				 ((u64)((u8)data[nn+5]) << 16) + 
				 ((u64)((u8)data[nn+6]) << 8)  + 
				 ((u8)data[nn+7]);
		bytes_used = nn+7;
		return vv;
	}

	// 11111111 dddddddd dddddddd dddddddd dddddddd dddddddd
	// dddddddd dddddddd dddddddd
	u64 vv =     ((u64)((u8)data[nn+1]) << 56) + 
				 ((u64)((u8)data[nn+2]) << 48) + 
				 ((u64)((u8)data[nn+3]) << 40) + 
				 ((u64)((u8)data[nn+4]) << 32) + 
				 ((u64)((u8)data[nn+5]) << 24) + 
				 ((u64)((u8)data[nn+6]) << 16) + 
				 ((u64)((u8)data[nn+7]) << 8)  + 
				 ((u8)data[nn+8]);
	bytes_used = nn+8;
	return vv;
}


inline int AosEncodeVInt(char *data, const int len, u64 value)
{
	int nn = 0;
	if (value <= 0x7f)
	{
		data[nn++] = value;
		return nn;
	}

	if (value <= 0x3fff)
	{
		// 10dddddd dddddddd
		data[0] = 0x80 + ((u8)(value >> 8));
		data[1] = (u8)value;
		return 2;
	}

	if (value <= 0x1fffff)
	{
		// 110ddddd dddddddd dddddddd
		data[0] = 0xc0 + ((u8)(value >> 16));
		data[1] = (u8)(value >> 8);
		data[2] = (u8)value;
		return 3;
	}

	if (value <= 0x0fffffff)
	{
		// 1110dddd dddddddd dddddddd dddddddd
		data[0] = 0xe0 + ((u8)(value >> 24));
		data[1] = (u8)(value >> 16);
		data[2] = (u8)(value >> 8);
		data[3] = (u8)value;
		return 4;
	}

	if (value <= 0x07ffffffffULL)
	{
		// 11110ddd dddddddd dddddddd dddddddd dddddddd
		data[0] = 0xf0 + ((u8)(value >> 32));
		data[1] = (u8)(value >> 24);
		data[2] = (u8)(value >> 16);
		data[3] = (u8)(value >> 8);
		data[4] = (u8)value;
		return 5;
	}

	if (value <= 0x03ffffffffffULL)
	{
		// 111110dd dddddddd dddddddd dddddddd dddddddd dddddddd
		data[0] = 0xf8 + ((u8)(value >> 40));
		data[1] = (u8)(value >> 32);
		data[2] = (u8)(value >> 24);
		data[3] = (u8)(value >> 16);
		data[4] = (u8)(value >> 8);
		data[5] = (u8)value;
		return 6;
	}

	if (value <= 0x01ffffffffffffULL)
	{
		// 1111110d dddddddd dddddddd dddddddd dddddddd dddddddd
		// dddddddd
		data[0] = 0xfc + ((u8)(value >> 48));
		data[1] = (u8)(value >> 40);
		data[2] = (u8)(value >> 32);
		data[3] = (u8)(value >> 24);
		data[4] = (u8)(value >> 16);
		data[5] = (u8)(value >> 8);
		data[6] = (u8)value;
		return 7;
	}

	if (value <= 0xffffffffffffffULL)
	{
		// 11111110 dddddddd dddddddd dddddddd dddddddd dddddddd
		// dddddddd dddddddd
		data[0] = 0xfe + ((u8)(value >> 56));
		data[1] = (u8)(value >> 48);
		data[2] = (u8)(value >> 40);
		data[3] = (u8)(value >> 32);
		data[4] = (u8)(value >> 24);
		data[5] = (u8)(value >> 16);
		data[6] = (u8)(value >> 8);
		data[7] = (u8)value;
		return 8;
	}

	// 11111111 dddddddd dddddddd dddddddd dddddddd dddddddd
	// dddddddd dddddddd dddddddd
	data[0] = 0xff;
	data[1] = (u8)(value >> 56);
	data[2] = (u8)(value >> 48);
	data[3] = (u8)(value >> 40);
	data[4] = (u8)(value >> 32);
	data[5] = (u8)(value >> 24);
	data[6] = (u8)(value >> 16);
	data[7] = (u8)(value >> 8);
	data[8] = (u8)value;
	return 9;
}
#endif
