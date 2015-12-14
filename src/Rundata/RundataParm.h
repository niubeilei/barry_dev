////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/02/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Rundata_RundataParm_h
#define Aos_Rundata_RundataParm_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosRundataParm :  virtual public OmnRCObject 
{
	OmnDefineRCObject;
public:
	enum Type
	{
		eInvalidType, 

		eIILExecutor,

		eMaxType
	};

private:
	Type				mType;

public:
	AosRundataParm();

	AosRundataParm(const Type type);

	virtual ~AosRundataParm();

	static bool isValid(const Type type)
	{
		return type > eInvalidType && type < eMaxType;
	}
	Type	getType() const {return mType;}
};
#endif

