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
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_NetFile_NetFile_h
#define AOS_NetFile_NetFile_h

#include "NetFile/NetFileClt.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/NetFileObj.h"
#include "Util/BuffArrayVar.h"


class AosNetFile : virtual public AosNetFileObj,
				   virtual public AosAioCaller 
{
	OmnDefineRCObject;

protected:
	int				mServerId;
	int64_t			mStartOffset;
	int64_t			mFileLength;
	int64_t			mCrtOffset;

	AosBuffPtr 				mBuff;
	AosFileReadListenerPtr	mCaller; 

public:
	AosNetFile();

	AosNetFile(const int server_id);

	~AosNetFile();

	virtual int getServerId();

	virtual AosBuffPtr getIoBuff(const u64 &reqId, const int64_t &size AosMemoryCheckDecl);

	virtual void dataRead(const Parameter &parm, const AosBuffPtr &buff);

	virtual void readError(u64 &reqid, OmnString &errmsg);

	virtual void dataWrite(const u64 &reqid);

	virtual void writeError(u64 &reqid, OmnString &errmsg);

	virtual void setCaller(const AosFileReadListenerPtr &caller);

	virtual AosFileReadListenerPtr getCaller();

	virtual void seek(const int64_t start_pos);
	virtual void moveToFront(const int64_t pos);

	AosBuffPtr getBuff();
};


class AosIdNetFile : public AosNetFile 
{
	u64		mFileId;

public:
	AosIdNetFile(
			const int server_id,
			const u64 &fileid);
	
	AosIdNetFile(const AosFileInfo &fileinfo);
	
	~AosIdNetFile();
	
	virtual u64 getFileId();

	virtual int64_t length(AosRundata *rdata);

	virtual bool readData(
			const u64 &reqId, 
			const int64_t &blocksize, 
			AosRundata *rdata);
};


class AosNameNetFile : public AosNetFile
{

private:
	OmnString		mFileName;

public:
	AosNameNetFile(
			const int server_id,
			const OmnString &filename);

	AosNameNetFile(const AosFileInfo &fileinfo);

	~AosNameNetFile();

	virtual int64_t length(AosRundata *rdata);

	virtual bool readData(
			const u64 &reqId,
			const int64_t &blocksize,
			AosRundata *rdata);
};


class AosIdNetFileVar : virtual public AosNetFile 
{
public:
	u64							mFileId;
	bool						mUsingArray;
	bool						mIsReading;
	deque<AosBuffArrayVarPtr>	mArray;
	AosCompareFunPtr        	mComp;

public:
	AosIdNetFileVar(
			const int server_id,
			const u64 &fileid);

	AosIdNetFileVar(
			const int server_id,
			const u64 &fileid,
			const AosCompareFunPtr &comp);
	
	AosIdNetFileVar(const AosFileInfo &fileinfo);

	~AosIdNetFileVar();
	
	virtual int64_t length(AosRundata *rdata);

	virtual bool readData(
			const u64 &reqId, 
			const int64_t &blocksize, 
			AosRundata *rdata);

	virtual void dataRead(const Parameter &parm, const AosBuffPtr &buff);

	virtual bool sanitycheck(char *begin, int length);

	virtual bool checkBodyAddrs(set<i64> &bodyAddrSet);
};

class AosCompNetFile : public AosNetFile 
{
	u32							mIdx;
	map<int, AosFileInfo>		mFileInfos;
public:
	AosCompNetFile(const AosFileInfo &fileinfo);
	
	~AosCompNetFile();
	
	virtual int64_t length(AosRundata *rdata);

	virtual bool readData(
			const u64 &reqId, 
			const int64_t &blocksize, 
			AosRundata *rdata);

	virtual void dataRead(const Parameter &parm, const AosBuffPtr &buff) ;
};


#endif
