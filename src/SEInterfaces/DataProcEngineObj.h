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
// 2013/08/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataProcEngineObj_h
#define Aos_SEInterfaces_DataProcEngineObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosDataProcEngineObj : public AosJimo
{
protected:

public:
	AosDataProcEngineObj(const u32 version);
	virtual ~AosDataProcEngineObj();

	// AosJimo Interface
	virtual OmnString toString() const;
};

#endif

