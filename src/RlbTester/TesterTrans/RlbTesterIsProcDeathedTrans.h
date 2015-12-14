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
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RlbTester_TesterTrans_RlbTesterIsProcDeathedTrans_h
#define Aos_RlbTester_TesterTrans_RlbTesterIsProcDeathedTrans_h

#include "TransUtil/TaskTrans.h"

class AosRlbTesterIsProcDeathedTrans : public AosTaskTrans
{

public:
	AosRlbTesterIsProcDeathedTrans(const bool regflag);
	AosRlbTesterIsProcDeathedTrans(
			const int svr_id,
			const u32 proc_id);
	~AosRlbTesterIsProcDeathedTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eRlbTester_IsProcDeathed"; };
	virtual AosTransPtr clone();
	virtual bool proc();
	
};
#endif

