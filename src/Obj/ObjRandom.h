////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjRandom.h
// Description:
//	This is an object that has a randomly generated integer. Note that
//  there is no guarantee that the random numbers in different instances
//  are unique. The random number shall not be used as a key.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_ObjRandom_h
#define Omn_Obj_ObjRandom_h

#include "Util/Object.h"


class OmnObjRandom : virtual public OmnObject
{
private:
	int		mRandom;

public:
	OmnObjRandom(const OmnClassId::E classId);
	virtual ~OmnObjRandom() {}

	int		getRandom() const {return mRandom;}
};
#endif
