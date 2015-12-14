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
// 07/27/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerUtil/TestFixture.h"

#include "Alarm/Alarm.h"
#include "alarm/alarm.h"
#include "Debug/Debug.h"
#include "ProgramAid/Util.h"
#include "Thread/Mutex.h"
#include "TorturerUtil/Variable.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"


AosTestFixture::AosTestFixture()
:
mLock(OmnNew OmnMutex())
{
}


AosTestFixture::~AosTestFixture()
{
}


bool
AosTestFixture::config(const OmnXmlItemPtr &def)
{
	// 
	// 	<TestFixture>
	// 		<Name>
	// 		<Variables>
	// 			<Global> 
	// 			...
	// 		</Variables>
	// 		<Tables>
	// 			<Table>
	// 				...
	// 			</Table>
	// 			...
	// 		</Tables>
	// 	</TestFixture>
	//
	aos_assert_r(def, false);

	OmnXmlGetStr(mName, def, "Name", "", false);
	OmnXmlItemPtr vars = def->getItem("Variables");
	if (vars)
	{
		vars->reset();
		OmnString name;
		OmnString value;
		while (vars->hasMore())
		{
			OmnXmlItemPtr item = vars->next();
			AosVariablePtr var = AosVariable::createVariable(item);
			if (!var)
			{
				OmnAlarm << "Failed to create variable: " 
					<< item->toString() << " in config: " 
					<< def->toString() << enderr;
				return false;
			}
			mVariables.append(var);
		}
	}

	return true;
}


bool 
AosTestFixture::getVarValue(const OmnString &name, OmnString &value) const
{
	mLock->lock();
	for (int i=0; i<mVariables.entries(); i++)
	{
		if (mVariables[i]->getName() == name)
		{
			value = mVariables[i]->getValue();
			mLock->unlock();
			return true;
		}
	}
	mLock->unlock();
	return false;
}


bool
AosTestFixture::removeVar(const OmnString &name)
{
	mLock->lock();
	for (int i=0; i<mVariables.entries(); i++)
	{
		if (mVariables[i]->getName() == name)
		{
			mVariables.remove(i, false);
			mLock->unlock();
			return true;
		}
	}
	mLock->unlock();
	return false;
}


bool
AosTestFixture::variableExist(const OmnString &name) const
{
	mLock->lock();
	for (int i=0; i<mVariables.entries(); i++)
	{
		if (mVariables[i]->getName() == name)
		{
			mLock->unlock();
			return true;
		}
	}
	mLock->unlock();
	return false;
}

