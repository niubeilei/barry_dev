////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tracer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Alarm_AlarmProc_h
#define Omn_Alarm_AlarmProc_h

#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/MemoryChecker.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/BasicTypes.h"
#include "Util1/TimerObj.h"
#include "XmlParser/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <fstream>

using namespace std;

OmnDefineSingletonClass(OmnAlarmProcSingleton,
						OmnAlarmProc,
						OmnAlarmProcSelf,
                		OmnSingletonObjId::eTracer, 
						"AlarmProc");


class OmnAlarmProc : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnThreadPtr 					mThread;
	OmnString						mFileName;
	fstream							mAlarmFile;
	OmnMutexPtr						mLock;
	OmnString						mModuleName;
	
	OmnAlarmProc();
	~OmnAlarmProc();

public:
    // Singleton class interface
	static OmnAlarmProc *	getSelf();
    virtual bool        	start();
    virtual bool        	stop();
    virtual bool			config(const AosXmlTagPtr &def);
	
	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId){return true;}

	OmnString	getModuleName(){return mModuleName;}
	bool 	saveAlarmToFile(const OmnString &docstr);
	
	void	resetFile();
};
#endif
