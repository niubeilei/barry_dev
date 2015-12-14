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
// 
////////////////////////////////////////////////////////////////////////////
#include "Util/File.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "Porting/FileOpenErr.h"
#include "Porting/GetErrno.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/FileWatcher.h"
#include "util_c/strutil.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


// This constant is used to limit the maximum length of the buffer
// if one wants to read the entire file into memory. Currently, it is
// set to 100M. 
const int sgMaxBufLength = 100000000;

int64_t OmnFile::smDftReadSize = OmnFile::eDftFileReadSize;

OmnFile::OmnFile(AosMemoryCheckDeclBegin)
:
OmnLocalFile("OmnFile"),
mFile(0),
mMode(eInvalid),
mLock(OmnNew OmnMutex()),
mBlocksToCache(0),
mCachedBlockSize(0),
mDeviceId(-1),
mError("")
{
	AosMemoryCheckerObjCreated(AosClassName::eAosFile);
}


OmnFile::OmnFile(const OmnString &name, const Mode mode AosMemoryCheckDecl)
:
OmnLocalFile("OmnFile"),
mName(name),
mFile(0),
mMode(mode),
mFileDesc(name),
mLock(OmnNew OmnMutex()),
mBlocksToCache(0),
mCachedBlockSize(0),
mDeviceId(-1),
mError("")
{
	mFile = openFile(mName, mode);
	if (!mFile)
	{
		mError = strerror(errno);
	}
	AosMemoryCheckerObjCreated(AosClassName::eAosFile);
}


OmnFile::~OmnFile()
{
	if (mFile)
	{
		::fclose(mFile);
		mFile = 0;
	}
	AosMemoryCheckerObjDeleted(AosClassName::eAosFile);
}


bool
OmnFile::openFile(const Mode mode)
{
	mFile = openFile(mName, mode);
	if (!mFile)
	{
		mError = strerror(errno);
	}

	return (mFile != 0);
}


FILE *
OmnFile::openFile(const OmnString &filename, const Mode mode)
{
	FILE *ff = 0;
	switch (mode)
	{
	case eReadOnly:
		 ff = ::fopen(filename.data(), "rb");
		 break;
		 
	case eAppend:
		 ff = ::fopen64(filename.data(), "ab");
		 break;

	case eCreate:
		 ff = ::fopen64(filename.data(), "w+b");
		 break;
		 
	case eReadWrite:
		 ff = ::fopen64(filename.data(), "r+b");
		 break;

	case eWriteCreate:
		 ff = ::fopen64(filename.data(), "r+b");
		 if (!ff)
		 {
			 ff = ::fopen64(filename.data(), "w+b");
		 }
		 break;

	case eReadWriteCreate:
		 ff = ::fopen64(filename.data(), "a+b");
		 break;

	default:
		 OmnWarnProgError << "Unrecognized file mode: " << mode 
			<< ". File name = " << filename << enderr;
		 return 0;
	}
	return ff;
}


bool
OmnFile::put(
		const u64 &offset, 
		const char *str, 
		const int len, 
		const bool toFlush)
{
	if (!str || len <= 0) return true;

	//if (offset != eNoSeek) aos_assert_r(seek(offset), false);
	if (mFile)
	{
		//
		// Chen Ding, 10/15/2004
		//
		// According to the manual, 'fputs()' shall return the number of bytes
		// written to the file. But it seems that Microsoft always returns 0.
		// For this reason, we have to change the following not to include 0.
		//
		//	if (::fputs(str, mFile) <= 0)
		// Chen Ding, 2009/10/08
		// Changed it to fwrite since we need to control how many bytes to 
		// write. 
		// if (::fputs(str, mFile) < 0)
		if ((long)offset != ftell(mFile))
		{
			int rslt = seek(offset);
			if (rslt == -1)
			{
				OmnAlarm << "write err: " << strerror(errno) << enderr;
				exceptionThrow(__FILE__, __LINE__, errno);
				return false;
			}
		}
		AOSFILE_COUNTING_WRITE;

		int nn = ::fwrite(str, 1, len, mFile);
		if (nn != len)
		{
			// 
			// The caller should pring a warning, not this
			// function in case this file
			// is used for logs, we don't want to print too
			// many alarms.
			//				Chen Ding, 12-07-2002
			OmnAlarm << "write err: " << strerror(errno) << enderr;
			exceptionThrow(__FILE__, __LINE__, errno);
			return false;
		}
		if (toFlush)
		{
			AOSFILE_COUNTING_FLUSH;
			OmnFlushFile(mFile);
		}

		return true;
	}

	OmnAlarm << "not a file: " << enderr;
	return false;
}


/*
OmnFile &	
OmnFile::operator << (const char *str)
{
	// 
	// Write to the file. 
	//
	if (mFile)
	{
		AOSFILE_COUNTING_WRITE;
		if (::fputs(str, mFile) <= 0)
		{
			OmnWarnProgError << "Failed to write to file: " << mName << enderr;
		}
	}

	return *this;
}
*/


u64
OmnFile::readU64(const u64 &offset, const u64 &dft)
{
	aos_assert_r(seek(offset), dft);
	char word[100];
	int length = getWord(word, 100);
	if (length <= 0) return dft;
	char *endptr;
	return ::strtoull(word, &endptr, 10);
}


int 
OmnFile::readBinaryInt(const u64 &offset, const int dft)
{
	aos_assert_r(seek(offset), dft);
	int value;
	AOSFILE_COUNTING_READ;
	int bytesread = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (bytesread != sizeof(value)) return dft;
	return value;
}


int64_t 
OmnFile::readBinaryInt64(const u64 &offset, const int64_t &dft)
{
	aos_assert_r(seek(offset), dft);
	int64_t value;
	AOSFILE_COUNTING_READ;
	int bytesread = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (bytesread != sizeof(value)) return dft;
	return value;
}


u32
OmnFile::readBinaryU32(const u64 &offset, const u32 dft)
{
	aos_assert_r(seek(offset), dft);
	u32 value;
	AOSFILE_COUNTING_READ;
	int rr = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (rr != sizeof(value)) return dft;
	return value;
}


u64
OmnFile::readBinaryU64(const u64 &offset, const u64 &dft)
{
	aos_assert_r(seek(offset), dft);
	u64 value;
	AOSFILE_COUNTING_READ;
	//aos_assert_r(::fread((void *)&value, 1, 
	//	sizeof(value), mFile) == sizeof(value), false);
	int rr = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (rr != sizeof(value)) return dft;
	return value;
}


// ken Lee, 2011/6/17
char
OmnFile::readChar(const u64 &offset, const char &dft)
{
	aos_assert_r(seek(offset), dft);
	char value;
	AOSFILE_COUNTING_READ;
	aos_assert_r(::fread((void *)&value, 1, 
		sizeof(value), mFile) == sizeof(value), false);
	return value;
}


int 
OmnFile::readInt(const u64 &offset, const int dft)
{
	aos_assert_r(seek(offset), dft);
	char word[100];
	int length = getWord(word, 100);
	if (length <= 0) return dft;
	return ::atoi(word);
}


u32
OmnFile::readU32(const u64 &offset, const u32 dft)
{
	aos_assert_r(seek(offset), dft);
	char word[100];
	int length = getWord(word, 100);
	if (length <= 0) return dft;
	char *endptr;
	u32 vv = strtoul(word, &endptr, 10);
	return vv;
}


bool
OmnFile::readInt(int &v)
{
	char word[100];
	int length = getWord(word, 100);
	if (length <= 0) return false;
	v = ::atoi(word);
	return true;
}


int
OmnFile::getWord(char *word, const int buflen)
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
			// 
			// Didn't get anything
			//
			word[0] = 0;
			return 0;
		}

		if (c >= 33 && c <= 126)
		{
			// 
			// First printable character
			//
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
OmnFile::closeFile()
{
	aos_assert_r(mFile, false);
	bool rt = closeFile(mFile);
	mFile = 0;
	return rt;
}


bool
OmnFile::closeFile(FILE *f)
{
	if (!f)
	{
		return true;
	}

	::fclose(f);
	f = 0;
	return true;
}


void
OmnFile::resetFile()
{
	// 
	// This function erases all the contents. It assumes the 
	// file must be valid. Otherwise, it does nothing.
	//
	closeFile();

	mFile = openFile(mName, eCreate);
}


// 
// It reads from the current position until it hits either 
// '\n' or EOF.
//
OmnString
OmnFile::getLine(bool &finished)
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
		//<a href=mailto:xw_cn@163.com>wu xia</a> 09/18/2006
		//the function is not implemented,so i do it
		buf<<c;
		//end
	}
	
	// 
	// Should never come to this point.
	//
	return buf;
}


bool
OmnFile::readToString(OmnString &buf)
{
	// 
	// This function reads from the current position to either 
	// EOF or the 'buflen' number of characters. 
	//
	// buf.reset();
	buf = "";

	aos_assert_r(mFile, false);

	char c;
	while (buf.length() < sgMaxBufLength)
	{
		AOSFILE_COUNTING_GETC;
		c = fgetc(mFile);
		if (c == EOF)
		{
			// 
			// The end of file. Time to return.
			//
			return true;
		}

		buf << c;
	}
		
	return true;
}


int64_t			
OmnFile::readToBuff(const u64 &startPos,
			const u32 size,
			char *buff)
{
	// This function reads the contents from the file starting from the position: 
	// 'startPos' and reads maximum 'size' number of bytes. If the file has more than
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

	// Get the file size and check the size parameters
	
	//AOSFILE_COUNTING_SEEK;
	//::fseeko64(mFile, 0, SEEK_END);
	//int64_t fsize = ::ftello64(mFile);
	//if ((u64)fsize <= startPos) 
	//{
	//	return -1;
	//}
	
	if ((int64_t)startPos != ftell(mFile))
	{
		int rslt = seek(startPos);
		if (rslt == -1)
		{
			OmnAlarm << "write err: " << strerror(errno) << enderr;
			exceptionThrow(__FILE__, __LINE__, errno);
			return -1;
		}
		int64_t offset = ftell(mFile);
		if ((u64)offset != startPos)
		{
			return 0;
		}
	}

	int rr = ::fread(buff, 1, size, mFile);
	
	/*
	int64_t asize = size;
	if ((u64)fsize < startPos + size)
	{
		asize = fsize - startPos;
	}

	int rr = -1;
//	if (::ftello64(mFile) != (int64_t)startPos)
//	{
		AOSFILE_COUNTING_SEEK;
		rr = ::fseeko64(mFile, startPos, SEEK_SET);
//	}
	AOSFILE_COUNTING_READ;
	rr = ::fread(buff, 1, asize, mFile);
	*/
	return rr;
}


bool
OmnFile::append(const OmnString &buff, const bool toFlush)
{
	// It appends the contents 'buff' to the file. If failed, it returns false.
	// Otherwise, it returns true.
	aos_assert_r(mFile, false);
	if (buff.length() <= 0)
	{
		return false;
	}

	AOSFILE_COUNTING_WRITE;
	int asize = ::fwrite(buff.data(), 1, buff.length(), mFile);
	//if (asize < 0) return false; 
	if (asize != buff.length())
	{
		OmnAlarm << "write err: " << strerror(errno) << enderr;
		exceptionThrow(__FILE__, __LINE__, errno);
		return false;
	}

	if (toFlush)
	{
		AOSFILE_COUNTING_FLUSH;
		OmnFlushFile(mFile);
	}

	return true;
}


bool
OmnFile::skipChar()
{
	// 
	// Move the cursor to the next
	//
	aos_assert_r(mFile, false);
	AOSFILE_COUNTING_GETC;
	fgetc(mFile);
	return true;
}


bool
OmnFile::skipTo(const char *pattern, const bool after)
{
	// 
	// Move the file pointer to the beginning of 'parttern' from the 
	// current position. If the pattern is found, the file pointer
	// should point to the first character of the pattern (if 
	// 'after' is false), or right after the pattern (if 'after'
	// is true).  If not found, the file stops at the end of the file.
	//
	// The function returns true if the pattern is found. Otherwise,
	// it returns false.
	//
	const unsigned int len = strlen(pattern);
	
	unsigned int index = 0;
	unsigned int startPos = 0;
	bool		isSame = true;
	char c;
	OmnString tmpStr;
	aos_assert_r(mFile, false);
	while (1)
	{
		AOSFILE_COUNTING_GETC;
		c = fgetc(mFile);
		if (c == EOF)
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
				if (! (*(tmpStr.getBuffer() + startPos + index) == *(pattern + index)))
				{
					isSame = false;
					break;
				}
				index ++;
			}
			if(isSame)
			{
				// find it and return
				if (!after)
				{
					// 
					// Need to back off to the beginning of the pattern.
					//
					AOSFILE_COUNTING_SEEK;
					fseek(mFile, -len, SEEK_CUR);
				}
				return true;
			}
			startPos ++;
		}
	}

		
	return false;
}


bool
OmnFile::readUntil(OmnString &buffer, 
				  const char *pattern,
				  const u64 &max,
				  const bool containPattern, 
				  bool &finished)
{
	// 
	// It reads from the current position until:
	// 1. the end of the pattern,
	// 2. the end of the file, or
	// 3. exceeds 'max' (return false).
	//
	// If 'containPattern' is true, the pattern is read in 'buffer'.
	// Otherwise, it is not.
	//
	buffer = "";
	finished = false;
	aos_assert_r(mFile, false);
	const unsigned int len = strlen(pattern);
	unsigned int bytesRead = 0;
	char c;
	unsigned int index = 0;
	OmnString tmp;
	unsigned int startPos = 0;
	bool	isSame = true;
	while (bytesRead < max)
	{
		AOSFILE_COUNTING_GETC;
		c = fgetc(mFile);
		if (c == EOF)
		{
			finished = true;
			return true;
		}
		buffer << c;
		// Compare whether it match the pattern		
		if(startPos + len <= (unsigned int)buffer.length())
		{
			index = 0;
			isSame = true;
			while(index < len)
			{
				if (! (*(buffer.getBuffer() + startPos + index) == *(pattern + index)))
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
			
	return false;
}


bool
OmnFile::readBlock(
			const OmnString &start, 
			const OmnString &end,
			OmnString &contents, 
			bool &err)
{
	// 
	// This function reads a block. The block starts
	// with a line that matches 'start' and ends with
	// a line that matches 'end'. Anything in between
	// is retrieved into 'contents'.
	//
	
	err = false;
	if (!mFile)
	{
		aos_alarm("File pointer null: %s", mName.data());
		err = true;
		return false;
	}

	AOSFILE_COUNTING_REWIND;
	::rewind(mFile);

	// 
	// Move the cursor to the beginning of the start block
	//
	if (!skipTo(start.data(), true))
	{
		// 
		// The block is not in the file.
		//
		return false;
	}

	// 
	// Found the pattern. The cursor is right after the pattern.
	//
	bool finished;
	if (!readUntil(contents, end.data(), 100000, false, finished))
	{
		err = true;
		return false;
	}

	return true;
}		


bool
OmnFile::readAll(OmnString &contents)
{
	// 
	// It reads the entire file into 'contents'. 
	//
	if (!mFile)
	{
		aos_alarm("File is null");
		return false;
	}

	AOSFILE_COUNTING_REWIND;
	::rewind(mFile);

	char local[1000];
	char *s;
	AOSFILE_COUNTING_GETS;
	while ((s = ::fgets(local, 1000, mFile)))
	{
		contents << local;
	}

	return true;
}


bool
OmnFile::removeLine(u32 pos)
{
	// 
	// This function removes the line starting at "pos". 
	//
    OmnString newName;

	if (!mFile)
	{
		aos_alarm("File is null");
		cout<<"mFile error!"<<endl;
		return false;
	}

	// 1. Creat a the new file
	newName << mName << ".tmp";
	OmnFile tempfile(newName, OmnFile::eCreate AosMemoryCheckerArgs);
	if(tempfile.isGood() == false)
	{
		cout<<"open temp file failed!"<<endl;
		return false;  
	}
	
	// 2. Read file line by line , and write each line into a temp file except the line will be deleted
	AOSFILE_COUNTING_SEEK;
	fseek(mFile,0,SEEK_SET);
	u32 filepos;
	OmnString bufLine;
	bool finished = false;
	
	while(finished == false)
	{
		///////////////////////////////////////////////////////
		// it seems there is some problem with the function OmnFile::getLine(Omnstring &buf) .
		// it can not estimates the end of the file.
		// when the I/O pointer comes to the end of the file and call the function,
		// it will get a empty string instead of EOF.
		//                                     Harry Long   11/22/2006
		//
		AOSFILE_COUNTING_GETC;
		if(fgetc(mFile) == EOF) break;
		else fseek(mFile,-1,SEEK_CUR);

		filepos = ftell(mFile);
		bufLine = getLine(finished);
		bufLine += "\n";
		if(strcmp(bufLine.data(),"") == 0) continue;
		if(filepos != pos)
		{
			tempfile.append(bufLine);
		}
	}
	//fputc(EOF,mFile);

	// 3. Rename the temp file
  	closeFile();
	rename(newName.data(), mName.data());
	mFile = openFile(mName, mMode);	

	return true;
}

	
bool
OmnFile::fileExist(const OmnString &filename)
{
	FILE *f = openFile(filename, eReadOnly);
	if (!f)
	{
		return false;
	}

	closeFile(f);
	return true;
}


bool
OmnFile::replace(const OmnString &startPattern, 
				 const OmnString &endPattern, 
				 const OmnString &contents)
{
	// 
	// This function the contents that start with "startPattern" 
	// and ends at "endPattern" with the new contents 'contents'.
	//
	int start = find(startPattern);
	int end = find(endPattern);
	if (start < 0)
	{
		OmnAlarm << "To replace but the start pattern not found: " 
			<< startPattern << enderr;
		return false;
	}

	if (end < 0)
	{
		OmnAlarm << "To replace but the end pattern not found: " 
			<< endPattern << enderr;
		return false;
	}

	if (!replace((u32)start, (u32)(end - start + endPattern.length()), 
				contents))
	{
		OmnAlarm << "Failed to replace the contents. StartPattern: " 
			<< startPattern
			<< ". EndPattern: " << endPattern
			<< ". New Contents: " << contents << enderr;
		return false;
	}

	return true;
}


bool
OmnFile::replace(const u64 &startPos, const u32 len, const OmnString &contents)
{
	// 
	// This function replaces the substring [startPos, startPos + len] with
	// the new contents 'contents'. 
	//
	if (mFileContents == "")
	{
		if (!readAll(mFileContents))
		{
			OmnAlarm << "File too big: " << getLength() << enderr;
			return false;
		}
	}

	if (!mFileContents.replace(startPos, len, contents))
	{
		OmnAlarm << "Failed to replace the contents: " 
			<< startPos << ". Length: " << len 
			<< ". New Contents: " << contents << enderr;
		return false;
	}

	return true;
}


bool
OmnFile::replace(const OmnString &pattern, 
				 const OmnString &newContents, 
				 const bool allFlag)
{
	// 
	// This function searches the pattern "pattern" in the file. If that
	// pattern is found (a pattern is a string), that pattern is replaced
	// with the contents "contents". If 'allFlag' is true, it will replace
	// all the patterns found in the file. If 'allFlag' is false, it will
	// replace the first one found in the file. 
	//
	// If the pattern is found and the replacement is successful, it returns
	// true. Otherwise, it returns false. 
	//
	if (mFileContents == "")
	{
		if (!readAll(mFileContents))
		{
			OmnAlarm << "File too big: " << getLength() << enderr;
			return false;
		}
	}

	int loop = 0;
	bool found = false;
	u32 startPos = 0;
	while (1)
	{
		int pos = mFileContents.findSubString(pattern, startPos);
		if (pos < 0)
		{
			// 
			// Did not find the pattern. 
			//
			break;
		}

		found = true;
		// 
		// Replace it
		//
		if (!mFileContents.replace(pos, pattern.length(), newContents))
		{
			OmnAlarm << "Failed to replace the contents: " 
				<< startPos << ". Pattern: " << pattern
				<< ". New Contents: " << newContents
				<< ". Loop: " << loop << enderr;
			return false;
		}

		if (!allFlag)
		{
			break;
		}

		loop++;
		startPos = pos + newContents.length();
	};

	if (!found)
	{
		OmnAlarm << "Pattern not found: " << pattern << enderr;
		return false;
	}

	return true;
}


bool
OmnFile::flushFileContents()
{
	OmnFlushFile(mFile);
	return true;
}


// 
// Chen Ding, 03/20/2007
//
int
OmnFile::find(const OmnString &str)
{
	// 
	// It finds the string 'str' from the file. If found, it returns
	// the position at which the pattern starts. Otherwise, it returns -1.
	//
	aos_assert_r(mFile, -1);
	AOSFILE_COUNTING_REWIND;
	::rewind(mFile);

	const char *data = str.data();
	u32 len = str.length();

	u32 idx = 0;
	int start;
	int pos = 0;
	char c;
	while (1)
	{
		AOSFILE_COUNTING_GETC;
		c = fgetc(mFile);
		if (c == EOF)
		{
			// 
			// Didn't get anything
			//
			return -1;
		}

		if (c == data[idx])
		{
			if (idx == 0) start = pos;

			idx++;
			if (idx >= len)
			{
				// 
				// Found it.
				//
				return start;
			}
		}
		else
		{
			idx = 0;
		}

		pos++;
	}
	
	return -1;
}

int
OmnFile::getFiles(const OmnString &dir, 
		std::list<OmnString> &names, 
		const bool recursive)
{
	// This function retrieves file names in the given directory 'dir'. 
	// The files '.', '..', and "CVS" are ignored.
	struct dirent **namelist;
	int num = scandir(dir.data(), &namelist, 0, alphasort);
	int n = num;
	if (n < 0)
	{
		OmnAlarm << "Failed to scan: " << dir << enderr;
		return -1;
	}
	
	char *ff;
	int total = 0;
	OmnString dname = dir;
	dname << "/";
	struct stat thestat;
	while(n--)
	{
		ff = namelist[n]->d_name;
		if (ff[0] == '.' || 
			(ff[0] == 'C' && ff[1] == 'V' && ff[2] == 'S' && ff[3] == 0)) 
			continue;	
		OmnString dd = dname;
		dd << ff;
		//printf("File: %s\n", ff);

		// Check the file type
		stat(dd.data(), &thestat);
		if (S_ISDIR(thestat.st_mode))
		{
			if (recursive)
			{
				int nn = getFiles(dd, names, recursive);
				aos_assert_r(nn >= 0, -1);
				total += nn;
			}
			continue;
		}

		names.push_back(dd);
		total++;
		free(namelist[n]);
	}
	free(namelist);
	return total;
}


int
OmnFile::getFilesMulti(const OmnString &dirs, 
		std::list<OmnString> &names, 
		const OmnString &sep,
		const bool recursive)
{
	// This function retrieves files from multiple directories. All 
	// directories are specified by "dirs", which is in the following
	// format:
	// 	dir:dir:...
	chdir("/home/chen.ding/OpenLaszlo-4-4-1/lps-4.4.1/Server/lps-4.4.1");
	const int num_names = 500;
	char *dir_names[num_names];
	int nn = aos_sstr_split(dirs.data(), sep.data(), dir_names, num_names);
	aos_assert_r(nn < num_names, -1);

	int total = 0;
	int num = 0;
	for (int i=0; i<nn; i++)
	{
		num = getFiles(dir_names[i], names, recursive);
		aos_assert_r(num >= 0, -1);
		total += num;
	}

	aos_str_split_releasemem(dir_names, nn);
	return total;
}


bool
OmnFile::seek(const u64 &pos)
{
	if (!mFile)
	{
		OmnAlarm << "File null: " << mName << enderr;
		return false;
	}
	AOSFILE_COUNTING_REWIND;
	return (::fseek(mFile, pos, SEEK_SET) == 0);
}


bool
OmnFile::openFile1(const OmnString &fname, const Mode mode)
{
	mName = fname;
	mMode = mode;
	mFile = openFile(mName, mMode);
	return isGood();
}
	

u64
OmnFile::getFileCrtSize()
{
	aos_assert_r(mFile, 0);
	AOSFILE_COUNTING_SEEK;
	int64_t pos = ftell(mFile);
	::fseek(mFile, 0, SEEK_END);
	int64_t new_pos = ftell(mFile);	
	fseek(mFile, pos, SEEK_SET);
	return new_pos;
}


bool		
OmnFile::append(const char *buff, const int64_t &len, const bool toFlush)
{
	aos_assert_r(mFile, false);
	if (len <= 0) return false;

	AOSFILE_COUNTING_WRITE;
	int asize = ::fwrite(buff, 1, len, mFile) ;
	//if (asize < 0) return false; 
	if (asize != len)
	{
		OmnAlarm << "write err: " << strerror(errno) << enderr;
		exceptionThrow(__FILE__, __LINE__, errno);
		return false;
	}
	if (toFlush) 
	{
		AOSFILE_COUNTING_FLUSH;
		OmnFlushFile(mFile);
	}
	return true;
}


bool	
OmnFile::writeToFile(
		const OmnString &fname, 
		const u64 &offset, 
		const u32 size, 
		char *data)
{
	OmnFile ff(fname, eWriteCreate AosMemoryCheckerArgs);
	aos_assert_r(ff.isGood(), false);
	// aos_assert_r(::fseek(ff.mFile, offset, SEEK_SET) == 0, false);
	aos_assert_r(ff.put(offset, data, (int)size, true), false);
	ff.closeFile();
	return true;
}


// ken Lee, 2011/6/17
bool
OmnFile::setChar(const u64 &offset, 
		const char value, 
		const bool flushflag)
{
	// aos_assert_r(seek(offset), false);
	aos_assert_r(put(offset, (char *)&value, sizeof(value), flushflag), false);
	return true;
}


bool
OmnFile::setInt(const u64 &offset, 
		const int value, 
		const bool flushflag)
{
	// aos_assert_r(seek(offset), false);
	aos_assert_r(put(offset, (char *)&value, sizeof(value), flushflag), false);
	return true;
}



bool
OmnFile::setU32(const u64 &offset, 
		const u32 value, 
		const bool flushflag)
{
	// aos_assert_r(seek(offset), false);
	aos_assert_r(put(offset, (char *)&value, sizeof(value), flushflag), false);
	return true;
}



bool
OmnFile::setU64(const u64 &offset, 
		const u64 &value, 
		const bool flushflag)
{
	// aos_assert_r(seek(offset), false);
	aos_assert_r(put(offset, (char *)&value, sizeof(value), flushflag), false);
	return true;
}


bool
OmnFile::setStr(const u64 &offset, 
		const char *value, 
		const int len,
		const bool flushflag)
{
	if (len <= 0) return true;
	// aos_assert_r(seek(offset), false);
	aos_assert_r(put(offset, value, len, flushflag), false);
	return true;
}


bool
OmnFile::setStr(const u64 &offset, 
		const OmnString &value, 
		const bool flushflag)
{
	if (value.length() == 0) return true;
	// aos_assert_r(seek(offset), false);
	aos_assert_r(put(offset, value.data(), value.length(), flushflag), false);
	return true;
}


OmnString	
OmnFile::readStr(
		const u64 &offset, 
		const u32 len, 
		const OmnString &dft)
{
	OmnString str(len+1, ' ', true);
	char *data = (char *)str.data();
	u32 bytesRead = readToBuff(offset, len, data);
	if (bytesRead != len) return dft;
	return str;
}


bool
OmnFile::forceFlush()
{
	AOSFILE_COUNTING_FORCEFLUSH;
	aos_assert_r(mFile, false);
	::fclose(mFile);
	mFile = ::fopen64(mName.data(), "r+b");
	return true;
}


bool
OmnFile::verifyFile(const OmnFilePtr &file)
{
	if (!mFile) return false;
	if (!file->mFile) return false;

	FILE *rhs = file->mFile;

	AOSFILE_COUNTING_SEEK;
	::fseek(mFile, 0, SEEK_END);
	int64_t fsize1 = ::ftell(mFile);

	AOSFILE_COUNTING_SEEK;
	::fseek(rhs, 0, SEEK_END);
	AOSFILE_COUNTING_SEEK;
	int64_t fsize2 = ::ftell(rhs);

	if (fsize1 != fsize2) return false;

	const int bsize = 1000;
	char buff1[bsize+2];
	char buff2[bsize+2];
	for (int64_t i=0; i<fsize1; i+=bsize)
	{
		AOSFILE_COUNTING_SEEK;
		::fseek(mFile, i, SEEK_SET);
		AOSFILE_COUNTING_READ;
		int rr1 = ::fread(buff1, 1, bsize, mFile);

		AOSFILE_COUNTING_SEEK;
		::fseek(rhs, i, SEEK_SET);
		AOSFILE_COUNTING_READ;
		int rr2 = ::fread(buff2, 1, bsize, mFile);

		if (rr1 != rr2) return false;
		if (memcmp(buff1, buff2, rr1)) return false;
	}

	return true;
}


OmnFile::Mode
OmnFile::convertMode(const OmnString &mode)
{
	if (mode == AOSFILEMODE_READWRITE) return eReadWrite;
	if (mode == AOSFILEMODE_CREATE) return eCreate;
	if (mode == AOSFILEMODE_READONLY) return eReadOnly;
	if (mode == AOSFILEMODE_APPEND) return eAppend;
	return eInvalid;
}


void
OmnFile::setCaching(const int num_blocks, const int blockSize)
{
	mBlocksToCache = num_blocks;
	//mCacheBlockSize = blockSize;
}


int64_t
OmnFile::read(                      
		std::vector<AosBuffPtr> &buffs,
		const int64_t offset, 
		const int64_t len, 
		AosRundata *rdata)
{
	int readtotal = 0;
	for (size_t i=0; i<buffs.size(); i++)
	{
		int readsize= readToBuff(
				offset+readtotal, buffs[i]->buffLen(), buffs[i]->data());				
		if (readsize < 0)
		{
			break;
		}
		readtotal += readsize;
		buffs[i]->setDataLen(readsize);

		if (readsize != buffs[i]->buffLen())
		{
			break;
		}
	}
	return readtotal;
}


// Chen Ding, 2013/02/05
bool 
OmnFile::readToBuff(
		AosBuffPtr &buff, 
		const u64 &max_read_size)
{
	buff = 0;
	aos_assert_r(mFile, false);

	u64 length = getLength();
	aos_assert_r(length > 0 && length < max_read_size, false);

	try
	{
		buff = OmnNew AosBuff(length+100 AosMemoryCheckerArgs);
		char *data = buff->data();
		int64_t ss = readToBuff(0, length, data);
		aos_assert_r(ss > 0 && (u64)ss == length, false);
		buff->setDataLen(length);
		return true;
	}

	catch (...)
	{
		OmnAlarm << "run out of memory: " << length << enderr;
		return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


void
OmnFile::exceptionThrow(
		const OmnString &file_name, 
		const int line,
		const int t_errno)
{
	// 5  Input/output error; 6  No such device or address; 9  Bad file descriptor ;
	// 15 Block device required; 16 Device or resource busy; 18 Invalid cross-device link;
	// 19 No such device; 25 Inappropriate ioctl for device; 28 No space left on device;
	// 30 Read-only file system
	//	/usr/include/asm-generic/errno-base.h
	//	/usr/include/asm-generic/errno.h
	if (t_errno == EIO 
		||t_errno == ENXIO 
		||t_errno == EBADF 
		||t_errno == ENOTBLK 
		||t_errno == EBUSY
		||t_errno == EXDEV 
		||t_errno == ENODEV 
		||t_errno == ENOTTY
		||t_errno == ENOSPC
		||t_errno == EROFS)
	{
		OmnString errmsg = "<";
		errmsg << file_name << ":" << line 
			<< "> errno(" << t_errno << ") : " 
			<< strerror(t_errno);
		OmnThrowException2(OmnErrId::eIOError, errmsg);
	}
}

