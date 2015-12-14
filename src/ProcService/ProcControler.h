////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ProcControler_ProcControler_h
#define AOS_ProcControler_ProcControler_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "ProcServer/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "ProcUtil/ManagedServer.h"

#include <vector>
#include <map>
using namespace std;

OmnDefineSingletonClass(AosProcControlerSingleton,
						AosProcControler,
						AosProcControlerSelf,
						OmnSingletonObjId::eProcControler,
						"ProcControler");


class AosProcControler : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	struct AosProcess
	{
		OmnString mStartUpCmd;
		~ AosProcess()
		{
		}
	};

	OmnThreadPtr        mThread;
	OmnMutexPtr         mLock;
	AosRundataPtr		mRundata;
	AosXmlTagPtr		mConfig;
	map<OmnString, AosProcess> mProcesses;

public:
	AosProcControler();
	~AosProcControler();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);

    // Singleton class interface
    static AosProcControler *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	bool signal(const int threadLogicId);
	bool checkThread(OmnString &err, const int thrdLogicId) const;
    virtual OmnString   getSysObjName() const {return "AosProcControler";}
    virtual OmnSingletonObjId::E  
						getSysObjId() const 
						{
							return OmnSingletonObjId::eProcControler;
						}
//    virtual OmnRslt     config(const OmnXmlParserPtr &def);
	virtual bool        config(const AosXmlTagPtr &def);
	bool 	start(const AosXmlTagPtr &config);
	bool	getSysInfo(AosXmlTagPtr &cmd, OmnString &resp);
	bool	controlProcess(AosXmlTagPtr &cmd, OmnString &resp);
	bool	checkProcessExist();
	bool	getPidByName(const OmnString &name, pid_t &pid);
	bool	startProcess(const OmnString &pname, OmnString &resp);
	bool	killProcess(const OmnString &pname, OmnString &resp);
	void	testFunction();


};
#endif
