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
// This class implements the common functions for all semantic objects.  
// All semantic objects should derive from this class. 
//
// Modification History:
// 12/09/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticObj/SemanticObjCommon.h"

#include "alarm/Alarm.h"
#include "SemanticRules/SemanticRule.h"
#include "Semantics/SemanticsRuntime.h"



AosSemanticObjCommon::AosSemanticObjCommon(
			const std::string &name,
			const AosSOType::E type, 
			void *inst)
:
mName(name),
mType(type), 
mInst(inst),
mStatus(false)
{
	aos_assert(mInst);
	aos_assert(mType > AosSOType::eUnknown && 
			   mType < AosSOType::eMaxEntry);
	aos_assert(mName.length() > 0);
}


AosSemanticObjCommon::~AosSemanticObjCommon()
{
	mType = AosSOType::eInvalid;
	mInst = 0;
	mName = "deleted";
}


bool	
AosSemanticObjCommon::start()
{
	registerEvents();
	mStatus = true;
	return true;
}


bool	
AosSemanticObjCommon::stop()
{
	unregisterEvents();
	mStatus = false;
	return true;
}


bool
AosSemanticObjCommon::integrityCheck(std::list<u32> &error_ids)
{
	if (!mStatus) return true;

	AosRuleListItr itr;
	bool good = true;
	for (itr = mIntegrityRules.begin(); 
				itr != mIntegrityRules.end(); itr++)
	{
		if (!aos_evaluate_rule((*itr), error_ids))
		{
			good = false;
		}
	}

	return good;
}


u32		
AosSemanticObjCommon::getHashKey() const
{
	// 
	// It uses mInst as the hash key.
	//
	aos_assert_r(mInst, 0);
	return (u32)mInst;
}


bool	
AosSemanticObjCommon::isSameObj(const AosSemanticObjPtr &rhs) const
{
	return (mInst == ((AosSemanticObjCommon *)rhs.getPtr())->mInst);
}



