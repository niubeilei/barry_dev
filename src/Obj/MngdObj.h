////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MngdObj.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_MngdObj_h
#define Omn_Obj_MngdObj_h

#include "aosUtil/Types.h"
#include "Message/Ptrs.h"
#include "Util/Object.h"
#include "Obj/Ptrs.h"
#include "Porting/LongTypes.h"
#include "Util/String.h"


class OmnMngdObj : virtual public OmnObject
{
private:
	int64_t		mObjId;

public:
	OmnMngdObj(const OmnClassId::E classId);
	virtual ~OmnMngdObj();

	virtual bool		isTTLExpired() const = 0;

	int64_t	getObjId() const {return mObjId;}
	int64_t	getIndex() const {return mObjId;}
};
#endif
