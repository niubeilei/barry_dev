////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NullTrans.h
// Description:
//	This is the super class for transactions.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TransMgr_NullTrans_h
#define Omn_TransMgr_NullTrans_h


#include "TransMgr/Trans.h"


class OmnNullTrans : public OmnTrans 
{
public:
	virtual OmnTransType::E	getTransType() const {return OmnTransType::eNullTrans;}
	virtual bool housekeepingFailed(const uint tick) {return true;}
	virtual bool ttlExpired() const {return false;}
};
#endif

