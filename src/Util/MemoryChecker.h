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
// Modification History:
// 10/02/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_MemoryChecker_h
#define AOS_Util_MemoryChecker_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/ClassNames.h"
#include "Util/HashUtil.h"
#include <map>
using namespace std;


#define AosMemoryCheckDeclBegin const char *memory_checker_fname, const int memory_checker_lineno 
#define AosMemoryCheckDecl , const char *memory_checker_fname, const int memory_checker_lineno 
#define AosMemoryCheckNames , memory_checker_fname, memory_checker_lineno 
#define AosMemoryCheckerArgsBegin __FILE__, __LINE__
#define AosMemoryCheckerArgs ,__FILE__, __LINE__
#define AosMemoryCheckerFileLine ,memory_checker_fname,memory_checker_lineno
#define AosMemoryCheckerFileLineBegin memory_checker_fname,memory_checker_lineno
#define AosMemoryCheckerObjCreated(classname) AosMemoryChecker::getSelf()->objCreated(this, (classname), memory_checker_fname, memory_checker_lineno);
#define AosMemoryCheckerObjDeleted(classname) AosMemoryChecker::getSelf()->objDeleted(this, (classname));
#define AosMemoryCheckerSetFileLine(obj) (obj)->setFileLine(memory_checker_fname, memory_checker_lineno);
#define AosMemoryCheckerGetMembers(obj) ,(obj)->getMemoryCheckerFname().data(),(obj)->getMemoryCheckerLineno()

/*
#define AosMemoryCheckDeclBegin 
#define AosMemoryCheckDecl 
#define AosMemoryCheckNames 
#define AosMemoryCheckerArgsBegin 
#define AosMemoryCheckerArgs 
#define AosMemoryCheckerFileLine 
#define AosMemoryCheckerFileLineBegin 
#define AosMemoryCheckerObjCreated(classname) 
#define AosMemoryCheckerObjDeleted(classname) 
#define AosMemoryCheckerSetFileLine(obj) 
#define AosMemoryCheckerGetMembers(obj)
*/

OmnDefineSingletonClass(AosMemoryCheckerSingleton,
						AosMemoryChecker,
						AosMemoryCheckerSelf,
						OmnSingletonObjId::eMemoryChecker,
						"MemoryChecker");

class AosMemoryCheckerObj;

class AosMemoryChecker : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eNumBuckets = 10000,
		eBucketSize = 5,
		eMetaSize = 80,
		eMaxFnameLen = 30,
		eLineStart = 8,
		eFnameStart = 12
	};

	typedef hash_map<const u64, AosMemoryCheckerObj*, u64_hash, u32_cmp> map_t;
	typedef hash_map<const u64, AosMemoryCheckerObj*, u64_hash, u32_cmp>::iterator mapitr_t;

private:
	OmnMutexPtr 	mLock;
	OmnCondVarPtr	mCondVar;
	OmnThreadPtr	mThread;
	AosStr2Int64_t	mMap;
	map_t			mObjMap;

	// Chen Ding, 10/31/2012
	static u64			mThreadIds[eNumBuckets][eBucketSize];
	static const char *	mFnames[eNumBuckets][eBucketSize];
	static int			mLines[eNumBuckets][eBucketSize];

	static bool			smCheckOpen;
	static int			smCheckFreq;

public:
	AosMemoryChecker();
	~AosMemoryChecker();

    // Singleton class interface
    static AosMemoryChecker *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosMemoryChecker";}
    virtual bool		config(const AosXmlTagPtr &def);

	void 	report();
	bool	objCreated(
				AosMemoryCheckerObj *obj,
				const AosClassName::E name, 
				const char *file,
				const int line);
	bool	objDeleted(
				AosMemoryCheckerObj *obj,
				const AosClassName::E name); 

	static inline bool	getCheckOpen() {return smCheckOpen;}
	static inline int	getCheckFreq() {return smCheckFreq;}

};
#endif

