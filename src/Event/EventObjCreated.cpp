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
// 01/11/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Event/EventObjCreated.h"


AosEventObjCreated::AosEventObjCreated(
			const std::string &filename, 
			const int lineno,
			const std::string actor,
			const std::string className, 
			void *eventData, 
			const u32 dataLen)
:
AosEvent(AosEventId::eObjCreated, filename, lineno, actor, 
			className, eventData, dataLen)
{
}


AosEventObjCreated::~AosEventObjCreated()
{
}

