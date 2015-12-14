////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Trans.h
// Description:
//	This is the super class for transactions.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TransMgr_Trans_h
#define Omn_TransMgr_Trans_h

#include "aosUtil/Types.h"
#include "Debug/Rslt.h"
#include "Message/Ptrs.h"
#include "TransMgr/TransType.h"
#include "TransMgr/Ptrs.h"
#include "TransMgr/ForeignTransId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "Util1/Time.h"


class OmnTrans : public virtual OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum
	{
		eDefaultTTL = 15,			// 15 seconds
		eDefaultMaxActivity = 50
	};

	enum Activity_e
	{
		eNoActivity,
		eMsgSent,
		eMsgRecved,
		eAbort,
		eRetransResp,
		eStateChanged,
		eUnclaimedMsg,
		eProgramError,
		eTransFailed
	};

	struct Activity
	{
		Activity_e	mActivity;
		OmnString	mFile;
		int			mLine;
		OmnString	mMsgName;
		uint		mMsgSeqno;
	
		Activity()
			:
		mActivity(eNoActivity)
		{
		}

		Activity(const Activity_e a, 
				 const OmnString &file, 
				 const int line,
				 const OmnString &msgName,
				 const uint msgSeqno)
			:
		mActivity(a),
		mFile(file),
		mLine(line),
		mMsgName(msgName),
		mMsgSeqno(msgSeqno)
		{
		}
	};

protected:
    int64_t			mTransId;           // The transaction ID
	bool			mFinished;
	OmnErrId::E		mRespCode;
	OmnString		mErrmsg;
	uint64_t		mLastActivity;		// In second ticks
	u32				mTTL;				// Used to determine whether the transaciton
										// should be kicked out (or killed).
	OmnVList<Activity>	mActivityLog;		// Log the transaction activities
	bool			mTooManyActivity;

	OmnForeignTransId	mForeignTransId;

	static unsigned int		mMaxActivity;

public:
	OmnTrans();
	virtual ~OmnTrans() {}

	// 
	// Transaction interface
	//
	virtual OmnTransType::E	getTransType() const = 0;
	virtual bool		housekeepingFailed(const uint tick) = 0;
	virtual bool		ttlExpired() const;
	virtual bool		msgRecved(const OmnMsgPtr &msg) {return true;}
	virtual OmnRslt		start() {return true;}
	virtual bool		isSuccess() const {return mFinished && mRespCode == OmnErrId::e200Ok;}
	virtual OmnString	toString() const;
	virtual OmnString	getStatusStr() const {return "No Status";}

	bool addActivity(const Activity_e a, 
					 const OmnString &file, 
					 const int line, 
					 const OmnString &msgName,
					 const uint msgSeqno)
	{
		if (mActivityLog.entries() > mMaxActivity)
		{
			mTooManyActivity = true;
			return false;
		}

		mLastActivity = OmnTime::getSecTick();
		mActivityLog.append(Activity(a, file, line, msgName, msgSeqno));
		return true;
	}

	int64_t		getTransId() const { return mTransId; }
	bool        isFinished() const {return mFinished;}
	void        reset() {mFinished = false; mRespCode = OmnErrId::e200Ok; mTransId = 0;}
	uint		getHashKey() const {return aos_ll_hashkey(mTransId);}
	bool		hasSameObjId(const OmnTransPtr &rhs) const {return mTransId == rhs->mTransId;}
	uint		getHashKeyB() const {return mForeignTransId.getHashKey();}
	int64_t		getKey() const {return mTransId;}
	OmnForeignTransId &	getKeyB() {return mForeignTransId;}
	void		setFinished() {mFinished = true;}
	void		setTransId(const int64_t &transId) {mTransId = transId;}
	OmnString	getTransTypeStr() const {return OmnTransType::toStr(getTransType());}
	
	bool housekeeping(const uint tick)
	{
		if (tick - mLastActivity > mTTL)
		{
		}

		return false;
	}

	void setForeignTransId(const OmnIpAddr &addr,
						   const int port, 
						   const int64_t &transId)
	{
		mForeignTransId.mAddr = addr;
		mForeignTransId.mPort = port;
		mForeignTransId.mTransId = transId;
	}

	bool	isForeignTransIdValid() const {return mForeignTransId.mTransId > 0;}
	int64_t 	getForeignTransId() const {return mForeignTransId.mTransId;}
};
#endif

