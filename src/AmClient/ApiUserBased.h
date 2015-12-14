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
// 4/16/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AmClient_ApiUserBased_h
#define Aos_AmClient_ApiUserBased_h

#include "AmClient/AmApiProc.h"
#include "aosUtil/Types.h"
#include "AmUtil/Ptrs.h"
#include "AmUtil/AmUtil.h"
#include "AmClient/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosApiUserBased : public AosAmApiProc
{
	OmnDefineRCObject;

private: 
	OmnString		mApp;
	OmnString		mUser;
	OmnString		mOpr;
	OmnString		mRsc;
	u16				mTransId;
	u16				mSessionId;
	u32				mTimer;
	AosAmServerPtr	mServer;
	AosAmClientPtr	mClient;
	AosAmApiCallback mCallback;

public:
	AosApiUserBased(
				const OmnString &app, 
				const OmnString &user, 
				const OmnString &opr, 
				const OmnString &rsc, 
				const u16 transId, 
				const u16 sessionId, 
				const u32 timer,
				const AosAmServerPtr &server, 
				const AosAmClientPtr &client,
				AosAmApiCallback callback);
	~AosApiUserBased();

	virtual bool proc(const OmnThreadShellPtr &shell);

private:
};
#endif

