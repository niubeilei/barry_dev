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
#ifndef Omn_Event_EventObjRenamed_h
#define Omn_Event_EventObjRenamed_h

#include "Event/Event.h"
#include "Util/RCObjImp.h"


class AosEventObjRenamed : public AosEvent
{
private:

public:
	AosEventObjRenamed(
			const std::string &filename, 
			const int lineno,
			const std::string actor,
			const std::string className, 
			void *inst);
	~AosEventObjRenamed();

};

#endif

