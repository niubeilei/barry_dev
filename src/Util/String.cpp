////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: String.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/String.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Util1/MemMgr.h"
#include "Util1/DateTime.h"
#include "Util/UtUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

static int64_t sgInstances = 0;
static int64_t sgTotalInstances = 0;


OmnString::OmnString()
:
mFrontPoison(eFrontPoison),
mDataPtr(&mDefault[0]),
mMem(0),
mLen(0),
mMemSize(eDefaultSize),
mFlags(eAllowShrink),
mBackPoison(eBackPoison)
{
    mDefault[mLen] = 0;

	if (AosMemoryChecker::getCheckOpen())
	{
		__sync_fetch_and_add(&sgInstances, 1);
		__sync_fetch_and_add(&sgTotalInstances, 1);
	}
}


OmnString::OmnString(const OmnSimpleStr &str)
:
mFrontPoison(eFrontPoison),
mMem(0),
mLen(str.len),
mFlags(eAllowShrink),
mBackPoison(eBackPoison)
{
	init(str.mData, str.len);

	if (AosMemoryChecker::getCheckOpen())
	{
		__sync_fetch_and_add(&sgInstances, 1);
		__sync_fetch_and_add(&sgTotalInstances, 1);
	}
}


// 
// Convert std::string to OmnString
//
OmnString::OmnString(const std::string &rhs)
:
mFrontPoison(eFrontPoison),
mMem(0),
mLen(rhs.length()),
mFlags(eAllowShrink),
mBackPoison(eBackPoison)
{
	if (mLen < eDefaultSize)
	{
		mDataPtr = &mDefault[0];
		mMemSize = eDefaultSize;
	}
	else
	{
       	mMem = OmnNew char[mLen+5];
		mMemSize = mLen;
		mDataPtr = mMem;
	}

   	memcpy(mDataPtr, rhs.data(), mLen);
   	mDataPtr[mLen] = 0;

	if (AosMemoryChecker::getCheckOpen())
	{
		__sync_fetch_and_add(&sgInstances, 1);
		__sync_fetch_and_add(&sgTotalInstances, 1);
	}
}


//
// Copy constructor
//
OmnString::OmnString(const OmnString &rhs)
:
mFrontPoison(eFrontPoison),
mMem(0),
mLen(rhs.mLen),
mFlags(eAllowShrink),
mBackPoison(eBackPoison)
{
	if (mLen < eDefaultSize)
	{
		memcpy(mDefault, rhs.mDataPtr, mLen);
		mDefault[mLen] = 0;
		mDataPtr = &mDefault[0];
		mMemSize = eDefaultSize;
	}
	else
	{
		mMem = OmnNew char[mLen+5];
        memcpy(mMem, rhs.mDataPtr, mLen);
		mMemSize = mLen;
        mMem[mLen] = 0;
		mDataPtr = mMem;
    }

	if (AosMemoryChecker::getCheckOpen())
	{
		__sync_fetch_and_add(&sgInstances, 1);
		__sync_fetch_and_add(&sgTotalInstances, 1);
	}
}


// 
// Allocate a string with char 'c' repeated 'n' times.
//
OmnString::OmnString(const int n, const char c, const bool flag)
:
mFrontPoison(eFrontPoison),
mMem(0),
mLen(0),
mFlags(eAllowShrink),
mBackPoison(eBackPoison)
{
	// It creates a string with 'n' length. If 'flag' is true, 
	// it will initialize the string with 'c'. Otherwise, the 
	// string contents are not initialized.

	int size = n;
	// Make sure the size is not negative. If it is, set to 0.
	if (size < 0)
	{
		OmnAlarm << "String length passed in is negative: " 
			<< size << ". Reset to 0." << enderr;
		size = 0;
	}

	// Make sure the size is not too big.
	if (size > eMaxStrLen)
	{
		OmnAlarm << "String length passed in too long. Reset to: "
			<< eMaxStrLen << enderr;
		size = eMaxStrLen;
	}

	mLen = size;
		
	if (size < eDefaultSize)
	{
		mDataPtr = &mDefault[0];
		mMemSize = eDefaultSize;
	}
	else
	{
    	// mMem = OmnMemMgr::getSelf()->allocate(size+5, mMemPool, __FILE__, __LINE__);
		// aos_assert(mMemPool);
    	mMem = OmnNew char[size+5];
		mDataPtr = mMem;
		mMemSize = size;
	}

    if (flag) memset(mDataPtr, c, mLen);
    mDataPtr[mLen] = 0;

	if (AosMemoryChecker::getCheckOpen())
	{
		__sync_fetch_and_add(&sgInstances, 1);
		__sync_fetch_and_add(&sgTotalInstances, 1);
	}
}


OmnString::OmnString(const char *d)
:
mFrontPoison(eFrontPoison),
mMem(0),
mLen(0),
mFlags(eAllowShrink),
mBackPoison(eBackPoison)
{
    if (!d)
    {
        mLen = 0;
		mDataPtr = &mDefault[0];
		mMemSize = eDefaultSize;
    }
    else
    {
        mLen = strlen(d);
		if (mLen < eDefaultSize)
		{
			mDataPtr = &mDefault[0];
			mMemSize = eDefaultSize;
		}
		else
		{
        	// mMem = OmnMemMgr::getSelf()->allocate(mLen+5, mMemPool, __FILE__, __LINE__);
			// aos_assert(mMemPool);
        	mMem = OmnNew char[mLen+5];
			mDataPtr = mMem;
			mMemSize = mLen;
		}

    	memcpy(mDataPtr, d, mLen);
    	mDataPtr[mLen] = 0;
    }

	if (AosMemoryChecker::getCheckOpen())
	{
		__sync_fetch_and_add(&sgInstances, 1);
		__sync_fetch_and_add(&sgTotalInstances, 1);
	}
}


//
// Construct a string with length 'size' and copy the contents
// 'd' to it.
//
OmnString::OmnString(const char *d, const int size)
:
mFrontPoison(eFrontPoison),
mMem(0),
mLen(size),
mFlags(eAllowShrink),
mBackPoison(eBackPoison)
{
	init(d, size);

	if (AosMemoryChecker::getCheckOpen())
	{
		__sync_fetch_and_add(&sgInstances, 1);
		__sync_fetch_and_add(&sgTotalInstances, 1);
	}
}


void
OmnString::init(const char *d, const int size)
{
	//
	// Make sure the size is not negative. If it is, set to 0.
	//
	if (size < 0)
	{
		OmnAlarm << "String length passed in is negative: " 
			<< mLen << ". Reset to 0." << enderr;
		mLen = 0;
	}

	// 
	// Make sure the size is not too big.
	//
	if (mLen > eMaxStrLen)
	{
		OmnAlarm << "String length passed in too long: "
			<< mLen << ". Reset to: "
			<< eMaxStrLen << enderr;
		mLen = eMaxStrLen;
	}

	if (mLen < eDefaultSize)
	{
		mDataPtr = &mDefault[0];
		mMemSize = eDefaultSize;
	}
	else
	{
    	// mMem = OmnMemMgr::getSelf()->allocate(mLen+5, mMemPool, __FILE__, __LINE__);
		// aos_assert(mMemPool);
    	mMem = OmnNew char[mLen+5];
		mDataPtr = mMem;
		mMemSize = mLen;
	}

    if (d)
    {
       memcpy(mDataPtr, d, mLen);
    }
	else
	{
		mLen = 0;
	}
    mDataPtr[mLen] = 0;
}


OmnString::~OmnString()
{
	// if (mMem) mMemPool->release(mMem, __FILE__, __LINE__);
	mFrontPoison = eBackPoison;
	mBackPoison = eFrontPoison;

	if (mMem) OmnDelete[] mMem;
	mMem = 0;

	if (AosMemoryChecker::getCheckOpen())
	{
		__sync_fetch_and_sub(&sgInstances, 1);
	}
}

int
OmnString::visibleCharLen() const
{
	int i = 0;
	int count = 0;
	while(i < mLen)
	{
		if((u8)mDataPtr[i] > 31 && mDataPtr[i] != 127)
		{
			count++;
		}
		i++;
	}
	return  count;
}

bool
OmnString::determineMemory(const uint the_newsize)
{
	// 
	// This function determines the new size of the string: 'the_newsize'. Note that
	// this class uses two types of memory. If the size is under 'eDefaultSize', 
	// it uses 'char mDefault[eDefaultSize]'. Otherwise, it uses a memory 
	// allocated by mMem. 
	if (the_newsize < (u32)mMemSize) return true;
	if (mMemSize == eMaxStrLen) 
	{
		OmnExcept e(OmnFileLine, OmnErrId::eBuffTooBig, OmnString("String too big!"));
	}

	if (the_newsize < eDefaultSize)
	{
		// 
		// The new size is not larger than the default size. Use the
		// default size.
		//
		if (!allowShrink())
		{
			// 
			// Don't shrink.
			//
			return true;
		}

		if (mMem)
		{
			memcpy(mDefault, mMem, the_newsize);
			// mMemPool->release(mMem, __FILE__, __LINE__);
			OmnDelete [] mMem;
			mMem = 0;
			// mMemPool = 0;
			mDataPtr = &mDefault[0];
			mMemSize = eDefaultSize;
		}

		mDataPtr[the_newsize] = 0;
		return true;
	}

	u32 newsize = mLen + mLen;
	if (newsize < the_newsize) newsize = the_newsize;
	if (newsize < 300) newsize = 300;
	if (newsize >= eMaxStrLen)
	{
		newsize = eMaxStrLen;
        //cout << __FILE__ << ":" << __LINE__
        //   << "********** String too long: " << newsize << endl;
		//return false;
	}

	// Chen Ding, 2013/03/27
	aos_assert_r(newsize >= the_newsize, false);

	// 
	// The new size is larger than the default size.
	//
	if (mMem)
	{
		if (newsize < (u32)mMemSize) return true;

		// Need to allocate a new memory
		// AosMemPoolPtr newpool;
    	// char *mem = OmnMemMgr::getSelf()->allocate(newsize+eIncreaseSize+5, newpool, __FILE__, __LINE__);
    	char *mem = OmnNew char[newsize+eIncreaseSize+5];
		aos_assert_r(mem, false);
		// aos_assert_r(newpool, false);
		memset(mem, 0, newsize+eIncreaseSize+5);			// Ketty 2013/07/22
		memcpy(mem, mMem, mLen);
		// mMemPool->release(mMem, __FILE__, __LINE__);
		// mMemPool = newpool;
		OmnDelete [] mMem;
		mMem = mem;

		// Chen Ding, 2013/08/03
		// mMem[newsize] = 0;
		mMem[mLen] = 0;
		mMemSize = newsize + eIncreaseSize;
		mDataPtr = mMem;

		// if (!mMemPool->checkMemory(mMem, newsize))
		// {
		// 	cout << __FILE__ << ":" << __LINE__
		// 		<< "********** String too long: " << newsize << endl;
		// 	return false;
		// }

		return true;
	}

	// Need to allocate a new memory
    // mMem = OmnMemMgr::getSelf()->allocate(newsize+5, mMemPool, __FILE__, __LINE__);
	// aos_assert_r(mMemPool, false);
    mMem = OmnNew char[newsize+5];
	memset(mMem, 0, newsize+5);			// Ketty 2013/07/22
	memcpy(mMem, mDefault, mLen);
	mMem[newsize] = 0;
	mMemSize = newsize;
	mDataPtr = mMem;
	return true;
}
	

OmnString & 
OmnString::operator += (const char *rhs)
{
    if (!rhs)
    {
        return *this;
    }

    unsigned int len = strlen(rhs);
	if (len == 0)
	{
		return *this;
	}

	if (mLen + len >= (u32)mMemSize) 
	{
		bool rslt = determineMemory(mLen + len);
		aos_assert_r(rslt, *this);
	}
    memcpy(&mDataPtr[mLen], rhs, len);
	mLen += len;
	mDataPtr[mLen] = 0;
    return *this;
}


OmnString & 
OmnString::operator += (const OmnString &rhs)
{
	// Chen Ding, 2013/08/03
	if (rhs.mLen <= 0) return *this;

	if (mLen + rhs.mLen + 1>= (u32)mMemSize) 
	{
		bool rslt = determineMemory(mLen + rhs.mLen+1);
		aos_assert_r(rslt, *this);
	}
	// if (rhs.mLen <= 0) return *this;
    memcpy(&mDataPtr[mLen], rhs.mDataPtr, rhs.mLen);
	mLen += rhs.mLen;
	mDataPtr[mLen] = 0;
    return *this;
}


OmnString & 
OmnString::operator += (const std::string &rhs)
{
	if (mLen + rhs.length() >= (u32)mMemSize) 
	{
		bool rslt = determineMemory(mLen + rhs.length());
		aos_assert_r(rslt, *this);
	}
    memcpy(&mDataPtr[mLen], rhs.data(), rhs.length());
	mLen += rhs.length();
	mDataPtr[mLen] = 0;
    return *this;
}

OmnString &
OmnString::operator = (const OmnSimpleStr &rhs)
{
	if (!rhs.mData)
	{
		// 
		// Chen Ding, 02/12/2007
		// We should  allow one to assign 0 to an OmnString, such as:
		// OmnString str;
		// str = 0;
		//
		// OmnAlarm << "null pointer" << enderr;
		*this = "";
		return *this;
	}

	if (mMem)
	{
		// mMemPool->release(mMem, __FILE__, __LINE__);
		OmnDelete [] mMem;
		mMem = 0;
		// mMemPool = 0;
	}
	
	mLen = rhs.len;
	if (mLen > eMaxStrLen)
    {
       OmnAlarm << "String length passed in too long: "
          << mLen << ". Reset to: "
          << eMaxStrLen << enderr;
       mLen = eMaxStrLen;
    }

    if (mLen < eDefaultSize)
    {
        mDataPtr = &mDefault[0];
		mMemSize = eDefaultSize;
    }
    else
    {
        // mMem = OmnMemMgr::getSelf()->allocate(mLen+5, mMemPool, __FILE__, __LINE__);
		// aos_assert_r(mMemPool, *this);
        mMem = OmnNew char[mLen+5];
		mMemSize = mLen;
        mDataPtr = mMem;
    }

	memcpy(mDataPtr, rhs.mData, mLen);
	mDataPtr[mLen] = 0;
	return *this;
}

OmnString & 
OmnString::operator = (const std::string &rhs)
{
	assign(rhs.data(), rhs.length());
    return *this;
}


OmnString & 
OmnString::assign(const char * const data, const int len)
{ 
	// 
	// This function overrides the existing contents with 
	// the new one ('data', len')
	//
	// Ken Lee, 2013/07/17
	if (len < 0)
	{
        cout << __FILE__ << ":" << __LINE__
            << "********** Invalid length: " << len << endl;
		return *this;
	}
	
	if (len == 0)
	{
		mLen = 0;
		return *this;
	}

    if (!data)
    {
		cout << __FILE__ << ":" << __LINE__
             << "********** Null pointer passed in!" << endl;
		return *this;
    }

	uint newLen = len;
    if (newLen <= eDefaultSize)
    {
		if (mMem && allowShrink())
		{
			// mMemPool->release(mMem, __FILE__, __LINE__);
			OmnDelete [] mMem;
			mMem = 0;
			// mMemPool = 0;

			memcpy(mDefault, data, newLen);
			mLen = newLen;
			mDefault[mLen] = 0;
			mMemSize = eDefaultSize;
			mDataPtr = &mDefault[0];
		}
		else
		{
			// 
			// Just use the current memory.
			//
			memcpy(mDataPtr, data, newLen);
			mLen = newLen;
			mDataPtr[mLen] = 0;
		}

		return *this;
	}

	if (newLen > eMaxStrLen)
	{
		OmnAlarm << "String length passed in too long: "
			<< newLen << ". Reset to: "
			<< eMaxStrLen << enderr;
		newLen = eMaxStrLen;
	}

	if (newLen >= (u32)mMemSize) 
	{
		bool rslt = determineMemory(newLen);
		aos_assert_r(rslt, *this);
	}
	memcpy(mDataPtr, data, newLen);
	mLen = newLen;
	mDataPtr[mLen] = 0;

	return *this;
}


OmnString & 
OmnString::assign(const uint n, const char c)
{
	uint numChars = n;
	if (numChars > eMaxStrLen)
	{
		OmnAlarm << "String length passed in too long: "
			<< numChars << ". Reset to: "
			<< eMaxStrLen << enderr;
		numChars = eMaxStrLen;
	}
	
	if (numChars >= (u32)mMemSize) 
	{
		bool rslt = determineMemory(numChars);
		aos_assert_r(rslt, *this);
	}
    mLen = numChars;
    memset(mDataPtr, c, mLen);
    mDataPtr[mLen] = 0;
    return *this;
}


// 
// Chen Ding, 02/20/2007, Changed the function to return boolean
// 
bool
OmnString::replace(const u32 startPos, const u32 length, const OmnString &str)
{
	//
	// It replaces 'length' characters of this string starting at 'startPos', 
	// with the string 'str'. In other word, if 'length' is 0, it is 
	// the same as inserting.
	//
	int len = str.length();
	uint index = 0;
	uint pos = startPos;
	if (pos < 0 || pos >= mLen)
	{
        cout << __FILE__ << ":" << __LINE__
            << "********** Program error: Invalid pos: " << startPos << endl;
		return false;
	}
		
	if (length > 0)
	{	
		if ((int)length <= len)
		{
			//
			// Need to verride 'length' characters at position 'pos'. 
			//
			memcpy(&mDataPtr[pos], str.mDataPtr, length);
			pos += length;
			len -= length;
			index = length;
		}
		else
		{
			//
			// This means there are less to replace with
			//
			memcpy(&mDataPtr[pos], str.mDataPtr, len);
			index = pos + length;
			pos += len;
			len -= length;	
		}
	}

	//
	// At this point:
	// 	pos: the position right after the last replaced byte. Either
	//  	 deletion or insertion should start from this position.
	//  len: the number of bytes in difference. In other word, if 
	//       positive, the number of bytes to insert at position 'pos'.
	//		 If negative, the number of bytes to delete at position 'pos'.
	//  index: if len > 0, the starting position in 'str' to insert.
	//         if len < 0, the starting position in 'this' that holds
	//  	   valid value after the replaced segment. 
	//
	// Handled the replacing part. Need to either insert or delete
	//
	if (len == 0)
	{
		//
		// There is nothing to do
		//
		return true;
	}

	if (mLen+len > eDefaultSize) 
	{
	    
		bool rslt = determineMemory(mLen + len);
		aos_assert_r(rslt, false);
	}

	if (len > 0)
	{
		//
		// Need to insert 
		//

		//
		// Insert it now. First, shift the tailing characters
		//
		// Chen Ding, 03/02/2007
		// for (uint i=mLen-1; i>=pos; i--)
		// {
		// 	mDataPtr[i+len] = mDataPtr[i];
		// }
		memmove(&mDataPtr[pos+len], &mDataPtr[pos], mLen - pos);

		// 
		// Now ready to insert the new contents
		// Chen Ding, 03/02/2007
		//
		// for (int j=0; j<len; j++)
		// {
		// 	mDataPtr[j+pos] = str.mDataPtr[j + index];
		// }
		memmove(&mDataPtr[pos], &str.mDataPtr[index], len);

		mLen += len;
		mDataPtr[mLen] = 0;
		return true;
	}

	//
	// Otherwise, len < 0. Need to delete some
	//
	memmove(&mDataPtr[pos], &mDataPtr[index], mLen - index);
	mLen += len;

	// 
	// Chen Ding, 03/02/2007
	//
	// for (uint i=pos; i<mLen; i++) 
	// {
	// 	mDataPtr[i] = mDataPtr[index++];
	// }

	mDataPtr[mLen] = 0;
	if (mLen <= eDefaultSize) 
	{
		bool rslt = determineMemory(mLen);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool        
OmnString::retrieveInt(const u32 startIndex, u32 &length, int &value)
{
	//
	// Starting from position 'startIndex', it retrieves an integer
	// from the string. If successful, it returns true. 'length' 
	// is the number of characters consumed, and 'value' is the
	// value. If failed (i.e., the first printable character is
	// not a digit), it returns false.
	//
	if (startIndex >= mLen)
	{
		OmnAlarmProgError
			<< "Trying to retrieve an integer from string: " 
			<< data() 
			<< " but start index: " 
			<< startIndex 
			<< " is out of bound!" << enderr;
		return false;
	}

	//
	// Skip the white spaces
	//
	bool isMinus = false;
	u32 index = startIndex;
	char c;
	while (index < mLen && (c = mDataPtr[index]) &&
			(c == ' ' || c == '\t' || c == '\n')) index++;

	//
	// Check the first character to see whether it is a sign
	// character: +/-, or '.'
	//
	if (mDataPtr[index] == '+' || 
		mDataPtr[index] == '.')
	{
		index++;
	}

	if(mDataPtr[index] == '-')
	{
		if(isMinus)
		{
			// two '-', it's an error
			return false;
		}
		isMinus = true;
		index++;
	}

	if (index >= mLen)
	{
		//
		// No integer found
		//
		return false;
	}

	u32 start = index;
	while (index < mLen)
	{
		if (mDataPtr[index] < '0' || mDataPtr[index] > '9')
		{
			break;
		}
		
		index++;
	}

	if (index == start)
	{
		//
		// No integer found
		//
		return false;
	}

	//
	// Otherwise, it found an integer and 'index' currently points to 
	// the first non-digit character.
	//
	length = index - startIndex;
	value = atoi(&mDataPtr[start]);
	if(isMinus)
	{
		value = -value;
	}
	return true;
}

bool        
OmnString::retrieveInt(const uint startIndex, int &length, int &value)
{
	//
	// Starting from position 'startIndex', it retrieves an integer
	// from the string. If successful, it returns true. 'length' 
	// is the number of characters consumed, and 'value' is the
	// value. If failed (i.e., the first printable character is
	// not a digit), it returns false.
	//
	u32 startIndex_tmp;
	u32 length_tmp;
	int value_tmp;
	bool ret;

	startIndex_tmp = (u32)startIndex;
	length_tmp = (u32)length;
	value_tmp = value;
	
	ret = retrieveInt(startIndex_tmp, length_tmp, value_tmp);

	length =  (int)length_tmp;
	value = value_tmp;
	
	return ret;
}


OmnString &
OmnString::operator = (const OmnString &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	if (mMem)
	{
		// mMemPool->release(mMem, __FILE__, __LINE__);
		OmnDelete [] mMem;
		mMem = 0;
		// mMemPool = 0;
	}

	mFlags = rhs.mFlags;
	mLen = rhs.mLen;
	if (rhs.mMem)
	{
		mMemSize = rhs.mMemSize;
		// mMem = OmnMemMgr::getSelf()->allocate(mMemSize+5, mMemPool, __FILE__, __LINE__);
		mMem = OmnNew char[mMemSize+5];
		aos_assert_r(mMem, *this);
		// aos_assert_r(mMemPool, *this);
		mDataPtr = mMem;
		memcpy(mMem, rhs.mMem, mLen);
		mDataPtr[mLen] = 0;
		return *this;
	}

	//
	// Otherwise, it should use the default memory
	//
	memcpy(mDefault, rhs.mDefault, mLen);
	mDataPtr = &mDefault[0];
	mDataPtr[mLen] = 0;
	mMemSize = eDefaultSize;
	return *this;
}


// 
// Chen Ding, 05/28/2005
//
OmnString &
OmnString::operator = (const char *rhs)
{
	if (!rhs)
	{
		// 
		// Chen Ding, 02/12/2007
		// We should  allow one to assign 0 to an OmnString, such as:
		// OmnString str;
		// str = 0;
		//
		// OmnAlarm << "null pointer" << enderr;
		*this = "";
		return *this;
	}

	if (mMem)
	{
		// mMemPool->release(mMem, __FILE__, __LINE__);
		OmnDelete [] mMem;
		mMem = 0;
		// mMemPool = 0;
	}

	mLen = strlen(rhs);
	if (mLen > eMaxStrLen)
    {
       OmnAlarm << "String length passed in too long: "
          << mLen << ". Reset to: "
          << eMaxStrLen << enderr;
       mLen = eMaxStrLen;
    }

    if (mLen < eDefaultSize)
    {
        mDataPtr = &mDefault[0];
		mMemSize = eDefaultSize;
    }
    else
    {
        // mMem = OmnMemMgr::getSelf()->allocate(mLen+5, mMemPool, __FILE__, __LINE__);
        mMem = OmnNew char[mLen+5];
		// aos_assert_r(mMemPool, *this);
		mMemSize = mLen;
        mDataPtr = mMem;
    }

	memcpy(mDataPtr, rhs, mLen);
	mDataPtr[mLen] = 0;
	return *this;
}


void
OmnString::hexStringToStr(const char *data, const int len)
{
	(*this) = OmnString("");
	const unsigned char * curDataPtr = (unsigned char*)data;
	uint curData;
	char buff[10];
	int i;
	OmnString tempStr;
	for (i = 0;i < len;i ++)
	{			
		curData = *curDataPtr;
		//
		// add hex number, the last one do not have a space char ' '
		//			
		sprintf(buff,"%x",curData);
		tempStr = OmnString(buff);

		if(tempStr.length() == 1)
		{
			OmnString aa("0");
			tempStr = aa << tempStr;
		}


		if(i < len-1)
		{
			tempStr += " ";
		}
		else
		{
		}
	
		//
		// add the this string
		//
		
		(*this) +=  tempStr;
										
		//
		// jump next
		//														
		
		curDataPtr++;
	}
}																		


OmnString & 
OmnString::append(const char *data, const u32 len)
{ 
	// 
	// It appends 'len' number of characters from 'data'
	// to this class. If 'len' makes this instance too
	// long, only the allowed portion is appended. 
	// 
	// IMPORTANT: if 'len' is longer than 'data', 
	// some gabbage contents may be appended to
	// this instance. 
	//
	u32 length = len;

	if (length + mLen > eMaxStrLen)
	{
		OmnAlarm << "String length passed in too long: "
			<< length << ". Reset to: "
			<< eMaxStrLen << enderr;
		length = eMaxStrLen - mLen;
	}

	int newLength = mLen + length;
    if (newLength <= eDefaultSize && !mMem)
    {
		memcpy(&mDefault[mLen], data, length);
		mLen = newLength;
		mDefault[mLen] = 0;
		mDataPtr = &mDefault[0];
		mMemSize = eDefaultSize;
		return *this;
	}

	if (newLength >= mMemSize) 
	{
		bool rslt = determineMemory(newLength);
		aos_assert_r(rslt, *this);
	}
	memcpy(&mDataPtr[mLen], data, length);
	mLen = newLength;
	mDataPtr[mLen] = 0;
	return *this;
}


OmnString & 
OmnString::appendChar(const char data)
{ 
	if (1+ mLen > eMaxStrLen)
	{
		OmnAlarm << "String length passed in too long: "
			<< mLen << ". Reset to: "
			<< eMaxStrLen << enderr;
	}

	int newLength = mLen + 1;
    if (newLength <= eDefaultSize && !mMem)
    {
		mDefault[mLen] = data;
		mLen ++;
		mDefault[mLen] = 0;
		mDataPtr = &mDefault[0];
		mMemSize = eDefaultSize;
		return *this;
	}

	if (newLength >= mMemSize) 
	{
		bool rslt = determineMemory(newLength);
		aos_assert_r(rslt, *this);
	}
	mDataPtr[mLen] = data;
	mLen ++;
	mDataPtr[mLen] = 0;
	return *this;
}


OmnString & 
OmnString::trimLastChars(const unsigned int numOfChar)
{
	if(mLen >= numOfChar)
	{
		mLen -= numOfChar;
		mDataPtr[mLen] = 0;
	}
	return *this;
}



OmnString &
OmnString::operator << (const OmnDateTime &rhs)
{
	*this << rhs.toString();
	return *this;
}

		
bool 
OmnString::remove(const uint starting, const int length)
{
	// This function removes the contents from the starting position.
	if (starting < 0 || starting >= mLen)
	{
		OmnAlarm << "Invalid starting position: " 
			<< starting << enderr;
		return false;
	}

	if (length < 0 || starting + length > mLen)
	{
		OmnAlarm << "Invalid length: " << length << enderr;
		return false;
	}

	memcpy(&mDataPtr[starting], &mDataPtr[starting+length], mLen-starting-length);
	mLen -= length;
	mDataPtr[mLen] = 0;
	return determineMemory(mLen);
}


OmnString 
OmnString::addSlashes() const
{
	OmnString s;
	for (uint i=0; i<mLen; i++) 
	{
		if (mDataPtr[i] == '\0')
		{
			s << '\\' << '0';
		}
	    else if (mDataPtr[i] == '\\')
		{
			s << '\\' << mDataPtr[i];
		}
		else if (mDataPtr[i] == '\'')
		{
			s << '\'' << mDataPtr[i];
		}
		else {
			s << mDataPtr[i];
		}
	}
	return s;
}


void
OmnString::chgBackSlashes()
{
	for (uint i=0; i<mLen; i++) 
	{
		if (mDataPtr[i] == '\\')
		{
			mDataPtr[i] = '/';
		}
	}
}


OmnString
OmnString::trimLastWord()
{
	// 
	// Starting from the end, find the first space
	//
	int index = mLen-1;
	while (index >= 0 && mDataPtr[index] != ' ') index--;

	if (index < 0)
	{
		return "";
	}

	// 
	// Find the next non-space character
	//
	while (index >= 0 && mDataPtr[index] == ' ') index--;
	if (index < 0)
	{
		return "";
	}

	OmnString str(mDataPtr, index+1);
	return str;
}


int
OmnString::getSegment(unsigned int order, const char token, OmnString &substr) const
{
	unsigned int curPos=0;
	unsigned int startPos=0;

	if( order<1 )
	{
		substr="";
		return false;
	}

	while( order>1 && curPos<mLen )
	{
		if( mDataPtr[curPos]==token )
			order--;
		curPos++;
	}
	
	if( order>1 )
		return false;


	startPos = curPos;
	while( curPos<mLen && mDataPtr[curPos]!=token )
	{
		curPos++;
	}

	substr.assign( mDataPtr+startPos, curPos-startPos );

	return true;
}



int
OmnString::getWord(const unsigned int index,OmnString &substr) const
{
	// 
	// Starting from the end, find the first space
	//
	if(index >= mLen)
	{
		substr = "";
		return -1;
	}
	unsigned int curPos = index;

	while ( curPos < mLen && 
			( mDataPtr[curPos] == ' '
			  || mDataPtr[curPos] == '\t' 
			  || mDataPtr[curPos] == '\r' 
			  || mDataPtr[curPos] == '\n' 
			  )) 
	{
		curPos++;
	}

	if (curPos >= mLen)
	{
		substr = "";
		return curPos;
	}

	unsigned int startPos = curPos;
	// 
	// Find the next non-space character
	//
	//while (curPos < mLen && (!(mDataPtr[curPos] == ' ')) && (!(mDataPtr[curPos] == '\t'))) curPos++;

	while ( curPos < mLen 
			&& mDataPtr[curPos] != ' '
			&& mDataPtr[curPos] != '\t' 
			&& mDataPtr[curPos] != '\r' 
			&& mDataPtr[curPos] != '\n' 
			  ) 
	{
		curPos++;
	}

	substr.assign(mDataPtr+startPos,curPos-startPos);
	return curPos;
}

// add by Kevin Yang
bool OmnString::readBlock(const OmnString &start, const OmnString &end, OmnString &buffer, bool &err)
{
	// 
	// This function reads a block. The block starts
	// with a line that matches 'start' and ends with
	// a line that matches 'end'. Anything in between
	// is retrieved into 'contents'.
	//
	
	unsigned int len;
	unsigned int i=0;
	unsigned int index = 0;
	unsigned int startPos = 0;
	bool		isSame = true;
	char c;
	OmnString tmpStr;
	OmnString tmp;
	unsigned int bytesRead = 0;
	bool containPattern = false;

	len = strlen(start.data());
	while (1)
	{
		c = mDataPtr[i];
		i++;
		if (c == 0)
		{
			return false;
		}
		tmpStr.appendChar(c);
		// if length enough , check whether it matchs.
		if(startPos + len <= (unsigned int)tmpStr.length())
		{
			
			index = 0;
			isSame = true;
			while(index < len)
			{
				if (! (*(tmpStr.getBuffer() + startPos + index) == *(start.data() + index)))
				{
					isSame = false;
					break;
				}
				index ++;
			}
			if(isSame)
			{
				break;
				// find it and return
			}
			startPos ++;
		}
	}

	index = 0;
	isSame = true;
	startPos = 0;
	len = strlen(end.data());
	while (i < mLen)
	{
		c = mDataPtr[i];
		i++;
		if (c == 0)
		{
			return false;
		}
		buffer << c;
		// Compare whether it match the pattern		
		if(startPos + len <= (unsigned int)buffer.length())
		{
			index = 0;
			isSame = true;
			while(index < len)
			{
				if (! (*(buffer.getBuffer() + startPos + index) == *(end.data() + index)))
				{
					isSame = false;
					break;
				}
				index ++;
			}
			if(isSame)
			{
				// find it and return
				if (!containPattern)
				{
					buffer.trimLastChars(len);
				}
				return true;
			}
			startPos ++;			
		}

		bytesRead ++;

	}


//	beginIndex = skipTo(start.data());
//	readUtil(contents, end, beginIndex, 100000, false);
	if (buffer == 0)
	{
		err = false;
		return false;
	}

	return true;
}

int
OmnString::numWords() const
{
	int num = 0;
	uint index = 0;
	while (index < mLen)
	{
		while (index < mLen && mDataPtr[index] != ' ') index++;
		if (index >= mLen)
		{
			return num+1;
		}

		while (index < mLen && mDataPtr[index] == ' ') index++;
		num++;
	}

	return num;
}


void
OmnString::removeLeadingWhiteSpace()
{
	u32 idx = 0;
	while (idx < mLen)
	{
		if (mDataPtr[idx] != ' ' &&
			mDataPtr[idx] != '\n' &&
			mDataPtr[idx] != '\t' &&
			mDataPtr[idx] != '\r')
		{
			break;	
		}

		idx++;
	}

	if (idx > 0)
	{
		mLen -= idx;
		if (mLen > 0)
		{
			memmove(mDataPtr, &mDataPtr[idx], mLen);
		}
		mDataPtr[mLen] = 0;
	}
}


void
OmnString::removeTailWhiteSpace()
{
	while (mLen > 0)
	{
		if (mDataPtr[mLen-1] == ' ' ||
			mDataPtr[mLen-1] == '\n' ||
			mDataPtr[mLen-1] == '\t' ||
			mDataPtr[mLen-1] == '\r')
		{
			mLen--;
		}
		else
		{
			mDataPtr[mLen] = '\0';
			return;
		}
	}
}		


// Young Pan, 2013/08/07
void 
OmnString::replaceStrWithSpace()
{
	removeWhiteSpaces();
	if (mLen == 0) return;
	
	bool flag = false;
	OmnString newStr = "";
	for (u32 i=0; i<mLen; i++)         
	{                                          
		if (mDataPtr[i] == ' '  ||                  
			mDataPtr[i] == '\n' ||                  
			mDataPtr[i] == '\t' ||                  
			mDataPtr[i] == '\r')                    
		{                                      
			if (flag) continue;          
			flag = true;
			newStr << " ";                     
		}                                      
		else                                   
		{                                      
			flag= false;                         
			newStr << mDataPtr[i];                  
		}                                      
	}                                          
	mLen = newStr.length();
	memcpy(mDataPtr, newStr.data(), mLen);
	mDataPtr[mLen] = 0;
}


void
OmnString::removeTailSymbols()
{
	while (mLen > 0)
	{
		if (isSymbol(mDataPtr[mLen-1]))
		{
			mLen--;
		}
		else
		{
			return;
		}
	}
}		


bool
OmnString::isSymbol(const char c)
{
	if ((c >= 33 && c <= 47) || 
		(c >= 58 && c <= 64) || 
		(c >= 91 && c <= 96)) 
	return true; 

	if (c >= 123 && c <= 126) return true;
	return false;
}


//
// Find Index of the subStr from this string, if not found return negative integer. 
// Otherwise, it returns the starting index of the substring.
//
int 
OmnString::findSubStrStatic(
		const char *data, 
		const int data_len,
		const char *subStr,
		const int sub_len,
		const int startPos, 
		const bool reverse)
{
	if (!data || !subStr) return -1;
	if (startPos >= data_len) return -1;
	if (!reverse)
	{
		if ((startPos + sub_len) > data_len)
		{
			// The substring is longer than what is remained. 
			return -1;
		}

		// Chen Ding, 04/03/2010
		const char *ptr = strstr(&data[startPos], subStr);
		if (!ptr) return -1;
		return ptr - data;
	}

	// Starting from 'startPos', it searches backwards.
	int start = startPos;
	if (start >= data_len || start == -1) start = data_len-1;
	while (start >= sub_len)
	{
		if (data[start-sub_len+1] == subStr[0])
		{
			int subidx = 1;
			int idx = start-sub_len+2;
			while (subidx < sub_len && data[idx] == data[subidx])
			{
				idx++;
				subidx++;
			}

			if (subidx == sub_len) return start-sub_len+1;
		}
		start--;
	}
	return -1;
}


bool
OmnString::insert(const OmnString &str,const int insertPos)
{
	// This function inserts 'str' before the position 'insertPos'. 
	// This means that we cannot insert 'str' to the end of
	// the string. Appending contents can be done very easily
	// by:
	// 		str << another_str
	//
	// Chen Ding, 08/13/2011
	// If it is an empty string, inserting 'str' at Position 0
	// is allowed.
	//
	// In addition, if insertPos is mLen, it means to append.
	//
if (insertPos < 0 || (u32)insertPos > mLen)
	OmnMark;
	aos_assert_r(insertPos >= 0 && (u32)insertPos <= mLen, false);

	// Chen Ding, 08/13/2011
	if ((u32)insertPos == mLen)
	{
		// It is to append. 
		*this << str;
		return true;
	}

	int subStrLen = str.length();

	int newLen = mLen + subStrLen;
	if (newLen > eMaxStrLen)
	{
		OmnAlarm << "String length passed in too long: "
			<< newLen << ". Reset to: "
			<< eMaxStrLen << enderr;
		newLen = eMaxStrLen;
	}
	if (newLen >= mMemSize) 
	{
		bool rslt = determineMemory(newLen);
		aos_assert_r(rslt, false);
	}
	
	// Chen Ding, 05/02/2010
	// memcpy(mDataPtr+insertPos+subStrLen, mDataPtr+insertPos, mLen-insertPos);
	// memcpy(mDataPtr+insertPos,str.getBuffer(),subStrLen);
	memmove(mDataPtr+insertPos+subStrLen, mDataPtr+insertPos, mLen-insertPos);
	memcpy(mDataPtr+insertPos, str.getBuffer(), subStrLen);
	
	mLen = newLen;
	mDataPtr[mLen] = 0;
	return true;
}

bool
OmnString::hasPrefix(const OmnString &subStr)const
{
    unsigned int subStrLen = subStr.length();
    if(mLen < subStrLen)
    {
        return false;
    }
    return (OmnString(mDataPtr,subStrLen) == subStr);
}  


int 
OmnString::toInt() const
{
	return atoi(mDataPtr);
}

OmnString	
OmnString::subString(const u32 start, const u32 length) const
{
	// Ken Lee, 2013/07/20
	//if (length >= mLen - start)
	if (length > mLen - start)
	{
		return "";
	}
	else
	{
		return OmnString(&mDataPtr[start], length);
	}
}

OmnString	
OmnString::substr(const u32 start, const u32 end) const
{
	// 
	// Return a substring starting at position 'start'
	// and end at position 'end'. If 'end' is 0, it 
	// means the end of the string. 
	//
	// Errors:
	// 1. start >= mLen
	// 2. start > end (except end == 0)
	//
	if (start >= mLen)
	{
		cout << __FILE__ << ":" << __LINE__
			<< "********** start out of bound: " << start
			<< ". mLen = " << mLen << endl;
		return "";
	}

	// Ken Lee, 2013/09/04
	//u32 ee = (end == 0)?mLen-1:end;
	u32 ee = (end == 0 || end > mLen - 1) ? mLen - 1 : end;	
	if (start > ee)
	{
		cout << __FILE__ << ":" << __LINE__
			<< "********** start is larger than the end: " << start
			<< ". End: " << ee << endl;
		return "";
	}

	return OmnString(&mDataPtr[start], ee-start+1);
}


bool
OmnString::normalizeWhiteSpace(const bool trimLeadingSpaces, const bool trimTailingSpaces)
{
	// 
	// A white space is one of the following characters:
	//   a. Space
	//   b. Tab ('\t')
	//
	// This function will replace all white space substrings (a white space
	// substring is a substring that contains one or more white space
	// characters) with a single space character. 
	//
	// If 'trimLeadingSpaces' is true, it will remove all leading white space 
	// characters. If 'trimTailingSpaces' is true, it will remove all tailing
	// white space characters. 
	//
	// This function should always return true.
	//
	// If not specified, the function will keep the leading white spaces and
	// tailing spaces.
	int i;
	unsigned int iPrev;
	unsigned int nContentBegPos, nContentEndPos;
	if(!(mFlags & eAllowShrink))
	{
		cout << __FILE__ << ":" << __LINE__
			<< "********** string don't allow shrink" << endl;
		return false;
	}
	// 1. just erase the lead spaces 
	if(trimLeadingSpaces)
	{
		for(i = 0; i < (int)mLen; i++)
		{
			if( mDataPtr[i] == ' ' || 
				mDataPtr[i] == '\t' ||
				mDataPtr[i] == '\0')
			{
				continue;
			}
			else
			{
				break;
			}
		}
		if(i)
		{
			mLen = mLen - (u32)i;
			memmove(mDataPtr, mDataPtr + i, mLen + 1);
		}
		nContentBegPos = 0;
	}
	else
	{
		for(i = 0; i < (int)mLen; i++)
		{
			if( mDataPtr[i] == ' ' || 
				mDataPtr[i] == '\t' ||
				mDataPtr[i] == '\0')
			{
				continue;
			}
			else
			{
				break;
			}
		}
		nContentBegPos = (u32)i;
	}
	// 2. just erase the end spaces 
	if(trimTailingSpaces)
	{
		for(i = mLen-1; i >= 0; i--)
		{
			if( mDataPtr[i] == ' ' || 
				mDataPtr[i] == '\t' ||
				mDataPtr[i] == '\0')
			{
				continue;
			}
			else
			{
				break;
			}
		}
		mLen = i + 1;
		mDataPtr[mLen] = 0;
		nContentEndPos = mLen;
	}
	else
	{
		for(i = mLen-1; i >= 0; i--)
		{
			if( mDataPtr[i] == ' ' || 
				mDataPtr[i] == '\t' ||
				mDataPtr[i] == '\0')
			{
				continue;
			}
			else
			{
				break;
			}
		}
		nContentEndPos = i+1;
	}
	// 3. just normalize the content  (value 'i' is following the last 2nd step)
	for(i = (int)nContentBegPos; i < (int)nContentEndPos && mDataPtr[i] != 0; i++)
	{
		for(; i < (int)nContentEndPos; i++)
		{
			if( mDataPtr[i] == ' ' || 
				mDataPtr[i] == '\t' ||
				mDataPtr[i] == '\0')
			{
				break;
			}
			else
			{
				continue;
			}
		}
		iPrev = (int)i;
		for(; i < (int)nContentEndPos; i++)
		{
			if( mDataPtr[i] == ' ' || 
				mDataPtr[i] == '\t' ||
				mDataPtr[i] == '\0')
			{
				continue;
			}
			else
			{
				break;
			}
		}
		if(i < (int)nContentEndPos)
		{
			i--;
			mDataPtr[i] = ' ';
			if(i > (int)iPrev)
			{
				//mLen = mLen - (i - iPrev);
				//memmove(mDataPtr + iPrev, mDataPtr + i, mLen + 1);
				// Ken Lee, 2013/06/07
				memmove(mDataPtr + iPrev, mDataPtr + i, mLen + 1 - i);
				mLen = mLen - (i - iPrev);
				nContentEndPos -= (i - iPrev);
			}
		}
	}
	return true;
}


OmnString
OmnString::getLine(const bool removeFlag, const u32 lineIndex)
{
	// 
	// The function retrieves the 'lineIndex'-th line. A line is a string
	// that is ended by "\n". If 'removeFlag' is true, it will remove
	// the line. If the requested line is not found, an empty string
	// is returned. The first line is identified by 'lineIndex== 1'. 
	// 
	// Note that if the line is found, the '\n' is included in the
	// returned string (as the last character).
	//
	u32 start = 0;
	u32 idx = 0;
	u32 lineNo = 1;
	if (mLen <= 0)
	{
		return "";
	}

	while (idx < mLen)
	{
		if (mDataPtr[idx] == '\n')
		{
			// 
			// Found a line. Check whether this is the one we are looking for
			//
			if (lineIndex == lineNo)
			{
				// 
				// It is the one we are looking for. 
				// Construct the string.
				//
				OmnString str(&mDataPtr[start], idx - start + 1);

				// 
				// Check whether we need to remove the line.
				//
				if (removeFlag)
				{
					if (idx == mLen -1)
					{
						// 
						// The line is the last line. 
						//
						mDataPtr[start] = 0;
						mLen = start;
					}
					else
					{
						memmove(&mDataPtr[start], &mDataPtr[idx+1], mLen - idx - 1);
						mLen -= (idx - start + 1);
						mDataPtr[mLen] = 0;
					}

				}
				return str;
			}

			lineNo++;
			start = idx+1;
		}

		idx++;
	}

	if (lineIndex == 1)
	{
		OmnString str(mDataPtr, mLen);
		mLen = 0;
		mDataPtr[0] = 0;
		return str;
	}
		
	// 
	// Did not find the line
	//
	return "";
}


// 
// Chen Ding, 03/20/2007
//
u32
OmnString::substitute(char oldc, char newc, bool all)
{
	// 
	// If substitute the character 'oldc' with the new character 'newc'. 
	// If 'all' is true, it substitutes all. Otherwise, it will subsitute
	// the first occurance.
	//
	// It returns the number of substitutes. 
	//
	u32 idx = 0;
	u32 found = 0;
	while (idx < mLen)
	{
		if (mDataPtr[idx] == oldc)
		{
			mDataPtr[idx] = newc;
			found++;

			if (!all) return 1;
		}
		idx++;
	}

	return found;
}


// 
// Chen Ding, 05/10/2007
// Remove a line from 'startPos'. A line is ended by '\n', '\r', or both, 
// or the end of file
// It returns the number of bytes removed. If errors occured, return -1.
//
int 
OmnString::removeLine(const u32 startPos)
{
	if (startPos >= mLen)
	{
		OmnAlarm << "Invalid starting position: " 
			<< startPos<< enderr;
		return -1;
	}

	// 
	// Find the ending position
	//
	u32 index = startPos;
	while (index < mLen)
	{
		if (mDataPtr[index] == '\r' || mDataPtr[index] == '\n')
		{
			if (index + 1 < mLen && (
				mDataPtr[index+1] == '\r' || mDataPtr[index+1] == '\n'))
			{
				index++;
			}

			break;
		}
		index++;
	}

	u32 length = index - startPos;
	if (index < mLen) length++;

	memcpy(&mDataPtr[startPos], &mDataPtr[startPos+length], mLen-startPos-length);
	mLen -= length;
	mDataPtr[mLen] = 0;
	return determineMemory(mLen);
}


bool
OmnString::isEmptyLine() const
{
	for (u32 i=0; i<mLen; i++)
	{
		if (mDataPtr[i] != ' ' && mDataPtr[i] != '\t' &&
			mDataPtr[i] != '\n' && mDataPtr[i] != '\r')
		{
			return false;
		}
	}

	return true;
}


bool
OmnString::isDigitStr(const bool allow_sign, const bool allow_dot) const
{
	// It checks whether it is a digit string. A digit string by this function
	// is in the form:
	// 		[+|-][.]dddd
	// That is:
	// 	1. A digit string may start with a '+' or '-' (optional)
	// 	2. A digit string may start with a '.' (optional)
	// 	3. A digit string must contain 1 or more digits
	if (mLen == 0) return false;

	u32 start = 0;
	if (allow_sign)
	{
		if (mDataPtr[0] == '-' || mDataPtr[0] == '+')
		{
			if (mLen == 1) return false;
	
			start = 1;
		}
	}

	if (allow_dot)
	{
		if (mDataPtr[1] == '.')
		{
			start++;
			if (mLen == start) return false;	
		}
	}

	for (u32 i=start; i<mLen; i++)
	{
		if (mDataPtr[i] < '0' || mDataPtr[i] > '9')
		{
			return false;
		}
	}

	return true;
}


// 
// Chen Ding, 06/10/2007
// It replaces all the characters 'ori' with 'target'.
//
bool		
OmnString::replace(const char ori, const char target)
{
	for (u32 i=0; i<mLen; i++)
	{
		if (mDataPtr[i] == ori) mDataPtr[i] = target;
	}

	return true;
}


int 
OmnString::padChar(char c, u32 len, bool longerIsError)
{
	// 
	// This function will pad 'c' to make the string 'len' long. 
	// If the string is already 'len', it does nothing. If the
	// string is longer than 'len', whether it is treated as 
	// an error or not depends on 'longerIsError'. 
	//
	if (mLen > len)
	{
		if (longerIsError)
		{
			OmnAlarm << "String is longer than 'len': "
				<< mLen << ":" << len << enderr;
			return -1;
		}

		return 0;
	}

	if (mLen == len) return 0;

	OmnString str(len - mLen, c, true);
	*this << str;
	return len - mLen;
}


int
OmnString::replace(const OmnString &pattern, const OmnString &str, const bool all)
{
	// 
	// This function replaces the substring that matches 'pattern' with 
	// the new substring 'str'. If 'all' is true, it will replace all the
	// occurances of the pattern 'pattern'.
	//
	// It returns the number of occurances. If none is found, it returns 0.
	// If errors are encountered, -1 is returned.
	//
	if (pattern == "")
	{
		OmnAlarm << "pattern is null: " << str << enderr;
		return -1;
	}

	int found = 0;
	int idx = findSubString(pattern, 0);
	while (idx >= 0)
	{
		// 
		// Found an occurance of the pattern. 
		//
		if (!replace((u32)idx, pattern.length(), str))
		{
			OmnAlarm << "Failed to replace" << enderr;
			return -1;
		}

		if (!all) return true;
		found++;
		// Chen Ding, 2011/01/23
		// idx = findSubString(pattern, 0);
		idx = findSubString(pattern, idx + str.length());
	}

	return found;
}

// The following five functions are added by Chen Ding, 12/22/2008
bool	
OmnString::parseInt(const u32 startIdx, u32 &len, int &value) const
{
	// It starts the parsing at position 'startIdx'. If success, 
	// 'len' holds how many characters it consumed and 'value'
	// is the value retrieved. If errors (i.e., the first printable
	// character is not a digit), it returns false.
	//
//	aos_assert_r(startIdx < mLen, false);
	if(startIdx >= mLen)return false;
	char *endptr = 0;

	// Skip white spaces
	char c;
	u32 idx = startIdx;
	while (idx < mLen && (c = mDataPtr[idx]) &&
			(c == ' ' || c == '\t' || c == '\n')) idx++;

	if (idx >= mLen) return false;

	value = strtol(&mDataPtr[idx], &endptr, 10);
	if (endptr == &mDataPtr[idx]) return false;
	if (endptr) len = endptr - &mDataPtr[startIdx];
	else len = mLen - startIdx;
	return true;
}


bool	
OmnString::parseInt64(const u32 startIdx, u32 &len, int64_t &value) const
{
	// It starts the parsing at position 'startIdx'. If success, 
	// 'len' holds how many characters it consumed and 'value'
	// is the value retrieved. If errors (i.e., the first printable
	// character is not a digit), it returns false.
	//
//	aos_assert_r(startIdx < mLen, false);
	if(startIdx >= mLen)return false;
	char *endptr = 0;

	// Skip white spaces
	char c;
	u32 idx = startIdx;
	while (idx < mLen && (c = mDataPtr[idx]) &&
			(c == ' ' || c == '\t' || c == '\n')) idx++;

	if (idx >= mLen) return false;

	value = strtoll(&mDataPtr[idx], &endptr, 10);
	if (endptr == &mDataPtr[idx]) return false;
	if (endptr) len = endptr - &mDataPtr[startIdx];
	else len = mLen - startIdx;
	return true;
}


bool	
OmnString::parseU32(const u32 startIdx, u32 &len, u32 &value) const
{
	// It is similar to parseInt(...) except that it parses an unsigned long.
//	aos_assert_r(startIdx < mLen, false);
	if(startIdx >= mLen)return false;
	char *endptr = 0;

	// Skip white spaces
	char c;
	u32 idx = startIdx;
	while (idx < mLen && (c = mDataPtr[idx]) &&
			(c == ' ' || c == '\t' || c == '\n')) idx++;

	if (idx >= mLen) return false;

	value = strtoul(&mDataPtr[idx], &endptr, 10);
	if (endptr == &mDataPtr[idx]) return false;
	if (endptr) len = endptr - &mDataPtr[startIdx];
	else len = mLen - startIdx;
	return true;
}


/*
bool	
OmnString::parseInt64(const u32 startIdx, u32 &len, int64_t &value) const
{
//	aos_assert_r(startIdx < mLen, false);
	if(startIdx >= mLen)return false;
	char *endptr = 0;

	// Skip white spaces
	char c;
	u32 idx = startIdx;
	while (idx < mLen && (c = mDataPtr[idx]) &&
			(c == ' ' || c == '\t' || c == '\n')) idx++;

	if (idx >= mLen) return false;

	value = strtoll(&mDataPtr[idx], &endptr, 10);
	if (endptr == &mDataPtr[idx]) return false;
	if (endptr) len = endptr - &mDataPtr[startIdx];
	else len = mLen - startIdx;
	return true;
}
*/


u64
OmnString::parseU64(const u32 startIdx, u32 &len, const u64 &dft) const
{
	if (startIdx < 0 || startIdx >= mLen) return dft;

	// Skip white spaces
	u32 idx = startIdx;
	// char c;
	// while (idx < mLen && (c = mDataPtr[idx]) &&
	// 		(c == ' ' || c == '\t' || c == '\n')) idx++;
	char *endptr = 0;
	u64 value = strtoull(&mDataPtr[idx], &endptr, 10);
	if (endptr == &mDataPtr[idx]) return dft;

	if (endptr) len = endptr - &mDataPtr[startIdx];
	else len = mLen - startIdx;

	return value;
}


bool
OmnString::parseU64Bool(const u32 startIdx, u32 &len, u64 &value) const
{
	value = 0;
	if (startIdx < 0 || startIdx >= mLen) return false;

	u32 idx = startIdx;
	char *endptr = 0;
	value = strtoull(&mDataPtr[idx], &endptr, 10);
	if (endptr == &mDataPtr[idx]) return false;

	if (endptr) len = endptr - &mDataPtr[startIdx];
	else len = mLen - startIdx;

	return true;
}


bool	
OmnString::parseDouble(const u32 startIdx, u32 &len, double &value) const
{
//	aos_assert_r(startIdx < mLen, false);
	if(startIdx >= mLen)return false;
	char *endptr = 0;

	// Skip white spaces
	char c;
	u32 idx = startIdx;
	while (idx < mLen && (c = mDataPtr[idx]) &&
			(c == ' ' || c == '\t' || c == '\n')) idx++;

	if (idx >= mLen) return false;

	value = strtold(&mDataPtr[startIdx], &endptr);
	if (endptr == &mDataPtr[idx]) return false;
	if (endptr) len = endptr - &mDataPtr[startIdx];
	else len = mLen - startIdx;
	return true;
}


bool
OmnString::escapeFnameWS()
{
	// It escapes the following chars. If this string is used as 
	// file names, this is important:
	//	space, (, ),  
	if (!mDataPtr) return true;

	uint memsize = getMemorySize();
	for (uint i=0; i<mLen; i++)
	{
		char c = mDataPtr[i];
		if (c == ' ' || c == '(' || c == ')')
		{
			if (mLen + 1 >= memsize) 
			{
				setLength(memsize + 100);
				memsize =getMemorySize();
			}
			aos_assert_r(mLen + 1 < memsize, false);	

			memmove(&mDataPtr[i+1], &mDataPtr[i], mLen-i);
			mDataPtr[i] = '\\';
			mLen++;
			mDataPtr[mLen] = 0;
			i++;
		}
	}

	return true;
}


bool
OmnString::escapeSymbal(const char c, const char e)
{
	// This function escapes the symbal 'c' by prepending
	// it with 'e'. As the result, the string length may be increased. 
	u32 idx = 0;
	while (idx < mLen)
	{
		if (mDataPtr[idx] == c)
		{
			// Found the char. Need to prepend the char 'e'
			// in front of this char. 
			if (mLen + 50 >= (u32)mMemSize) 
			{
				bool rslt = determineMemory(mLen + 50);
				aos_assert_r(rslt, false);
			}
			memmove(&mDataPtr[idx+1], &mDataPtr[idx], mLen - idx);
			mDataPtr[idx] = e;
			mLen++;
			idx += 2;
		}
		else
		{
			idx++;
		}
	}
	return true;
}


// Chen Ding, 01/10/2010
bool
OmnString::removeQuotes(const bool flag)
{
	// It assumes the string is a quoted string. It can be quoted 
	// by either a single quote or double quote. If 'flag' is true,
	// the string MUST be a quoted string. Otherwise, the string
	// may not be a quoted string. A string is quoted string
	// if it starts with a quote and ends with the same quote. 
	if (mLen == 0) return true;

	if ((mDataPtr[0] != '\'' && mDataPtr[0] != '"') || mLen < 2)
	{
		aos_assert_r(!flag, false);
		return true;
	}

	aos_assert_r(mDataPtr[0] == mDataPtr[mLen-1], false);
	memmove(mDataPtr, &mDataPtr[1], mLen-1);
	mLen -= 2;
	mDataPtr[mLen] = 0;
	return true;
}


int
OmnString::indexOf(const int idx, const char pattern, const bool reverse) const
{
	// It returns the index of the first occurance of 'pattern'
	// starting from the position 'idx'. 
	// If not found, it returns -1.
	if (idx < 0 || (u32)idx >= mLen) return -1;

	if (mLen == 0) return -1;
	if (!reverse)
	{
		for (int i=idx; (u32)i<mLen; i++)
		{
			if (mDataPtr[i] == pattern)
			{
				return i;
			}
		}
		return -1;
	}

	// Search from the end
	for (int i=mLen-1; i>=idx; i--)
	{
		if (mDataPtr[i] == pattern)
		{
			return i;
		}
	}
	return -1;
}


int
OmnString::indexOfWithEscape(const int idx, const char pattern) const
{
	// It returns the index of the first occurance of 'pattern'
	// starting from the position 'idx'. It ignores 'pattern' if 
	// it appears in quotes (both single and double quotes).
	// If not found, it returns -1.
	// Single quotes can be escaped by either (1) '\' + ' or (2)
	// repeating the single quote (such as 'ddd''ddd' is equivalent
	// to contents = 'ddd'''ddd'ddd. 'ddd\'ddd'
	int single_quote = 0;
	int double_quote = 0;
	int round_brackets = 0;
	if (idx < 0 || (u32)idx >= mLen) return -1;

	if (mLen == 0) return -1;
	for (int i=idx; (u32)i<mLen; i++)
	{
		if (mDataPtr[i] == '\'')
		{
			if(mDataPtr[i+1] == '\'' )
			{
				single_quote++;
				continue;
			}
			if(mDataPtr[i+1] == '\\' && mDataPtr[i+2] == '\'')
			{
				i += 1;
				single_quote++;
				continue;
			}
			single_quote++;
		}
		if (mDataPtr[i] == '\"')
		{
			if(mDataPtr[i+1] == '\"')
			{
				double_quote++;
				continue;
			}
			if(mDataPtr[i+1] == '\\' && mDataPtr[i+2] == '\'')
			{
				i += 1;
				double_quote++;
				continue;
			}		
			double_quote++;
		}
		if(mDataPtr[i] == '(')
			round_brackets++;
		if(mDataPtr[i] == ')')
			round_brackets--;
		if(pattern == ';' && mDataPtr[i] == pattern)
			return i;
		if (mDataPtr[i] == pattern && (single_quote%2) == 0 && (double_quote%2) ==0 && round_brackets == 0)
		{
			return i;
		}
	}
	
	return -1;
	
}



int
OmnString::indexOf(const OmnString &pattern, const int idx) const
{
	// It returns the index of the first occurance of 'pattern'
	// starting from the position 'idx'. 
	// If not found, it returns -1.
	if (idx < 0 || (u32)idx >= mLen) return -1;

	char *pp = strstr(&mDataPtr[idx], pattern.data());
	if (!pp) return -1;
	return pp - mDataPtr;
}


bool
OmnString::increment()
{
	// It increments the string by one based on the following rules:
	// [0-8]:	add 1
	// [9]:		'a'
	// [a-y]:	add 1
	// [z]:		'A'
	// [A-Y]:	add 1
	// [Z]:		set to '0' and increment the next, if any
	for (int i=mLen-1; i>=0; i--)
	{
		char c = mDataPtr[i];
		if ((c >= '0' && c <= '8') ||
			(c >= 'a' && c <= 'y') ||
			(c >= 'A' && c <= 'Y')) 
		{
			mDataPtr[i]++;
			return true;
		}
		else if (c == '9') 
		{
			mDataPtr[i] = 'a';
			return true;
		}
		else if (c == 'z') 
		{
			mDataPtr[i] = 'A';
			return true;
		}
		else 
		{
			mDataPtr[i] = '0';
		}
	}

	return true;
}


bool	
OmnString::hasCommonWords(
		const OmnString &rhs, 
		const OmnString &sep) const
{
	// It returns true if this string and 'rhs' has at least one
	// common word. Words are separated by the characters defined
	// in 'sep'.
	// It starts from this string, finds the next word, and then
	// checks whether the word exists in 'rhs'.
	//
	// Returns: If any errors found or no common words found, 
	// it returns false. Otherwise, it returns true.
	list<OmnString> list1, list2, list3;
	bool rslt = AosGetStrDiff(*this, rhs, sep, list1, list2, list3);
	if (!rslt) return false;
	return (list3.size() > 0);
	/*
	u32 idx = 0;
	const char *sepp = sep.data();
	const int seplen = sep.length();
	while (idx < mLen)
	{
		// Skip anything defined in 'sep'
		while (idx < mLen)
		{
			char c = mDataPtr[idx];
			bool found = false;
			for (int i=0; i<seplen; i++) 
			{
				if (c == sepp[i])
				{
					// Found a leading separator. 
					found = true;
					break;
				}
			}
			if (!found) break;
			idx++;
		}

		if (idx >= mLen) return false;

		// Find the word
		int startidx = idx;
		while (idx < mLen)
		{
			char c = mDataPtr[idx];
			bool found = false;
			for (int i=0; i<seplen; i++)
			{
				if (c == sepp[i])
				{
					found = true;
					break;
				}
			}
			if (found) break;
			idx++;
		}

		int wordlen = idx - startidx;
		if (wordlen == 0) return false;
		char cc = mDataPtr[idx];
		mDataPtr[idx] = 0;
		char *ptr = (char *)strstr(rhs.data(), &mDataPtr[startidx]);
		if (ptr)
		{
			// Found the word in 'rhs', but need to check whether
			// it is stopped by one of the separate characters.
			char *dataptr = &mDataPtr[startidx];
			const u32 dalen = strlen(dataptr);
			const u32 plen = strlen(ptr);
			const char *ptrchars = ptr;

			for (int i=0; i<seplen; i++)
			{
				if (sepp[i] == ptrchars[dalen]) 
				{
					mDataPtr[idx] = cc;
					return true;
				}
			}
			if (dalen>=plen) return true;

			//
			// const char *rhschars = rhs.data();
			// const u32 rlen = rhs.length();
			// if (idx >= rlen) return true;
			// for (int i=0; i<seplen; i++)
			// {
			// 	if (sepp[i] == rhschars[idx]) 
			// 	{
			// 		mDataPtr[idx] = cc;
			// 		return true;
			// 	}
			// }
		}

		mDataPtr[idx] = cc;
	}

	return false;
	*/
}


int 
OmnString::removeWord(
		const OmnString &word, 
		const OmnString &sep, 
		const bool removeall)
{
	// This function removes either the first occurance or 
	// all the occurances of the word 'word', separated by 
	// 'sep'. It returns the number of occurances removed.
	// If error, it returns -1.
	u32 idx = 0;
	const char *sepp = sep.data();
	const int seplen = sep.length();
	int num_removed = 0;
	bool is_first = true;
	while (idx < mLen)
	{
		// Skip anything defined in 'sep'
		while (idx < mLen)
		{
			char c = mDataPtr[idx];
			bool found = false;
			for (int i=0; i<seplen; i++) 
			{
				if (c == sepp[i])
				{
					// Found a leading separator. 
					found = true;
					break;
				}
			}
			if (!found) break;
			idx++;
		}

		if (mLen - idx < (u32)word.length()) return num_removed;

		if (strncmp(&mDataPtr[idx], word.data(), word.length()) == 0)
		{
			// Found the word in 'rhs', but need to check whether
			// it is stopped by one of the separate characters.
			u32 endidx = idx + word.length();
			char c = mDataPtr[endidx];
			for (int i=0; i<seplen; i++)
			{
				if (sepp[i] == c) 
				{
					// It is stopped by one of the separator. Need to remove it.
					if (is_first)
					{
						// The removed is the first word. Need to remove the 
						// following separators, too.
						while (endidx < mLen)
						{
							bool found = false;
							for (int k=0; k<seplen; k++)
							{
								if (mDataPtr[endidx] == sepp[k])
								{
									endidx++;
									found = true;
									break;
								}
							}
							if (!found) break;
						}
					}

					memmove(&mDataPtr[idx], &mDataPtr[endidx], mLen - endidx);
					mLen -= endidx - idx;
					aos_assert_r(mLen >= 0, -1);
					mDataPtr[mLen] = 0;
					num_removed++;
					if (!removeall) return num_removed;
					break;
				}
			}

			is_first = false;
		}
	}

	return num_removed;
}


bool
OmnString::escapeSymbols()
{
	// 1. For letters and digits, do nothing
	// 2. For all other characters, use '\' to escape.
	// 3. Do not escape characters whose values are not ASCII.
	//
	// Example:
	// 	u=1792003241,1300216086&fm=2&gp=0[1].jpg
	for (int i=mLen-1; i>=0; i--)
	{
		char c = mDataPtr[i];
		if (IS_SPECIAL_CHAR(c))
		{
			//add '\'.
			OmnString escp = "\\";
			insert(escp, i);
		}
	}

	return true;
}


// Chen Ding, 12/22/2011
bool
OmnString::trimByChar(const char c)
{
	// This function trims the string from the position of the last
	// occurance of 'c'. If 'c' is not found, it does nothing.
	int idx = mLen-1; 
	while (idx >= 0 && mDataPtr[idx] != c) idx--;
	if (idx < 0) return false;
	aos_assert_r(mDataPtr[idx] == c, false);
	mLen = idx;
	mDataPtr[idx] = 0;
	return true;
}


OmnString &
OmnString::operator << (const long rhs)
{
	char buf[100];
	sprintf(buf, "%ld", rhs);
	return operator +=(buf);
}


OmnString &
OmnString::operator << (const int r)
{
	static const int lsBuffLen = 12;

	if (mLen + 12 >= (u32)mMemSize) 
	{
		bool rslt = determineMemory(mLen + 12);
		aos_assert_r(rslt, *this);
	}
	if (r == 0) 
	{
		mDataPtr[mLen++] = '0';
		mDataPtr[mLen] = 0;
		return *this;
	}

	int rhs = r;
	if (r < 0) 
	{
		mDataPtr[mLen++] = '-';
		rhs = -r;
	}
	
	char buff[lsBuffLen];
	int idx = lsBuffLen-1;
	while (rhs && idx)
	{
		buff[idx--] = (u8)(rhs % 10) + '0';
		rhs = rhs / 10;
	}
	aos_assert_r(idx, *this);
	idx++;

	memcpy(&mDataPtr[mLen], &buff[idx], lsBuffLen - idx);
	mLen += (lsBuffLen - idx);
	mDataPtr[mLen] = 0;
	return *this;
}


OmnString &
OmnString::operator << (const u32 rhs)
{
	char buf[100];
	sprintf(buf, "%u", (unsigned int)rhs);
	return operator +=(buf);
}


OmnString &
OmnString::operator << (const u64 rhs)
{
	char buf[100];
	sprintf(buf, "%llu", (long long unsigned int)rhs);
	return operator +=(buf);
}


// Chen Ding, 06/02/2012
OmnString	
OmnString::postfix(const char sep) const
{
	// This function returns the substring:
	char *ptr = rindex(mDataPtr, sep);
	if (!ptr) return mDataPtr;
	ptr++;
	return OmnString(ptr, mLen - (ptr - mDataPtr));
}


// Chen Ding, 06/02/2012
OmnString
OmnString::getPrefix(const char sep) const
{
	// This function returns the substring:
	char *ptr = rindex(mDataPtr, sep);
	if (!ptr || ptr == mDataPtr) return "";
	ptr--;
	int len = ptr - mDataPtr + 1;
	return OmnString(mDataPtr, len);
}


int64_t
OmnString::getNumInstances()
{
	// return sgAosCountObjInstCount;
	//return -1;
	return sgInstances;
}


int64_t
OmnString::getTotalInstances()
{
	// return sgAosCountObjTotalCount;
	//return -1;
	return sgTotalInstances;
}


void
OmnString::convertToScientific()
{
	if (mLen > 24) 
	{
		// It is too long. It does not convert.
		return;
	}

	if (mLen <= 3) 
	{
		// Nothing to convert
		return;
	}

	// It is guaranteed that there is enough space to span.
	int idx = mLen - 3;
	while (idx > 0)
	{
		memmove(&mDataPtr[idx+1], &mDataPtr[idx], mLen - idx);
		mLen++;
		mDataPtr[idx] = ',';
		idx -= 3;
	}

	mDataPtr[mLen] = 0;
}


bool
OmnString::trimSpaces(const bool trimLeadingSpaces, const bool trimTailingSpaces)
{
	if(!(mFlags & eAllowShrink))
	{
		cout << __FILE__ << ":" << __LINE__
			<< "********** string don't allow shrink" << endl;
		return false;
	}

	// 1. Trim the leading spaces 
	if (trimLeadingSpaces)
	{
		u32 i=0;
		while (i<mLen)
		{
			if(mDataPtr[i] != ' ' && mDataPtr[i] != '\t') break;
		}

		if (i)
		{
			mLen = mLen - i;
			memmove(mDataPtr, mDataPtr + i, mLen + 1);
		}
	}
	
	// 2. Trim the tail spaces 
	if (mLen == 0) return true;

	if(trimTailingSpaces)
	{
		u32 i;
		for(i = mLen-1; i >= 0; i--)
		{
			if(mDataPtr[i] != ' ' && mDataPtr[i] != '\t') break;
		}
		if (i == mLen-1) return true;

		mLen = i + 1;
		mDataPtr[mLen] = 0;
	}
	return true;
}


// Ken Lee, 2014/12/08
int
OmnString::countChar(const char c)
{
	int count = 0;
	for (u32 i=0; i<mLen; i++)
	{
		if (mDataPtr[i] == c) count++;
	}
	return count;
}

//
//Phil: 2015/01/03
//move escape/unescape to common string functions
//
bool
OmnString::escape()
{
	replace(".", "0x2e", true);
	replace("<", "0x3c", true);
	replace(">", "0x3e", true);
	replace("(", "0x28", true);
	replace(")", "0x29", true);
	replace("/", "0x2f", true);
	replace("*", "0x2a", true);
	replace(",", "0x2c", true);
	replace("\"", "0x22", true);
	replace(" ", "0x20", true);
	return  true;
}


bool
OmnString::unescape()
{
	replace("0x2e", ".", true);
	replace("0x3c", "<", true);
	replace("0x3e", ">", true);
	replace("0x28", "(", true);
	replace("0x29", ")", true);
	replace("0x2f", "/", true);
	replace("0x2a", "*", true);
	replace("0x2c", ",", true);
	replace("0x22", "\"", true);
	replace("0x20", " ", true);
	return  true;
}
