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
#ifndef Aos_RlbTester_TesterTrans_RlbTesterDamageDiskTrans_h
#define Aos_RlbTester_TesterTrans_RlbTesterDamageDiskTrans_h

#include "TransUtil/TaskTrans.h"

class AosRlbTesterDamageDiskTrans : public AosTaskTrans
{

public:
	AosRlbTesterDamageDiskTrans(const bool regflag);
	AosRlbTesterDamageDiskTrans(
			const int svr_id,
			const u32 proc_id);
	~AosRlbTesterDamageDiskTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eRlbTester_DamageDisk"; };
	virtual AosTransPtr clone();
	virtual bool proc();
	
};
#endif

