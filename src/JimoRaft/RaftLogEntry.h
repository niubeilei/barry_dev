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
// 2015/04/09 Created by Ketty Guo 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoRaft_RaftLogEntry_h
#define AOS_JimoRaft_RaftLogEntry_h

#include "BlobSE/BlobSEAPI.h"
#include "BlobSE/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "JimoRaft/RaftStateMachine.h"
#include "JimoRaft/Ptrs.h"
#include "JimoRaft/RaftLogMgr.h"
//#include "JimoRaft/RaftStateMachineBlobSE.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"


class AosRaftLogMgr;
class AosRaftLogEntry: virtual public OmnRCObject,
					   virtual public AosMemoryCheckerObj
{
	OmnDefineRCObject;

private:
	u64 					mLogId;
	u32  					mTermId;
	u32						mServerRecved;
	bool					mApplied;  //equals to committed

	//True if compacted, else false
	bool					mCompactFlag; //1 Byte
	AosBuffPtr				mStatMachHint;//currently 24 bytes
	AosBuffPtr				mDataBuff;

	OmnString				mTag;

public:
	//constructor and destructor
	AosRaftLogEntry(AosMemoryCheckDeclBegin);
	AosRaftLogEntry(
			u64 logId,
			u32 termId AosMemoryCheckDecl);

	~AosRaftLogEntry();

	bool serializeTo(
			AosRundata*  rdata,
			AosBuff *buff);

	bool serializeFrom(
			AosRundata*  rdata,
			AosBuff *buff);

	//getter and setters
	void	setTermId(u32 termId) { mTermId = termId; };
	u32		getTermId() { return mTermId; };

	void	setLogId(u64 logId){ mLogId = logId; };
	u64		getLogId() { return mLogId; };

	void	setApplied(bool applied) { mApplied = applied; }
	bool	isApplied() { return mApplied; }

	void	setStatMachHint(const AosBuffPtr &hint) { mStatMachHint = hint; }
	AosBuffPtr &getStatMachHint() { return mStatMachHint; }

	void	setCompactFlag(bool flag) { mCompactFlag = flag; }
	bool	isCompacted() { return mCompactFlag; }

	//for debugging purpose
	void	setTag(OmnString file, int line) 
	{
		mTag = file;
		mTag << ":" << line;
		OmnScreen << "Create log entry by tag: " << mTag << endl;
	}
	
	//data methods
	bool apply(
			AosRundata*  rdata,
			AosRaftStateMachine* statMach);

	AosBuffPtr &getData(
			AosRundata*  rdata,
			AosRaftStateMachine* statMach);

	void setData(AosBuffPtr &buff);

	u32 incReceivedCount() 
	{
		mServerRecved++; 
		return mServerRecved;
	}

};


#endif
