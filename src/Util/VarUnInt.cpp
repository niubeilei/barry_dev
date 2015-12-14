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
// 08/13/2014 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/VarUnInt.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"


bool
AosVarUnInt::encode(u8 vv, char *data, const i64 data_len, i64 &idx)
{
	// 7 bits, leading bit pattern: 0 , 1 bytes
	if (vv <= 127)
	{
		aos_assert_r(idx + 1 <= data_len, false);
		data[idx++] = vv;
		return true;
	}
	//14 bits, leading bit pattern: 10, 2 bytes
	aos_assert_r(idx + 2 <= data_len, false);
	data[idx++] = 0x80;
	data[idx++] = vv;
	return true;
}


bool
AosVarUnInt::encode(u8 vv, AosBuff *buff)
{
	// 7 bits, leading bit pattern: 0 , 1 bytes
	if (vv <= 127)
	{
		buff->append(vv);
		return true;
	}
	//14 bits, leading bit pattern: 10, 2 bytes
	buff->append(0x80, vv);
	return true;
}


bool
AosVarUnInt::encode(u16 vv, char *data, const i64 data_len, i64 &idx)
{
	if (vv <= 127) return encode((u8)vv, data, data_len, idx);
	//14 bits, leading bit pattern: 10, 2 bytes
	if (vv <= (u16)0x3FFF)
	{
		aos_assert_r(idx + 2 <= data_len, false);
		data[idx++] = (char)(vv >> 8) + 0x80;
		data[idx++] = (char)vv;
		return true;
	}

	//21 bits, leading bit pattern: 110 , 3 bytes 
	aos_assert_r(idx + 3 <= data_len, false);
	data[idx++] = 0xc0;
	data[idx++] = (char)(vv >> 8);
	data[idx++] = (char)(vv);
	return true;
}

bool
AosVarUnInt::encode(u16 vv, AosBuff *buff)
{
	if (vv <= 127) return encode((u8)vv, buff);
	//14 bits, leading bit pattern: 10, 2 bytes
	if (vv <= (u16)0x3FFF)
	{
		char c1 = (char)(vv >> 8) + 0x80;
		char c2 = (char)vv;
		buff->append(c1, c2);
		return true;
	}

	//21 bits, leading bit pattern: 110 , 3 bytes 
	buff->append(0xc0);
	char c1 = (char)(vv >> 8);
	char c2 = (char)(vv);
	buff->append(c1, c2);
	return true;
}

bool
AosVarUnInt::encode(u32 vv, char *data, const i64 data_len, i64 &idx)
{
	if (vv <= 127) return encode((u8)vv, data, data_len, idx);
	if (vv <= (u16)0x3FFF) return encode((u16)vv, data, data_len, idx);
	//21 bits, leading bit pattern: 110 , 3 bytes
	if (vv <= (u32)0x1FFFFF)
	{
		aos_assert_r(idx + 3 <= data_len, false);
		data[idx++] = (char)(vv >> 16) + 0xc0;
		data[idx++] = (char)(vv >> 8); 
		data[idx++] = (char) vv;
		return true;
	}

	//28bits, leading bit pattern: 1110 , 4 bytes
	if (vv <= (u32)0x0FFFFFFF)
	{
		aos_assert_r(idx + 4 <= data_len, false);
		data[idx++] = (char) (vv >> 24) + 0xe0;
		data[idx++] = (char)(vv >> 16);
		data[idx++] = (char)(vv >> 8);
		data[idx++] = (char)(vv);
		return true;
	}

	//35bits, leading bit pattern: 1111 0, 5 bytes
	aos_assert_r(idx + 5 <= data_len, false);
	data[idx++] = 0xf0;
	data[idx++] = (char)(vv >> 24);
	data[idx++] = (char)(vv >> 16);
	data[idx++] = (char)(vv >> 8);
	data[idx++] = (char)(vv);
	return true;
}


bool
AosVarUnInt::encode(u32 vv, AosBuff *buff)
{
	if (vv <= 127) return encode((u8)vv, buff);
	if (vv <= (u16)0x3FFF) return encode((u16)vv, buff);
	//21 bits, leading bit pattern: 110 , 3 bytes
	if (vv <= (u32)0x1FFFFF)
	{
		char c1 = (char)(vv >> 16) + 0xc0;
		char c2 = (char)(vv >> 8); 
		buff->append(c1, c2);
		buff->append((char)vv);
		return true;
	}

	//28bits, leading bit pattern: 1110 , 4 bytes
	if (vv <= (u32)0x0FFFFFFF)
	{
		char c1 = (char) (vv >> 24) + 0xe0;
		char c2 = (char)(vv >> 16);
		char c3 = (char)(vv >> 8);
		char c4 = (char)(vv);
		buff->append(c1, c2, c3, c4);
		return true;
	}

	//35bits, leading bit pattern: 1111 0, 5 bytes
	buff->append(0xf0);
	char c1 = (char)(vv >> 24);
	char c2 = (char)(vv >> 16);
	char c3 = (char)(vv >> 8);
	char c4 = (char)(vv);
	buff->append(c1, c2, c3, c4);
	return true;
}

bool
AosVarUnInt::encode(u64 vv, char *data, const i64 data_len, i64 &idx)
{
	if (vv <= 127) return encode((u8)vv, data, data_len, idx);
	if (vv <= (u16)0x3FFF) return encode((u16)vv, data, data_len, idx);
	if (vv <= (u32)0x0FFFFFFF) return encode((u32)vv, data, data_len, idx);
	//35bits, leading bit pattern: 1111 0 , 5 bytes
	if (vv <= (u64)0x07FFFFFFFF)
	{
		aos_assert_r(idx + 5 <= data_len, false);
		data[idx++] = (char)(vv >> 32) + 0xf0;
		data[idx++] = (char)(vv >> 24); 
		data[idx++] = (char)(vv >> 16); 
		data[idx++] = (char)(vv >> 8); 
		data[idx++] = (char) vv;
		return true;
	}

	// 42 bits, leading bit pattern: 1111 10, 6 bytes
	if (vv <= (u64)0x03FFFFFFFFFF)
	{
		aos_assert_r(idx + 6 <= data_len, false);
		data[idx++] = (char)(vv >> 40) + 0xf8;
		data[idx++] = (char)(vv >> 32);
		data[idx++] = (char)(vv >> 24); 
		data[idx++] = (char)(vv >> 16); 
		data[idx++] = (char)(vv >> 8); 
		data[idx++] = (char) vv;
		return true;
	}

	// 49 bits, leading bit pattern: 1111 110, 7 bytes
	if (vv <= (u64)0x01FFFFFFFFFFFF)
	{
		aos_assert_r(idx + 7 <= data_len, false);
		data[idx++] = (char)(vv >> 48) + 0xfc;
		data[idx++] = (char)(vv >> 40);
		data[idx++] = (char)(vv >> 32);
		data[idx++] = (char)(vv >> 24); 
		data[idx++] = (char)(vv >> 16); 
		data[idx++] = (char)(vv >> 8); 
		data[idx++] = (char) vv;
		return true;
	}

	// 56 bits, leading bit patterns: 1111 1110, 8 bytes
	if (vv <= (u64)0x00FFFFFFFFFFFFFF)
	{
		aos_assert_r(idx + 8 <= data_len, false);
		data[idx++] = (char)(vv >> 56) + 0xfe;
		data[idx++] = (char)(vv >> 48);
		data[idx++] = (char)(vv >> 40);
		data[idx++] = (char)(vv >> 32);
		data[idx++] = (char)(vv >> 24); 
		data[idx++] = (char)(vv >> 16); 
		data[idx++] = (char)(vv >> 8); 
		data[idx++] = (char) vv;
		return true;
	}

	// 63 bits, leding bit pattern: 1111 1111 0, 9 bytes.
	if (vv <= (u64)0x7FFFFFFFFFFFFFFF)
	{
		aos_assert_r(idx + 9 <= data_len, false);
		data[idx++] = 0xff;
		data[idx++] = (char)(vv >> 56);
		data[idx++] = (char)(vv >> 48);
		data[idx++] = (char)(vv >> 40);
		data[idx++] = (char)(vv >> 32);
		data[idx++] = (char)(vv >> 24); 
		data[idx++] = (char)(vv >> 16); 
		data[idx++] = (char)(vv >> 8); 
		data[idx++] = (char) vv;
		return true;
	}

	// 70 bits, leading bit pattern: 1111 1111 10, 10 bytes
	aos_assert_r(idx + 10 <= data_len, false);
	data[idx++] = 0xff;
	data[idx++] = 0x80;
	data[idx++] = (char)(vv >> 56);
	data[idx++] = (char)(vv >> 48);
	data[idx++] = (char)(vv >> 40);
	data[idx++] = (char)(vv >> 32);
	data[idx++] = (char)(vv >> 24); 
	data[idx++] = (char)(vv >> 16); 
	data[idx++] = (char)(vv >> 8); 
	data[idx++] = (char) vv;
	return true;
}

bool
AosVarUnInt::encode(u64 vv, AosBuff *buff)
{
	if (vv <= 127) return encode((u8)vv, buff);
	if (vv <= (u16)0x3FFF) return encode((u16)vv, buff);
	if (vv <= (u32)0x0FFFFFFF) return encode((u32)vv, buff);
	//35bits, leading bit pattern: 1111 0 , 5 bytes
	if (vv <= (u64)0x07FFFFFFFF)
	{
		char c1 = (char)(vv >> 32) + 0xf0;
		char c2 = (char)(vv >> 24); 
		char c3 = (char)(vv >> 16); 
		char c4 = (char)(vv >> 8); 
		buff->append(c1, c2, c3, c4);
		buff->append((char)vv);
		return true;
	}

	// 42 bits, leading bit pattern: 1111 10, 6 bytes
	if (vv <= (u64)0x03FFFFFFFFFF)
	{
		char c1 = (char)(vv >> 40) + 0xf8;
		char c2 = (char)(vv >> 32);
		char c3 = (char)(vv >> 24); 
		char c4 = (char)(vv >> 16); 
		buff->append(c1, c2, c3, c4);

		c1 = (char)(vv >> 8); 
		c2 = (char) vv;
		buff->append(c1, c2);
		return true;
	}

	// 49 bits, leading bit pattern: 1111 110, 7 bytes
	if (vv <= (u64)0x01FFFFFFFFFFFF)
	{
		char c1 = (char)(vv >> 48) + 0xfc;
		char c2 = (char)(vv >> 40);
		char c3 = (char)(vv >> 32);
		char c4 = (char)(vv >> 24); 
		buff->append(c1, c2, c3, c4);

		c1 = (char)(vv >> 16); 
		c2 = (char)(vv >> 8); 
		buff->append(c1, c2);

		buff->append((char)vv);
		return true;
	}

	// 56 bits, leading bit patterns: 1111 1110, 8 bytes
	if (vv <= (u64)0x00FFFFFFFFFFFFFF)
	{
		char c1 = (char)(vv >> 56) + 0xfe;
		char c2 = (char)(vv >> 48);
		char c3 = (char)(vv >> 40);
		char c4 = (char)(vv >> 32);
		buff->append(c1, c2, c3, c4);
		c1 = (char)(vv >> 24); 
		c2 = (char)(vv >> 16); 
		c3 = (char)(vv >> 8); 
		c4 = (char) vv;
		buff->append(c1, c2, c3, c4);
		return true;
	}

	// 63 bits, leding bit pattern: 1111 1111 0, 9 bytes.
	if (vv <= (u64)0x7FFFFFFFFFFFFFFF)
	{
		buff->append(0xff);
		char c1 = (char)(vv >> 56);
		char c2 = (char)(vv >> 48);
		char c3 = (char)(vv >> 40);
		char c4 = (char)(vv >> 32);
		buff->append(c1, c2, c3, c4);
		c1 = (char)(vv >> 24); 
		c2 = (char)(vv >> 16); 
		c3 = (char)(vv >> 8); 
		c4 = (char) vv;
		buff->append(c1, c2, c3, c4);
		return true;
	}

	// 70 bits, leading bit pattern: 1111 1111 10, 10 bytes
	buff->append(0xff, 0x80);
	char c1 = (char)(vv >> 56);
	char c2 = (char)(vv >> 48);
	char c3 = (char)(vv >> 40);
	char c4 = (char)(vv >> 32);
	buff->append(c1, c2, c3, c4);
	c1 = (char)(vv >> 24); 
	c2 = (char)(vv >> 16); 
	c3 = (char)(vv >> 8); 
	c4 = (char) vv;
	buff->append(c1, c2, c3, c4);
	return true;
}

bool
AosVarUnInt::encodeDocid(u64 vv, char *data, const i64 data_len, i64 &idx)
{
	// A docid is in the form:
	// 		Byte 1		type (highest byte)
	// 		Byte 2
	// 		Byte 3
	// 		...
	// 		Byte 8		(lowest byte)
	//
	// Below is an example:
	// 0x01 0x00 0x00 0x00 0x00 0x36 0x13 0x33
	
	data[idx++] = (char)(vv >> 56);
	return encode((u64)(vv & 0xFFFFFFFFFFFFFF), data, data_len, idx);
}

bool
AosVarUnInt::encodeDocid(u64 vv, AosBuff *buff)
{
	// A docid is in the form:
	// 		Byte 1		type (highest byte)
	// 		Byte 2
	// 		Byte 3
	// 		...
	// 		Byte 8		(lowest byte)
	//
	// Below is an example:
	// 0x01 0x00 0x00 0x00 0x00 0x36 0x13 0x33
	
	buff->append((char)(vv >> 56));
	return encode((u64)(vv & 0xFFFFFFFFFFFFFF), buff);
}


bool
AosVarUnInt::decodeDocid(char *data, const i64 data_len, i64 &idx, AosValueRslt &vv)
{
	// A docid is in the form:
	// 		Byte 1		type (highest byte)
	// 		Byte 2
	// 		Byte 3
	// 		...
	// 		Byte 8		(lowest byte)
	//
	// Below is an example:
	// 0x01 0x00 0x00 0x00 0x00  0x36 0x13 0x33
	
	u64 type = data[idx++];
	bool rslt = decode(data, data_len, idx, vv);
	aos_assert_r(rslt, false);
	u64 value = vv.getU64();
	aos_assert_r(value > 0, false);
	value += (type << 56);
	vv.setU64(value);
	return true;
}

bool
AosVarUnInt::decode(const char *data, const i64 len, i64 &idx, AosValueRslt &vv)
{
	aos_assert_r(len > 0, false);

	//0 , 1 bytes
	if ((data[idx] & 0x80) == 0)
	{
		u64 cc = data[idx];
		vv.setU64(cc);
		idx +=1;
		return true;
	}

	//10, 2 bytes 
	if ((data[idx] & 0xc0) == 0x80)
	{
		aos_assert_r(len >= 2, false);
		u64 v = 0;
		char *cc = (char *)&v;

		cc[0] = data[idx+1];
		cc[1] = data[idx] & 0x3f;
		vv.setU64(v);
		idx +=2;
		return true;
	}

	//110 , 3 bytes
	if ((data[idx] & 0xe0) == 0xc0)
	{
		aos_assert_r(len >= 3, false);
		u64 v = 0;
		char *cc = (char *)&v;

		cc[0] = data[idx+2];
		cc[1] = data[idx+1];
		cc[2] = data[idx] & 0x1f;
		idx +=3;
		vv.setU64(v);
		return true;
	}

	//1110, 4 bytes 
	if ((data[idx] & 0xf0) == 0xe0)
	{
		aos_assert_r(len >= 4, false);
		u64 v = 0;
		char *cc = (char *)&v;

		cc[0] = data[idx+3];
		cc[1] = data[idx+2];
		cc[2] = data[idx+1];
		cc[3] = data[idx] & 0x0f;
		vv.setU64(v);
		idx +=4;
		return true;
	}

	//1111 0xxx 5bytes
	if ((data[idx] & 0xf8) == 0xf0)
	{
		aos_assert_r(len >= 5, false);
		u64 v = 0;
		char *cc = (char *)&v;

		for (int i=0; i<4; i++)
		{
			cc[i] = data[idx+4-i];
		}
		cc[4] = data[idx] & 0x07;
		vv.setU64(v);
		idx +=5;
		return true;
	}

	//1111 10xx 6bytes
	if ((data[idx] & 0xfc) == 0xf8)
	{
		aos_assert_r(len >= 6, false);
		u64 v = 0;
		char *cc = (char *)&v;

		for (int i=0; i<5; i++)
		{
			cc[i] = data[idx+5-i];
		}
		cc[5] = data[idx] & 0x03;
		vv.setU64(v);
		idx +=6;
		return true;
	}

	//1111 110x 7bytes
	if ((data[idx] & 0xfe) == 0xfc)
	{
		aos_assert_r(len >= 7, false);
		u64 v = 0;
		char *cc = (char *)&v;

		for (int i=0; i<6; i++)
		{
			cc[i] = data[idx+6-i];
		}

		cc[6] = data[idx] & 0x01;
		vv.setU64(v);
		idx +=7;
		return true;
	}

	//1111 1110 8bytes
	if ((data[idx] & 0xff) == 0xfe)
	{
		aos_assert_r(len >= 8, false);
		u64 v = 0;
		char *cc = (char *)&v;

		for (int i=0; i<7; i++)
		{
			cc[i] = data[idx+7-i];
		}
		cc[7] = data[idx] & 0x00;
		vv.setU64(v);
		idx +=8;
		return true;
	}

	//1111 1111 0xxx xxxx 9bytes
	if ((data[idx] == (char)0xff) && ((data[idx+1] & 0x80) == 0))
	{
		aos_assert_r(len >= 9, false);
		u64 v = 0;
		char *cc = (char *)&v;

		for (int i=0; i<7; i++)
		{
			cc[i] = data[idx+8-i];
		}
		cc[7] = data[idx+1] & 0x7f;
		cc[8] = data[idx] & 0x00;
		vv.setU64(v);
		idx +=9;
		return true;
	}
	
	//1111 1111 10XX XXXX 10BYTES
	if ((data[idx] == (char)0xff) && ((data[idx+1] & 0xc0) == 0x80))
	{
		aos_assert_r(len >= 10, false);
		u64 v = 0;
		char *cc = (char *)&v;

		for (int i=0; i<8; i++)
		{
			cc[i] = data[idx+9-i];
		}

		cc[8] = data[idx+1] & 0x3f;
		cc[9] = data[idx] & 0x00;
		vv.setU64(v);
		idx +=10;
		return true;
	}
	return false;
}


bool 
AosVarUnInt::encodeNorm(const u64 vv, char *data, const i64 data_len, i64 &idx)
{
	aos_assert_r(idx + 8 <= data_len, false);
	*(u64 *)&data[idx] = vv;
	idx += 8;
	return true;
}

bool 
AosVarUnInt::decodeNorm(char *data, const i64 data_len, i64 &idx, AosValueRslt &vv)
{
	aos_assert_r(idx + 8 <= data_len, false);
	u64 value = *(u64*)&data[idx];
	idx += 8;
	vv.setU64(value);
	return true;
}
