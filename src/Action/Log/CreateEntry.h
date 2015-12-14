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
// 03/24/2009: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Action_Log_CreateEntry_h
#define Omn_Action_Log_CreateEntry_h

#include "Action/ActionDef/Action.h"
#include "Event/Ptrs.h"


class AosLogActCreateEntry : public AosAction
{
   
public:	
	AosLogActCreateEntry();
	~AosLogActCreateEntry();
	virtual bool doAction(const AosNetRequestPtr &request, OmnString &errmsg);
	virtual bool doAction(const AosEventPtr &event, OmnString &errmsg);
};
#endif

