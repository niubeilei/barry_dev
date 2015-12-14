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
#ifndef AOS_ManagedServer_ManagedServer_h
#define AOS_ManagedServer_ManagedServer_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "ProcUtil/Ptrs.h"
#include "Thread/Mutex.h"



class AosManagedServer : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString		mAddr;
	int				mPort;
	OmnTcpClientPtr	mConn;
	OmnMutexPtr     mLock;

public:
	AosManagedServer(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	~AosManagedServer();

	bool	sendRequest(
				const OmnString &req,
				OmnString &resp,
				OmnString &errmsg);
	OmnString getAddr(){return mAddr;}
	int       getPort(){return mPort;}
	void closeConn();
	bool	connect(OmnString &errmsg);
};
#endif
