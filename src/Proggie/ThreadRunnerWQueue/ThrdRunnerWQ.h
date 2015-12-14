////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 03/25/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Proggie_ThreadRunnerWQueue_ThrdRunnerWQ_h
#define Omn_Proggie_ThreadRunnerWQueue_ThrdRunnerWQ_h

#include "Thread/Ptrs.h"
#include "Util/RCObject.h"


class AosThrdRunnerWQ : public virtual OmnRCObject
{
public:
	AosThrdRunnerWQ(std::list<AosThrdRunnerWQObjPtr> &genThrds, 
			std::list<AosThrdRunnerWQObjPtr> &procThrds, 
			const int max_gen_thrds,
			const int max_proc_thrds);
	AosThrdRunnerWQ(const int num_gen_thrds, const int num_proc_thrds);
	~AosThrdRunnerWQ();

	bool	start();
	bool	stop();
	bool	stopCreateThread(const OmnThreadPtr &thread);
	bool	stopProcThread(const OmnThreadPtr &thread);
	int		getNumCreateThreads() const;
	int		getNumProcThreads() const;
	int		getAverageQueueSize(const AosTimeEnum t) const;
	int		getMaxQueueSize(const AosTimeEnum t) const;
	int		getMinQueueSize(const AosTimeEnum t) const;
	int		getTotalReqsProced(const AosTimeEnum t) const;
	int		getTotalSuccessReqs(const AosTimeEnum t) const;
	int		getTotalFailedReqs(const AosTimeEnum t) const;
};
#endif

