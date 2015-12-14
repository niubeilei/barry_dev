////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjSeqKeyed.h
// Description:
//	This is an object that has a sequentially generated integer key.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_ObjSeqKeyed_h
#define Omn_Obj_ObjSeqKeyed_h

#include "Util/Object.h"


class OmnSeqKeyedObj : virtual public OmnObject
{
private:
	int		mKey;

public:
	OmnSeqKeyedObj(const OmnClassId::E classId);
	virtual ~OmnSeqKeyedObj() {}

	int		getKey() const {return mKey;}
};
#endif
