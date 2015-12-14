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
// 05/01/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ErrorMgr_ErrorCode_h
#define Aos_ErrorMgr_ErrorCode_h

#include "Util/String.h"


class AosErrorCode
{
public:
	enum E
	{
		eInvalid, 

		eNoListener,
		eCommandNotRecognized,
		eInvalidModuleId,
		eMissingJobDocid,
		eMissingTaskSeqno,
		eTaskNotFound,
		eTaskNotLocal,
		eFailedCreateTask,
		eFailedScheduleTask,
		eTaskScheduled,

		eMax
	};
};
#endif
