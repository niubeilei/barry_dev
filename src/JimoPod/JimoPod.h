////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/05/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocServer_JimoPod_h
#define AOS_DocServer_JimoPod_h

#include "SEInterfaces/JimoPodObj.h"

class AosJimoPod : public AosJimoPodObj
{
private:
	int		mStoreSize;

public:
	AosJimoPod();
	AosJimoPod(const int store_size);
	~AosJimoPod();

	virtual u32 getJSID(const u64 docid) const;
};
#endif
