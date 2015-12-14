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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_JimoCallDialer_h
#define Aos_JimoCall_JimoCallDialer_h

#include "ErrorMgr/ErrmsgId.h"
#include "JimoCall/Ptrs.h"
#include "Message/MsgId.h"
#include "SEInterfaces/CubeMapObj.h"
#include "Util/Buff.h"
#include "UtilComm/CommListener.h"

#include <map>
using namespace std;
//using namespace Jimo;

class AosRundata;
class AosEndPointInfo;

class AosJimoCallDialer : public OmnCommListener
{
	typedef hash_map<const u64, AosJimoCallPtr, u64_hash, u64_cmp> call_t;
	typedef hash_map<const u64, AosJimoCallPtr, u64_hash, u64_cmp>::iterator callitr_t;

protected:
	enum Type
	{
		eInvalidType,

		eManual,
		eRoundRobin,
		eAuto,

		eMaxType
	};

	enum
	{
		eFirstDialerID = 1001
	};

	OmnMutexPtr		mLock;
	OmnMutex *		mLockRaw;

	Type			mType;
	u32				mSelfEPID;
	call_t	     	mPendingCalls;

	static AosJimoCallDialer *	smIILManualDialer;
	static AosJimoCallDialer *	smDocManualDialer;
	static AosJimoCallDialer *	smRoundRobinDialer;

public:
	AosJimoCallDialer(
			AosRundata *rdata,
			const Type type);
	~AosJimoCallDialer();

	static AosJimoCallDialerPtr createDialer(AosRundata *rdata, const AosXmlTagPtr &conf);
	Type getDialerType() const {return mType;}

	virtual u64		nextJimoCallID(AosRundata *rdata) = 0;
	virtual bool	makeJimoCall(AosRundata *rdata,
						const AosEndPointInfo &endpoint,
						const AosJimoCallPtr &call,
						AosBuff *buff) = 0;

	virtual bool	getTargetCubes(AosRundata *rdata, vector<AosCubeMapObj::CubeInfo> &cube_infos) = 0;
	virtual void	eraseJimoCall(const u64 &jimo_callid);

	// OmnCommListener interface
	virtual bool	msgRead(const OmnConnBuffPtr &buff);
	virtual void	readingFailed();
	virtual OmnString getCommListenerName() const;

	static bool		init(AosRundata *rdata);
	static AosJimoCallDialer *getIILManualDialer(AosRundata *rdata);
	static AosJimoCallDialer *getDocManualDialer(AosRundata *rdata);
	static AosJimoCallDialer *getRoundRobinDialer(AosRundata *rdata);

private:
	bool	logError(
				AosRundata *rdata, 
				const AosErrmsgId::E errid,
				const u32 from_epid,
				const u32 to_epid,
				const u64 jimocall_id,
				const u32 msg_id,
				const int func,
				const int package);
};

#endif

