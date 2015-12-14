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
// 03/03/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MySqlAdapter_MySqlServer_h
#define Aos_MySqlAdapter_MySqlServer_h

#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/AddrDef.h"
#include "UtilComm/PortDef.h"
#include "UtilComm/CommTypes.h"
#include "XmlUtil/Ptrs.h"
#include <list>


class AosMySqlServer: public OmnTcpListener
{
	OmnDefineRCObject;

private:
	OmnTcpServerEpollPtr	mServer;

public:
	AosMySqlServer();
	~AosMySqlServer();

	bool	start();
	bool	stop();

	// OmnTcppListener Interface
	virtual OmnString	getTcpListenerName() const;
	virtual void		msgRecved(const OmnConnBuffPtr &buff, 
							const OmnTcpClientPtr &conn);

	void        connCreated(const OmnTcpClientPtr &conn){return;}
	void        connClosed(const OmnTcpClientPtr &conn){return;}
};
#endif

