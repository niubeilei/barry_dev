////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MgdDbObj.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_MgdDbObj_h
#define Omn_Obj_MgdDbObj_h

#include "Obj/MngdObj.h"
#include "Obj/ObjDb.h"


class OmnMgdDbObj : public OmnMngdObj,
					public OmnDbObj
{
public:
	OmnMgdDbObj(const OmnClassId::E classId)
	:
	OmnObject(classId),
	OmnMngdObj(classId),
	OmnDbObj(classId)
	{
	}

	virtual OmnMgdDbObjPtr	clone() const = 0;
	virtual OmnMgdDbObjPtr	getNotInDbObj() const = 0;
	virtual bool			hasSameObjId(const OmnMgdDbObjPtr &rhs) const = 0;
	virtual int				getHashKey() const = 0;
};
#endif
