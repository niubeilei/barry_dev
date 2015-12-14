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
// 02/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ValueMapObj_h
#define Aos_SEInterfaces_ValueMapObj_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosValueMapObj : virtual public OmnRCObject
{
private:
	static AosValueMapObjPtr smValueMap;

public:
	virtual bool getMappedValue(
					const u64 &iilid,
					const int64_t &key, 
					int64_t &value,
					AosRundata *rdata) = 0;

	static AosValueMapObjPtr getValueMap() {return smValueMap;}
	static void setValueMap(const AosValueMapObjPtr &d) {smValueMap = d;}
};

inline bool AosGetMappedValue(
					const u64 &iilid,
					const int64_t &key, 
					int64_t &value,
					AosRundata *rdata)
{
	AosValueMapObjPtr valuemap = AosValueMapObj::getValueMap();
	aos_assert_r(valuemap, false);
	return valuemap->getMappedValue(iilid, key, value, rdata);
}
#endif

