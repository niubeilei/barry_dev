////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Description:
// Modification History:
// 01/05/2011: Created by James
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MsgServer_MsgServer_h
#define Aos_MsgServer_MsgServer_h

//#include "TransServer/TransProc.h"
//#include "TransServer/TransModuleSvr.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEInterfaces/MsgServerObj.h"

OmnDefineSingletonClass(AosMsgServerSingleton,
						AosMsgServer,
						AosMsgServerSelf,
						OmnSingletonObjId::eMsgServer,
						"MsgServer");

class AosMsgServer : public AosMsgServerObj
{
	OmnDefineRCObject;
private:
public:
	AosMsgServer();
	~AosMsgServer();

	// Singleton class interface
	static AosMsgServer *     getSelf();
	virtual bool    start();
	virtual bool    stop();
	virtual bool    config(const AosXmlTagPtr &def);

	virtual bool procRequest(
			const AosXmlTagPtr &root,
			const AosRundataPtr &rdata);

private:
	void sendResp(
			const AosXmlRc errcode,
			const OmnString &errmsg,
			const AosRundataPtr &rdata);
};
#endif

