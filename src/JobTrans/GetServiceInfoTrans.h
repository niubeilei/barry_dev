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
// 2015/08/21	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_GetServiceInfoTrans_h
#define Aos_JobTrans_GetServiceInfoTrans_h

#include "TransUtil/TaskTrans.h"
#include "TaskUtil/Ptrs.h"

class AosGetServiceInfoTrans : virtual public AosTaskTrans
{

public:
	AosGetServiceInfoTrans(const bool regflag);
	AosGetServiceInfoTrans(
			const int server_id,
			const u32 to_proc_id);
	~AosGetServiceInfoTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

