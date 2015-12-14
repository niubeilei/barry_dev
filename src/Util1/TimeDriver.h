////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimeDriver.h
// Description:
//   This is a Singleton class.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util1_TimeDriver_h
#define Omn_Util1_TimeDriver_h

#include "Debug/Debug.h"
#include "Porting/LongTypes.h"
#include "Porting/TimeOfDay.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util1/TimerObj.h"
#include "Util1/Ptrs.h"

#ifndef OmnTimestampStart
#define OmnTimestampStart timeval OmnTimestampSnapshot; OmnGetTimeOfDay(&OmnTimestampSnapshot); 
#endif

#ifndef OmnTimestampEnd
#define OmnTimestampEnd(x) timeval OmnTimestampSnapshotEnd; OmnGetTimeOfDay(&OmnTimestampSnapshotEnd); \
			OmnTrace << x << ": " \
				<< OmnTimestampSnapshotEnd.tv_usec - OmnTimestampSnapshot.tv_usec << endl;
#endif


OmnDefineSingletonClass(OmnTimeDriverSingleton,
                		OmnTimeDriver, 
						OmnTimeDriverSelf,
						OmnSingletonObjId::eTimeDriver, 
						"TimeDriver");

class OmnTimeDriver : public virtual OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eTimeFreq = 5000
	};

	OmnThreadPtr	mSecTickThread;
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;

public:
	OmnTimeDriver();
	~OmnTimeDriver();

	static OmnTimeDriver *	getSelf();
	virtual bool		start();
	virtual bool		stop();
	virtual bool        config(const AosXmlTagPtr &def);

	//
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state, 
							const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 
};

#endif
