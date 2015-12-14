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
#ifndef Aos_TransUtil_CubicTrans_h
#define Aos_TransUtil_CubicTrans_h

#include "aosUtil/Types.h"
#include "API/AosApi.h"
#include "Debug/Rslt.h"
#include "Message/Ptrs.h"
#include "Rundata/Rundata.h"
#include "TransUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/MemoryCheckerObj.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpClient.h"
#include "Util1/Time.h"
#include "TransBasic/Trans.h"


class AosCubicTrans : public AosTrans 
{

private:
	int 	mCubeId;
	u32		mCubeGrpId;

public:
	AosCubicTrans(
			const AosTransType::E type,
			const bool reg_flag);
	AosCubicTrans(
		const AosTransType::E type,
		const u64 dist_id,
		const bool need_save,
		const bool need_resp);
	AosCubicTrans(
		const AosTransType::E type,
		const u32 cube_id,
		const bool is_cube,
		const bool need_save,
		const bool need_resp);
	~AosCubicTrans();
	
	virtual bool 	serializeFrom(const AosBuffPtr &buff);
	virtual bool 	serializeTo(const AosBuffPtr &buff);
	virtual u64 	getSendKey();	// Ketty 2013/09/04
	virtual u32		getNeedAckNum();
	virtual u32		getBkpSvrNum(){ return AosGetCubeBkpNum(); };

	virtual AosTransPtr clone(){ OmnShouldNeverComeHere; return 0; };
	virtual bool	proc(){ OmnShouldNeverComeHere; return false;};

	int		getCubeId(){ return mCubeId; };
	
private:
	int 	initCubeId(const u64 dist_id);

};
#endif

