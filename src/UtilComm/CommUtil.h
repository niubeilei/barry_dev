////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommUtil.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_CommUtil_h
#define Omn_UtilComm_CommUtil_h

#include "Debug/ErrId.h"
#include "Debug/Rslt.h"
#include "Util/String.h"
#include "Util/IpAddr.h"


class OmnCommUtil
{
public:
	static OmnString		getHostName();
	static OmnIpAddr		getLocalIpAddr();
	static bool				isSelectErrorRecoverable(const OmnErrId::E errId);
	static OmnRslt			createUdpSock(int &sock, 
								const OmnIpAddr &loalIpAddr,
								int &localPort,
								OmnString &err,
								const bool raiseAlarm);
	// Chen Ding, 02/23/2014
	static void reportGetSockNameError(const int err_no, const int sock);
};

#endif

