////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013-05-21	Create by Young 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "TaskMgr/TaskDataDocIds.h"

#include "API/AosApi.h"
#include "SEInterfaces/DataBlobObj.h"
#include "Util/StrSplit.h"
#include <vector>


AosTaskDataDocIds::AosTaskDataDocIds(const bool flag)
:
AosTaskData(AosTaskDataType::eDocIds, AOSTASKDATA_DOCIDS, flag),
mPhysicalId(-1),
mScannerType(AosDataScannerType::eInvalid)
{
}


AosTaskDataDocIds::AosTaskDataDocIds(
		const vector<u64> &docids, 
		const int physical_id, 
		const OmnString &row_delimiter)
:
AosTaskData(AosTaskDataType::eDocIds, AOSTASKDATA_DOCIDS, false),
mPhysicalId(physical_id),
mRowDelimiter(row_delimiter),
mScannerType(AosDataScannerType::eInvalid)
{
	mDocids = docids;
}


AosTaskDataDocIds::~AosTaskDataDocIds()
{
}


bool
AosTaskDataDocIds::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	/*
	mPhysicalId = def->getAttrInt(AOSTAG_PHYSICALID, -1);
	if (mPhysicalId < 0)
	{
		AosSetErrorU(rdata, "invalid_physical_id:") << def->toString();
		return false;
	}
	*/

	mRowDelimiter = def->getAttrStr(AOSTAG_ROW_DELIMITER);

	mScannerType = AosDataScannerType::eInvalid;
	AosXmlTagPtr scanner_tag = def->getFirstChild(AOSTAG_DATA_SCANNER);
	if (scanner_tag)
	{
		mScannerType = AosDataScannerType::toEnum(scanner_tag->getAttrStr(AOSTAG_TYPE));
	}
	return true;
}


bool
AosTaskDataDocIds::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString scanner_type = AosDataScannerType::toString(mScannerType); 
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< "total =\"" << mDocids.size() << "\" " 
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\" "
		<< "zky_scanner_type" << "=\"" << scanner_type << "\" "
		<< AOSTAG_ROW_DELIMITER << "=\"" << mRowDelimiter << "\">";

	for (u32 i = 0; i<mDocids.size(); i++)
	{
		if (i == mDocids.size() -1)
		{
			docstr << mDocids[i];
		}
		else
		{
			docstr << mDocids[i] << ",";
		}
	}
	docstr	<< "</" << AOSTAG_TASKDATA_TAGNAME << ">";	

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataDocIds::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	//<task_data__n zky_physicalid="xxxx">xxxxx,xxxxx,...</task_data__n>

	mPhysicalId = xml->getAttrInt(AOSTAG_PHYSICALID, -1);
	mRowDelimiter = AosParseRowDelimiter(xml->getAttrStr(AOSTAG_ROW_DELIMITER));
	mScannerType = AosDataScannerType::toEnum(xml->getAttrStr("zky_scanner_type", ""));

	int total = xml->getAttrInt("total", -1);
	aos_assert_r(total >= 0, false);

	OmnString tmpDocids = xml->getNodeText();
	aos_assert_r(tmpDocids != "", false);

	vector<OmnString> strDocids;
	OmnString sep = ",";
	bool finished = false;
	int nn = AosStrSplit::splitStrBySubstr(tmpDocids.data(), sep.data(), strDocids, total, finished);
	aos_assert_r(nn == total, false);

	vector<OmnString>::iterator itr = strDocids.begin();

	for (u32 i=0; i<strDocids.size(); i++)
	{
		mDocids.push_back((*itr).toInt());
		itr++;
	}

	bool rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosTaskDataObjPtr
AosTaskDataDocIds::create() const
{
	return OmnNew AosTaskDataDocIds(false);
}


AosTaskDataObjPtr
AosTaskDataDocIds::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	AosTaskDataDocIds * data = OmnNew AosTaskDataDocIds(false);
	bool rslt = data->config(def, rdata);
	aos_assert_r(rslt, 0);
	return data;
}


AosDataCacherObjPtr 
AosTaskDataDocIds::createDataCacher(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	AosDataScannerObjPtr scanner;
	if (mScannerType == AosDataScannerType::eDoc) 
	{
		scanner = AosDataScannerObj::createDocScannerStatic(mDocids, rdata);
	}
	else
	{
		scanner = AosDataScannerObj::createBinaryDocScannerStatic(mDocids, mPhysicalId, rdata);
	}

	AosDataBlobObjPtr blob;
	if (mRowDelimiter == "")
	{
		blob = AosDataBlobObj::createDataBlobStatic(record, rdata);
	}
	else
	{
		blob = AosDataBlobObj::createDataBlobStatic(mRowDelimiter, rdata);
	}
	aos_assert_r(blob, 0);
	AosDataCacherObjPtr cacher = AosDataCacherObj::createDataCacher(
		scanner, blob, rdata);	
	return cacher;
}


AosDataScannerObjPtr 
AosTaskDataDocIds::createDataScanner(const AosRundataPtr &rdata)
{
	AosDataScannerObjPtr scanner;
	if (mScannerType == AosDataScannerType::eDoc) 
	{
		scanner = AosDataScannerObj::createDocScannerStatic(mDocids, rdata);
	}
	else
	{
		scanner = AosDataScannerObj::createBinaryDocScannerStatic(mDocids, mPhysicalId, rdata);
	}
	return scanner;	
}


bool
AosTaskDataDocIds::isDataExist(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

#endif
