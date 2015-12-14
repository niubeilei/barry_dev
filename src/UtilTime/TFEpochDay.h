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
// 08/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilTime_TFEpochDay_h
#define Omn_UtilTime_TFEpochDay_h

#include "UtilTime/TimeFormat.h"

class AosTFEpochDay : public AosTimeFormat
{
public:
	AosTFEpochDay(const bool regflag);
	~AosTFEpochDay();

	virtual int getDataLen() const {return sizeof(u32);}

	virtual bool convert(
					const char *data, 
					const int len,
					const E target_format,
					AosValueRslt &value, 
					const AosRundataPtr &rdata) const;

	//virtual bool check(const AosRundataPtr &rdata) const;
private:

//	bool checkRslt(
//			const E &format,
//			const u64 &day,
//			const AosValueRslt &value, 
//			const AosRundataPtr &rdata) const;
};
#endif

