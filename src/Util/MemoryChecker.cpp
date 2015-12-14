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
#include "Util/MemoryChecker.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/IILMgrObj.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/MemoryCheckerObj.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"

#include <map>

/*
void * operator new(std::size_t size, const char *fname, const int line, int, int, int)
{
	cout << "To new memory: " << size << endl;
	char *mem = (char *)malloc(size + AosMemoryChecker::eMetaSize);
	AosMemoryChecker::memAllocated(mem);
	return (void *)&mem[AosMemoryChecker::eMetaSize];
}

void operator delete(void *ptr)
{
	AosMemoryChecker::memDeleted(ptr);
}
*/

// Chen Ding, 10/31/2012
static bool sgInited = false;
static OmnMutex sgLock;
const u64 sgInvalidThreadId = 0xffffffffffffffffULL;
const u64 sgPoison = 0x637da46e0376837fULL;

OmnSingletonImpl(AosMemoryCheckerSingleton,
                 AosMemoryChecker,
                 AosMemoryCheckerSelf,
                "AosMemoryChecker");

u64 AosMemoryChecker::mThreadIds[AosMemoryChecker::eNumBuckets][AosMemoryChecker::eBucketSize];
const char*	AosMemoryChecker::mFnames[AosMemoryChecker::eNumBuckets][AosMemoryChecker::eBucketSize];
int AosMemoryChecker::mLines[AosMemoryChecker::eNumBuckets][AosMemoryChecker::eBucketSize];

	
bool AosMemoryChecker::smCheckOpen = true;
int AosMemoryChecker::smCheckFreq = 30;

AosMemoryChecker::AosMemoryChecker()
:
mLock(::new OmnMutex())
{
	sgInited = true;
	for (int i=0; i<eNumBuckets; i++)
	{
		for (int j=0; j<eBucketSize; j++)
		{
			mThreadIds[i][j] = sgInvalidThreadId;
		}
	}
}


AosMemoryChecker::~AosMemoryChecker()
{
}

bool
AosMemoryChecker::start()
{
	return true;
}

bool
AosMemoryChecker::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);

	AosXmlTagPtr memorycheck = config->getFirstChild("memorycheck");
	if (!memorycheck) return true;

	smCheckOpen = memorycheck->getAttrBool("check_open", true);
	smCheckFreq = memorycheck->getAttrInt("check_freq", 30);
	if (smCheckFreq < 10) smCheckFreq = 10;

	return true;
}


bool
AosMemoryChecker::stop()
{
	return true;
}


void
AosMemoryChecker::report()
{
	if (!smCheckOpen) return;

	mLock->lock();
	
	OmnString ctime = OmnGetTime(AosLocale::eChina);
	cout << endl
		 << "============================================================" << endl
		 << "             Memory Summary: " << ctime << endl
		 << "------------------------------------------------------------" << endl;

	multimap<int, OmnString> tempmap;
	for (AosStr2Int64Itr_t itr = mMap.begin(); itr != mMap.end(); itr++)
	{
		//if (itr->second != 0)
		if (itr->second > 1)
			tempmap.insert(make_pair(itr->second, itr->first));
	}

	OmnString filename;
	int num;
	for (multimap<int, OmnString>::iterator itr = tempmap.begin(); itr != tempmap.end(); itr++)
	{
		num = itr->first;
		filename = itr->second;
		printf("%50s%10d\n", filename.data(), num);
	}

	// Report class memory usage
	cout << "------------------------------------------------------------" << endl;
	AosStr2Int64_t map;
	mapitr_t tt = mObjMap.begin();
	while (tt != mObjMap.end())
	{
		AosClassName::E name = tt->second->getMemoryCheckerClassName();
		OmnString classname = AosClassName::toString(name);
		if (classname != "" && AosClassName::isValid(name))
		{
			AosStr2Int64Itr_t mm = map.find(classname);
			if (mm == map.end())
			{
				map[classname] = tt->second->getMemorySize();
			}
			else
			{
				mm->second += tt->second->getMemorySize();
			}
		}
		tt++;
	}

	AosStr2Int64Itr_t mm = map.begin();
	while (mm != map.end())
	{
		if (mm->second > 0)
		{
			cout << mm->first << ":" << AosSizeToHumanRead(mm->second) << endl;
		}
		mm++;
	}

	cout << "------------------------------------------------------------" << endl;
	cout << "Total AosBuff: " << AosBuff::getNumInstances() << ":" << AosBuff::getTotalInstances() << endl;
	cout << "Total OmnString: " << OmnString::getNumInstances() << ":" << OmnString::getTotalInstances() << endl;
	cout << "------------------------------------------------------------" << endl;
	if (AosIILMgrObj::getIILMgr())
	{
		cout << "Total IIL:" << AosIILMgrObj::getIILMgr()->getMemoryStatus() << endl;
	}
	cout << "============================================================" << endl;
	mLock->unlock();
}


bool	
AosMemoryChecker::objCreated(
		AosMemoryCheckerObj *obj,
		const AosClassName::E name, 
		const char *file,
		const int line)
{
	if (!smCheckOpen) return true;

	obj->setFileLine(file, line);
	obj->setMemoryCheckerClassName(name);

	OmnString key = AosClassName::toString(name);
	key << ":" << file << ":" << line;
	mLock->lock();
	AosStr2Int64Itr_t itr = mMap.find(key);
	if (itr != mMap.end())
	{
		// Found it. Increment it.
		(itr->second)++;
	}
	else
	{
		// Did not find it, add it.
		mMap[key] = 1;
	}

	// Chen Ding, 2013/05/13
	u64 nn = (u64)obj;

	mapitr_t ttt2 = mObjMap.find(nn);
	aos_assert_r(ttt2 == mObjMap.end(), false);
	
	mObjMap[nn] = obj;
	mLock->unlock();
	return true;
}


bool	
AosMemoryChecker::objDeleted(
		AosMemoryCheckerObj *obj,
		const AosClassName::E name) 
{
	if (!smCheckOpen) return true;

	aos_assert_r(obj, false);
	OmnString key = AosClassName::toString(name);
	key << ":" << obj->getMemoryCheckerFname() << ":" << obj->getMemoryCheckerLineno();
	mLock->lock();
	AosStr2Int64Itr_t itr = mMap.find(key);
	if (itr != mMap.end())
	{
		// Found it. Increment it.
		(itr->second)--;
	}
	else
	{
		OmnAlarm << "Error: " << key << enderr;
	}

	u64 nn = (u64)obj;
	int num = mObjMap.erase(nn);
	mLock->unlock();
	aos_assert_r(num == 1, false);
	return true;
}

