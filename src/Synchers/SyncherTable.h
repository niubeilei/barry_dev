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
// 2015/09/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Synchers_SyncherTable_h
#define AOS_Synchers_SyncherTable_h

#include "Synchers/Syncher.h"
#include "JSON/JSON.h"
#include "Util1/TimerObj.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"

class AosSyncherTable : public AosSyncher,
						public OmnThreadedObj,
						public OmnTimerObj 
{
private:
	OmnMutexPtr                 mLock;
	OmnCondVarPtr				mCondVar;
	bool						mBigEnough;
	u32 						mTimerSec; 
	u32 						mTimerUsec;
	OmnThreadPtr				mThread;

	OmnString 					mTableObjid;
	vector<AosSyncherObjPtr>	mSynchers;

public:
	AosSyncherTable(const int version);
	virtual ~AosSyncherTable();

	// ThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);

	// TimerObj interface
	virtual void timeout(const int timerId, 
						const OmnString &timerName, void *parm);	

	// SyncherObj interface 
	virtual AosSyncherType::E getType() { return AosSyncherType::eTable; }
	virtual bool proc();
	virtual AosBuffPtr serializeToBuff();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual AosJimoPtr cloneJimo() const;
	virtual int getDestCubeID() { return 0; }

	virtual bool appendRecord(AosRundata *rdata, AosDataRecordObj *record);

private:
	bool 		init(AosRundata *rdata);
	bool 		proc(AosRundata* rdata, const AosDatasetObjPtr &dataset);

	AosXmlTagPtr generatorIndexDatasetConf(AosRundata *rdata);
	bool		flushDeltaBeans(AosRundata *rdata);
};

#endif

