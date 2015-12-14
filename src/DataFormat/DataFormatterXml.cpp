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
#include "DataFormat/DataFormatterXml.h"

#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosDataFormatterXml::AosDataFormatterXml(
		const AosXmlTagPtr &formatter, 
		const AosRundataPtr &rdata)
:
AosDataFormatter(eAosDataFormatType_Xml, formatter, rdata)
{
}


AosDataFormatterXml::~AosDataFormatterXml()
{
}


AosXmlTagPtr
AosDataFormatterXml::serializeToXmlDoc(
		const char *data, 
		const int data_len,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << mName << ">";
	int idx = 0;
	for (u32 i=0; i<mFields.size(); i++)
	{
		bool rslt = mFields[i]->serializeToXmlDoc(docstr, idx, data, data_len, rdata.getPtr());
		aos_assert_r(rslt, 0);
	}
	
	docstr << "</" << mName << ">";
	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	return doc;
}


AosBuffPtr
AosDataFormatterXml::serializeToBuff(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(doc, rdata, 0);
		
	//AosBuffPtr buff = OmnNew AosBuff(eBuffInitSize, eBuffIncSize AosMemoryCheckerArgs);
	//felicia, 2012/09/26
	AosBuffPtr buff = OmnNew AosBuff(eBuffInitSize AosMemoryCheckerArgs);
	for (u32 i=0; i<mFields.size(); i++)
	{
		bool rslt = mFields[i]->serializeToBuff(doc, buff, rdata.getPtrNoLock());
		aos_assert_r(rslt, 0);
	}
	return buff;	
}


const char * 
AosDataFormatterXml::getCharStr(
		const char *record, 
		const int idx, 
		const char * const dft) const
{
	OmnNotImplementedYet;
	return 0;
}


u64 
AosDataFormatterXml::getU64(const char *record, const int idx, const u64 &dft) const
{
	OmnNotImplementedYet;
	return 0;
}


u32 
AosDataFormatterXml::getU32(const char *record, const int idx, const u32 dft) const
{
	OmnNotImplementedYet;
	return 0;
}


char
AosDataFormatterXml::getChar(const char *record, const int idx, const char dft) const
{
	OmnNotImplementedYet;
	return 0;
}


