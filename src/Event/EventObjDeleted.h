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
// 01/10/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Event_EventObjDeleted_h
#define Omn_Event_EventObjDeleted_h

#include "Event/Event.h"
#include "Util/RCObjImp.h"


class AosEventObjDeleted : public AosEvent
{
private:

public:
	AosEventObjDeleted(
			const std::string &filename, 
			const int lineno,
			const std::string actor,
			const std::string className, 
			void *inst);
	~AosEventObjDeleted();

};

#endif

