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
// 12/07/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticObj_ThreadSO_h
#define Aos_SemanticObj_ThreadSO_h

#include "SemanticObj/SemanticObj.h"
#include "SemanticObj/Ptrs.h"

class AosThreadSO : public AosSemanticObj
{
private:
	bool			mIsTryingToLock;
	AosLockSOPtr	mPendingLock;

public:
	bool			isTryingToLock() const {return mIsTryingToLock;}
	AosLockSOPtr	getPendingLock() const;
	void			tryToLock(const AosLockSOPtr &lock);
};

#endif
