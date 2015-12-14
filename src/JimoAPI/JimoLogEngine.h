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
// 2015/03/17 Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_LogEngine_h
#define Aos_JimoAPI_LogEngine_h

#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "BSON/BSON.h"
#include "Rundata/Rundata.h"


namespace Jimo
{

class LogEngine
{
public:
	bool addLog(AosRundata *rdata,
					const OmnString &log_name,
					AosBSON *log);

	bool addLog(AosRundata *rdata,
					const OmnString &log_name,
					AosBSON *log, 
					u64 &logid);
};

extern LogEngine gLogEngine;

};
#endif

