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
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_RollBackTrans_h
#define Aos_JobTrans_RollBackTrans_h

#include "TransUtil/TaskTrans.h"

class AosRollBackTrans : virtual public AosTaskTrans
{

private:

public:
	AosRollBackTrans(const bool regflag);
	AosRollBackTrans(
			const int svr_id);
	~AosRollBackTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

