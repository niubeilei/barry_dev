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
// Modification History:
// 2014/08/19,  Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryFilterObj_h
#define Aos_SEInterfaces_QueryFilterObj_h

#include "SEInterfaces/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosQueryFilterObj : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum FilterType
	{
		eInvalid,

		eKeyField,
		eStrKey,
		eU64Key,
		eValue,

		eMax
	};

public:
	static bool			isValidType(const FilterType type) {return type > eInvalid && type < eMax;}
	static OmnString	EnumToStr(const FilterType type);
	static FilterType	StrToEnum(const OmnString &str);

	virtual AosQueryFilterObjPtr clone() const = 0;
	virtual OmnString	toXmlString() const = 0;

	virtual bool	evalFilter(
						const OmnString &key,
						const u64 &value,
						const AosRundataPtr &rdata) const = 0;
	virtual bool	evalFilter(
						const u64 &key,
						const u64 &value,
						const AosRundataPtr &rdata) const = 0;
};


#endif

