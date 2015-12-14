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
// 01/23/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TorturerServer_TSTrans_h
#define Aos_TorturerServer_TSTrans_h
 
#include "TorturerServer/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/SerialFrom.h"
#include "UtilComm/Ptrs.h"


class AosTSTrans : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosTorturerServerPtr	mServer;
	OmnTcpClientPtr			mConn;
	OmnSerialFrom			mBuffer;
	AosTortMsgPtr			mMsg;

public:
	AosTSTrans(const AosTorturerServerPtr server, 
			const OmnTcpClientPtr &conn); 
	~AosTSTrans();

	bool	msgReceived(const OmnConnBuffPtr &buff, 
						const OmnTcpClientPtr &conn);
	int		getSock() const;

private:
};

#endif

