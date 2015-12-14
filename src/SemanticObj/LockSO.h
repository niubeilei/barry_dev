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
#ifndef Aos_SemanticObj_LockSO_h
#define Aos_SemanticObj_LockSO_h

#include "SemanticObj/Ptrs.h"
#include "SemanticObj/SemanticObj.h"

class AosLockSO : public AosSemanticObj
{
private:
	bool			mIsLocked;
	AosThreadSOPtr	mOwner;

public:
	AosLockSO();
	~AosLockSO();

	void			locked() {mIsLocked = true;}
	void			unlocked() {mIsLocked = false;}
	bool			isLocked() const {return mIsLocked;}
	AosThreadSOPtr	getOwner() const;
	void			setOwner(const AosThreadSOPtr &thread);
};

#endif
