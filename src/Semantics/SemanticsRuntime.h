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
// 12/05/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Semantics_SemanticsRuntime_h
#define Aos_Semantics_SemanticsRuntime_h

#include "Event/Ptrs.h"
#include "Event/EventId.h"
#include "SemanticObj/Ptrs.h"
#include "SemanticRules/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"
#include <queue>

OmnDefineSingletonClass(AosSemanticsRuntimeSingleton,
		  				AosSemanticsRuntime,
		  				AosSemanticsRuntimeSelf,
		  				OmnSingletonObjId::eSemanticsRuntime,
						"SemanticsRuntime");


class AosSemanticsRuntime : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

private:
	AosEventMgrPtr	mEventMgr;

public:
	AosSemanticsRuntime();
	virtual ~AosSemanticsRuntime();

	// 
	// Singleton Class interface
	//
	static AosSemanticsRuntime*	getSelf();
	virtual bool		start();
	virtual bool		stop();
	virtual OmnRslt		config(const OmnXmlParserPtr &configData);

	//  
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state,
						    const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
	virtual void heartbeat(const int tid);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;

	bool 	registerEvent(const AosEventListenerPtr &listener,
						  const AosEventId::E eventId, 
						  void *eventData, 
						  const u32 dataLen);
	bool 	unregisterEvent(const AosEventListenerPtr &listener,
						  const AosEventId::E eventId, 
						  void *eventData, 
						  const u32 dataLen);

	bool	addEvent(const AosEventId::E eventId,
				 	 const std::string &filename,
				 	 const int lineno,
				 	 const std::string &actor,
				 	 const std::string &classname,
					 void *eventData, 
					 const u32 dataLen, 
					 const int flag);
	AosLockSOPtr	getLock(void *lock);
	AosHashtabSOPtr	getHashtab(void *table);
	

private:
};
#endif

