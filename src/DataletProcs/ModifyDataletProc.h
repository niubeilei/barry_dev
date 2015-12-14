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
// 2015/4/08 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataletProcs_ModifyDataletProc_h
#define Aos_DataletProcs_ModifyDataletProc_h

#include "DataletProcs/DataletProc.h"

class AosModifyDataletProc : public AosDataletProc
{
	OmnDefineRCObject;
public:
	AosModifyDataletProc(const int version);

	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual bool isValid() const;
	virtual AosJimoPtr cloneJimo() const;

private:
	bool modifyDatalet(AosRundata *rdata, AosJimoCall &jimo_call);
	bool writeLockDatalet(AosRundata *rdata, AosJimoCall &jimo_call);
	bool writeUnLockDatalet(AosRundata *rdata, AosJimoCall &jimo_call);
	bool mergeSnapshot(AosRundata *rdata, AosJimoCall &jimo_call);
	bool commitSnapshot(AosRundata *rdata, AosJimoCall &jimo_call);
	bool rollbackSnapshot(AosRundata *rdata, AosJimoCall &jimo_call);
	bool createSnapshot(AosRundata *rdata, AosJimoCall &jimo_call);

};
#endif

