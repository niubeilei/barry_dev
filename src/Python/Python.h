////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 01/06/2011	Created by Ken
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Python_Python_h
#define AOS_Python_Python_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Security/Ptrs.h"
#include "XmlInterface/XmlRc.h"

#include <python2.6/Python.h>

OmnDefineSingletonClass(AosPythonSingleton,
						AosPython,
						AosPythonSelf,
						OmnSingletonObjId::ePython,
						"Python");

class AosPython : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
		OmnMutexPtr         mLock;
		PyObject *			mPythonModule;

public:
	AosPython();
	~AosPython();

    // Singleton class interface
    static AosPython*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool	sendEmail(
			const OmnString &to,
			const OmnString &from,
			const OmnString &senderpasswd,
			const OmnString &subject,
			const OmnString &body,
			const OmnString &server,
			const OmnString &count, 
			const AosRundataPtr &rdata);
};
#endif

