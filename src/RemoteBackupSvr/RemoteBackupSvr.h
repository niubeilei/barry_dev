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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 02/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_RemoteBackupSvr_RemoteBackupSvr_h
#define AOS_RemoteBackupSvr_RemoteBackupSvr_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "RemoteBackupSvr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "XmlInterface/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosRemoteBackupSvr : public AosNetReqProc, public OmnThreadedObj
{
	OmnDefineRCObject;
	
public:
	enum
	{
		eDftMaxConns = 500,
		eForceCheck = 100
	};

private:
	AosRundataPtr 		mRundata;

	static bool				smHasRecordsInDb;
	static OmnThreadPtr		smThread;
	static OmnMutexPtr		smLock;
	static OmnCondVarPtr	smCondVar;
	static AosU642U64_t		smLastTransid;

public:
	AosRemoteBackupSvr();
	~AosRemoteBackupSvr();

    static bool     	startStatic(const AosNetReqProcPtr &server);
    static bool    		stopStatic();
    static bool			configStatic(const AosXmlTagPtr &def);

	// AosNetReqProc interface
	virtual bool                procRequest(const OmnConnBuffPtr &req);
	virtual AosNetReqProcPtr    clone();

	// ThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

private:
	void sendResp(const AosRundataPtr &rdata); 
	void sendResp( 	const AosWebRequestPtr &req, 
					const OmnString &status,
					const OmnString &contents);
	bool saveToDb(const AosXmlTagPtr &req, OmnString &errmsg);
	bool procOneRecord();
};
#endif

