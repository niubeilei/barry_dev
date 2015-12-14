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
// The super class for all container type semantics data. 
//
// Modification History:
// 01/28/2008: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticData/VarContainer.h"

#include "alarm/Alarm.h"
#include "RVG/CommonTypes.h"
#include "RVG/Table.h"
#include "SemanticData/ClassObj.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"


AosVarContainer::AosVarContainer(const std::string &name)
:
AosVar(name),
mLock(OmnNew OmnMutex())
{
}


AosVarContainer::~AosVarContainer()
{
}


AosVarPtr	
AosVarContainer::getVar(const std::string &name)
{
	mLock->lock();
	std::list<AosVarPtr>::iterator itr;
	for (itr = mVars.begin(); itr != mVars.end(); itr++)
	{
		if ((*itr)->getName() == name)
		{
			AosVarPtr ptr = *itr;
			mLock->unlock();
			return ptr;
		}
	}
	mLock->unlock();
	return 0;
}


AosValuePtr
AosVarContainer::getValue(const std::string &name)
{
	AosVarPtr var = getVar(name);
	if (!var) return 0;
	return var->getValue();
}


AosTablePtr 
AosVarContainer::getTable(const std::string &name)
{
	mLock->lock();
	std::list<AosTablePtr>::iterator itr;
	for (itr = mTables.begin(); itr != mTables.end(); itr++)
	{
		if ((*itr)->getName() == name)
		{
			AosTablePtr ptr = *itr;
			mLock->unlock();
			return ptr;
		}
	}
	mLock->unlock();
	return 0;
}


bool		
AosVarContainer::exist(const std::string &name) const
{
	return false;
}


bool		
AosVarContainer::addVar(const AosVarPtr &)
{
	return false;
}


bool		
AosVarContainer::addTable(const AosTablePtr &)
{
	return false;
}


bool		
AosVarContainer::isTableEmpty(const std::string &name) const
{
	return false;
}


AosVarType::E	
AosVarContainer::getType() const
{
	return AosVarType::eContainer;
}


AosClassObjPtr 	
AosVarContainer::getClassObj(const std::string &name)
{
	aos_not_implemented_yet;
	return 0;
}


