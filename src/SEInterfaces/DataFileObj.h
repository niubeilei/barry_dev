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
// 2013/11/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataFileObj_h
#define Aos_SEInterfaces_DataFileObj_h

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"



class AosDataFileObj : virtual public OmnRCObject
{
public:
	AosDataFileObj();
	~AosDataFileObj();

	virtual int 	getPhysicalId() const = 0;
	virtual bool 	getNextBlock(AosRundata *rdata, AosBuffDataPtr &info) = 0;
};
#endif

