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
// 01/10/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ObjSet_ObjSet_h
#define Aos_ObjSet_ObjSet_h

#include "Event/EventEntry.h"
#include "Event/EventListener.h"
#include "Event/EventUtil.h"
#include "Event/Ptrs.h"
#include "Util/RCObject.h"
#include <list>

class AosObjSet : virtual public AosEventListener
{
protected:
	std::list<AosEventEntry>	mEventData;

public:
	AosObjSet();
	virtual ~AosObjSet();

	bool	start();
	bool	stop();

	virtual AosEventRc::E	procEvent(const AosEventPtr &event) = 0;
};

#endif

