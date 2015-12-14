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
// For the definition of SR00033 that checks double locking. Please 
// refer to the document for more details.  
//
// Modification History:
// 12/07/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticRules/SR00033.h"

#include "Alarm/Alarm.h"
#include "alarm/Alarm.h"
#include "Debug/ErrorMgr.h"
#include "Event/Event.h"
#include "SemanticObj/Ptrs.h"
#include "SemanticObj/HashtabSO.h"
#include "Semantics/SemanticsRuntime.h"



AosSR00033::AosSR00033(const u32 max) 
:
AosSemanticRuleCommon(AosRuleId::eSR00033, "SR00033", 0, 0),
mMax(max)
{
}


AosSR00033::~AosSR00033()
{
}


int	
AosSR00033::evaluate(const AosVarContainerPtr &data, 
					 std::string &errmsg)
{
	return 0;
}


bool 
AosSR00033::procEvent(const AosEventPtr &event, 
					  bool &cont)
{
	cont = true;
	switch (event->getEventId())
	{
	case AosEventId::eHashTableElemAdded:
		 return checkElemAdded(event);

	default:
		 OmnAlarm << "This is an unsolicited event: " 
			 << event->toString() << enderr;
		 return true;
	}

	aos_should_never_come_here;
	return false;
}


// 
//
bool
AosSR00033::checkElemAdded(const AosEventPtr &event)
{
	// 
	// An element was added to a hash table. This rule 
	// checks whether the total number of entries has exceeded
	// the maximum.
	//
	aos_assert_r(event->getEventId() == AosEventId::eHashTableElemAdded, false);
	void *table = event->getEventData();
	aos_assert_r(table, false);
	AosHashtabSOPtr tableSO = AosSemanticsRuntimeSelf->getHashtab(table);
	aos_assert_r(tableSO, false);
	if (tableSO->getNoe() >= mMax)
	{
		std::string errmsg = mName;
		errmsg << ": Hash table contains more than the maximum allowed:"
			<< mMax;
		aos_raise_error(errmsg);
	}

	return true;
}


