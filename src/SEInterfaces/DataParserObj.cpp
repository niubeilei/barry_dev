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
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataParserObj.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"

AosDataParserObj::AosDataParserObj(const int version)
:
AosJimo(AosJimoType::eDataParser, version)
{
}


AosDataParserObj::~AosDataParserObj()
{
}

AosDataParserObjPtr
AosDataParserObj::createParserStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc)
{
	if (!worker_doc)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc);
	if (!jimo) return 0;
	if (jimo->getJimoType() != AosJimoType::eDataParser)
	{
		AosLogError(rdata, true, "not_data_parser") 
			<< worker_doc << enderr;
		return 0;
	}

	AosDataParserObjPtr parser = dynamic_cast<AosDataParserObj*>(jimo.getPtr());
	if (!parser)
	{
		AosSetError(rdata, "internal_error") << worker_doc << enderr;
		return 0;
	}

	return parser;
}

