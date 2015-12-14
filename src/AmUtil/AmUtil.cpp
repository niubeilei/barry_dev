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
// 3/31/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "AmUtil/AmUtil.h"

#include "alarm/Alarm.h"
#include "Alarm/Alarm.h"
#include "Util/OmnNew.h"


AosAmUtil::AosAmUtil()
{
}


AosAmUtil::~AosAmUtil()
{
}


bool
AosAmUtil::expandBuf(char *&buff, const u32 buflen)
{
	char *newbuf = OmnNew char[buflen];
	aos_assert_r(newbuf, false);

	memcpy(newbuf, buff, buflen);
	OmnDelete [] buff;
	buff = newbuf;
	return true;
}


int 
AosAmUtil::addTag(char *buff, const u32 buflen, AosAmTagId::E tagId, const OmnString &value)
{
	// 
	// buff[0]:     TagId
	// buff[1-2]: 	length
	// buff[3-]:	value
	//
	if (buflen < (u32)value.length() + 3)
	{
		OmnAlarm << "Buffer too short" << enderr;
		return -1;
	}

	buff[0] = tagId;
	setU16(&buff[1], value.length());
	memcpy(&buff[3], value.data(), value.length());
	return value.length() + 3;
}


bool
AosAmUtil::setU16(char *buff, const u16 value)
{
	buff[0] = (char) (value >> 8);
	buff[1] = (char) (value);
	return true;
}


bool
AosAmUtil::setU32(char *buff, const u32 value)
{
	buff[0] = (char) (value >> 24);
	buff[1] = (char) (value >> 16);
	buff[2] = (char) (value >> 8);
	buff[3] = (char) (value);
	return true;
}


u16
AosAmUtil::getU16(char *buff)
{
	return (buff[0] << 8) + buff[1];
}


u32
AosAmUtil::getU32(char *buff)
{
	u32 retVal;
	char buff_ret[4];
//OmnAlarm << (u32)(buff[0] << 24) << " " << (u32)(buff[1] << 16) << " " << (u32)(buff[1] << 8) << " " << (u32)buff[3]  << enderr;
//OmnAlarm << (buff[0] << 24) << " " << (buff[1] << 16) << " " << (buff[1] << 8) << " " << buff[3]  << enderr;
//	return (u32)(buff[0] << 24) + (u32)((buff[1] << 16) + 16777216) + (u32)((buff[2] << 8) + 65536) + buff[3];
	buff_ret[0] = buff[3];
	buff_ret[1] = buff[2];
	buff_ret[2] = buff[1];
	buff_ret[3] = buff[0];
	memcpy(&retVal, buff_ret, 4);
	return retVal;
}



