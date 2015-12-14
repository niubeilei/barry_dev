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
// 01/01/2013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_CompUint_h
#define AOS_Util_CompUint_h

class AosCompUint
{
private:
	u32		mType;
	u64		mModuleId;
	u64		mMemberId;

public:
	AosCompUint(const char *data);
	~AosCompUint() { }

	bool isValid() const
	{
		return false;
	}
};
#endif
