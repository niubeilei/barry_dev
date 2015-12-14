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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataSender_DataSenderNorm_h
#define Aos_DataSender_DataSenderNorm_h

#include "DataSender/DataSender.h"


class AosDataSenderNorm : virtual public AosDataSender
{
	OmnDefineRCObject;

private:

public:
	AosDataSenderNorm(const bool regflag);
	AosDataSenderNorm(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosDataSenderNorm();

	// AosDataSender Interface
	virtual bool send(
				const AosBuffPtr &input, 
				const AosRundataPtr &rdata);

	virtual AosDataSenderObjPtr clone(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata);

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif

