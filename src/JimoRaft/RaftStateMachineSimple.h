////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/05/01 Created by Phil Pei
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoRaft_RaftStateMachineSimple_h
#define AOS_JimoRaft_RaftStateMachineSimple_h

#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "JimoRaft/Ptrs.h"
#include "JimoRaft/RaftStateMachine.h"

class AosRaftStateMachineSimple : virtual public AosRaftStateMachine 
{
public:
	//construtor/destructors
	AosRaftStateMachineSimple();
	~AosRaftStateMachineSimple();

	virtual AosJimoPtr cloneJimo() const;  

	//interface methods need to be implemented
	//by child class
	virtual AosBuffPtr appendEntry(
			const AosRundataPtr &rdata,
			const u64 logId,
			const u32			termId,
			const AosBuffPtr &buff);

	virtual bool  apply(
			const AosRundataPtr &rdata,
			const u64 logId,
			AosBuffPtr &hint);

	virtual bool  getEntry(
			const AosRundataPtr &rdata,
			const u64 logId,
			u32	&termId,
			const AosBuffPtr &hint,
			AosBuffPtr &buff);

	virtual bool  removeEntry(
			const AosRundataPtr &rdata,
			const u64 logId,
			AosBuffPtr &hint);

	virtual OmnString getClassName()
	{
		return "RaftStateMachineSimple";
	}

	OmnString toString()
	{
		return "";
	}
};

#endif
