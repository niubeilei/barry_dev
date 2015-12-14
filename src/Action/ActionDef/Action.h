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
// 11/16/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Action_ActionDef_Action_h
#define Omn_Action_ActionDef_Action_h

#include "Event/Ptrs.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Util/RCObject.h"
#include <string>


class OmnString;

class AosAction : virtual public OmnRCObject
{
   
public:	
	virtual bool doAction(const AosNetRequestPtr &request, OmnString &errmsg) = 0;
	virtual bool doAction(const AosEventPtr &event, OmnString &errmsg) = 0;
};
#endif

