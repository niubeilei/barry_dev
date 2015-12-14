////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GenericDataContainer.h
// Description:
// mPrimaryKeyIndex: if it is -1, the table has no primary key. Otherwise, 
// it is the field index of the field that serves as the primary key.
//
// Modification History:
// 05/15/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Util/VarList.h"

#include "Alarm/Alarm.h"

AosVarList::AosVarList()
{
}


AosVarList::~AosVarList()
{
}


bool	
AosVarList::addVar(const OmnString &name, const bool checkExist)
{
	// 
	// This function adds a new variable to the list. If 'checkExist'
	// is true and the variable is already in the list, it is an error.
	// Otherwise, it is not considered an error.
	//
	for (int i=0; i<mNames.entries(); i++)
	{
		if (mNames[i] == name)
		{
			if (!checkExist)
			{
				return true;
			}

			OmnAlarm << "Variable already exist: " << name << enderr;
			return false;
		}
	}

	mNames.append(name);
	mValues.append("");
	return true;
}


bool	
AosVarList::removeVar(const OmnString &name)
{
	for (int i=0; i<mNames.entries(); i++)
	{
		if (mNames[i] == name)
		{
			mNames.remove(i);
			return true;
		}
	}

	OmnAlarm << "Variable not found: " << name << enderr;
	return false;
}


bool	
AosVarList::setValue(const OmnString &name, 
					 const OmnString &value, 
					 const bool addIfNotExist)
{
	for (int i=0; i<mNames.entries(); i++)
	{
		if (mNames[i] == name)
		{
			mValues[i] = value;
			return true;
		}
	}

	if (addIfNotExist)
	{
		mNames.append(name);
		mValues.append(value);
		return true;
	}

	OmnAlarm << "Variable not found: " << name << enderr;
	return false;
}


bool	
AosVarList::getValue(const OmnString &name, OmnString &value) const
{
	for (int i=0; i<mNames.entries(); i++)
	{
		if (mNames[i] == name)
		{
			value = mValues[i];
			return true;
		}
	}

	OmnAlarm << "Variable not found: " << name << enderr;
	return false;
}


OmnString
AosVarList::toString() const
{
	OmnString str;
	for (int i=0; i<mNames.entries(); i++)
	{
		str << "Name: " << mNames[i] << " = " << mValues[i] << "\n";
	}

	return str;
}

