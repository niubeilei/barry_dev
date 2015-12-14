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
#ifndef Aos_TransUtil_Trans_h
#define Aos_TransUtil_Trans_h

#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/TransType.h"
#include "TransBasic/AppMsg.h"
#include "Thread/Sem.h"
#include "Thread/ThrdShellProc.h"
#include "TransUtil/Ptrs.h"
#include "Util/TransId.h"
#include "Util/Buff.h"
#include "Util/MemoryCheckerObj.h"

#define AosTransMemoryCheckDecl , const char *memory_checker_fname, const int memory_checker_lineno
#define AosTransObjCreated(classname) \
	 AosMemoryChecker::getSelf()->objCreated(this, (classname), memory_checker_fname, memory_checker_lineno);
#define AosTransObjDeleted(classname) \
	 AosMemoryChecker::getSelf()->objDeleted(this, (classname));


class AosTrans : public AosAppMsg, public AosMemoryCheckerObj
{
	//OmnDefineRCObject;

	class SendRespThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		AosRundataPtr	mRdata;
		AosTransPtr		mRespTrans;

		public:
		SendRespThrd(
				const AosRundataPtr &rdata,
				const AosTransPtr &resp_trans)
		:
		OmnThrdShellProc("SendRespThrd"),
		mRdata(rdata),
		mRespTrans(resp_trans)
		{
		}
		virtual bool run();
		virtual bool procFinished(){return true;}
	};
	
	enum
	{
		eAckWaitTime = 1000*60*1,
		eAttrFlagOff = AosTransId::eEntrySize,
		eLevelFlagOff = eAttrFlagOff + sizeof(u8),
	};

	enum
	{
		eNeedRespFlag = 0b00000001,
		eIsSyncRespFlag = 0b00000010,
		eNeedSaveFlag = 0b00000100,
		eIsResendFlag = 0b00001000,
		eIsRespRecvedFlag = 0b00010000,
		eIsGetRespFlag = 0b00100000,

		eIsCubeTransFlag = 0b00000001,
		eIsSystemTransFlag = 0b00000010,
		eIsAdminTransFlag = 0b00000100,
	};

private:
	AosTransId	mTransId;
	u8			mAttrFlag;
	u8			mLevelFlag;
	
private:
	OmnSemPtr 	mSem;		// for trans client.
	AosBuffPtr  mResp;		// for trans client.
	bool		mSvrDeath;	// for trans client.
	
	bool		mIsRecoverd;	// for trans svr.

protected:
	AosRundataPtr	mRdata;

public:
	AosTrans(const AosTransType::E type,
			const bool reg_flag = false);
	AosTrans(const AosTransType::E type,
			const int to_svrid,
			const u32 to_proc_id,
			const bool need_save,
			const bool need_resp);
	AosTrans(
		const AosTransType::E type,
		const int to_svrid,
		const AosProcessType::E to_proc_tp,
		const int cube_grp_id,
		const bool need_save,
		const bool need_resp);
	virtual ~AosTrans();
	
	static AosTransPtr getNextTrans(const AosBuffPtr &cont);
	static AosBuffPtr  getNextTransBuff(const AosBuffPtr &cont);
	static u32 getNextTransSize(const AosBuffPtr &cont);
	static AosTransId  getTransIdFromBuff(const AosBuffPtr &buff);
	static AosTransPtr serializeFromStatic(const AosBuffPtr &buff);

	// connMst call this static func.
	static AosConnMsgPtr serializeStatic(const AosBuffPtr &buff,
							const u32 beg_idx,
							const AosTransType::E tp);

	virtual bool 	serializeFrom(const AosBuffPtr &buff) = 0;
	virtual bool 	serializeTo(const AosBuffPtr &buff) = 0;
	virtual AosTransPtr clone() = 0;
	virtual AosConnMsgPtr clone2();
	virtual bool	proc() = 0;
	virtual bool	directProc();
	virtual bool	procGetResp(){ OmnShouldNeverComeHere; return true;};
	virtual OmnString toString(){ return ""; };
	virtual bool 	isTrans(){ return true; };
	virtual bool	isNeedResend(){ return true; };		// sub trans can rewrite this func.

	virtual u32		getNeedAckNum(){ return 1; };
	virtual u32		getBkpSvrNum(){ return 1; };
	virtual u64 getSendKey(){return 0; };		// Ketty 2013/09/04
	u64			getCltKey(){ return mTransId.getCltKey(); };
	
	AosTransId	getTransId(){ return mTransId; };
	void		setTransId(const AosTransId &tid){ mTransId = tid; };

	void		setRundata(const AosRundataPtr &rdata);
	AosRundataPtr getRundata() {return mRdata;}

	bool        isNeedResp(){ return (mAttrFlag & eNeedRespFlag) == eNeedRespFlag; };
	bool        isNeedSave(){ return (mAttrFlag & eNeedSaveFlag) == eNeedSaveFlag; };
	bool		isResend(){ return (mAttrFlag & eIsResendFlag) == eIsResendFlag; };
	bool		isRespRecved(){ return (mAttrFlag & eIsRespRecvedFlag) == eIsRespRecvedFlag; };
	bool		isGetResp(){ return (mAttrFlag & eIsGetRespFlag) == eIsGetRespFlag; };
	bool		isSyncResp(){ return (mAttrFlag & eIsSyncRespFlag) == eIsSyncRespFlag; }
	void 		setNeedSyncResp();
	void 		setNeedAsyncResp();
	void		setIsResend();
	void		setGetResp();
	void 		setIsRespRecved();	//tmp

	bool		isCubeTrans(){ return (mLevelFlag & eIsCubeTransFlag) == eIsCubeTransFlag; };
	bool		isSystemTrans(){ return (mLevelFlag & eIsSystemTransFlag) == eIsSystemTransFlag; };
	bool		isAdminTrans(){ return (mLevelFlag & eIsAdminTransFlag) == eIsAdminTransFlag; };

	void        setFinishLater(){};

	// Gavin, 2015/08/24
	// STORAGE-31
	virtual bool sendResp(const AosBuffPtr &resp);		// server port.

	bool 		sendAck();		// server port.
	void		setIsRecover(){ mIsRecoverd = true; };
	bool		isRecoverd(){ return mIsRecoverd; };

	void 		setSem(const OmnSemPtr &sem) {mSem = sem;}

	bool		ackRecved();
	bool		waitAck();
	bool		respRecved(const AosBuffPtr &resp);
	AosBuffPtr  waitResp();
	bool		setSvrDeath();
	bool		isSvrDeath(){ return mSvrDeath; };	
	AosBuffPtr  getResp(){return mResp;}
	virtual bool respCallBack(){ OmnShouldNeverComeHere; return false; };
	
	void 		saveAttrFlagToConnBuff();

protected:
	void		setIsAdminTrans(){ mLevelFlag |= eIsAdminTransFlag; };
	void		setIsSystemTrans(){ mLevelFlag |= eIsSystemTransFlag; };
	void		setIsCubeTrans(){ mLevelFlag |= eIsCubeTransFlag; };

private:

};
#endif

