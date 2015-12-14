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
// 03/10/2012 Copied from UdpCommNew.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm1_UdpXmlCommListener_h
#define Omn_UtilComm1_UdpXmlCommListener_h

#include "UtilComm1/UdpXmlComm.h"
#include "Util/RCObject.h"
#include "XmlUtil/Ptrs.h"

class OmnIpAddr;

class AosUdpXmlCommListener : public OmnRCObject
{
public:
	virtual bool msgRead(
						const AosUdpXmlCommPtr &comm, 
						const AosXmlTagPtr &msg, 
						const OmnIpAddr &remote_addr, 
						const int remote_port) = 0;
	virtual OmnString getListenerName() const = 0;
	virtual void readingFailed() = 0;
};
#endif

