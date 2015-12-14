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
// 09/04/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_UtilHash_HashedObjU64_h
#define AOS_UtilHash_HashedObjU64_h

#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "Alarm/Alarm.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "UtilHash/Ptrs.h"
#include "UtilHash/HashedObj.h"

class AosHashedObjU64 : public AosHashedObj
{
	OmnDefineRCObject;

private:
	u64						mValue;

public:
	AosHashedObjU64();
	AosHashedObjU64(
			const OmnString &key, 
			const u64 &value);
	~AosHashedObjU64(){}

	virtual AosHashedObjPtr clone();
	virtual bool reset();

	virtual bool isSame(const AosHashedObjPtr &rhs) const;

	virtual int doesRecordMatch(
			AosBuff &buff,
			const OmnString &key_in_buff,
			const OmnString &expected_key,
			AosHashedObjPtr &obj);

	virtual int setContentsToBuff(AosBuff &buff);

	u64		getValue() const {return mValue;}
	void 	setValue(u64 &value) {mValue = value;}
};
#endif
