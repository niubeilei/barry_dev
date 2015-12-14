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
// 02/22/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SoapServerObj_h
#define Aos_SEInterfaces_SoapServerObj_h

#include "alarm_c/alarm.h"
#include "MsgProc/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosSoapServerObj : virtual public OmnRCObject
{
private:
	static AosSoapServerObjPtr smSoapServer;

public:
	virtual bool registerMsgProc(const OmnString &appid, const AosMsgProcPtr &proc) = 0;

	static AosSoapServerObjPtr getSoapServer() {return smSoapServer;}
	static void setSoapServer(const AosSoapServerObjPtr &d) {smSoapServer = d;}
};

inline bool AosRegisterSoapMsgProc(const OmnString &appid, const AosMsgProcPtr &proc)
{
	AosSoapServerObjPtr soapserver = AosSoapServerObj::getSoapServer();
	aos_assert_r(soapserver, false);
	return soapserver->registerMsgProc(appid, proc);
}

#endif
