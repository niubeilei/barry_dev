////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NmsServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_NMS_NmsServer_h
#define Omn_NMS_NmsServer_h

#include "NMS/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"

class OmnSoMgcpEndpoint;

class OmnNmsServer : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnTcpServerPtr			mServer;
	bool					mFinished;
	OmnNmsSvrObjPtr			mCallback;

public:
	OmnNmsServer();
	~OmnNmsServer();

	bool		start();
	bool		stop();
	bool		config(const OmnXmlItemPtr &def);
	void		setNmsSvrObj(const OmnNmsSvrObjPtr &obj);

	OmnIpAddr	getLocalIpAddr() const;
	int			getLocalPort() const;

	void		epAdded(const OmnSoMgcpEndpoint &so);
	void		epDeleted(const OmnSoMgcpEndpoint &so);
	void		epModified(const OmnSoMgcpEndpoint &so);

private:
	void 		procMsg(const OmnConnBuffPtr &, const OmnTcpClientPtr &);
	void 		procNmsCommand(const OmnConnBuffPtr &, const OmnTcpClientPtr &);
	void 		procEpCommand(const OmnConnBuffPtr &, const OmnTcpClientPtr &);
	bool		checkMemory(const OmnTcpClientPtr &, OmnString &);
	void		procChangeState(const OmnConnBuffPtr &, 
						const OmnTcpClientPtr &);
};
#endif
