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
#ifndef Aos_Util_VarList_h
#define Aos_Util_VarList_h

#include "Util/String.h"
#include "Util/DynArray.h"


class AosVarList : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eArrayInitSize = 100,
		eArrayIncSize = 50,
		eArrayMaxSize = 10000
	};

private:
	OmnDynArray<OmnString, eArrayInitSize, eArrayIncSize, eArrayMaxSize>	mNames;
	OmnDynArray<OmnString, eArrayInitSize, eArrayIncSize, eArrayMaxSize>	mValues;

public:
	AosVarList();
	~AosVarList();

	bool	addVar(const OmnString &name, const bool checkExist);
	bool	removeVar(const OmnString &name);
	bool	setValue(const OmnString &name, 
					 const OmnString &value, 
					 const bool addIFnotExist);
	bool	getValue(const OmnString &name, OmnString &value) const;
	OmnString	toString() const;
};
#endif

