////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemChecker.h
// Description:
//   This class provides memory management.   
//
// Modification History:
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util1_MemChecker_h
#define Omn_Util1_MemChecker_h

#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
using namespace std;

#define OmnCheckMemory 1

// define all to null first
#define OmnMemAdd(x,y)
#define OmnMemRemove(x,y)
#define OmnMemCheck(x,y)

#ifdef OmnCheckMemory

#ifndef OmnMemAdd
#define OmnMemAdd(x,y) mMemChecker.addMem((x),(y),__FILE__,__LINE__);
#endif

#ifndef OmnMemRemove
#define OmnMemRemove(x,y) mMemChecker.removeMem((x),(y),__FILE__,__LINE__);
#endif

#ifndef OmnMemCheck
#define OmnMemCheck(x,y) mMemChecker.checkMem((x),(y),__FILE__,__LINE__);
#endif

#endif

//#define AosIILStrCheckMemory(mem, size) 
//#define AosIILStrCheckMemory(mem, size) mMemChecker.checkMem((mem), (size), __FILE__, __LINE__)


class AosMemChecker : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eInitSize = 100000
	};
	
	struct AosMemBlk{
		void *     	mem;
		u32			size;
		OmnString 	filename;
		u32			line;

		AosMemBlk()
		{
		}
		
		AosMemBlk(void * m,const u32 s,const OmnString &name,const u32 l)
		:
		mem(m),
		size(s),
		filename(name),
		line(l)
		{
		}
		
		AosMemBlk(const AosMemChecker::AosMemBlk &b2)
		:
		mem(b2.mem),
		size(b2.size),
		filename(b2.filename),
		line(b2.line)
		{
		}
		void clear()
		{
			mem = 0;
			size = 0;
			filename = "";
			line = 0;
		}	
	};

private:
	OmnMutexPtr			mLock;
	bool				mCheckFlag;
	AosMemBlk *			mBlocks;
//	u32	*				mMemory;
//	u32	*				mSize;
	u64					mCrtSize;
	int					mNumEntries;

public:
	AosMemChecker();
	virtual ~AosMemChecker();

	bool	addMem(void *mem, const int size,const OmnString &file,const int &line) 
			{
				if (!mCheckFlag) return true;
				return addMemPriv(mem, size, file, line);
			}
	bool	removeMem(void *mem, const int size,const OmnString &file,const int &line)
			{
				if (!mCheckFlag) return true;
				return removeMemPriv(mem, size, file, line);
			}
	bool	checkMem(void *mem, const int size,const OmnString &file,const int &line)
			{
				if (!mCheckFlag) return true;
				return checkMemPriv(mem, size, file, line);
			}

private:
	bool	addMemPriv(void *mem, const int size,const OmnString &file,const int &line);
	bool	removeMemPriv(void *mem, const int size,const OmnString &file,const int &line);
	bool	checkMemPriv(void *mem, const int size,const OmnString &file,const int &line);
	bool	removeEntry(const int idx);
	bool	insertAt(const int idx, const AosMemBlk &block);
};
#endif

