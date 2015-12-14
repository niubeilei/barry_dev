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
// 2013/09/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransServer_TransFileReq_h
#define AOS_TransServer_TransFileReq_h

#include "alarm_c/alarm.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Sem.h"
#include "Thread/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

#include <vector>
using namespace std;

class AosTransFileReq: public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum Type
	{
		eClean,
		eReadUnfinish,
	};

private:
	Type	mType;
	int		mReadId;
	AosBuffPtr mTransBuffs;
	bool	mFinish;
	OmnSemPtr mSem;

public:
	AosTransFileReq(const Type tp, const int read_id);
	
	Type	getType(){ return mType; };
	void	setReadId(const int read_id){ mReadId = read_id; };
	int		getReadId(){return mReadId; };
	void	addTransBuffs(const AosBuffPtr &buff){ mTransBuffs = buff; }
	AosBuffPtr getTransBuffs(){ return mTransBuffs; };

	void	setReadFinish(){ mFinish = true;};
	bool	isReadFinish(){ return mFinish;};

	OmnString	toString();
	void	reset(){ mTransBuffs = 0; mFinish = false;};

	void	wait(){ mSem->wait(); };
	void	wakeup(){ mSem->post(); };
};

class AosCleanTFReq: public AosTransFileReq
{

public:
	AosCleanTFReq()
	:
	AosTransFileReq(eClean, -1)
	{
	}

};

class AosReadUnfinishTFReq: public AosTransFileReq
{

public:
	AosReadUnfinishTFReq(const int read_id)
	:
	AosTransFileReq(eReadUnfinish, read_id)
	{
	}

};

#endif
