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
#include "Thread/ThrdTrans.h"

#include "UtilComm/ConnBuff.h"



OmnThrdTrans::OmnThrdTrans()
:
mTransId(0), 
mFinished(false)
{
}


// OmnThrdTrans::OmnThrdTrans(const OmnConnBuffPtr &buff, const u32 transId)
// :
// mTransId(transId), 
// mBuff(buff),
// mFinished(false)
// {
// }


OmnThrdTrans::~OmnThrdTrans()
{
}


OmnConnBuffPtr
OmnThrdTrans::getBuff() const
{
	return mBuff;
}

