////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2013/11/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataReader_DataReaderDummy_h
#define AOS_DataReader_DataReaderDummy_h

#include "DataReader/DataReader.h"

class AosDataReaderDummy : virtual public AosDataReader
{
	OmnDefineRCObject;

public:
	AosDataReaderDummy(const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc);
	~AosDataReaderDummy();

	virtual bool readBlock(
					const AosRundataPtr &rdata, 
					const int64_t start, 
					const int64_t len,
					AosBuffPtr &buff);

	virtual bool readNextBlock(
					const AosRundataPtr &rdata, 
					AosBuffPtr &buff);
};
#endif

