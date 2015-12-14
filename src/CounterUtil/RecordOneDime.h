////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 08/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_CounterUtil_RecordOneDime_h
#define AOS_CounterUtil_RecordOneDime_h

#include "CounterUtil/RecordFormat.h"

class AosOneDime : public AosRecordFormat 
{
public:
	AosOneDime();
	AosOneDime(const AosXmlTagPtr &term, const AosRundataPtr &rdata);
	~AosOneDime();

	virtual bool addValue(
		const OmnString &counterid,
		const AosStatType::E stattype,
		const OmnString &cname, 
		const u64 &docid, 
		const AosRundataPtr &rdata);

	virtual OmnString createRecord(i64 &index, const AosRundataPtr &rdata);
};
#endif

