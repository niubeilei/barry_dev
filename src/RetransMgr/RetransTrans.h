////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RetransTrans.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Retrans_RetransTrans_h
#define Omn_Retrans_RetransTrans_h

#include "aosUtil/Types.h"
#include "Message/Ptrs.h"
#include "Porting/LongTypes.h"
#include "RetransMgr/Ptrs.h"
#include "RetransMgr/RetransTypes.h"
#include "RetransMgr/RetransRequester.h"
#include "TransMgr/Trans.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Array.h"


class OmnRetransTrans : public OmnTrans
{
	OmnDefineRCObject;

private:
	OmnRetransSchedule		mSchedule;
	int						mState;
	OmnMsgPtr				mMsg;
	OmnRetransRequesterPtr	mRequester;
	int64_t					mStartTime;
	bool					mIsValid;
	void			       *mUserData;

public:
	OmnRetransTrans(const OmnMsgPtr &msg,
					const OmnRetransRequesterPtr &requester,
					const OmnRetransSchedule &schedule,
					void *userData);
	~OmnRetransTrans();

	// 
	// OmnTrans interface
	//
	virtual OmnTransType::E	getTransType() const {return OmnTransType::eRetrans;}
	virtual bool			housekeepingFailed(const uint tick);

	void		reset(const int msec, const OmnMsgPtr &msg);
	bool		isTooOld(const int64_t &msec);
	bool		isRetransTriggered(const int64_t &msec);
	void		removeHandler() {mIsValid = false;}
	bool		isValid() const {return mIsValid;}
	void		stop() {mIsValid = false;}
	void		informRequester(const int64_t &msec)
	{
		// 
		// Check whether the transaction is too old. If yes, inform the 
		// requester.
		//
		if (isTooOld(msec))
		{
			mRequester->retransFailed(mTransId, mState, mMsg, mUserData);
			return;
		}

		if (mState < 0 || mState >= mSchedule.entries())
		{
			OmnAlarm << "Invalid state: " << mState << ":" << mSchedule.entries() << enderr;
			return;
		}

		// 
		// If a retransmission is triggered, this function informs the requester
		// of the retransmission. It is the requester's responsibility 
		// to retransmit the message. 
		//
		if (msec - mStartTime >= mSchedule[mState])
		{
			mRequester->resend(mTransId, mState, mMsg, mUserData);
			mState++;
		}
	}
};

#endif
