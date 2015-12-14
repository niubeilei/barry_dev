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
// [0, 235]					1 byte
// [236, (2, 16)-1] 		236 + 2 byte	3
// [(2, 16), (2, 24)-1]		237 + 3 bytes	4
// [(2, 24), (2, 32)-1]		238 + 4 bytes	5
// [(2, 32), (2, 40)-1]		239 + 5 bytes	6
// [(2, 40), (2, 48)-1]		240 + 6 bytes	7
// [(2, 48), (2, 56)-1]		241 + 7 bytes	8
// [(2, 40), (2, 64)-1]		242 + 8 bytes 	9
// 243	NULL				1 bytes
// 244	ABSENT				1 bytes
// 245	Docid	Variable	(need to supported)
// 246	Unsigned Integer Pair	(not supported yet)
// 247	Signed Integer Pair	(not supported yet)
// 248-254	Reserved	
// 255	Will use the second byte as the control byte	Future use
//
// Modification History:
// 2015/11/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/JEUIA.h"

#include "Util/Buff.h"

bool
AosJEUIA::encode(
		const u64	vv,
		AosBuff *	buff)
{
	aos_assert_r(buff, false);
	if (vv <= eValueSize01) 
	{
		buff->appendU8((u8)vv);
		return true;
	}

	if (vv <= eValueSize02) 
	{
		buff->appendU8((u8)eMark02);
		buff->appendU16((u16)vv);
		return true;
	}

	if (vv <= eValueSize03)
	{
		buff->appendU8((u8)eMark03);
		buff->appendU24((u32)vv);
		return true;
	}

	if (vv <= eValueSize04)
	{
		buff->appendU8((u8)eMark04);
		buff->appendU32((u32)vv);
		return true;
	}

	if (vv <= eValueSize05)
	{
		buff->appendU8((u8)eMark05);
		buff->appendU40((u32)vv);
		return true;
	}

	buff->appendU8((u8)eMark08);
	buff->appendU64((u64)vv);
	return true;
}


bool
AosJEUIA::encodeIntoCur(
		const u64	vv,
		AosBuff *	buff)
{
	aos_assert_r(buff, false);
	if (vv <= eValueSize01)
	{
		buff->setU8((u8)vv);
		return true;
	}

	if (vv <= eValueSize02)
	{
		buff->setU8((u8)eMark02);
		buff->setU16((u16)vv);
		return true;
	}

	if (vv <= eValueSize03)
	{
		buff->setU8((u8)eMark03);
		buff->setU24((u32)vv);
		return true;
	}

	if (vv <= eValueSize04)
	{
		buff->setU8((u8)eMark04);
		buff->setU32((u32)vv);
		return true;
	}

	if (vv <= eValueSize05)
	{
		buff->setU8((u8)eMark05);
		buff->setU40((u32)vv);
		return true;
	}

	buff->setU8((u8)eMark08);
	buff->setU64((u64)vv);
	return true;
}


bool 
AosJEUIA::decode(
		AosBuff *	buff,
		u64			&vv)
{
	aos_assert_r(buff, false);
	u8 mark = buff->getU8(0);
	if (mark <= eMark01)
	{
		vv = mark;
		return true;
	}
    switch(mark)
    {
    case eMark02:
    	vv = buff->getU16(0);
		break;

    case eMark03:
    	vv = buff->getU24(0);
    	break;

    case eMark04:
    	vv = buff->getU32(0);
    	break;

    case eMark05:
    	vv = buff->getU40(0);
    	break;

	case eMark06:	//not supported yet
	case eMark07:	//not supported yet
    default:
    	aos_assert_r(mark == eMark08, false);
		vv = buff->getU64(0);
		break;
    }

	return true;
}


u64
AosJEUIA::decodeU64(
		 const char* const  buff,
		 const u64			def)
{
	aos_assert_r(buff, def);
	u64 value = 0;
	u8 mark = *(u8 *)buff;
	if (mark <= eMark01)
	{
		value =mark;
		return value;
	}

	if (mark == eMark02)
	{
		value =*(u16 *)&buff[sizeof(u8)];
		return value;
	}

	if (mark == eMark03)
	{
		value = (u8)buff[1] +
				((((u32)buff[2]) << 8) & 0xFFFF) +
				(((u32)buff[3]) << 16);
		value &= 0xFFFFFF;
		return value;
	}

	if (mark == eMark04)
	{
		value = *(u32 *)&buff[sizeof(u8)];
		return value;
	}

	if (mark == eMark05)
	{
		 value = (u8)buff[1] +
				((((u32)buff[2]) << 8) & 0xFFFF) +
				((((u32)buff[3]) << 16) & 0xFFFFFF) +
				(((u32)buff[4]) << 24) +
				(((u64)buff[5]) << 32);
		value &= 0xFFFFFFFFFF;
		return value;
	}

	aos_assert_r(mark == eMark08, def);
	value = *(u64 *)&buff[sizeof(u8)];
	return value;
}


u32
AosJEUIA::decodeU32(
			const char* const	buff,
			const u32			def)
{
	aos_assert_r(buff, def);
	u32 value = 0;
	u8 mark = *(u8 *)buff;
	if (mark <= eMark01)
	{
		value = mark;
		return value;
	}

	if (mark == eMark02)
	{
		value =*(u16 *)&buff[sizeof(u8)];
		return value;
	}

	if (mark == eMark03)
	{
		value = (u8)buff[1] +
				((((u32)buff[2]) << 8) & 0xFFFF) +
				(((u32)buff[3]) << 16);
		value &= 0xFFFFFF;
		return value;
	}

	if (mark == eMark04)
	{
		value = *(u32 *)&buff[sizeof(u8)];
		return value;
	}

	return def;
}


u16
AosJEUIA::decodeU16(
			const char* const	buff,
			const u16			def)
{
	aos_assert_r(buff, def);
	u16 value = 0;
	u8 mark = *(u8 *)buff;
	if (mark <= eMark01)
	{
		value = mark;
		return value;
	}

	if (mark == eMark02)
	{
		value =*(u16 *)&buff[sizeof(u8)];
		return value;
	}

	return def;
}


u8
AosJEUIA::decodeU8(
			const char* const	buff,
			const u8			def)
{
	aos_assert_r(buff, def);
	u8 mark = *(u8 *)buff;
	if (mark <= eMark01)
	{
		return mark;
	}

	return def;
}


int
AosJEUIA::getsize(const u64	vv)
{
	if (vv <= eValueSize01)
	{
		return 1;
	}

	if (vv <= eValueSize02)
	{
		return 3;
	}

	if (vv <= eValueSize03)
	{
		return 4;
	}

	if (vv <= eValueSize04)
	{
		return 5;
	}

	if (vv <= eValueSize05)
	{
		return 6;
	}

	return 9;
}

