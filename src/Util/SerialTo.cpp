////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SerialTo.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/SerialTo.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"



OmnSerialTo::OmnSerialTo()
:
mBuff(0),
mData((unsigned char *)mBuff->getBuffer()),
mDataLength(0),
mCrtIndex(2)		// The first two bytes are reserved for len.
{
}


OmnSerialTo::~OmnSerialTo()
{
}


int
OmnSerialTo::getDataLength() const
{
	mBuff->setDataLength(mCrtIndex);
	return mCrtIndex;
}


void
OmnSerialTo::setDataLength()
{
	mBuff->setDataLength(mCrtIndex);
}


//
// Taken out by Chen Ding, 01/24/2008
//
// void
// OmnSerialTo::setBuff(const OmnConnBuffPtr &buff)
// {
// 	mBuff = buff;
// 	mData = mBuff->getBuffer();
// 	mCrtIndex = buff->getDataLength();
// }


OmnSerialTo &
OmnSerialTo::operator << (const OmnMsgId::E type)
{
    //
	// It stores message ID in the form:
    // :dd
	//

    if (mCrtIndex + 3 >= mBuff->getBufferLength())
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			"Run out of bound when serialize SoField");
        throw e;
    }

    mData[mCrtIndex++] = ':';
	mData[mCrtIndex++] = (char)type;
	mData[mCrtIndex++] = (char)(type>>8);
	mBuff->setDataLength(mCrtIndex);
    return *this;
}


OmnSerialTo &
OmnSerialTo::operator << (const float &value)
{
    //
    // It puts a float:
    //
    // :float
    //
    // into the current position.
    //

    if (mCrtIndex + 1 + (int)sizeof(float) >= mBuff->getBufferLength())
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			"Run out of bound when serialize SoField");
        throw e;
    }

    mData[mCrtIndex++] = ':';
    memcpy(&mData[mCrtIndex], &value, sizeof(float));
    mCrtIndex += sizeof(float);
	mBuff->setDataLength(mCrtIndex);
    return *this;
}


OmnSerialTo &
OmnSerialTo::operator << (const OmnString &value)
{
    //
    // It puts a string:
    //
    // :xx:cc...c
    //
    // into the current position, where 'xx' is the length of the
    // string and 'cc...' is the content of the string. It assumes
    // mData contains sufficient memory.
    //

    if (mCrtIndex + 4 + value.length() >= mBuff->getBufferLength())
    {
		OmnAlarm << "SerialTo run out of bound: " 
			<< mCrtIndex
			<< ", value.length: " << value.length()
			<< ", buff length: " << mBuff->getBufferLength() << enderr;

        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, "Run out of bound when serialize SoField");
        throw e;
    }

	int len = value.length();
    mData[mCrtIndex++] = ':';
    mData[mCrtIndex++] = (char)len;
    mData[mCrtIndex++] = (char)(len >> 8);
    mData[mCrtIndex++] = ':';
    memcpy(&mData[mCrtIndex], value.data(), value.length());

    mCrtIndex += value.length();
	mBuff->setDataLength(mCrtIndex);
    return *this;
}


OmnSerialTo &
OmnSerialTo::operator << (const std::string &value)
{
    //
    // It puts a string:
    //
    // :xx:cc...c
    //
    // into the current position, where 'xx' is the length of the
    // string and 'cc...' is the content of the string. It assumes
    // mData contains sufficient memory.
    //

    if (mCrtIndex + 4 + (int)value.length() >= 
			mBuff->getBufferLength())
    {
		OmnAlarm << "SerialTo run out of bound: " 
			<< mCrtIndex
			<< ", value.length: " << value.length()
			<< ", buff length: " << mBuff->getBufferLength() << enderr;

        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, "Run out of bound when serialize SoField");
        throw e;
    }

	int len = value.length();
    mData[mCrtIndex++] = ':';
    mData[mCrtIndex++] = (char)len;
    mData[mCrtIndex++] = (char)(len >> 8);
    mData[mCrtIndex++] = ':';
    memcpy(&mData[mCrtIndex], value.data(), value.length());

    mCrtIndex += value.length();
	mBuff->setDataLength(mCrtIndex);
    return *this;
}


/*
bool
OmnSerialTo::operator << (const char *value)
{
    //
    // It puts a string:
    //
    // :xx:cc...c
    //
    // into the current position, where 'xxx' is the length of the
    // string and 'cc...' is the content of the string. It assumes
    // mData contains sufficient memory.
    //

	int len = strlen(value);
    if (mCrtIndex + 5 + len >= mBuff->getBufferLength())
    {
		OmnAlarm << "SerialTo run out of bound: " 
			<< mCrtIndex
			<< ", value length: " << len
			<< ", buff length: " << mBuff->getBufferLength() << enderr;

        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, "Run out of bound when serialize SoField");
        throw e;
    }

    mData[mCrtIndex++] = ':';
    mData[mCrtIndex++] = (char)len;
    mData[mCrtIndex++] = (char)(len >> 8);
    mData[mCrtIndex++] = ':';
    strncpy(&mData[mCrtIndex], value, len);
    mCrtIndex += len;
	mBuff->setDataLength(mCrtIndex);
    return true;
}
*/


OmnSerialTo &
OmnSerialTo::operator << (const char value)
{
    //
    // It puts a string:
    //
    // :c
    //
    // into the current position. It assumes mData contains
    // sufficient memory.
    //

    if (mCrtIndex + 2 >= mBuff->getBufferLength())
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, "Run out of bound when serialize SoField");
        throw e;
    }

    mData[mCrtIndex++] = ':';
    mData[mCrtIndex++] = value;
	mBuff->setDataLength(mCrtIndex);
    return *this;
}


OmnSerialTo &
OmnSerialTo::operator << (const bool value)
{
    //
    // It puts a string:
    //
    // :c
    //
    // into the current position. It assumes mData contains
    // sufficient memory.
    //

    if (mCrtIndex + 2 >= mBuff->getBufferLength())
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, "Run out of bound when serialize SoField");
        throw e;
    }

    mData[mCrtIndex++] = ':';
    mData[mCrtIndex++] = value;
	mBuff->setDataLength(mCrtIndex);
    return *this;
}


OmnSerialTo &
OmnSerialTo::operator << (const int64_t &value)
{
	//
	// OmnUint8 is stored as 8 bytes.
	//
	//	:dddddddd
	//
	if (mCrtIndex + 9 >= mBuff->getBufferLength())
	{
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, "Run out of bound when serialize SoField");
        throw e;
    }

    mData[mCrtIndex++] = ':';
    memcpy(&mData[mCrtIndex], &value, 8);
    mCrtIndex += 8;
	mBuff->setDataLength(mCrtIndex);
    return *this;
}


OmnSerialTo &
OmnSerialTo::operator << (const u64 &value)
{
	//
	// OmnUint8 is stored as 8 bytes.
	//
	//	:dddddddd
	//
	if (mCrtIndex + 9 >= mBuff->getBufferLength())
	{
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, "Run out of bound when serialize SoField");
        throw e;
    }

    mData[mCrtIndex++] = ':';
    memcpy(&mData[mCrtIndex], &value, 8);
    mCrtIndex += 8;
	mBuff->setDataLength(mCrtIndex);
    return *this;
}


OmnSerialTo &
OmnSerialTo::operator << (const int value)
{
    //
    // It puts a int:
    //
    // :int
    //
    // into the current position. It assumes mData contains
    // sufficient memory.
    //

    if (mCrtIndex + (int)sizeof(int) + 1 >= mBuff->getBufferLength())
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, "Run out of bound when serialize SoField");
        throw e;
    }

    mData[mCrtIndex++] = ':';
    memcpy(&mData[mCrtIndex], &value, sizeof(int));
    mCrtIndex += sizeof(int);
	mBuff->setDataLength(mCrtIndex);
    return *this;
}


OmnSerialTo &
OmnSerialTo::operator << (const uint value)
{
    //
    // It puts a int:
    //
    // :int
    //
    // into the current position. It assumes mData contains
    // sufficient memory.
    //

    if (mCrtIndex + (int)sizeof(uint) + 1 >= mBuff->getBufferLength())
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, "Run out of bound when serialize SoField");
        throw e;
    }

    mData[mCrtIndex++] = ':';
    memcpy(&mData[mCrtIndex], &value, sizeof(uint));
    mCrtIndex += sizeof(uint);
	mBuff->setDataLength(mCrtIndex);
    return *this;
}


OmnSerialTo &
OmnSerialTo::operator << (const OmnIpAddr &value)
{   
    // 
	// An IP address is an int 
    //  
	return operator << (value.getIPv4());    
}   


OmnSerialTo &
OmnSerialTo::operator << (const OmnDateTime &value)
{
	// 
	// An OmnDateTime is stored in strings.
	//
	return operator << (value.toString(OmnDateTime::eYYYYMMDDHHMMSS));
}


OmnSerialTo &
OmnSerialTo::operator << (const OmnConnBuffPtr &buff)
{
    //
    // It puts a buffer:
    //
    // :xx:bb...b
    //
    // into the current position, where 'xx' is the length of the
    // buffer and 'bb...' is the content of the buffer. It assumes
    // mData contains sufficient memory.
    //

    if (mCrtIndex + 4 + buff->getDataLength() >= mBuff->getBufferLength())
    {
		OmnAlarm << "SerialTo run out of bound: " 
			<< mCrtIndex
			<< ", data length: " << buff->getDataLength()
			<< ", buff length: " << mBuff->getBufferLength() << enderr;

        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, "Run out of bound when serialize SoField");
        throw e;
    }

	int len = buff->getDataLength();
    mData[mCrtIndex++] = ':';
    mData[mCrtIndex++] = (char)len;
    mData[mCrtIndex++] = (char)(len >> 8);
    mData[mCrtIndex++] = ':';
    memcpy(&mData[mCrtIndex], buff->getBuffer(), buff->getDataLength());

    mCrtIndex += buff->getDataLength();

	mBuff->setDataLength(mCrtIndex);
    return *this;
}


bool 
OmnSerialTo::push(const OmnMsgId::E msgType)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const char *value)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const char value)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const int  value)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const bool value)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const uint  value)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const OmnString &value)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const OmnIpAddr &value)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const float &value)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const int64_t &value)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const u64 &value)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const OmnDateTime &value)
{
	aos_not_implemented_yet;
	return false;
}


bool 
OmnSerialTo::push(const char memberTag, const OmnConnBuffPtr &buff)
{
	aos_not_implemented_yet;
	return false;
}


// 
// Chen Ding, 01/24/2008
// One should always use "<< endl;" to finish the
// packing. Otherwise, it may not be correct.
//
OmnSerialTo & 
OmnSerialTo::operator << (ostream & (*f)(ostream &outs))
{
	// 
	// Set the total buffer length to the first two bytes
	//
	aos_assert_r(mData, *this);
	aos_assert_r(mBuff, *this);
	aos_assert_r(mCrtIndex > 0, *this);
	aos_assert_r((u32)mCrtIndex < (u32)0xffff, *this);
	mData[0] = (unsigned char)(mCrtIndex >> 8);
	mData[1] = (unsigned char)mCrtIndex;
	mBuff->setDataLength(mCrtIndex);
	return *this;
}

