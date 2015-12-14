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
// For the definition of SR00031, please refer to the document.  
//
// Modification History:
// 12/07/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticRules_SR00031_h
#define Aos_SemanticRules_SR00031_h

#include "aosUtil/Types.h"
#include "Event/Ptrs.h"
#include "Event/ActivityTracker.h"
#include "SemanticRules/SemanticRuleCommon.h"
#include "SemanticData/Types.h"
#include "SemanticObj/Ptrs.h"
#include "Util/RCObjImp.h"
#include <string>
#include <list>

class AosSR00031 : public AosSemanticRuleCommon
{
	OmnDefineRCObject;

private:

public:
	AosSR00031();
	~AosSR00031();

	virtual int	evaluate(const AosVarContainerPtr &data, 
						 std::string &errmsg);
	virtual bool procEvent(const AosEventPtr &event, 
						 bool &cont);

private:
	bool	checkToLockEvent(const AosEventPtr &event, bool &cont);
	bool	checkToUnlockEvent(const AosEventPtr &event, bool &cont);
	bool	checkLockedEvent(const AosEventPtr &event, bool &cont);
	bool	checkUnlockedEvent(const AosEventPtr &event, bool &cont);
	void	generateError(const std::list<AosThreadSOPtr> &threads, 
						   const std::list<AosLockSOPtr> &locks, 
						   const AosEventPtr &event);
};
#endif

