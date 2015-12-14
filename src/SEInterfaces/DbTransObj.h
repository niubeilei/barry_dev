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
//
// Modification History:
// 2014/10/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DbTransObj_h
#define Aos_SEInterfaces_DbTransObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosDbTransObj : public AosJimo
{
protected:
	enum Status
	{
		eInvalid,

		eActive,
		eCommitted,
		eRolledBack,
	};

	Status		mStatus;
	u64			mTransId;
	u64			mStartTime;
	int			mMaxTime;
	u64			mDocid;
	u64			mCreator;

public:
	AosDbTransObj(const int version);
	~AosDbTransObj();

	u64	 	getTransID() const {return mTransId;}
	u64	 	getStartTime() const {return mStartTime;}
	u64	 	getDocid() const {return mDocid;}
	u64	 	getCreator() const {return mCreator;}
	Status 	getStatus() const {return mStatus;}
	int 	getMaxTime() const {return mMaxTime;}

	virtual bool start(AosRundata *rdata) = 0;
	virtual bool commit(AosRundata *rdata) = 0;
	virtual bool rollback(AosRundata *rdata) = 0;
};
#endif

