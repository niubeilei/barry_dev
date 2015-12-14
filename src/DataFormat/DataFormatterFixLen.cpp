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
#include "DataFormat/DataFormatterFixLen.h"

#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosDataFormatterFixLen::AosDataFormatterFixLen(
		const AosXmlTagPtr &formatter, 
		const AosRundataPtr &rdata)
:
AosDataFormatter(eAosDataFormatType_FixLen, formatter, rdata)
{
}


AosDataFormatterFixLen::~AosDataFormatterFixLen()
{
}


AosXmlTagPtr 
AosDataFormatterFixLen::serializeToXmlDoc(
		const char *data,
		const int data_len,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}


AosBuffPtr 
AosDataFormatterFixLen::serializeToBuff(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


