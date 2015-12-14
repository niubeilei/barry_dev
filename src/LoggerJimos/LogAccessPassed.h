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
// 2013/12/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_LoggerJimos_LogAccessPassed_h
#define Aos_LoggerJimos_LogAccessPassed_h

#include "LoggerJimos/BuffLogger.h"
#include "Util/Ptrs.h"


class AosLogAccessPassed : public AosBuffLogger
{
private:
	enum
	{
		eNumFields = 8
	};

	AosBuffPtr		mBuff;

	static u64		smSchemaID;
	static bool		smLogEnabled;


public:
	AosLogAccessPassed(const OmnString &version);
	~AosLogAccessPassed();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// AosDataRecord interface
	virtual bool 	setData(
						char *data, 
						const int len, 
						const AosBuffDataPtr &metaData, 
						const int64_t offset,
						const bool need_copy);

	virtual bool createLog(const AosRundataPtr &rdata,
						AosSecReq &sec_req,
						const AosXmlTagPtr &access_tag);

	virtual bool	parseData(const AosRundataPtr &rdata);

	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif

