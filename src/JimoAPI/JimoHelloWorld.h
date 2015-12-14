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
// 2015/03/18 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoHelloWorld_h
#define Aos_JimoAPI_JimoHelloWorld_h

#include "JimoCall/JimoCall.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"


namespace Jimo
{
	extern bool jimoCallSyncNorm(
					AosRundata *jimo_rdata,
					const OmnString &message,
					OmnString &resp_str);

	extern bool jimoCallSyncNorm(
					AosRundata *jimo_rdata,
					const OmnString &message,
					OmnString &resp_str, 
					const int endpoint_id);

	extern bool jimoCallSyncRead(
					AosRundata *jimo_rdata,
					const OmnString &message,
					OmnString &resp_str);

	extern bool jimoCallSyncWrite(
					AosRundata *jimo_rdata,
					const OmnString &message,
					OmnString &resp_str);

	extern bool jimoCallAsyncNorm(
					AosRundata *jimo_rdata,
					const OmnString &message,
					OmnString &resp_str);

	extern bool jimoCallAsyncRead(
					AosRundata *jimo_rdata,
					const OmnString &message,
					OmnString &resp_str);

	extern bool jimoCallAsyncWrite(
					AosRundata *jimo_rdata,
					const OmnString &message,
					OmnString &resp_str);

};

#endif


