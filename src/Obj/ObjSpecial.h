////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjSpecial.h
// Description:
//	This is the super class for all objects, including programming 
//  objects, data centric objects, system objects, etc.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_ObjSpecial_h
#define Omn_Obj_ObjSpecial_h

/*
#include "Obj/Obj.h"

class OmnInvalidObj : public OmnObject
{
private:

public:
	OmnInvalidObj();
	virtual ~OmnInvalidObj();

	virtual OmnString		toString() const {return "Null Object";}
	virtual OmnObjType::E	getObjType() const {return OmnObjType::eInvalidObj;}
	virtual OmnObjId		getObjId() const {return OmnObjId::getInvalidObjId();}
};
*/


/*
class OmnSpecialObj
{
private:
	static OmnNullObj		mNullObj;

public:
	static OmnInvalidObjPtr	getInvalidObj() {return &mNullObj;}
};
*/
#endif
