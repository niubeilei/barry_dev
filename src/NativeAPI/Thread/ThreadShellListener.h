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
// 07/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Thread_ThreadShellListener_h 
#define Omn_Thread_ThreadShellListener_h

#include "Rundata/Ptrs.h"
class AosValueRslt;
#include "Util/RCObject.h"

class AosThreadShellListener : public OmnRCObject
{
public:
	virtual bool threadShellFinished(
					const bool status, 
					const AosValueRslt &user_data, 
					const AosRundataPtr &rdata) = 0;
};
#endif
