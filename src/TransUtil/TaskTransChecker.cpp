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
// Created: 03/12/2014 by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/TaskTransChecker.h"

#include "Alarm/Alarm.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SingletonClass/SingletonImpl.cpp"

OmnSingletonImpl(AosTaskTransCheckerSingleton, 
				 AosTaskTransChecker, 
				 AosTaskTransCheckerSelf, 
				"AosTaskTransChecker");

AosTaskTransChecker::AosTaskTransChecker()
:
mShowLog(false),
mLock(OmnNew OmnMutex())
{
}


AosTaskTransChecker::~AosTaskTransChecker()
{
}

bool
AosTaskTransChecker::start()
{
	return true;
}


bool
AosTaskTransChecker::stop()
{
	return true;
}

bool
AosTaskTransChecker::config(const AosXmlTagPtr &config)
{
	return true;
}


void
AosTaskTransChecker::addEntry(
		const u64 &task_docid, 
		const u32 &virtual_id,
		const u64 &snap_id)
{
	aos_assert(task_docid);
	OmnString key = composeKey(task_docid, virtual_id);
OmnScreen << "LLL; key:" << key << ";snap_id:" << snap_id << endl;
	mLock->lock();
	mapitr_t itr = mMap.find(key);
	if(itr == mMap.end())
	{
		mMap.insert(make_pair(key, snap_id));	
	}
	else
	{
		if (mShowLog) 
		{
			OmnScreen << "TaskTransChecker; exists!! taskdocid:" << task_docid 
				<< ";old_snap_id:" << itr->second << ";new_snap_id:" << snap_id 
				<< ";"<< endl;
		}
		mMap[key] = snap_id;	
	}
	mLock->unlock();
}


void	
AosTaskTransChecker::removeEntry(
		const u64 &task_docid,
		const u32 &virtual_id,
		const u64 &snap_id)
{
	aos_assert(task_docid);
	OmnString key = composeKey(task_docid, virtual_id);
//OmnScreen << "LLL; key:" << key << ";snap_id:" << snap_id << endl;
	mLock->lock();
	mapitr_t itr = mMap.find(key);
	if(itr == mMap.end())
	{
		mLock->unlock();
		//Jozhi 2014-07-15 may cube.exe restarted
		//OmnAlarm << "KKKKKKK" << enderr;
		OmnScreen << "may be alarm KKKKKKK" << endl;
		return;
	}
	if (itr->second != snap_id)
	{
		mLock->unlock();
		OmnAlarm << "EEEEEEE" << enderr;
		return;
	}

	mMap.erase(itr);	
	mLock->unlock();
}


OmnString 
AosTaskTransChecker::composeKey(
		const u64 &task_docid,
		const u32 &virtual_id)
{
	OmnString key;
	key << task_docid << "_" << virtual_id;
	return key;
}


bool
AosTaskTransChecker::checkTrans(
		const u64 &task_docid,
		const u32 &virtual_id,
		const u64 &snap_id)
{
	if (snap_id == 0) return true;

	aos_assert_r(task_docid, false);
	OmnString key = composeKey(task_docid, virtual_id);
	mLock->lock();
	mapitr_t itr = mMap.find(key);
	//may be fail task
	if (itr == mMap.end())
	{
		mLock->unlock();
		OmnScreen << "may be alarm" << endl;
		return false;
	}
	aos_assert_rl(itr != mMap.end(), mLock, false);
	
	if (itr->second != snap_id) 
	{
		mLock->unlock();
		if (mShowLog) 
		{
			OmnScreen << "TaskTransChecker; Discard!! taskdocid:" << task_docid 
				<< ";snap_id:" << snap_id << ";new_snap_id:" << itr->second 
				<< ";"<< endl;
		}
		return false;
	}
	mLock->unlock();
	return true;
}
