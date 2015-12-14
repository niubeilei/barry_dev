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
// 10/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_Util_FileWBack_h
#define Snt_Util_FileWBack_h

#include <vector>
#include "aosUtil/Types.h"
#include "Porting/File.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

using namespace std;


class AosFileWBack : public virtual OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxCopies = 10
	};

	enum Mode
	{
		eSingle,
		eNormal
	};

	OmnMutexPtr			mLock;
	OmnString			mFname;
	int					mCopies;
	bool				mCreate;
	OmnFilePtr			mActiveFiles[eMaxCopies];
	int					mActiveLocs[eMaxCopies];
	int					mNumActives;
	vector<OmnString>	mLocations;

public:
	AosFileWBack();
	AosFileWBack(const AosXmlTagPtr &config);
	virtual ~AosFileWBack();

	bool	init(const AosXmlTagPtr &config);
	bool	init();
	bool isGood() const 
	{
		if (mNumActives <= 0) return false;
		return true;
	}

	bool 		put(const u64 &, const char *str, const int len, const bool flush);
	bool		readBinaryInt(
					const u64 &offset, 
					int &v, 
					const int &dft, 
					const int &min, 
					const int &max);
	bool		readBinaryInt64(
					const u64 &offset, 
					int64_t &v, 
					const int64_t &dft, 
					const int64_t &min, 
					const int64_t &max);
	bool		readBinaryU32(
					const u64 &offset, 
					u32 &v, 
					const u32 &dft, 
					const u32 &min, 
					const u32 &max);
	bool		readBinaryU64(
					const u64 &offset, 
					u64 &v, 
					const u64 &dft, 
					const u64 &min, 
					const u64 &max);
	int         readToBuff(const u64 &startPos,
					const u32 &size,
					char *buff);

	bool		setInt(const u64 &offset, 
					const int value, 
					const bool flushflag);
	bool		setU32(const u64 &offset, 
					const u32 value, 
					const bool flushflag);
	bool		setU64(const u64 &offset, 
					const u64 &value, 
					const bool flushflag);
	bool		setStr(const u64 &offset, 
					const char *value, 
					const int len,
					const bool flushflag);
	bool		setStr(const u64 &offset, 
					const OmnString &value, 
					const bool flushflag);
	bool		resetToEmpty();
	OmnString	toString() const;

private:
	bool	repairFile(const int idx);
	bool	locationUsed(const int idx);
	OmnFilePtr	openFile(const int idx AosMemoryCheckDecl);
	bool	consistenceCheck();
};
#endif

