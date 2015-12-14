////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/01/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IdGen_IdGenDef_h
#define AOS_IdGen_IdGenDef_h

#include "Thread/Ptrs.h"
#include "IdGen/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/SyncServerReqCb.h"


class AosIdGenDef : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxNumRecords = 10000,
		eMaxLenValue = 10000,
		eMaxBlockSize = 1000000,

		eOffsetNumRecords = 40,
		eRecordSize = 2000,
		eRecordStart = 2000,
		eFlag = 0x63246396,
		
		eIdNameMaxLen = 100,
		eCrtIdMaxLen = 100,
		eMaxIdMaxLen = 20,
		eRangesMaxLen = 1000,

		eOffsetRecordSize = 0, 
		eOffsetFlag = eOffsetRecordSize + 4,
		eOffsetMaxLen = eOffsetFlag + 4,
		eOffsetBlockSize = eOffsetMaxLen + 4,
		eOffsetNextBlockSize = eOffsetBlockSize + 4,
		eOffsetIdName = eOffsetNextBlockSize + 4,
		eOffsetCrtId = eOffsetIdName + eIdNameMaxLen,
		eOffsetMaxId = eOffsetCrtId + eCrtIdMaxLen,
		eOffsetRanges = eOffsetCrtId + eMaxIdMaxLen
	};

private:
	OmnString		mIdName;
	u32				mIdx;
	u32				mMaxLen;
	u32				mBlockSize;
	u32				mNextBlockSize;
	OmnString		mCrtId;
	u64				mMaxId;
	OmnString		mRanges;
	OmnString       mRemoteAddr;	
	int             mRemotePort;

public:
	AosIdGenDef(const AosFileWBackPtr &file, const u32 idx);
	~AosIdGenDef();

	static AosIdGenDefPtr createNewDef(
		const OmnString &name,
		const AosFileWBackPtr &backup, 
		const AosXmlTagPtr &def, 
		const int idx);

	static bool	readDefs(
		const AosFileWBackPtr &file, 
		AosIdGenDefPtr *defs, 
		int &num);

	OmnString	getRanges() const {return mRanges;}
	u32			getBlockSize() const {return mBlockSize;}
	u32			getMaxLen() const {return mMaxLen;}
	OmnString	getName() const {return mIdName;}
	OmnString	getCrtId() const {return mCrtId;}
	u64			getCrtIdU64() const {return atoll(mCrtId.data());}
	u64			getMaxIdU64() const {return mMaxId;}

	bool 	updateId(const OmnString &crtid);
	bool 	updateId(const u64 &crtid)
	{
		OmnString dd;
		dd << crtid;
		return updateId(dd);
	}
	bool		getNextBlock(u64 &newId, u64 &maxId);
	bool 		getCrtidU64(u64 &crtid);

	bool 		setCrtid(const u64 &id);
	bool		setBlocksize(const u32 &bsize);
	u32			getBlock(){return mBlockSize;}
	OmnString	getCrtid(){return mCrtId;}
	bool		readDef(const AosFileWBackPtr &file, const int idx, OmnString &errmsg);
	AosIdGenDefPtr	createNewDef(
					const AosFileWBackPtr &file, 
					const AosXmlTagPtr &def, 
					const int idx);

private:
	bool 	incrementCrtId(const AosFileWBackPtr &file, u64 &crtid);
	bool 	modifyField(const u32 offset, const u32 &value);
	bool 	modifyField(const u32 offset, const OmnString &value);
};

#endif
