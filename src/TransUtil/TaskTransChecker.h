////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 09/06/2013 by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransUtil_TaskTransChecker_h
#define AOS_TransUtil_TaskTransChecker_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"

#include <map>

OmnDefineSingletonClass(AosTaskTransCheckerSingleton,
						AosTaskTransChecker,
						AosTaskTransCheckerSelf,
						OmnSingletonObjId::eTaskTransChecker,
						"TaskTransChecker");

class AosTaskTransChecker : public OmnRCObject 
{
	OmnDefineRCObject;

	typedef map<OmnString, u64> map_t;
	typedef map<OmnString, u64> ::iterator mapitr_t;

private:
	bool					mShowLog;
	OmnMutexPtr				mLock;
	map_t					mMap;

public:

	AosTaskTransChecker();
	~AosTaskTransChecker();

public:
    // Singleton class interface
    static AosTaskTransChecker* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	void addEntry(
			const u64 &task_docid, 
			const u32 &virtual_id,
			const u64 &snap_id); 

	void removeEntry(
			const u64 &task_docid,
			const u32 &virtual_id,
			const u64 &snap_id);

	bool checkTrans(
			const u64 &task_docid,
			const u32 &virtual_id,
			const u64 &snap_id);
private:
	OmnString  composeKey(
			const u64 &task_docid,
			const u32 &virtual_id);
};

#endif

