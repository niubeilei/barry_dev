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
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataFormat_DataFormatterRaw_h
#define Aos_DataFormat_DataFormatterRaw_h

#include "DataFormat/DataFormatter.h"


class AosDataFormatterRaw : virtual public AosDataFormatter
{

public:
	AosDataFormatterRaw(
			const AosXmlTagPtr &formatter,
			const AosRundataPtr &rdata);
	~AosDataFormatterRaw();

	virtual AosXmlTagPtr serializeToXmlDoc(
			                const char *data,
							const int data_len,
							const AosRundataPtr &rdata);

	virtual AosBuffPtr serializeToBuff(
			                const AosXmlTagPtr &doc,
							const AosRundataPtr &rdata);
};

#endif

