////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description
//
// Modification History:
// 2013/10/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_ObjectCacher_h
#define AOS_SEUtil_ObjectCacher_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosObjectCacher : virtual public OmnRCObject
{
public:
	struct CachedObjKey
	{
		u64 docid;
		u32 siteid;
		CachedObjKey(const u64 did, const u32 sid)
		:
		docid(did),
		siteid(sid)
		{
		}
	};

	struct CachedObjHash
	{
		inline size_t operator()(const CachedObjKey &rhs) const
		{
			return rhs.docid;
		}
	};

	struct CachedObjCompare
	{
		bool operator ()(const CachedObjKey &lhs, const CachedObjKey &rhs) const
		{
			return lhs.siteid == rhs.siteid && lhs.docid == rhs.docid;
		}
	};

	OmnDefineRCObject;

public:
	enum
	{
		eMaxCachedObjs = 1000
	};

private:
	typedef hash_map<const CachedObjKey, AosCachedObjectPtr, CachedObjHash, CachedObjCompare> map_t;
	typedef hash_map<const CachedObjKey, AosCachedObjectPtr, CachedObjHash, CachedObjCompare>::iterator mapitr_t;

	OmnMutexPtr			mLock;
	map_t				mObjMap;
	AosCachedObjectPtr	mHead;
	u32					mMaxCachedObjs; 

public:
	AosObjectCacher(const u64 max_cached_objs);
	~AosObjectCacher();

	bool start();
	bool stop();
	bool config(const AosXmlTagPtr &conf);

	virtual bool addObj(const AosRundataPtr &rdata, 
						const AosCachedObjectPtr &doc, 
						const u64 docid);
	virtual bool removeObj(
						const AosRundataPtr &rdata,
						const u64 docid);
	virtual AosCachedObjectPtr getObjByDocid(
						const AosRundataPtr &rdata,
						const u64 docid);
	virtual OmnString	getLogs();

	// Testing purpose only, Chen Ding, 2011/01/27
	u32	 			getNumCached(){return mObjMap.size();};
	AosCachedObjectPtr	getLastObj();
	AosCachedObjectPtr	getFirstObj() const;
	u32				getCacheSize() const {return mMaxCachedObjs;}
	bool			sanityCheck(const bool lock = true);
	
private:
	bool			removeNodeLocked(const AosCachedObjectPtr &node);
	bool			moveNodeLocked(const AosCachedObjectPtr &node);
};
#endif

