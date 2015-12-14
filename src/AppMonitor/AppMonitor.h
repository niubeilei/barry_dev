////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppMonitor.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AppMonitor_AppMonitor_h
#define Omn_AppMonitor_AppMonitor_h

#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util1/TimerObj.h"
#include "XmlParser/Ptrs.h"


OmnDefineSingletonClass(AosAppMonitorSingleton,
						AosAppMonitor,
						AosAppMonitorSelf,
						OmnSingletonObjId::eKernelInterface, 
						"AppMonitor");



class AosAppMonitor : public OmnTimerObj
{
	OmnDefineRCObject;

private:

public:
	AosAppMonitor();
	~AosAppMonitor();

	bool		start();
	bool		stop();
	OmnRslt		config(const OmnXmlParserPtr &conf);

	static AosAppMonitor *	getSelf();

	// 
	// TimerObj interface
	//
    virtual void        timeout(const int timerId,
                                const OmnString &timerName,
                                void *parm);

private:
};

#endif

