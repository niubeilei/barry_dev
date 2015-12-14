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
// 10/27/2010: Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEServerCgi_FcgiConn_h
#define Aos_SEServerCgi_FcgiConn_h

#include "SEServer/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "UtilComm/TcpClient.h"


class AosFcgiConn : virtual public OmnTcpClient
{
	OmnDefineRCObject;

private:
	OmnString	mResponse;

public:
	AosFcgiConn(const OmnString &name,
			const OmnString &uPath,
			const AosTcpLengthType lt = eAosTLT_LengthIndicator);

	~AosFcgiConn();

	virtual bool smartSend(const char *data, const int len)
	{
		mResponse = data;
		return true;
	}

	OmnString getResponse() {return mResponse;}
};
#endif

