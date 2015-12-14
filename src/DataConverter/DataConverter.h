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
// 2013/08/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataConverter_DataConverter_h
#define Aos_DataConverter_DataConverter_h

#include "SEInterfaces/DataConverterObj.h"


class AosDataConverter : public AosDataConverterObj
{
protected:
	OmnString		mConverterType;

public:
	AosDataConverter(const OmnString &type, const OmnString &version);
	~AosDataConverter();

	virtual bool serializeTo(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);

	virtual bool serializeFrom(
						const AosRundataPtr &rdata, 
						const AosBuffPtr &buff);

	virtual bool config(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);
};

#endif
