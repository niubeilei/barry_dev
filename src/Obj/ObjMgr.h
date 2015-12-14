////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_ObjMgr_h
#define Omn_Obj_ObjMgr_h

#include "Message/Ptrs.h"
#include "Util/Object.h"
#include "Obj/Ptrs.h"
#include "Porting/LongTypes.h"
#include "Util/String.h"


class OmnObjMgr : virtual public OmnObject
{
public:
	OmnObjMgr(const OmnClassId::E classId)
		:
	OmnObject(classId)
	{
	}

	virtual ~OmnObjMgr() {}
};
#endif
