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
// 2013/11/08 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataReaderObj_h
#define Aos_SEInterfaces_DataReaderObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosDataReaderObj : virtual public AosJimo 
{
public:
	AosDataReaderObj();
	AosDataReaderObj(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc, 
			const AosXmlTagPtr &jimo_doc);
	~AosDataReaderObj();

	virtual bool readBlock(
					const AosRundataPtr &rdata, 
					const AosBuffDataPtr &buff_data,
					const int64_t start, 
					const int64_t len) = 0;

	virtual bool readNextBlock(
					const AosRundataPtr &rdata, 
					const AosBuffDataPtr &buff_data) = 0;

	static AosDataReaderObjPtr createDataReader(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc, 
					const AosXmlTagPtr &jimo_doc);
};
#endif
