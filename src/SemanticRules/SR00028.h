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
// For the definition of SR00028, please refer to the document.  
//
// Modification History:
// 12/05/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticRules_SR00028_h
#define Aos_SemanticRules_SR00028_h

#include "aosUtil/Types.h"
#include "Event/ActivityTracker.h"
#include "SemanticRules/SemanticRuleCommon.h"
#include "SemanticData/Types.h"
#include "Util/RCObjImp.h"
#include <string>
#include <list>

class AosSR00028 : public AosSemanticRuleCommon
{
	OmnDefineRCObject;

private:
	std::string			mThreadName;
	std::string			mLockName;

public:
	AosSR00028(const std::string &thread_name, 
			   const std::string &lockname, 
			   void *lock);
	~AosSR00028();

	virtual int	evaluate(const AosVarContainerPtr &data, 
						 std::string &errmsg);
	virtual bool procEvent(const AosEventPtr &event, 
						 bool &cont);

private:
	bool	checkToLockEvent(const AosEventPtr &event, bool &cont);
	bool	checkToUnlockEvent(const AosEventPtr &event, bool &cont);
	bool	checkLockedEvent(const AosEventPtr &event, bool &cont);
	bool	checkUnlockedEvent(const AosEventPtr &event, bool &cont);
};
#endif

