////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HouseKp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_NMS_HouseKp_h
#define Omn_NMS_HouseKp_h

#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/ValList.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util1/TimerObj.h"
#include "XmlParser/Ptrs.h"

class OmnHouseKpObj;


class OmnHouseKp : public OmnThreadedObj, public OmnTimerObj
{
	OmnDefineRCObject;

public:
	enum Event 
	{
		eNothing,
		eExit,
		eHouseKeeping
	};

	enum
	{
		eBlockSize = 50,
		eDefaultKpFreq = 30
	};

private:
	OmnVList<OmnHouseKpObj*>	mObjects;
	Event						mEvent;
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	OmnThreadPtr				mThread;
	int							mTimerId;
	int							mFrequency;
	bool						mIsStarted;

public:
	OmnHouseKp(const OmnXmlItemPtr &def);
	~OmnHouseKp();

	static int		getBlockSize() {return eBlockSize;}
	bool			start();
	bool			stop();
	OmnRslt 		config(const OmnXmlItemPtr &def);
    virtual bool 	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    virtual bool 	signal(const int threadLogicId);
    virtual bool 	checkThread(OmnString &errmsg, const int tid) const;
	virtual void 	timeout(const int timerId, const OmnString &name, void *parm);

	void			addObj(OmnHouseKpObj *obj);

private:
	void			procEvent();
	void			procHouseKeeping();
};
#endif
