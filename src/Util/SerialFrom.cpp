////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SerialFrom.cpp
// Description:
//	Message Serializer is used for two purposes: 
//	1. To convert an object into a string
//	2. To reconstruct an object from a string
//
//  To create a 'eTo' serializer, use the default constructor. The 
//  constructor will allocate memory for itself. 
//
//  To create a 'eFrom' serializer, one should already had a 
//  memory, or OmnConnBuff. Use the OmnSerialFrom(OmnConnBuffPtr)
//  constructor to create it.
//
//  To put data into a serializer (again, it must be eTo type), use
//  the 'push(...)' member functions. If it runs out of memory or
//  encounter other errors, it throws exception.
//
//  To retrieve data from a serializer (it can be either type but normally
//  it should be eFrom type), use 'operator >> (...)' member functions. If 
//  any error occurs, it throws exceptions.
//
//	mBuff: 			Holds the OmnConnBuff
//  mData:			It is the memory from mBuff. Do not delete it!!!
//  mDataLength:	The data length. It is not the buffer length.
//  mCrtIndex:		Used by the 'operator >> (...)' and 'push(...)' member functions.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/SerialFrom.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util1/DateTime.h"
#include "UtilComm/ConnBuff.h"


//
// Use this constructor to create a SerialFrom object.
//
OmnSerialFrom::OmnSerialFrom(const OmnConnBuffPtr &buff)
:
mBuff(buff),
mData(buff->getBuffer()),
mDataLength(buff->getDataLength()),
mCrtIndex(2)
{
	aos_assert(mBuff);
	aos_assert(mDataLength >= 2);

	mExpectedLength = ((unsigned int)mData[0] << 8) + 
					  (unsigned int)mData[1];
	mComplete = (mExpectedLength <= mDataLength)?true:false;
}

OmnSerialFrom::OmnSerialFrom()
:
mData(0),
mDataLength(0),
mCrtIndex(0),
mExpectedLength(-1),
mComplete(false)
{
}


void 
OmnSerialFrom::resetBuffer()
{
	mData = 0;
	mBuff = 0;
	mDataLength = 0;
	mCrtIndex = 0;
	mExpectedLength = -1;
	mComplete = false;
}


/*
 * This function is removed from this class. One should 
 * use appendBuffer(...). 
 * 
 * Chen Ding, 01/23/2008
 */
/*
void
OmnSerialFrom::setBuffer(const OmnConnBuffPtr &buff)
{
	mBuff = buff;
	mData = mBuff->getBuffer();
	mDataLength = mBuff->getDataLength();
	mCrtIndex = 0;
}
*/


bool
OmnSerialFrom::appendBuffer(const OmnConnBuffPtr &buff)
{
	aos_assert_r(buff, false);
	if (!mBuff)
	{
		mBuff = buff;
	}
	else
	{
		mBuff->append(buff);
	}
	mData = mBuff->getBuffer();
	mDataLength = mBuff->getDataLength();
	mCrtIndex = 2;

	mExpectedLength = ((unsigned int)mData[0] << 8) + 
					  (unsigned int)mData[1];
	mComplete = (mExpectedLength <= mDataLength)?true:false;
	return mComplete;
}


OmnSerialFrom::~OmnSerialFrom()
{
}


OmnMsgId::E
OmnSerialFrom::popMsgId()
{
	//
	// It retrieves the message ID:
	//
	// :dd
	//
	// and converts it into the enum value.
	//

	if (mCrtIndex + 4 > mDataLength)
	{
		OmnWarn << OmnErrId::eWarnCommError
			<< "SoField parsing out of bound at Position: "
			<< mCrtIndex << ": " << mData << enderr;
		return OmnMsgId::eInvalidMsgId;
	}

	if (mData[mCrtIndex++] != ':')
	{
		OmnWarn << OmnErrId::eWarnCommError
			<< "Parsing SoField syntax error (expecting :) at Position: "
			<< mCrtIndex-1 << ": " << mData << enderr;
		return OmnMsgId::eInvalidMsgId;
	}

	int type = mData[mCrtIndex] + 
			   (mData[mCrtIndex+1]>>8) + 
			   (mData[mCrtIndex+2]>>16);

	mCrtIndex += 3;
	return (OmnMsgId::E)type;
}

	
OmnSerialFrom &
OmnSerialFrom::operator >> (int &value)
{
	//
	// The next field is an integer. An integer is stored in the form:
	//
	//	:dddd
	//
	// If it contains any error, it throws an exception.
	//

	if (mCrtIndex + 1 + (int)sizeof(int) > mDataLength)
	{
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("SoField parsing out of bound at Position: ")
				<< mCrtIndex << ": " << mData);
        throw e;
	}

	if (mData[mCrtIndex++] != ':')
	{
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing SoField syntax error (expecting :)")
            << " at Position: " << mCrtIndex-1 
			<< ": " << mData);
		throw e;
	}

	value = 0;
	memcpy(&value, &mData[mCrtIndex], sizeof(int));
	mCrtIndex += sizeof(int);
	return *this;
}


OmnSerialFrom &
OmnSerialFrom::operator >> (uint &value)
{
    //
    // The next field is an integer. An integer is stored in the form:
    // 
    //  :dddd
    //  
    // If it contains any error, it throws an exception.
    // 
    
    if (mCrtIndex + 1 + (int)sizeof(int) > mDataLength)
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("SoField parsing out of bound at Position: ")
            << mCrtIndex << ": " << mData);
		throw e;
    }   
    
    if (mData[mCrtIndex++] != ':')
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing SoField syntax error (expecting :)")
            << " at Position: " << mCrtIndex-1 
			<< ": " << mData);
		throw e;
    }

    value = 0;
    memcpy(&value, &mData[mCrtIndex], sizeof(uint));
    mCrtIndex += sizeof(uint);
    return *this;
}


OmnSerialFrom &
OmnSerialFrom::operator >> (char &value)
{
    //
    // The next field is a char. A char is stored in the form:
    //
    //  :c
    //
    // If it contains any error, it throws an exception.
    //

    if (mCrtIndex + 2 > mDataLength)
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("SoField parsing out of bound at Position: ")
            << mCrtIndex << ": " << mData);
		throw e;
    }

	if (mData[mCrtIndex++] != ':')
	{
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing SoField syntax error (expecting :)")
            << " at Position: " << mCrtIndex-1 
			<< ": " << mData);
		throw e;
	}

	value = mData[mCrtIndex++];
	return *this;
}


OmnSerialFrom &
OmnSerialFrom::operator >> (bool &value)
{
	char c;
	return operator >> (c);
}


OmnSerialFrom &
OmnSerialFrom::operator >> (float &value)
{
    //
    // The next field is a float. A float is stored in the form:
    //
    //  :ff...f
    //
    // If it contains any error, it throws an exception.
    //

    if (mCrtIndex + (int)sizeof(float) + 1 > mDataLength)
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("SoField parsing out of bound at Position: ")
            << mCrtIndex << ": " << mData);
		throw e;
    }

    if (mData[mCrtIndex++] != ':')
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing SoField syntax error (expecting :)")
            << " at Position: " << mCrtIndex-1 
			<< ": " << mData);
		throw e;
    }

	value = 0;
	memcpy(&value, &mData[mCrtIndex], sizeof(float));
	mCrtIndex += sizeof(float);
	return *this;
}


OmnSerialFrom &
OmnSerialFrom::operator >> (OmnString &value)
{
	int len;
	char *str = retrieveStr(len);
	value.assign(str, len);
	return *this;
}


OmnSerialFrom &
OmnSerialFrom::operator >> (std::string &value)
{
	int len;
	char *str = retrieveStr(len);
	value.assign(str, len);
	return *this;
}


char *
OmnSerialFrom::retrieveStr(int &len)
{
    //
    // The next field is a string. A string is stored in the form:
    //
    //  :dd:cc...c
    //
    // If it contains any error, it throws an exception.
    //

    if (mCrtIndex + 3 > mDataLength)
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("SoField parsing out of bound at Position: ")
            << mCrtIndex << ": " << mData);
		throw e;
    }

    if (mData[mCrtIndex++] != ':')
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing SoField syntax error (expecting :)")
            << " at Position: " << mCrtIndex-1 
			<< ": " << mData);
		throw e;
    }

	len = 0;
	len = (0xff & mData[mCrtIndex]) + ((mData[mCrtIndex+1] << 8) & 0xff00);
	mCrtIndex += 2;

    if (mData[mCrtIndex++] != ':')
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing SoField syntax error (expecting :) at Position: ")
			<< mCrtIndex-1 << ": " << mData);
		throw e;
    }

	if (mCrtIndex + len > mDataLength)
	{
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("SoField parsing out of bound at Position: ")
            << mCrtIndex << ": " << mData);
		throw e;
    }

	mCrtIndex += len;
	return &mData[mCrtIndex-len];
}


/*
bool
OmnSerialFrom::operator >> (char *value)
{
	// 
	// This is the same as OmnString. The caller must make sure
	// 'value' has enough space for the contents.
	//
	OmnString str;
	if (!operator >> (str))
	{
		return false;
	}

	strncpy(value, str.getBuffer(), str.getLength());
	return true;
}
*/


OmnSerialFrom &
OmnSerialFrom::operator >> (OmnIpAddr &value)
{
    //
    //	Ip Address is stored as an int(IPv4). 
    //

	int v;
	operator >> (v);
	value.set(v);
	return *this;
}


OmnSerialFrom &
OmnSerialFrom::operator >> (int64_t &value)
{
	//
	// The next field are 8 bytes. 
	//
	//	:dddddddd
	//
	// If it contains any error, it throws an exception.
	//

	if (mCrtIndex + 9 > mDataLength)
	{
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing out of bound at Position: ")
            << mCrtIndex << ": " << mData);
		throw e;
	}

	if (mData[mCrtIndex++] != ':')
	{
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing syntax error (expecting :)")
            << " at Position: " << mCrtIndex-1 
			<< ": " << mData);
		throw e;
	}

	memcpy(&value, &mData[mCrtIndex], 8);
	mCrtIndex += 8;
	return *this;
}


OmnSerialFrom &
OmnSerialFrom::operator >> (u64 &value)
{
	//
	// The next field are 8 bytes. 
	//
	//	:dddddddd
	//
	// If it contains any error, it throws an exception.
	//

	if (mCrtIndex + 9 > mDataLength)
	{
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing out of bound at Position: ")
            << mCrtIndex << ": " << mData);
		throw e;
	}

	if (mData[mCrtIndex++] != ':')
	{
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing syntax error (expecting :)")
            << " at Position: " << mCrtIndex-1 
			<< ": " << mData);
		throw e;
	}

	memcpy(&value, &mData[mCrtIndex], 8);
	mCrtIndex += 8;
	return *this;
}


OmnSerialFrom &
OmnSerialFrom::operator >> (OmnDateTime &value)
{
    //
    // OmnDateTime is tored in strings.
	//
	OmnString tmp;
	operator >> (tmp);
 	value.set(tmp.getBuffer(), OmnDateTime::eYYYYMMDDHHMMSS);
	return *this;
}


OmnSerialFrom &
OmnSerialFrom::operator >> (OmnConnBuffPtr &buff)
{
    //
    // It retrieves an OmnConnBuff:
    //
    // :xx:bb...b
    //
    // from the current position, where 'xx' is the length of the
    // buffer and 'bb...' is the content of the buffer. 
    //
    if (mCrtIndex + 3 > mDataLength)
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Out of bound at Position: ")
            << mCrtIndex << ": " << mData);
		throw e;
    }

    if (mData[mCrtIndex++] != ':')
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing SoField syntax error (expecting :)")
            << " at Position: " << mCrtIndex-1 
			<< ": " << mData);
		throw e;
    }

	int len = 0;
	len = (0xff & mData[mCrtIndex]) + ((mData[mCrtIndex+1] << 8) & 0xff00);
	mCrtIndex += 2;

    if (mData[mCrtIndex++] != ':')
    {
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Parsing SoField syntax error (expecting :) at Position: ")
			<< mCrtIndex-1 << ": " << mData);
		throw e;
    }

	if (mCrtIndex + len > mDataLength)
	{
        OmnExcept e(OmnFileLine, OmnErrId::eOutOfBound, 
			OmnString("Field parsing out of bound at Position: ")
            << mCrtIndex << ": " << mData);
		throw e;
    }

	mCrtIndex += len;
	try
	{
		buff = OmnNew OmnConnBuff(&mData[mCrtIndex-len], len, false);
	}
	catch(...)
	{
	}
	return *this;
}


bool		
OmnSerialFrom::pop(const char memberTag, char *value)
{
	aos_not_implemented_yet;
	return false;
}


bool		
OmnSerialFrom::pop(const char memberTag, char &value)
{
	aos_not_implemented_yet;
	return false;
}


bool		
OmnSerialFrom::pop(const char memberTag, int &value)
{
	aos_not_implemented_yet;
	return false;
}


bool		
OmnSerialFrom::pop(const char memberTag, bool &value)
{
	aos_not_implemented_yet;
	return false;
}


bool		
OmnSerialFrom::pop(const char memberTag, uint &value)
{
	aos_not_implemented_yet;
	return false;
}


bool		
OmnSerialFrom::pop(const char memberTag, OmnString &value)
{
	aos_not_implemented_yet;
	return false;
}


bool		
OmnSerialFrom::pop(const char memberTag, OmnIpAddr &value)
{
	aos_not_implemented_yet;
	return false;
}


bool		
OmnSerialFrom::pop(const char memberTag, float &value)
{
	aos_not_implemented_yet;
	return false;
}


bool		
OmnSerialFrom::pop(const char memberTag, int64_t &value)
{
	aos_not_implemented_yet;
	return false;
}


bool		
OmnSerialFrom::pop(const char memberTag, u64 &value)
{
	aos_not_implemented_yet;
	return false;
}


bool		
OmnSerialFrom::pop(const char memberTag, OmnDateTime &value)
{
	aos_not_implemented_yet;
	return false;
}


bool		
OmnSerialFrom::pop(const char memberTag, OmnConnBuffPtr &buff)
{
	aos_not_implemented_yet;
	return false;
}


