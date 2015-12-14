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
// 1. This class maintaines the fixed numbers of buckets to cache iiltrans 
// that is a sorted  array of AosIILEntry. 
// 2. Every bucket has boundary of low and hight, the entries will append
// to the matched bucket by the range. 
// 3. The bucket can automaticly ajust range.  The mechanism of mergering 
// and spliting bucket by the aging and total entries  of the bucket. 
// If the bucket is too old. it will be merged to it's left bucket or the
// right bucket, and the bucket will free. If the bucket has full, 
// the entries will be proccesed, and the bucket will free. when the entrie 
// has not in all the buckets range .it will get the  free buckets 
// to split the range.
// 
// Modification History:
// 04/09/2012 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "IILMerger/IILMergerMgr.h"

#include "IILMerger/IILMerger.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"

OmnSingletonImpl(AosIILMergerMgrSingleton,
				 AosIILMergerMgr,
				 AosIILMergerMgrSelf,
				 "IILMergerMgr");

OmnMutexPtr		AosIILMergerMgr::smLock = OmnNew OmnMutex(); 
map<OmnString, AosIILMergerPtr> AosIILMergerMgr::smMergers;

AosIILMergerMgr::AosIILMergerMgr()
{
}


AosIILMergerMgr::~AosIILMergerMgr()
{
}

bool
AosIILMergerMgr::start()
{
	return true;
}

bool
AosIILMergerMgr::stop()
{
	return true;
}

bool
AosIILMergerMgr::config(const AosXmlTagPtr &def)
{
	return true;
}

bool 
AosIILMergerMgr::create(
		const OmnString &tableid,  
		const AosIILMergerHandlerPtr &handler,
		const int num_syncs,
		const int maxbkt,
		const u32 maxmementries,
		const u32 maxentries,
		const AosRundataPtr &rdata)
{
	aos_assert_r(maxentries > maxmementries, false);
	smLock->lock();
	AosIILMergerPtr merger;
	map<OmnString, AosIILMergerPtr>::iterator itr = smMergers.find(tableid);
	if (itr == smMergers.end())	
	{
		OmnScreen << "mmmmmmmmmmm: Create New: " << tableid << endl;
		merger = OmnNew AosIILMerger(handler, maxbkt, maxmementries, maxentries, 0, 0);
		merger->setNumSyncs(num_syncs);
		merger->setTableid(tableid);
		smMergers.insert(make_pair(tableid, merger));
	}
	else
	{
		merger = itr->second;
		merger->setNumSyncs(num_syncs);
		OmnScreen << "mmmmmmmmmmm: Found one" << endl;
	}
	aos_assert_rl(merger, smLock, false);

	merger->updateTotalClient();
	int totalclient = merger->getTotalClient();

	smLock->unlock();
	
	OmnScreen << "mmmmmmmmmmmmmm: " << tableid << ", mTotalClient:" << totalclient
		<< ":" << merger->getNumSyncs() << ":" << merger->getTotalFinished() << endl;
	return true;
}

bool 
AosIILMergerMgr::add(
		const OmnString &tableid, 
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	smLock->lock();
	map<OmnString, AosIILMergerPtr>::iterator itr = smMergers.find(tableid);
	if (itr != smMergers.end())	
	{
		AosIILMergerPtr merger = itr->second;
		smLock->unlock();

		aos_assert_r(merger, false);
		return merger->addDataSync(buff, rdata);
	}
	
	smLock->unlock();
	OmnAlarm << "iil merger has not created!.tableid:" << tableid << enderr;
	return false;
}


u64
AosIILMergerMgr::getNumEntries(const OmnString &tableid)
{
	smLock->lock();
	map<OmnString, AosIILMergerPtr>::iterator itr = smMergers.find(tableid);
	if (itr != smMergers.end())	
	{
		AosIILMergerPtr merger = itr->second;
		smLock->unlock();

		aos_assert_r(merger, false);
		return merger->getNumEntries();
	}
	
	smLock->unlock();
	OmnAlarm << "iil merger has not created!.tableid:" << tableid << enderr;
	return 0;
}


bool 
AosIILMergerMgr::finish(
		const OmnString &tableid, 
		const u64 num_entries,
		const u64 &jobdocid,
		const int &jobServerId,
		const OmnString &tokenid,
		const AosRundataPtr &rdata)
{
	OmnScreen << "mmmmmmmmmmmmmm Finished: " << tableid << endl;
	OmnThrdShellProcPtr runner;
	smLock->lock();
	map<OmnString, AosIILMergerPtr>::iterator itr = smMergers.find(tableid);
	if (itr != smMergers.end())	
	{
		AosIILMergerPtr merger = itr->second;
		merger->updateTotalFinished();
		int totalfinished = merger->getTotalFinished();
		merger->addFinishedEntries(num_entries);
		smLock->unlock();
		aos_assert_r(merger, false);
		
		OmnScreen << "mmmmmmmmmmmmmm: " << tableid << ", mTotalClient:" 
			<< merger->getTotalClient() << ":" << merger->getNumSyncs()
			<< ":" << totalfinished << endl;

		if (merger->isFinished())
		{
			OmnScreen << "mmmmmmmmm: begin merging, starttime:" << OmnGetSecond() << endl;
			return merger->mergeFinshed(tableid, jobdocid, jobServerId, tokenid, rdata);	
			//runner = OmnNew MergeData(AosIILMergerMgr::getSelf(), merger.getPtr(), tableid, jobobjid, jobdocid, jobServerId, tokenid, rdata);
			//bool rslt = OmnThreadShellMgr::getSelf()->proc(runner);
		}
		return true;
	}
	
	smLock->unlock();
	OmnAlarm << "iil merger has not created!.tableid : " << tableid << enderr;
	return false;
}

bool
AosIILMergerMgr::deleteMerger(const OmnString tableid)
{
	smLock->lock();
	map<OmnString, AosIILMergerPtr>::iterator itr = smMergers.find(tableid);
	if (itr != smMergers.end())	
	{
		smMergers.erase(itr);
		OmnScreen << " mmmmmmmm : merger successed !" << endl;
	}
	else
	{
		OmnAlarm << "iil merger has not created!.tableid : " << tableid << enderr;
		smLock->unlock();
		return false;
	}
	smLock->unlock();
	return true;
}

