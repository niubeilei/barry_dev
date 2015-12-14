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
// 2013/07/25 Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_KeyValuePair_h
#define AOS_Util_KeyValuePair_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include <map>


class AosKeyValuePair : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	OmnString 					mStrLine;
	map<OmnString, OmnString> 	mKVMap; 			

public:
	AosKeyValuePair(AosRundata *rdata,
					const OmnString &contents);
	AosKeyValuePair(const OmnString pline);
	~AosKeyValuePair();

	u32 		getAttrU32(const OmnString &key, const u64 &dft);
	u64 		getAttrU64(const OmnString &key, const u64 &dft);
	OmnString	getAttrStr(const OmnString &key, const OmnString &dft = ""); 
	
private:
	OmnString 	replaceStrWithSpace(OmnString str);
	bool 		createPairs(
					AosRundata *rdata, 
					const OmnString &contents);
	OmnString 	getKey(OmnString pline);
	OmnString 	getValue(OmnString pline);
};
#endif
