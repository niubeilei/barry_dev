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
#ifndef Omn_UtilTime_TFEpochTime_h
#define Omn_UtilTime_TFEpochTime_h

#include "UtilTime/TimeFormat.h"

class AosTFEpochTime : public AosTimeFormat
{
public:
	AosTFEpochTime(const bool regflag);
	~AosTFEpochTime();

	virtual int getDataLen() const {return 14;}
	
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
	//		const u64 &day,
	//		const AosValueRslt &value, 
	//		const AosRundataPtr &rdata) const;
};
#endif

