////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpEff.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_UdpEff_h
#define Omn_UtilComm_UdpEff_h

#include "Debug/ErrId.h"
#include "Debug/Rslt.h"
#include "Thread/Ptrs.h"
#include "Util/SPtr.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObjImp.h"
#include "Util/RCObject.h"


class OmnUdpEff : public OmnRCObject
{
	OmnDefineRCObject;
 
private:
	int					mSock;
	OmnIpAddr			mLocalIpAddr;
	int					mLocalPort;
	OmnString			mName;

	//
	// Do not use these two
	//
	OmnUdpEff(const OmnUdpEff& rhs);
	OmnUdpEff& operator = (const OmnUdpEff& rhs);

public:
	OmnUdpEff(const OmnString &name, 
		const OmnIpAddr &localIpAddr, 
		const int localPort);
	~OmnUdpEff();

	bool		closeConn();
	OmnRslt		connect(OmnString &err);

	OmnString	toString() const;

	OmnString	getName() const {return mName;}
	int			getLocalPort() const {return mLocalPort;}
	int			getSock() const {return mSock;}

	bool 		select(const int sec, const int usec, bool &timeout);
	OmnIpAddr	getLocalIpAddr() const {return mLocalIpAddr;}
	OmnRslt		readFrom(char *data, int &length,
						OmnIpAddr &remoteIpAddr,
						int &remotePort);

	OmnRslt		writeTo(const char *data, 
						const int dataLength,
						const OmnIpAddr &remoteIpAddr, 
						const int remotePort);
};
#endif
