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
// 04/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataFormat_DataFormatterFixLen_h
#define Aos_DataFormat_DataFormatterFixLen_h

#include "DataFormat/DataFormatter.h"


class AosDataFormatterFixLen : virtual public AosDataFormatter
{

public:
	AosDataFormatterFixLen(
			const AosXmlTagPtr &formatter,
			const AosRundataPtr &rdata);
	~AosDataFormatterFixLen();

	virtual AosXmlTagPtr serializeToXmlDoc(
			                const char *data,
							const int data_len,
							const AosRundataPtr &rdata);

	virtual AosBuffPtr serializeToBuff(
			                const AosXmlTagPtr &doc,
							const AosRundataPtr &rdata);
};

#endif

