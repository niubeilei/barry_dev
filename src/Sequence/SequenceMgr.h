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
// 2015/02/11 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Sequence_SequenceMgr_h
#define Aos_Sequence_SequenceMgr_h

#include "SEInterfaces/SequenceMgrObj.h"
#include "Sequence/Sequence.h"

#include <map>
using namespace std;


class AosSequenceMgr : public AosSequenceMgrObj
{
	OmnDefineRCObject;

private:
	OmnMutexPtr						mLock;
	OmnMutex *						mLockRaw;

	map<OmnString, AosSequencePtr>  mSequences;
	
public:
	AosSequenceMgr();
	~AosSequenceMgr();
	
	virtual bool	getNextValue(
						const AosRundataPtr &rdata,
						const OmnString &name,
						int &value);
	virtual bool	getCrtValue(
						const AosRundataPtr &rdata,
						const OmnString &name,
						int &value);

	virtual bool	checkSequenceExist(
						const AosRundataPtr &rdata,
						const OmnString &name,
						const OmnString &func); 

private:
	AosSequencePtr	getSequence(
						const AosRundataPtr &rdata,
						const OmnString &name);
	
};

#endif

