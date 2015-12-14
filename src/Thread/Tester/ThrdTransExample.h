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
// 4/16/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Thread_ThrdTransExample_h
#define Aos_Thread_ThrdTransExample_h

#include "Thread/ThrdTrans.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosThrdTransExample : public OmnThrdTrans
{
	OmnDefineRCObject;

private: 

public:
	AosThrdTransExample(const u32 transId);
	~AosThrdTransExample();

    virtual bool    isSuccess() const;
	virtual bool    msgRcved(const OmnConnBuffPtr &buff);

private:
};
#endif

