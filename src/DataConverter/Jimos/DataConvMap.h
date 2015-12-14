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
// 2013/11/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataConverter_Jimos_DataConvMap_h
#define Aos_DataConverter_Jimos_DataConvMap_h

#include "DataConverter/DataConverter.h"

class AosDataConvMap : public AosDataConverter
{
	OmnDefineRCObject;

private:

public:
	AosDataConvMap(const OmnString &version);
	~AosDataConvMap();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &worker_doc,
				const AosXmlTagPtr &jimo_doc);

	virtual bool run(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &data);

	virtual bool run(
					const AosRundataPtr &rdata,
					const OmnString &data);
};
#endif

