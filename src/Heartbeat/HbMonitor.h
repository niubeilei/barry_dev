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
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_Heartbeat_HbMonitor_h
#define Omn_Heartbeat_HbMonitor_h

#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"


class OmnHbMonitor : public virtual OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eStartTick = 5,
		eRecoveryFreq = 10,
		eStartHbFreq = 10
	};

private:
	//
	// For reliability, it may maintain two connections to one
	// remote peer. If just one used, mConn2 is null.
	//
	OmnString			mName;

	bool				mIsMonitoredOk;
	OmnHbObjPtr			mCallback;
	int					mMonitorId;
	bool				mNeedToKill;
	State				mStatus;
	bool				mIsNegativeResp;
	OmnSmHbResp 		mNegativeResp;
	bool				mResp1Received;
	bool				mResp2Received;
	int					mControlTick;
	int					mNumRespMissed;

public:
	OmnHbMonitor(const OmnHbObjPtr &callback, const AosXmlTagPtr &def);
	~OmnHbMonitor();

	// 
	// OmnCommObj Interface
	//
	virtual bool	msgRead(const OmnConnBuffPtr &buff);

	void		sendHeartbeat();
	bool		checkHeartbeat();
	void		restore();
	void		stopHeartbeat();
	bool		startHeartbeat();
	OmnString	getName() const {return mName;}
	int			getMonitorId() const {return mMonitorId;}

private:
	bool		heartbeatFailed();
	bool		killMonitored();
};

#endif
#endif
