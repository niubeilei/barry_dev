////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: File.h
// Description:
//   
//
// Modification History:
// 03/31/2009 Created by Sally Sheng 
// 12/31/2012 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_LogServer_LogFile_h 
#define AOS_LogServer_LogFile_h

#include "aosUtil/Types.h"
#include "Util/File.h"
#include "LogServer/Ptrs.h"
#include "Util/String.h"
#include "LogServer/LogEntry.h" 
#include "Util/Ptrs.h"
#include "Util/RCObject.h"


class AosLogFile : public virtual OmnRCObject
{
	OmnDefineRCObject;

public:
	enum TruncateMethod
    {
        ePercent,
        eSize,
        eEntry
    };

private:
	OmnFilePtr      mFile;
	u64             mFileLength;
	u64             mMaxFileSize;
	TruncateMethod  mTruncateMethod;
	u64             mTotalEntries;
	

public:
	AosLogFile();
	AosLogFile(const OmnString &filename, const u64 &maxsize, const int &truMethod);
	~AosLogFile();


	u64            getFileLength() const {return mFileLength;}
	u64            getMaxFileSize() const {return mMaxFileSize;}
	TruncateMethod getTruncateMethod() const {return mTruncateMethod;}
	OmnFilePtr     getFile() const {return mFile;}
	u64            getTotalEntries() const {return mTotalEntries;}

	OmnFilePtr     createFile(const OmnString &filename);
	bool           addLogEntry(TiXmlNode *logentry);
	int            getPage(const OmnString &logid);
	bool           openFile();
    bool           deleteFile();
	bool           clearFile();
	bool           closeFile();

	void           setMaxFileSize(const u64 &maxsize) {mMaxFileSize = maxsize;}
	void           setFileSize(const u64 filesize) {mFileSize = filesize;}
	void           setTruncateMethod(const int &method) {mTruncateMethod =(TruncateMethod)method;}
	void           setTotalEntries(const u64 &totalentries) {mTotalEntries = totalentries;}
	bool           setFile(const OmnString &filename); 


private:
    bool           truncate();

};
#endif
#endif
