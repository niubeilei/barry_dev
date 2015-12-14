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
#ifndef Omn_UtilTime_TFYYMMDD_h
#define Omn_UtilTime_TFYYMMDD_h

#include "UtilTime/TimeFormat.h"

class AosTFYYMMDD : public AosTimeFormat
{
public:
	AosTFYYMMDD(const bool regflag);
	~AosTFYYMMDD();
	
	virtual int getDataLen() const {return 6;}
	virtual int str2EpochDay(const char *data, const int len) const;
	virtual int str2EpochHour(const char *data, const int len) const;
	virtual int str2EpochMonth(const char *data, const int len) const;
	virtual i64 str2EpochTime(const char *data, const int len) const;

	virtual bool convert(
					const char *data, 
					const int len,
					const E target_format,
					AosValueRslt &value, 
					const AosRundataPtr &rdata) const;

	//virtual bool check(const AosRundataPtr &rdata) const;
private:
	//bool checkRslt(
	//		const E &format,
	//		const OmnString &digitstr,
	//		const AosValueRslt &value, 
	//		const AosRundataPtr &rdata) const;
};
#endif

