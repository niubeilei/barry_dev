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
// 10/09/2013	Created by Jackie
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocBatchGetIILDocids.h"

#include "Debug/Error.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "Security/Session.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"
#include "XmlInterface/WebRequest.h"
#include "SEInterfaces/IILClientObj.h"


AosSdocBatchGetIILDocids::AosSdocBatchGetIILDocids(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_BATCHGETIILDOCIDS, AosSdocId::eBatchGetIILDocids, flag)
{
}


AosSdocBatchGetIILDocids::~AosSdocBatchGetIILDocids()
{
}


bool
AosSdocBatchGetIILDocids::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This smartdoc retrieves a vector of docids.
	AosBuffPtr column = rdata->getColumn();
	aos_assert_rr(column, rdata, false);
	OmnString iilname = sdoc->getAttrStr("iilname");
	aos_assert_rr(iilname != "", rdata, false);
	AosIILClientObj::getIILClient()->getBatchDocids(rdata, column, iilname);
	rdata->setArg1("is_async_call", 1);
	return true;
}

