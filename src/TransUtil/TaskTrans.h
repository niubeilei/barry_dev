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
#ifndef Aos_TransUtil_TaskTrans_h
#define Aos_TransUtil_TaskTrans_h

#include "Rundata/Rundata.h"
#include "TransBasic/Trans.h"


class AosTaskTrans : public AosTrans 
{
	static 	AosTransProcThrdPtr		smTaskProcThrd;

public:
	AosTaskTrans(
			const AosTransType::E type,
			const bool reg_flag);
	AosTaskTrans(
		const AosTransType::E type,
		const int svr_id,
		const u32 to_proc_id,
		const bool need_save,
		const bool need_resp);
	AosTaskTrans(
		const AosTransType::E type,
		const int svr_id,
		const AosProcessType::E to_proc_tp,
		const int cube_grp_id,
		const bool need_save,
		const bool need_resp);
	AosTaskTrans(
		const AosTransType::E type,
		const int svr_id,
		const bool need_save,
		const bool need_resp);
	AosTaskTrans(
		const AosTransType::E type,
		const int svr_id,
		const AosProcessType::E to_proc_tp,
		const bool need_save,
		const bool need_resp);
	~AosTaskTrans();
	
	virtual bool 	serializeFrom(const AosBuffPtr &buff);
	virtual bool 	serializeTo(const AosBuffPtr &buff);
	virtual u64 	getSendKey();	// Ketty 2013/09/04
	
	virtual AosTransPtr clone(){ OmnShouldNeverComeHere; return 0; };
	virtual bool	proc(){ OmnShouldNeverComeHere; return false;};
	virtual bool	directProc();


};
#endif

