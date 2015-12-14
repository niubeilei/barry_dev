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
// 2013/05/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SysConsole_SysConsole_h
#define AOS_SysConsole_SysConsole_h

#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "TransServer/TransProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <queue>
using namespace std;

OmnDefineSingletonClass(AosSysConsoleSingleton,
						AosSysConsole,
						AosSysConsoleSelf,
						OmnSingletonObjId::eSysConsole,
						"SysConsole");

class AosSysConsole : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	// AosPhyServerPtr		mLocalServer;
	vector<int>			mPhyServers;

public:
	AosSysConsole();
	~AosSysConsole();

    // Singleton class interface
    static AosSysConsole *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// OmnThreadedObj interface
	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;
	
	// AosPhyServerPtr getLocalServer(const AosRundataPtr &rdata);
	// AosPhyServerPtr getPhyServer(const AosRundataPtr &rdata, const int phyid);

private:
};

AosSysConsole *AosGetSysConsole() {return AosSysConsole::getSelf();}
#endif

