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
// This is a specialized class. All it does is reading a file, sequencially.
// Initially, one should call 'seek()', after that, reading is always 
// sequential.
//   
//
// Modification History:
// 03/18/2012, Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_Util_ReadFile_h
#define Snt_Util_ReadFile_h

#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "Porting/File.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/FileDesc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"

class AosReadFile : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString			mName;
	FILE *				mFile;
	OmnMutexPtr			mLock;
	int64_t				mFileLength;

public:
	AosReadFile();
	AosReadFile(const OmnString &name);
	virtual ~AosReadFile();

	bool		openFile();
	bool		closeFile();
	void		lock() {mLock->lock();}
	void		unlock() {mLock->unlock();}

	bool		isGood() const {return mFile != 0;}
	OmnString	getFileName() const {return mName;}

	int64_t		readBinaryInt64(const int64_t &dft);
	int			readBinaryInt(const int dft);
	u32			readBinaryU32(const u32 dft);
	u64			readBinaryU64(const u64 &dft);
	char		readChar(const char &dft);
	OmnString	readStr(const u32, const OmnString &);
	int			getWord(char *word, const int length);
	OmnString	getLine(bool &finished);
	bool		readToString(OmnString &buf, const int buflen);
	int			readToBuff(const u32 size, char *buff);
	void		setName(const OmnString &name) {mName = name;}
	FILE*		getFile() {return mFile;}
	bool		seek(const u64 &pos);
	int64_t		getLength()
				{
					::fseeko64(mFile, 0, SEEK_END);
					return ftello64(mFile);
				}

	void goToFileEnd()
	{
		aos_assert(mFile);
		::fseeko64(mFile, 0, SEEK_END);
	}
};
#endif

