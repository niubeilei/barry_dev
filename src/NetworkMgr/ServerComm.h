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
//
// Modification History:
// 03/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NetworkMgr_ServerComm_h
#define Aos_NetworkMgr_ServerComm_h

#include "NetworkMgr/NetworkMgr.h"
#include "UtilComm1/UdpXmlCommListener.h"
#include "UtilComm1/UdpXmlComm.h"
#include <vector>

using namespace std;
class AosServerComm : public AosUdpXmlCommListener
{
	OmnDefineRCObject;

private:
	OmnMutexPtr			mLock;
	OmnString			mLocalName;
	OmnString			mLocalAddr;
	int					mLocalPort;
	int					mNumPorts;
	AosUdpXmlCommPtr	mUdpComm;
	AosRundataPtr		mRundata;

public:
	AosServerComm(const AosXmlTagPtr &def);
	~AosServerComm();

	// CommListener Interface
	virtual bool        msgRead(
							const AosUdpXmlCommPtr &comm,
							const AosXmlTagPtr &msg, 
							const OmnIpAddr &remote_addr, 
							const int remote_port);
	virtual OmnString   getListenerName() const;
	virtual void        readingFailed();

	bool config(const AosXmlTagPtr &config);

	OmnString getName() const {return mLocalName;}
	OmnString getAddr() const {return mLocalAddr;}
	int getPort() const {return mLocalPort;}
	int getNumPorts() const {return mNumPorts;}
	
	bool sendMsg(
			const int virtual_id,
			const AosModuleId::E moduleId,
			const AosNetCommand::E command,
			const AosBuffPtr &buff, 
			const OmnIpAddr &remote_addr,
			const int remote_port,
			const AosRundataPtr &rdata);

	bool sendMsg(
			const int virtual_id,
			const AosModuleId::E moduleId,
			const AosNetCommand::E command,
			const AosXmlTagPtr &request, 
			const OmnIpAddr &remote_addr,
			const int remote_port,
			const AosRundataPtr &rdata);

	bool sendMsg(
			const AosXmlTagPtr &request, 
			const OmnIpAddr &remote_addr,
			const int remote_port,
			const AosRundataPtr &rdata);
};
#endif

