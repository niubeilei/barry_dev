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
// 2013/12/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_Jimos_DataProcBitmapBuilder_h
#define Aos_DataProc_Jimos_DataProcBitmapBuilder_h

#include "DataProc/Jimos/JimoDataProc.h"


#include <queue>
class AosDataProcBitmapBuilder : public AosJimoDataProc
{
	OmnDefineRCObject;

private:

public:
	AosDataProcBitmapBuilder(const OmnString &version);
	~AosDataProcBitmapBuilder();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo()  const;

private:
};
#endif

