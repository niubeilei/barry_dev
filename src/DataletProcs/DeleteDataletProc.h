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
// 2015/3/31 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataletProcs_DeleteDataletProc_h
#define Aos_DataletProcs_DeleteDataletProc_h

#include "DataletProcs/DataletProc.h"


class AosDeleteDataletProc : public AosDataletProc
{
	OmnDefineRCObject;
public:
	AosDeleteDataletProc(const int version);

	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual bool isValid() const;
	virtual AosJimoPtr cloneJimo() const;

private:
	bool deleteDatalet(AosRundata *rdata, AosJimoCall &jimo_call);
};

#endif

