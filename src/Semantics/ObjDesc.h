////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 12/02/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Semantics_ObjDesc_h
#define Aos_Semantics_ObjDesc_h

#include "aosUtil/Types.h"
#include "Semantics/ObjectType.h"
#include "Semantics/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosObjDesc : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	void		   *mObject;	// The described object
	AosObjectType	mObjType;

public:
	AosObjDesc(void *obj);
	virtual ~AosObjDesc();

	AosObjectType	getObjType() const {return mObjType;}
	u32		getHashKey() const;
	bool	isSameObj(const AosObjDescPtr &rhs) const;

private:
};

#endif

