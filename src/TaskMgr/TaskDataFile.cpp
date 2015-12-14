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
// 05/14/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "TaskMgr/TaskDataFile.h"

#include "API/AosApi.h"
#include "SEInterfaces/DataBlobObj.h"
#include "Debug/Debug.h"


AosTaskDataFile::AosTaskDataFile(const bool flag)
:
AosTaskData(AosTaskDataType::eFile, AOSTASKDATA_FILE, flag),
mPhysicalId(-1),
mIgnoreHead(false)
{
}


AosTaskDataFile::AosTaskDataFile(
		const OmnString &file_name,
		const int physical_id,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter)
:
AosTaskData(AosTaskDataType::eFile, AOSTASKDATA_FILE, false),
mFileName(file_name),
mPhysicalId(physical_id),
mIgnoreHead(ignore_head),
mCharacterType(character_type),
mRowDelimiter(row_delimiter)
{
}


AosTaskDataFile::~AosTaskDataFile()
{
}


bool
AosTaskDataFile::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << endl;

	bool rslt = AosTaskData::config(def, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "config_error:") << def->toString();
		return false;
	}

	mFileName = def->getNodeText();
	if (mFileName == "")
	{
		AosSetErrorU(rdata, "missing_file_name:") << def->toString();
		return false;
	}

	mPhysicalId = def->getAttrInt(AOSTAG_PHYSICALID, -1);
	if (mPhysicalId < 0)
	{
		AosSetErrorU(rdata, "invalid_physical_id:") << def->toString();
		return false;
	}

	mIgnoreHead = def->getAttrBool(AOSTAG_IGNORE_HEAD, false);
	mRowDelimiter = def->getAttrStr(AOSTAG_ROW_DELIMITER);

	mCharacterType = def->getAttrStr(AOSTAG_CHARACTER, "UTF8");
	aos_assert_r(CodeConvertion::isValid(mCharacterType), false);

	//JACKIE-HADOOP
	mIsHadoop =  def->getAttrBool("ishadoop", false);

	return true;
}


bool
AosTaskDataFile::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\" "
		<< AOSTAG_IGNORE_HEAD << "=\"" << mIgnoreHead << "\" "
		<< AOSTAG_CHARACTER << "=\"" << mCharacterType << "\" "
		//JACKIE-HADOOP
		<< "ishadoop" << "=\"" << mIsHadoop << "\" "
		<< AOSTAG_ROW_DELIMITER << "=\"" << mRowDelimiter << "\">"
		<< "<![CDATA[" << mFileName << "]]></" << AOSTAG_TASKDATA_TAGNAME << ">";	

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataFile::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	mPhysicalId = xml->getAttrInt(AOSTAG_PHYSICALID, -1);
	mFileName = xml->getNodeText();
	aos_assert_r(mFileName != "", false);

	mIgnoreHead = xml->getAttrBool(AOSTAG_IGNORE_HEAD, false);
	mCharacterType = xml->getAttrStr(AOSTAG_CHARACTER, "UTF8");
	//JACKIE-HADOOP
	mIsHadoop = xml->getAttrBool("ishadoop", false);
	mRowDelimiter = AosParseRowDelimiter(xml->getAttrStr(AOSTAG_ROW_DELIMITER));

	bool rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosTaskDataObjPtr
AosTaskDataFile::create() const
{
	OmnTagFuncInfo << endl;
	return OmnNew AosTaskDataFile(false);
}


AosTaskDataObjPtr
AosTaskDataFile::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	OmnTagFuncInfo << endl;
	AosTaskDataFile * data = OmnNew AosTaskDataFile(false);
	bool rslt = data->config(def, rdata);
	aos_assert_r(rslt, 0);
	return data;
}


AosDataCacherObjPtr 
AosTaskDataFile::createDataCacher(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << endl;
	aos_assert_r(record, 0);

	//JACKIE-HADOOP
	AosDataScannerObjPtr scanner = AosDataScannerObj::createFileScannerStatic(
		mFileName, mPhysicalId, mIgnoreHead, mCharacterType,
		mRowDelimiter, record->getRecordLen(), mIsHadoop, rdata);
	aos_assert_r(scanner, 0);
	
	AosDataBlobObjPtr blob;
	if (mRowDelimiter == "")
	{
		blob= AosDataBlobObj::createDataBlobStatic(record, rdata);
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
AosTaskDataFile::createDataScanner(const AosRundataPtr &rdata)
{
	//JACKIE-HADOOP
	OmnTagFuncInfo << endl;
	AosDataScannerObjPtr scanner = AosDataScannerObj::createFileScannerStatic(
		mFileName, mPhysicalId, mIgnoreHead,
		mCharacterType, mRowDelimiter, 0, mIsHadoop, rdata);
	return scanner;
}


bool
AosTaskDataFile::isDataExist(const AosRundataPtr &rdata)
{
	return AosNetFileCltObj::fileIsGoodStatic(
		mFileName, mPhysicalId, rdata.getPtr());
}

#endif
