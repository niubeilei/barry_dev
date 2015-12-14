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
#ifndef Aos_DataFormat_DataFormatterXml_h
#define Aos_DataFormat_DataFormatterXml_h

#include "DataFormat/DataFormatter.h"

class AosDataFormatterXml : virtual public AosDataFormatter
{
	enum
	{
		eBuffInitSize = 500,
		eBuffIncSize = 500
	};

public:
	AosDataFormatterXml(
			const AosXmlTagPtr &formatter,
			const AosRundataPtr &rdata);
	~AosDataFormatterXml();

	virtual AosXmlTagPtr	serializeToXmlDoc(
								const char *data, 
								const int data_len,
								const AosRundataPtr &rdata);

	virtual AosBuffPtr		serializeToBuff(
								const AosXmlTagPtr &doc,
								const AosRundataPtr &rdata);

	// Chen Ding, 04/29/2012
	virtual const char * getCharStr(
					const char * const record, 
					const int idx, 
					const char * const dft) const;
	virtual u64 getU64(const char *record, const int idx, const u64 &dft) const;
	virtual u32 getU32(const char *record, const int idx, const u32 dft) const;
	virtual char getChar(const char *record, const int idx, const char dft) const;
};

#endif

