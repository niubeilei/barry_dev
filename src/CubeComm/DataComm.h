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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeComm_DataComm_h
#define Aos_CubeComm_DataComm_h

#include "SEInterfaces/JimoCaller.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DataTypes.h"
#include "Util/ValueRslt.h"
#include "Rundata/Rundata.h"
#include "Jimo/Jimo.h"

/*class AosDataComm : public AosJimo,
					public AosJimoCaller
{
private:

public:
	AosDataComm(const int version);

	virtual bool createData(AosRundata *rdata, 
							const u32 cubeid,
							const u64 aseid,
							const char * const data, 
							const int length) = 0;
};*/
#endif
