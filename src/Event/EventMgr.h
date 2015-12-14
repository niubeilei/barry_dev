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
//
// Modification History:
// 11/30/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Event_EventMgr_h
#define Aos_Event_EventMgr_h

#include "aosUtil/Memory.h"
#include "Event/Ptrs.h"
#include "Event/EventId.h"
#include "Event/EventEntry.h"
#include "SemanticRules/SemanticRuleUtil.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashObj.h"
#include "XmlParser/Ptrs.h"
#include <queue>



class AosEventMgr : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

private:
	OmnThreadPtr					mThread;
	OmnMutexPtr						mLock;
	OmnCondVarPtr					mCondVar;
	std::queue<AosEventPtr>			mEvents;
	OmnObjHash<AosEventEntryPtr, 0xfff>	mEventRegisters;
	OmnMutexPtr						mRegLock;
	bool							mThreadStatus;

public:
	AosEventMgr();
	virtual ~AosEventMgr();

	bool	start();
	bool	stop();
	bool	config(const OmnXmlParserPtr &def);

	//  
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state,
						    const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
	virtual void heartbeat(const int tid);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;

	bool	addEvent(const AosEventPtr &event);
	bool	registerEvent(const AosEventListenerPtr &listener, 
						  const AosEventId::E eventId,
						  void *eventData, 
						  const u32 dataLen);
	bool	unregisterEvent(const AosEventListenerPtr &listern, 
						  const AosEventId::E eventId,
						  void *eventData, 
						  const u32 dataLen);

private:
	void	procEvent(const AosEventPtr &event);
};

#endif

