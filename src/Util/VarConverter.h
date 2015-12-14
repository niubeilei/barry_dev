////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: VarMapper.h
// Description:
//	This class converts values from one set to another set. As an example,
//		Original Values: {"0", "1"}
//		Converted Values: {"off", "on"}
//
//	The class of the values must support the operator "==".
//
// Modification History:
// 07/27/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_VarMapper_h
#define Aos_Util_VarMapper_h


template <class C>
class AosVarMapper
{
private:
	OmnDynArray<C, 10, 10>	mOriValues;
	OmnDynArray<C, 10, 10>	mMappedValueds;

public:
	AosVarMapper(const OmnDynArray<C, 10, 10> &orig, 
				 const OmnDynArray<C, 10, 10> &mapped)
	:
	mOriValues(orig),
	mMappedValues(mapped)
	{
	}

	~AosVarMapper() {}

	bool	mapTo(const C &value, C &mapped) const
	{
		for (int i=0; i<mOriValues.entries(); i++)
		{
			if (mOriValues[i] == value)
			{
				mapped = mMappedValues[i];
				return true;
			}
		}

		return false;
	}

	bool	mapFrom(const C &value, C &mapped) const
	{
		for (int i=0; i<mMappedValues.entries(); i++)
		{
			if (mMappedValues[i] == value) 
			{
				mapped = mOriValues[i];
				return true;
			}
		}

		return false;
	}

};
#endif

