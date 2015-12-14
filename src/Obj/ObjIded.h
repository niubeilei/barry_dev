////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjIded.h
// Description:
//	This is an object that has an unique OmnObjId. Object IDs are grouped
//  by classes. Object IDs are guaranteed unique for the same class but
//  they may be duplicated for different classes.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_ObjSeqKeyed_h
#define Omn_Obj_ObjSeqKeyed_h

#include "Obj/Obj.h"


class OmnSeqKeyedObj : virtual public OmnObj
{
private:
	int		mKey;

public:
	OmnSeqKeyedObj();
	virtual ~OmnSeqKeyedObj() {}

	int		getKey() const {return mKey;}
};
#endif
