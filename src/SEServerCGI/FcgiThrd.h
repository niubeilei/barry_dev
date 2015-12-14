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
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEServerCGI_FcgiThrd_h
#define AOS_SEServerCGI_FcgiThrd_h

#include "Debug/Debug.h"
#include "SEUtil/Ptrs.h"
#include "SEServerCGI/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "SEServerCGI/FcgiReqProc.h"
#include "Proggie/ReqDistr/NetReqProc.h"

class FCGX_Request;

class AosFcgiThrd : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eNumThreads = 10
	};

private:
	OmnThreadPtr			mThread;
	AosFcgiReqProcPtr		mProc;
	static int				mNumThreads;
	static AosFcgiThrdPtr	mThreads[eNumThreads];

public:
	AosFcgiThrd(const AosFcgiReqProcPtr &req);
	~AosFcgiThrd();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	bool      		start();
	bool        	stop();
	OmnRslt     	config(const OmnXmlParserPtr &def);

	static bool		startThreads(const AosFcgiReqProcPtr &req);

private:
};
#endif

