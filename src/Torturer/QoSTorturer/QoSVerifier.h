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
// 08/04/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Torturer_QoSTorturer_QoSVerifier_h
#define Omn_Torturer_QoSTorturer_QoSVerifier_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosQoSVerifier : public OmnRCObject 
{
	OmnDefineRCObject;

public:

private:
	OmnDynArray<OmnTcpPairPtr>		mTcpConns;
	OmnDynArray<OmnUdpPairPtr>		mUdpConns;

public:
	AosQoSVerifier();
	virtual ~AosQoSVerifier();

	bool	start();
	bool	suspend();
	bool	resume();
	bool	stop();
};

#endif

