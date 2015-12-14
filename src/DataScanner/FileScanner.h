////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 07/11/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataScanner_FileScanner_h
#define AOS_DataScanner_FileScanner_h

#include "DataScanner/DataScanner.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Util/ReadFile.h"

class AosFileScanner : virtual public AosDataScanner
{
	OmnDefineRCObject;

private:
	enum
	{
		eMinBuffSize = 100000000,
		eMinRecordSize = 100000
	};

	AosXmlTagPtr		mWriteFileInfo;
	AosXmlTagPtr 		mReadFileInfo;
	OmnString 			mFileName;
	int64_t				mStart;
	int64_t				mLength;
	int					mPhysicalid;
	u32					mRecordLen;
	int64_t				mFileLen;
	int64_t				mReadLength;
	int					mMaxTasks;		
	OmnString			mDelimiter;	
	AosBuffPtr			mBuff;
	int64_t				mCrtReadIdx;
	int64_t				mFileReadSize;
	OmnString			mRowDelimiter;
	OmnMutexPtr			mLock;
	bool				mIgnoreHead;
	OmnString			mCharacterType;

	//JACKIE-HADOOP
	bool				mIsHadoop;

public:
	AosFileScanner(const bool flag);
	AosFileScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosFileScanner();

	virtual bool	getNextBlock(
						AosBuffDataPtr &info,
						const AosRundataPtr &rdata);

	virtual bool	getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool	getNextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata);
	virtual bool 	appendBlock(
						const AosBuffPtr &buff, 
						const AosRundataPtr &rdata);
	virtual bool 	modifyBlock(
						const int64_t &seekPos, 
						const AosBuffPtr &buff, 
						const AosRundataPtr &rdata);
	virtual AosDataScannerObjPtr clone(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);

	virtual bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);

	// Chen Ding, 2013/11/25
	// virtual int64_t getTotalFileLength() { return mLength; }
	virtual int64_t getTotalSize() const {return mLength;}
	virtual OmnString getCrtFileName(){ return mFileName; }
	virtual AosJimoPtr cloneJimo() const;
	virtual int getPhysicalId() const;

	bool initFileScanner(
			const OmnString &fname,
			const int physical_id,
			const bool ignore_head,
			const OmnString &character_type,
			const OmnString &row_delimiter,
			const int record_len,
			//JACKIE-HADOOP
			const bool &ishadoop,
			const AosRundataPtr &rdata);

private:
	bool config(
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);

	bool createFileScanner(
			vector<AosDataScannerObjPtr> &scanners,
			const int64_t &start,
			const int64_t &len,
			const int64_t &filelen,
			const u32 recrodlen,
			const AosRundataPtr &rdata);
	bool getNextRecordByDelimiter(AosDataRecordObjPtr &record, const AosRundataPtr &rdata);
	bool getNextRecordFixedLen(AosDataRecordObjPtr &record, const AosRundataPtr &rdata);
	bool readVarLengthToBuff(AosBuffPtr &buff, const AosRundataPtr &rdata);
	bool readFixedLengthToBuff(AosBuffPtr &buff, const AosRundataPtr &rdata);
	char* strrstr(const char* s1, const int i, const char* s2);

};
#endif

