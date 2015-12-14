////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This data scanner uses a data reader to read data. A data reader 
// may be defined by a doc. 
//
// 2013/11/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataScanner/GenericScanner.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/UtUtil.h"
#include "XmlUtil/SeXmlParser.h"



AosGenericScanner::AosGenericScanner(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc) 
:
AosDataScanner(AOSDATASCANNER_FILE, AosDataScannerType::eFile, false)
{
	bool rslt = config(conf, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


bool
AosGenericScanner::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return true;
}


AosGenericScanner::~AosGenericScanner()
{
}


AosDataScannerObjPtr
AosGenericScanner::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosGenericScanner(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}


bool
AosGenericScanner::getNextBlock(
		const AosRundataPtr &rdata,
		const AosBuffMetaDataPtr &buff_data)
{
	aos_assert_rr(buff_data, rdata, false);
	AosBuffPtr buff;
	bool rslt = getNextBlock(buff, rdata);
	aos_assert_r(rslt, false);
	buff_data->setBuff(buff);
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse("<metaData/>", "" AosMemoryCheckerArgs);
	xml->setAttr(AOSTAG_SOURCE_FNAME, mFileName);
	xml->setAttr(AOSTAG_SOURCE_LENGTH, mLength);
	int idx = mFileName.indexOf(0, '/', true);
	aos_assert_r(idx >= 0, false);
	OmnString name = mFileName.substr(idx + 1);
	xml->setAttr(AOSTAG_SOURCE_NAME, name);
	OmnString path = mFileName.substr(0, idx - 1);
	xml->setAttr(AOSTAG_SOURCE_PATH, path);
	buff_data->setXmlDoc(xml);
	return true;
}


bool
AosGenericScanner::getNextBlock(
		const AosRundataPtr &rdata,
		AosBuffPtr &buff)
{
	bool rslt = false;
	if (mRowDelimiter == "")
	{
		rslt = readFixedLengthToBuff(buff, rdata);

	}
	else
	{
		rslt = readVarLengthToBuff(buff, rdata);
	}
	if (!buff || buff->dataLen() <= 0)
	{
		return true;
	}
	if (mCharacterType != CodeConvertion::mDefaultType)
	{
		int64_t len = buff->dataLen() * 2;
		AosBuffPtr newbuff = OmnNew AosBuff(len AosMemoryCheckerArgs);
		OmnString to_type = CodeConvertion::mDefaultType;
		int64_t newlen = CodeConvertion::convert(mCharacterType.data(), to_type.data(),
				buff->data(), buff->dataLen(), newbuff->data(), len);
		aos_assert_r(newlen >= 0, false);
		newbuff->setDataLen(newlen);
		buff = newbuff;
	}
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosGenericScanner::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	return false;
}


bool 
AosGenericScanner::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	return false;
}
#endif
