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
#if 0
#include "SEInterfaces/DataReaderObj.h"


#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

AosDataReaderObj::AosDataReaderObj(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
:
AosJimo(rdata, worker_doc, jimo_doc, AOS_JIMOTYPE_DATA_READER)
{
}


AosDataReaderObj::~AosDataReaderObj()
{
}


AosDataReaderObjPtr 
AosDataReaderObj::createDataReader(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc, jimo_doc);
	if (!jimo) return 0;

	AosDataReaderObjPtr reader = dynamic_cast<AosDataReaderObj*>(jimo.getPtr());
	aos_assert_rr(reader, rdata, 0);
	return reader;
}

#endif
