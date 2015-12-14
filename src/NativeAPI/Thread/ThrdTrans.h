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
// 4/5/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Thread_ThrdTrans_h
#define Aos_Thread_ThrdTrans_h

#include "aosUtil/Types.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"


class OmnThrdTrans : virtual public OmnRCObject
{
	OmnDefineRCObject;

private: 
	u32 			mTransId;
	OmnConnBuffPtr	mBuff;
	bool			mFinished;

public:
	OmnThrdTrans();
	OmnThrdTrans(const u32 transId)
	:
	mTransId(transId),
	mFinished(false)
	{
	}

	~OmnThrdTrans();

	virtual bool	isSuccess() const {return true;}
	virtual bool	msgRcved(const OmnConnBuffPtr &buff) {return true;}

	bool		finished() const {return mFinished;}
	u32			getTransId() const {return mTransId;}
	OmnConnBuffPtr	getBuff() const;

private:
};
#endif

