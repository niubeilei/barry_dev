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
#ifndef Aos_TransUtil_AdminTrans_h
#define Aos_TransUtil_AdminTrans_h

#include "Rundata/Rundata.h"
#include "TransBasic/Trans.h"
#include "TransUtil/TaskTrans.h"


class AosAdminTrans : public AosTaskTrans 
{

public:
	AosAdminTrans(
			const AosTransType::E type,
			const bool reg_flag);
	AosAdminTrans(
		const AosTransType::E type,
		const int svr_id,
		const bool need_save,
		const bool need_resp);
	AosAdminTrans(
		const AosTransType::E type,
		const int svr_id,
		const u32 to_proc_id,
		const bool need_save,
		const bool need_resp);
	~AosAdminTrans();
	
	virtual bool 	serializeFrom(const AosBuffPtr &buff);
	virtual bool 	serializeTo(const AosBuffPtr &buff);
	
	virtual AosTransPtr clone(){ OmnShouldNeverComeHere; return 0; };
	virtual bool	proc(){ OmnShouldNeverComeHere; return false;};
};
#endif

