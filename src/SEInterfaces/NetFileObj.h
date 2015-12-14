////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/08/07	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_NetFileObj_h
#define AOS_SEInterfaces_NetFileObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/FileReadListener.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/AioCaller.h"

#include <dirent.h>
#include <sys/stat.h>
#include <vector>

using namespace std;
class AosDiskStat;


class AosDirInfo
{
public:
	int         mPhysicalId;
	OmnString   mDirName;
	OmnString   mCharset;

	AosDirInfo();

	AosDirInfo(const OmnString &dirname);

	bool	serializeFrom(const AosBuffPtr &buff);

	bool	serializeTo(const AosBuffPtr &buff);

	bool	serializeFrom(const AosXmlTagPtr &xml);

	OmnString	serializeToXmlStr();
};


class AosFileInfo
{
public:
	u64         			mFileId;
	OmnString   			mFileName;
	int64_t     			mFileLen;
	int64_t     			mTotalLen;
	int         			mCrtBlockIdx;
	int64_t    				mStartOffset;
	int         			mPhysicalId;
	int64_t    				mShuffleId;
	OmnString   			mCharset;
	u64						mFromTaskDocid;
	OmnString				mDataColId;
	OmnString 				mTaskDataType;
	int						mLevel;
	map<int, AosFileInfo> 	mFileBlocks;

	AosFileInfo();
	AosFileInfo(struct stat &st);
	bool	serializeFrom(const AosBuffPtr &buff);

	bool	serializeTo(const AosBuffPtr &buff);

	bool	serializeFrom(const AosXmlTagPtr &xml);

	OmnString	serializeToXmlStr();
};


class AosNetFileObj : public OmnRCObject
{
public:
	virtual bool	append(
						const char *buff,
						const int len,
						const bool flush = false);
	virtual int64_t	read(
						vector<AosBuffPtr> &buffs,
						const int64_t offset,
						const int64_t len,
						const AosRundataPtr &rdata);

	virtual int64_t	length(AosRundata *rdata);

	virtual bool	readData(
						const u64 &reqId, 
						const int64_t &blocksize,
						AosRundata *rdata);

	virtual void	setCaller(const AosFileReadListenerPtr &caller);

	virtual AosFileReadListenerPtr	getCaller();

	virtual void	seek(const int64_t start_pos);
	virtual void moveToFront(const int64_t pos);

	virtual AosBuffPtr	getBuff();

	virtual int getServerId();

	virtual u64 getFileId();
};

#endif

