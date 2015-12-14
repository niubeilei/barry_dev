////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BandWidthMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Util1_BandWidthMgr_h
#define Aos_Util1_BandWidthMgr_h

#include "Util1/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util1/TimerObj.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"



class AosBandWidthMgr: public OmnThreadedObj,OmnTimerObj
{
	OmnDefineRCObject;
public:	
	
private:

	bool			mStarted;
	int				mBytesLimit;
	int				mCurrentVolume;
	int				mIntervalSec;
	int				mIntervalUSec;


	OmnThreadPtr	mThread;
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;

public:
	AosBandWidthMgr();
	virtual ~AosBandWidthMgr();

	void			SetBandWidth(const int sec,const int usec,const int bytesLimit);
	bool			start();
	bool			stop();

	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	virtual void		timeout(const int timerId, 
								const OmnString &timerName,
								void *parm);
	
	bool			getPermission(const int requestLen,int &lenAllowed);


private:
	

};
#endif
