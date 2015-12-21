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
#ifndef AOS_JimoRaft_RaftStateMachine_h
#define AOS_JimoRaft_RaftStateMachine_h

#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "JimoRaft/Ptrs.h"

#define RAFT_MAX_STATMACH_HINT_SIZE	24

typedef hash_map<const OmnString, AosRaftStateMachinePtr, Omn_Str_hash, compare_str> mapptr_t;
typedef hash_map<const OmnString, AosRaftStateMachine*, Omn_Str_hash, compare_str> map_t;
typedef hash_map<const OmnString, AosRaftStateMachine*, Omn_Str_hash, compare_str>::iterator itr_t;

class AosRaftStateMachine : virtual public AosJimo
{
	OmnDefineRCObject;

protected:
	AosBuffPtr		mBuff;

public:
	//construtor/destructors
	AosRaftStateMachine();
	~AosRaftStateMachine();

	static AosRaftStateMachine *createRaftStateMachineStatic(
				const AosRundataPtr &rdata,
				const OmnString &className,
				const int version);

	//interface methods need to be implemented
	//by child class
	virtual bool  appendEntry(
			const AosRundataPtr &rdata,
			const u32	termId,
			const u64 logId,
			const AosBuffPtr &buff)
	{
		return true;
	}

	virtual bool  apply(
			const AosRundataPtr &rdata,
			const u64 logId)
	{
		return true;
	}

	virtual bool  getEntry(
			const AosRundataPtr &rdata,
			const u64 logId,
			u32	&termId,	
			AosBuffPtr &buff)
	{
		return true;
	}

	virtual bool  removeEntry(
			const AosRundataPtr &rdata,
			const u64 logId)
	{
		return true;
	}

	virtual OmnString getClassName() = 0;

	virtual bool serializeTo(
			const AosRundataPtr &rdata,
			AosBuff *buff)
	{ return true; }

	virtual bool serializeFrom(
		const AosRundataPtr &rdata,
		AosBuff *buff)
	{ return true; }

	virtual bool init(
			const AosRundataPtr	&rdata,
			const u64 ullLastAppliedOprID)	//added by White, 2015-08-26 18:08:46
	{ return true;}

	virtual AosBuffPtr  appendEntry(
			const AosRundataPtr &rdata,
			const u64 logId,
			const u32 termId,
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

	//getter/setters
	AosBuffPtr	&getBuff() { return mBuff; }
	void setBuff(AosBuffPtr &buff) { mBuff = buff; }

	virtual int getCubeId() { return 0; }
	virtual u32 getAseId() { return 0; }
	virtual OmnString getBaseDir() { return ""; }

	//methods to get info from stat machine
	virtual bool isLogExisting(u64 logId, 
					   AosBuffPtr &hint) { return true; }

	virtual bool isLogApplied(u64 logId,
			          AosBuffPtr &hint) { return true; }

	virtual bool getHintSize() { return RAFT_MAX_STATMACH_HINT_SIZE; }

	virtual u64 getLastLogIdAppended() { return 0; }
	virtual u64 getLastLogIdApplied() { return 0; }

};


#endif
