////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Timer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util1_OmnTimer_h 
#define Omn_Util1_OmnTimer_h

//#include "Config/ConfigEntity.h"
#include "Debug/Rslt.h"
#include "Porting/Timeval.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "Util1/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"


OmnDefineSingletonClass(OmnTimerSingleton,
						OmnTimer,
						OmnTimerSelf,
                		OmnSingletonObjId::eTimer, 
						"Timer");



class OmnTimer : public OmnThreadedObj
{
	OmnDefineRCObject;

public:
    struct TimerData
    {
   		int					timerId;
		OmnString			timerName;
   		OmnTimerObjPtr		caller;
   		struct timeval 		endTime;
		void *				parm;
  	};

	enum
	{
		eClientThread,
		eServerThread
	};
 
private:
    int 					mReadSock;
	int						mWriteSock;
	int						mNfds;
    fd_set 					mReadfds;
    timeval 				mTimeval;
    OmnVList<TimerData>	mTimerList;
	OmnThreadPtr			mThread;
	int						mPort;
	OmnMutexPtr				mLock;
	OmnUdpPtr				mReader;
	OmnUdpPtr				mWriter;
	OmnIpAddr				mIpAddr;

public:
    OmnTimer();
    virtual ~OmnTimer();

	static OmnTimer *	getSelf();
	virtual bool		start();
	virtual bool		stop();
	virtual bool		config(const AosXmlTagPtr &def);

    int			startTimer(const OmnString &timerName, 
						   int timerSec,
						   int timerUsec, 
             			   const OmnTimerObjPtr &caller,
						   void *parm = 0);

    bool 		cancelTimer(const int timerId);
    bool	 	cancelTimersForObj(const void *obj);
    
	//
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 




private:
    void		resetNtimeout();
	bool		createTimerSock();
};

#endif


