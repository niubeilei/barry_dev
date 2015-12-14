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
// 03/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_SizeIdMgr_h
#define Aos_StorageEngine_SizeIdMgr_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/SizeIdMgrObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "StorageEngine/Ptrs.h"
#include "StorageEngine/SizeIdIDGen.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"

#include <map>
#include <list>

using namespace std;


OmnDefineSingletonClass(AosSizeIdMgrSingleton,
		                AosSizeIdMgr,
						AosSizeIdMgrSelf,
						OmnSingletonObjId::eSizeIdMgr,
						"SizeIdMgr");
	

struct AosSizeIdKey
{
	u32 siteid;
	u64 sizeid;

	AosSizeIdKey(const u32 siteid, const u64 &sizeid)
	:
	siteid(siteid),
	sizeid(sizeid)
	{
	}

	bool operator < (const AosSizeIdKey &rhs) const
	{
		if (siteid != rhs.siteid)
		{
			return siteid < rhs.siteid;
		}
		else
		{
			return sizeid < rhs.sizeid;
		}
	}
};

typedef map<u32, AosSizeIdIDGenPtr>				AosSizeIdGenMap;
typedef map<u32, AosSizeIdIDGenPtr>::iterator	AosSizeIdGenMapItr;

typedef map<AosSizeIdKey, u64>			 AosDataRecordDocidMap;
typedef map<AosSizeIdKey, u64>::iterator AosDataRecordDocidMapItr;

typedef map<u64, AosDataRecordObjPtr>			AosDataRecordMap;
typedef map<u64, AosDataRecordObjPtr>::iterator AosDataRecordMapItr;

typedef list<AosDataRecordMapItr>			AosDataRecordList;
typedef list<AosDataRecordMapItr>::iterator	AosDataRecordListItr;

class AosSizeIdMgr : public AosSizeIdMgrObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxDataRecordNum = 10
	};

private:
	OmnMutexPtr     	mLock;
	AosSizeIdGenMap		mMap;

	AosDataRecordDocidMap	mDataRecordDocidMap;
	AosDataRecordMap		mDataRecordMap;
	AosDataRecordList		mDataRecordList;

public:
	AosSizeIdMgr();
	~AosSizeIdMgr();
	
	// Singleton class interface
	static AosSizeIdMgr*	getSelf();
	virtual bool            start();
	virtual bool            stop();
	virtual bool            config(const AosXmlTagPtr &def);
	
	virtual u64			createSizeId(
							const int size,
							const AosXmlTagPtr &record,
							const AosRundataPtr &rdata);
	
	virtual u64			createSizeId(
							const u64 &record_docid,
							const AosRundataPtr &rdata);
	virtual u64 		getCrtSizeId(
							const u64 &record_docid,
							const AosRundataPtr &rdata);
	virtual u64			getNextDocid(
							const u64 &record_docid,
							const AosRundataPtr &rdata);
	
	virtual bool		getDocids(
							const u64 &sizeid,
							u64 &start_docid,
							int &num_docids,
							bool &overflow,
							const AosRundataPtr &rdata);

	virtual inline u64	getSizeIdByDocid(const u64 &docid)
	{
		return AosSizeIdIDGen::getSizeIdByDocid(docid);
	}
	virtual inline u64	getLocalIdByDocid(const u64 &docid)
	{
		return AosSizeIdIDGen::getLocalIdByDocid(docid);
	}

	virtual u64			getDataRecordDocidBySizeId(
							const u32 siteid,
							const u64 &sizeid,
							const AosRundataPtr &rdata);
	
	virtual AosDataRecordObjPtr	getDataRecordBySizeId(
							const u32 siteid,
							const u64 &sizeid,
							const AosRundataPtr &rdata);
	
	virtual AosDataRecordObjPtr	getDataRecordByDocid(
							const u32 siteid,
							const u64 &docid,
							const AosRundataPtr &rdata);
	
	virtual int			getDataRecordLenBySizeId(
							const u32 siteid,
							const u64 &sizeid,
							const AosRundataPtr &rdata);

	virtual int			getDataRecordLenByDocid(
							const u32 siteid,
							const u64 &docid,
							const AosRundataPtr &rdata);

	virtual bool		removeDataRecordBySizeId(
							const u32 siteid,
							const u64 &sizeid,
							const bool flag,
							const AosRundataPtr &rdata);

private:
	AosSizeIdIDGenPtr	getSizeIdIDGen(
							const u32 siteid,
							const AosRundataPtr &rdata);

	AosDataRecordObjPtr	getDataRecordFromMap(
							const u64 &docid,
							const AosRundataPtr &rdata);

	bool 				setDataRecordToMap(
							const u64 &docid,
							const AosDataRecordObjPtr &record,
							const AosRundataPtr &rdata);

	u64					getDataRecordDocidFromMap(
							const u32 siteid,
							const u64 &sizeid,
							const AosRundataPtr &rdata);

	bool 				setDataRecordDocidToMap(
							const u32 siteid,
							const u64 &sizeid,
							const u64 &docid,
							const AosRundataPtr &rdata);


};
#endif

