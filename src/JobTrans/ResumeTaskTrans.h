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
#ifndef Aos_JobTrans_ResumeTaskTrans_h
#define Aos_JobTrans_ResumeTaskTrans_h

#include "TransUtil/TaskTrans.h"

class AosResumeTaskTrans : virtual public AosTaskTrans
{

private:

public:
	AosResumeTaskTrans(const bool regflag);
	AosResumeTaskTrans(
			const int svr_id);
	~AosResumeTaskTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif

