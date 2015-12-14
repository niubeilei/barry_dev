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
// 2013/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataReader_Jimos_DataReaderFile_h
#define AOS_DataReader_Jimos_DataReaderFile_h

#include "DataReader/DataReader.h"

class AosDataReaderFile : public AosDataReader 
{
	OmnDefineRCObject;

private:

public:
	AosDataReaderFile(const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc, 
			const AosXmlTagPtr &jimo_doc);
	~AosDataReaderFile();

private:
	bool config( 		const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);

	virtual bool readNextBlock(
					const AosRundataPtr &rdata, 
					const int64_t start, 
					const int64_t len,
					AosBuffPtr &buff);

	virtual bool readNextBlock(
					const AosRundataPtr &rdata, 
					AosBuffPtr &buff);
};
#endif

