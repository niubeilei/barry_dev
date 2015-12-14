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
// Modification History:
// 02/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SqlServer_SqlServer_h
#define AOS_SqlServer_SqlServer_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"


OmnDefineSingletonClass(AosSqlServerSingleton,
						AosSqlServer,
						AosSqlServerSelf,
						OmnSingletonObjId::eSqlServer,
						"SqlServer");

class AosSqlServer : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:

public:
	AosSqlServer();
	~AosSqlServer();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, 
						const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

    // Singleton class interface
    static AosSqlServer *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);
};
#endif
