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
// [-110, 127]					1 byte
// [-(2, 15)+1, (2, 15)-1] 		-111 + 2 byte	3
// [-(2, 23)+1, (2, 23)-1]		-112 + 3 bytes	4
// [-(2, 31)+1, (2, 31)-1]		-113 + 4 bytes	5
// [-(2, 39)+1, (2, 39)-1]		-114 + 5 bytes	6
// [-(2, 47)+1, (2, 47)-1]		-115 + 6 bytes	7
// [-(2, 55)+1, (2, 55)-1]		-116 + 7 bytes	8
// [-(2, 63)+1, (2, 63)-1]		-117 + 8 bytes 	9
// -118	NULL				1 bytes
// -119	ABSENT				1 bytes
// -120 - -126	Reserved	
// -127	Used to extend the control byte  	Future use
//
// Modification History:
// 2015/11/17 Created by tracy
////////////////////////////////////////////////////////////////////////////
#include "Util/JESIA.h"
#include "alarm_c/alarm.h"
#include "Util/Buff.h"


bool
AosJESIA::encode(
			const i64	vv,
			AosBuff *	buff)
{
	aos_assert_r(buff,false);
	if ( (vv >= eMark01) && (vv <= eValueSize01) ) 
	{
		buff->appendU8((u8)vv);
		return true;
	}

	if ( (vv >= -eValueSize02) && (vv <= eValueSize02) ) 
	{
		buff->appendU8((u8)eMark02);
		buff->appendU16((u16)vv);
		return true;
	}

	if ( (vv >= -eValueSize03) && (vv <= eValueSize03) )
	{
		buff->appendU8((u8)eMark03);
		buff->appendU24((u32)vv);
		return true;
	}

	if ( (vv >= -eValueSize04) && (vv <= eValueSize04) )
	{
		buff->appendU8((u8)eMark04);
		buff->appendU32((u32)vv);
		return true;
	}

	if ( (vv >= -eValueSize05) && (vv <= eValueSize05))
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
AosJESIA::decode(
			AosBuff *	buff,
			i64 		&vv)
{
	aos_assert_r(buff, false);
	int mark = (i8)(buff->getU8(0));
	if ( (mark >= eMark01) && (mark <= eValueSize01)) 
	{
		vv = mark;
		return true;
	}

	switch (mark)
	{
	case eMark02:
		vv = (i16)(buff->getU16(0));
		break;

	case eMark03:
		vv =  buff->getI24(0);
		break;

	case eMark04:
		vv = (i32)(buff->getU32(0));
		break;

	case eMark05:
		vv =  buff->getI40(0);
		break;

	case eMark06:	//not supported yet
	case eMark07:	//not supported yet

	default:
		aos_assert_r(mark == eMark08, false);
		vv = (i64)(buff->getU64(0));
		break;
	}

	return true;
}


i64
AosJESIA::decodeI64(
			const char*		buff,
			const i64  		def)
{
	aos_assert_r(buff, def);
	i64 value = 0;
	int mark = *(i8 *)buff;
	if ( (mark >= eMark01) && (mark <= eValueSize01))
	{
		value = mark;
		return value;
	}

	switch (mark)
	{
	case eMark02:
		value = *(i16*)&buff[sizeof(u8)];
		break;

	case eMark03:
		value = (u8)buff[1] +
				((((u32)buff[2]) << 8) & 0xFFFF) +
				(((i32)buff[3]) << 16);
		if ((value & 0x800000) == 0)
		{
			value &= 0xFFFFFF;
		}
		else
		{
			value |= 0xFFFFFFFFFF000000;
		}
		break;

	case eMark04:
		value = *(i32*)&buff[sizeof(u8)];
		break;

	case eMark05:
		value = (u8)buff[1] +
				((((u32)buff[2]) << 8) & 0xFFFF) +
				((((u32)buff[3]) << 16) & 0xFFFFFF) +
				(((u32)buff[4]) << 24) +
				(((u64)buff[5]) << 32);
		if((value & 0x8000000000) == 0)
		{
			value &= 0xFFFFFFFFFF;
		}
		else
		{
			value |= 0xFFFFFF0000000000;
		}
		break;

	case eMark06:	//not supported yet
	case eMark07:	//not supported yet
	default:
		aos_assert_r(mark == eMark08, def);
		value = *(i64*)&buff[sizeof(u8)];
		break;
	}

    return value;
}


i32
AosJESIA::decodeI32(
			const char*		buff,
			const i32		def)
{
	aos_assert_r(buff, def);
	i32 value = 0;
	int mark = *(i8 *)buff;
	if ( (mark >= eMark01) && (mark <= eValueSize01))
	{
		value = mark;
		return value;
	}

	switch (mark)
	{
	case eMark02:
		value = *(i16*)&buff[sizeof(u8)];
		break;

	case eMark03:
		value = (u8)buff[1] +
				((((u32)buff[2]) << 8) & 0xFFFF) +
				(((i32)buff[3]) << 16);
		if ((value & 0x800000) == 0)
		{
			value &= 0xFFFFFF;
		}
		else
		{
			value |= 0xFF000000;
		}
		break;

	case eMark04:
		value = *(i32*)&buff[sizeof(u8)];
		break;

	default:
		return def;
	}

	return value;
}


i16
AosJESIA::decodeI16(
				const char*		buff,
				const i16		def)
{
	aos_assert_r(buff, def);
	i16 value = 0;
	int mark = *(i8 *)buff;
	if ( (mark >= eMark01) && (mark <= eValueSize01))
	{
		value = mark;
		return value;
	}
    if( mark == eMark02)
    {
    	value = *(i16*)&buff[sizeof(u8)];
    	return value;
    }

	return def;
}


i8
AosJESIA::decodeI8(
				const char*		buff,
				const i8 		def)
{
	aos_assert_r(buff, def);
	i8 value = 0;
	int mark = *(i8 *)buff;
	if ( (mark >= eMark01) && (mark <= eValueSize01))
	{
		value = mark;
		return value;
	}

	return def;
}


int
AosJESIA::getsize(const i64 	vv)
{
	if ( (vv >= eMark01) && (vv <= eValueSize01) )
	{
		return 1;
	}

	if ( (vv >= -eValueSize02) && (vv <= eValueSize02) )
	{
		return 3;
	}

	if ( (vv >= -eValueSize03) && (vv <= eValueSize03) )
	{
		return 4;
	}

	if ( (vv >= -eValueSize04) && (vv <= eValueSize04) )
	{
		return 5;
	}

	if ( (vv >= -eValueSize05) && (vv <= eValueSize05))
	{
		return 6;
	}

	//eValueSize06 and eValueSize07 are not supported yet

	return 9;
}
