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
// 2015/03/31 Created by Ma Yagzong
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataletProcs_DataletProc_h
#define Aos_DataletProcs_DataletProc_h

#include "SEInterfaces/DataletProcObj.h"
#include "JimoCall/JimoCall.h"

class AosDataletProc : public AosDataletProcObj
{
public:
	AosDataletProc(const int version);
	virtual ~AosDataletProc();
};

#endif

