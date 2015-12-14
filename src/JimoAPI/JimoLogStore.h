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
#ifndef Aos_JimoAPI_LogStore_h
#define Aos_JimoAPI_LogStore_h

#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "BSON/BSON.h"

class AosRundata;

namespace Jimo
{


class LogStore
{
public:
	u64 addLog(AosRundata *rdata,
					const OmnString &log_name,
					AosBSON *log);
	u64 saveLog(AosRundata *rdata, 
					const u64 docid,
					const OmnString &log_name, 
					AosBuff *buff);
};

extern LogStore gLogStore;

};
#endif

