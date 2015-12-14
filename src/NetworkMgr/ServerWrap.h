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
#ifndef Aos_NetworkMgr_ServerWrap_h
#define Aos_NetworkMgr_ServerWrap_h

#include "Util/String.h"
#include "NetworkMgr/NetworkMgr.h"
#include "XmlUtil/XmlTag.h"

class AosServerWrap : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum ServerType
	{
		eInvalid,

		eFrontend,
		eBackend,

		eMax
	};

private:
	OmnString		mName;
	OmnIpAddr		mAddr;
	int				mPort;
	int				mNumPorts;
	ServerType		mServerType;
	int				mPhysicalId;

public:
	AosServerWrap(
			const ServerType type, 
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata); 
	~AosServerWrap();

	bool 		config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	ServerType 	getServerType() const {return mServerType;}
	bool 		isFrontend() const {return mServerType == eFrontend;}
	bool 		isBackend() const {return mServerType == eBackend;}
	bool		isServerLocal() const;
	OmnString 	getName() const {return mName;}
	OmnIpAddr	getAddr() const {return mAddr;}
	OmnString 	getAddrStr() const {return mAddr.toString();}
	int 		getPort() const {return mPort;}
	int 		getNumPorts() const {return mNumPorts;}

	bool stopServer(
			const u64 &transid, 
			const AosRundataPtr &rdata);
	bool verifyServer(const AosXmlTagPtr &response, const AosRundataPtr &rdata);
	OmnString getServerInfo(const AosXmlTagPtr &req, const AosRundataPtr &rdata);
	AosXmlTagPtr getRuntimeInfo(const AosRundataPtr &rdata);
};
#endif

