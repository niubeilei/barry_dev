////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LocalFile.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_LocalFile_h
#define Snt_Util_LocalFile_h

class OmnLocalFile : public OmnRCObject
{
protected:
	OmnString		mType;

public:
	OmnLocalFile(const OmnString &type)
	:
	mType(type)
	{
	}

	virtual OmnString getFileType() const {return mType;}
	virtual OmnString getFileName() const = 0;
	virtual int64_t getDeviceId() = 0;

	virtual bool 	isGood() const = 0;

	virtual bool    write(
			const int64_t &startPos,
			const char *buff,           
			const int64_t &len,
			const bool flush = false) = 0;

	virtual int64_t read(
			const int64_t &startPos, 
			const char *buff,
			const int64_t &len) = 0;

	virtual bool append(
			const char *buff,
			const int64_t &len,
			const bool flush = false) = 0;

	//bool    closeFile();

	virtual void    unlock() = 0;

	virtual void    readlock() = 0;

	virtual void    writelock() = 0;

	virtual void 	resetFile() = 0;

	virtual void deleteFile(AosRundata *rdata) = 0;

	virtual u64 getLength() = 0; 
};
#endif

