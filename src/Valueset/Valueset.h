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
// This is a utility to select docs.
//
// Modification History:
// 2013/12/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Valueset_Valueset_h
#define AOS_Valueset_Valueset_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Valueset/ValuesetTypes.h"



class AosValueset : virtual public AosJimo
{
protected:
	OmnString		mValuesetType;

public:
	AosValueset(const OmnString &type, const OmnString &version);
	virtual ~AosValueset();

	virtual bool resetReadLoop() = 0;
	virtual bool resetWriteLoop() = 0;
	virtual AosValue *nextValue(const AosRundataPtr &rdata) = 0;
	virtual bool setData(
						const AosRundataPtr &rdata, 
						const AosValuePtr &data) = 0;
};
#endif
