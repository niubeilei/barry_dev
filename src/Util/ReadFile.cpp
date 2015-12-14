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
//
// Modification History:
// 03/18/2012, Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/ReadFile.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Porting/FileOpenErr.h"
#include "Porting/GetErrno.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/FileWatcher.h"
#include "util_c/strutil.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


AosReadFile::AosReadFile()
:
mFile(0),
mLock(OmnNew OmnMutex())
{
}


AosReadFile::AosReadFile(const OmnString &name)
:
mName(name),
mFile(0),
mLock(OmnNew OmnMutex())
{
	mFile = ::fopen64(name.data(), "rb");
}


AosReadFile::~AosReadFile()
{
	if (mFile)
	{
		::fclose(mFile);
		mFile = 0;
	}
}


bool
AosReadFile::openFile()
{
	mFile = ::fopen64(mName.data(), "rb");
	return (mFile != 0);
}


int 
AosReadFile::readBinaryInt(const int dft)
{
	int value;
	int bytesread = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (bytesread != sizeof(value)) return dft;
	return value;
}


int64_t 
AosReadFile::readBinaryInt64(const int64_t &dft)
{
	int64_t value;
	int bytesread = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (bytesread != sizeof(value)) return dft;
	return value;
}


u32
AosReadFile::readBinaryU32(const u32 dft)
{
	u32 value;
	int rr = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (rr != sizeof(value)) return dft;
	return value;
}


u64
AosReadFile::readBinaryU64(const u64 &dft)
{
	u64 value;
	size_t nn = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (nn != sizeof(value)) return dft;
	return value;
}


char
AosReadFile::readChar(const char &dft)
{
	char value;
	int nn = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (nn != sizeof(value)) return dft;
	return value;
}


int
AosReadFile::getWord(char *word, const int buflen)
{
	// This function reads a word from its current position.
	if (!mFile)
	{
		OmnAlarm << "Failed not good: " << mName << enderr;
		return 0;
	}

	// First skip the leading spaces
	char c;
	while (1)
	{
		AOSFILE_COUNTING_GETC;
		c = fgetc(mFile);
		if (c == EOF)
		{
			// Didn't get anything
			word[0] = 0;
			return 0;
		}

		if (c >= 33 && c <= 126)
		{
			// First printable character
			break;
		}
	}
		
	int index = 1;
	word[0] = c;
	while (index < buflen)
	{
		AOSFILE_COUNTING_GETC;
		c = fgetc(mFile);
		if (c == EOF)
		{
			// 
			// Either it is an error or it is EOF. 
			//
			word[index] = 0;
			return index;
		}

		// 
		// Read a char. 
		//
		if (c < 33 || c > 126)
		{
			// 
			// We consider printable characters are in [33, 126].
			//
			word[index] = 0;
			return index;
		}

		word[index++] = c;
	}

	// 
	// This means it runs out of buffer.
	// This normally indicates an error
	//
	cout << "******************* Reading a word from file: " 
		<< mName.data()
		<< " but buffer runs out of bound!" << endl;
	return 0;
}


bool
AosReadFile::closeFile()
{
	aos_assert_r(mFile, false);
	bool rt = ::fclose(mFile);
	mFile = 0;
	return rt;
}


// It reads from the current position until it hits either 
// '\n' or EOF.
OmnString
AosReadFile::getLine(bool &finished)
{
	aos_assert_r(mFile, "");
	OmnString buf;
	finished = false;
	char c;
	while (1)
	{
		AOSFILE_COUNTING_GETC;
		c = fgetc(mFile);
		if (c == EOF)
		{
			finished = true;
			return buf;
		}

		if (c == '\n')
		{
			return buf;
		}
		buf<<c;
	}
	
	// Should never come to this point.
	return buf;
}


bool
AosReadFile::readToString(OmnString &buf, const int buflen)
{
	// This function reads from the current position to either 
	// EOF or the 'buflen' number of characters. 
	// buf.reset();
	buf = "";

	aos_assert_r(mFile, false);

	char c;
	int len = 0;
	while (len < buflen)
	{
		c = fgetc(mFile);
		if (c == EOF)
		{
			// The end of file. Time to return.
			return true;
		}

		buf << c;
		len++;
	}
		
	return true;
}


int			
AosReadFile::readToBuff(const u32 size, char *buff)
{
	// This function reads the contents from the file current position: 
	// maximum 'size' number of bytes. If the file has more than
	// 'size' bytes from 'startPos', only 'size' number of bytes are read. The caller
	// is responsible for allocating sufficient memory in 'buff'.
	//
	// If successful, it returns the number of bytes read. 
	// If error, it , it returns -1. 
	// If EOF, it returns 0.
	if (!mFile)
	{
		OmnAlarm << "File is null: " << mName << enderr;
		return -1;
	}

	return ::fread(buff, 1, size, mFile);
}


bool
AosReadFile::seek(const u64 &pos)
{
	if (!mFile)
	{
		OmnAlarm << "File null: " << mName << enderr;
		return false;
	}
	return (::fseeko64(mFile, pos, SEEK_SET) == 0);
}


OmnString	
AosReadFile::readStr(const u32 len, const OmnString &dft)
{
	OmnString str(len+1, ' ', true);
	char *data = (char *)str.data();
	u32 bytesRead = readToBuff(len, data);
	if (bytesRead != len) return dft;
	return str;
}

