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
// 2013/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StreamFS/SmallDocStore.h"

#include "ReliableFile/ReliableFile.h"
#include "Thread/Mutex.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/DocTypes.h"
#include "SEInterfaces/Ptrs.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "StreamFS/SmallDocContainer.h"

OmnSingletonImpl(AosSmallDocStoreSingleton,
				 AosSmallDocStore,
				 AosSmallDocStoreSelf,
				 "SmallDocStore");


AosSmallDocStore::AosSmallDocStore()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosSmallDocStore::~AosSmallDocStore()
{
}


bool 
AosSmallDocStore::retrieveDoc(
		const AosRundataPtr &rdata, 
		const u64 docid,
		AosDataRecordObjPtr &doc)
{
	// This function retrieves the doc identified by 'docid'. 
	// Given a docid, it determines the container's docid by
	// dividing 'docid' by the container size. It then retrieves
	// the container and asks the container to retrieve the doc.
	//doc = 0;
	//AosSmallDocContainer container = getContainer(rdata, docid);
	//if (!container) true;

	//return container->retrieveDoc(rdata, docid, doc);
	
	return true;
}


AosBuffPtr
AosSmallDocStore::retrieveDoc(
		const AosRundataPtr &rdata, 
		const u64 &smallDocid, 
		const u32 &entryNums)
{
	// 1. get doc with really docid.
	// 	docid = eLogDoc(8) + groupid(56)
	mLock->lock();
	u64 group_id = getRealDocid(smallDocid, entryNums);
OmnScreen << "group_id: " << group_id << endl;
	map<u64, AosSmallDocContainerPtr>::iterator itr = mCntrMap.find(group_id);
	if (itr == mCntrMap.end())
	{
		mLock->unlock();
		return OmnNew AosBuff(100 AosMemoryCheckerArgs);
	}	
	AosSmallDocContainerPtr cntr = itr->second;
	aos_assert_rl(cntr, mLock, 0); 
	AosBuffPtr buff = cntr->retrieveDoc(rdata, smallDocid, entryNums);
	aos_assert_rl(buff, mLock, 0);
	mLock->unlock();
	return buff;
}


bool 
AosSmallDocStore::addDocs(
		const u64 group_id, 
		const u32 entryNums,
		const AosBuffPtr &contents,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	// This function adds all the docs in 'contents' to the store. 
	// 'contents' is a buff in the following format:
	// If finds the current active container. If the current active
	// container has not been created yet, it creates one. It adds
	// the contents to the container until either all the docs
	// are added to the container or the container is full. If 
	// the container is full, it will create another container.

	mLock->lock();
	aos_assert_r(contents && entryNums > 0, false);
	map<u64, AosSmallDocContainerPtr>::iterator cntrMapItr = mCntrMap.find(group_id);
	if (cntrMapItr == mCntrMap.end())
	{
		AosSmallDocContainerPtr cntr = getContainer(rdata, group_id);
		aos_assert_rl(cntr, mLock, false);
		bool rslt = cntr->addDocs(group_id, contents, trans_id, rdata);
		aos_assert_rl(rslt, mLock, false);
		mCntrMap[group_id] = cntr;
	}
	else
	{
		AosSmallDocContainerPtr cntr = cntrMapItr->second;
		aos_assert_rl(cntr, mLock, false);
		bool rslt = cntr->addDocs(group_id, contents, trans_id, rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	
	mLock->unlock();
	return true;
}


AosSmallDocContainerPtr 
AosSmallDocStore::getContainer(
		const AosRundataPtr &rdata, 
		const u64 group_id)
{
	// This function retrieves a new container 
	aos_assert_r(group_id > 0, 0);
	try
	{
		return OmnNew AosSmallDocContainer();
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_container");
		return 0;
	}

	return 0;
}


bool 
AosSmallDocStore::ageOutDocs(
		const AosRundataPtr &rdata,
		const u64 group_id,
		const AosTransId &trans_id)
{
	aos_assert_r(rdata, false);
	map<u64, AosSmallDocContainerPtr>::iterator itr = mCntrMap.find(group_id);
	if (itr != mCntrMap.end())
	{
		AosSmallDocContainerPtr cntrobj = itr->second;
		cntrobj->ageOutDocs(rdata, group_id, trans_id);
		mCntrMap.erase(itr);
	}
	else
	{
		AosSetErrorU(rdata, "group_id_not_found") << enderr;
		return false;
	}

	return true;
}


u64 		
AosSmallDocStore::getRealDocid(
		const u64 smallDocid, 
		const u32 entryNums)
{
	// the samll docid is not real docid
	AosDocType::E type = AosDocType::getDocType(smallDocid);
	u64 ownSmallDocid = AosXmlDoc::getOwnDocid(smallDocid);
	u64 rdocid = ownSmallDocid / entryNums;
	AosDocType::setDocidType(type, rdocid);

	return rdocid;
}
