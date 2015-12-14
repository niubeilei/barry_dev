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
#include "TaskMgr/TaskDataDir.h"

#include "API/AosApi.h"
#include "SEInterfaces/DataBlobObj.h"
#include "Debug/Debug.h"

AosTaskDataDir::AosTaskDataDir(const bool flag)
:
AosTaskData(AosTaskDataType::eDir, AOSTASKDATA_DIR, flag),
mPhysicalId(-1),
mRecursion(true),
mIgnoreHead(false),
mIsFiles(false),
mIsUnicomFile(false)
{
}


AosTaskDataDir::AosTaskDataDir(
		vector<OmnString> &dir_paths,
		const int physical_id,
		const bool recursion,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter)
:
AosTaskData(AosTaskDataType::eDir, AOSTASKDATA_DIR, false),
mPhysicalId(physical_id),
mRecursion(recursion),
mIgnoreHead(ignore_head),
mCharacterType(character_type),
mRowDelimiter(row_delimiter),
mIsFiles(false),
mIsUnicomFile(false)
{
	mDirPaths = dir_paths;
}


AosTaskDataDir::AosTaskDataDir(
		vector<AosFileInfo> &fileinfos,
		const int physical_id,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const bool isUnicomFile)
:
AosTaskData(AosTaskDataType::eDir, AOSTASKDATA_DIR, false),
mPhysicalId(physical_id),
mRecursion(true),
mIgnoreHead(ignore_head),
mCharacterType(character_type),
mRowDelimiter(row_delimiter),
mIsFiles(true),
mIsUnicomFile(isUnicomFile)
{
	mFileInfos = fileinfos;
}


AosTaskDataDir::~AosTaskDataDir()
{
}


bool
AosTaskDataDir::config(
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

	mDirPaths.clear();
	OmnString dir_path = def->getNodeText();
	if(dir_path != "")
	{
		mDirPaths.push_back(dir_path);
	}
	else
	{
		AosXmlTagPtr tag = def->getFirstChild(true);
		while(tag)
		{
			dir_path = tag->getNodeText();
			if(dir_path != "") mDirPaths.push_back(dir_path);
			tag = def->getNextChild();
		}
	}
	
	if (mDirPaths.size() <= 0)
	{
		AosSetErrorU(rdata, "missing_dir_path:") << def->toString();
		return false;
	}

	mPhysicalId = def->getAttrInt(AOSTAG_PHYSICALID, -1);
	mRecursion = def->getAttrBool(AOSTAG_RECURSION, true);
	mIgnoreHead = def->getAttrBool(AOSTAG_IGNORE_HEAD, false);
	mRowDelimiter = def->getAttrStr(AOSTAG_ROW_DELIMITER);

	mCharacterType = def->getAttrStr(AOSTAG_CHARACTER, "UTF8");
	aos_assert_r(CodeConvertion::isValid(mCharacterType), false);

	return true;
}


bool
AosTaskDataDir::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\" "
		<< AOSTAG_RECURSION << "=\"" << mRecursion << "\" "
		<< AOSTAG_IGNORE_HEAD << "=\"" << mIgnoreHead << "\" "
		<< AOSTAG_CHARACTER << "=\"" << mCharacterType << "\" "
		<< AOSTAG_ROW_DELIMITER << "=\"" << mRowDelimiter << "\" "
		<< AOSTAG_ISFILES << "=\"" << mIsFiles << "\" "
		<< AOSTAG_ISUNICOMFILE << "=\"" << mIsUnicomFile << "\">";

	if (!mIsFiles)
	{
		docstr << "<dirs>";
		for(u32 i=0; i<mDirPaths.size(); i++)
		{
			docstr << "<dir><![CDATA[" << mDirPaths[i] << "]]></dir>";	
		}
		docstr << "</dirs>";
	}
	else
	{
		docstr << "<files>";
		OmnString str;
		for(u32 i=0; i<mFileInfos.size(); i++)
		{
			str = mFileInfos[i].serializeToXmlStr();
			aos_assert_r(str != "", false);
			docstr << str; 	
		}
		docstr << "</files>";
	}
	docstr << "</" << AOSTAG_TASKDATA_TAGNAME << ">";

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataDir::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	mPhysicalId = xml->getAttrInt(AOSTAG_PHYSICALID, -1);
	mRecursion = xml->getAttrBool(AOSTAG_RECURSION, true);
	mIgnoreHead = xml->getAttrBool(AOSTAG_IGNORE_HEAD, false);
	mCharacterType = xml->getAttrStr(AOSTAG_CHARACTER, "UTF8");
	mRowDelimiter = AosParseRowDelimiter(xml->getAttrStr(AOSTAG_ROW_DELIMITER));
	mIsFiles = xml->getAttrBool(AOSTAG_ISFILES, false);
	mIsUnicomFile = xml->getAttrBool(AOSTAG_ISUNICOMFILE, false);

	bool rslt = true;
	if (!mIsFiles)
	{
		mDirPaths.clear();
		OmnString dir_path;

		AosXmlTagPtr dirs = xml->getFirstChild("dirs");
		aos_assert_r(dirs, false);
		
		AosXmlTagPtr entry = dirs->getFirstChild(true);
		while(entry)
		{
			dir_path = entry->getNodeText();
			aos_assert_r(dir_path != "", false);
			mDirPaths.push_back(dir_path);
			entry = dirs->getNextChild();
		}
	}
	else
	{
		mFileInfos.clear();
		AosXmlTagPtr files = xml->getFirstChild("files");	
		aos_assert_r(files, false);
		
		AosXmlTagPtr entry = files->getFirstChild(true);
		while(entry)
		{
			AosFileInfo info;
			rslt = info.serializeFrom(entry);
			aos_assert_r(rslt, false);
			mFileInfos.push_back(info);
			entry = files->getNextChild();
		}
	}
	
	rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosTaskDataObjPtr
AosTaskDataDir::create() const
{
	return OmnNew AosTaskDataDir(false);
}


AosTaskDataObjPtr
AosTaskDataDir::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	AosTaskDataDir * data = OmnNew AosTaskDataDir(false);
	bool rslt = data->config(def, rdata);
	OmnTagFuncInfo << endl;
	aos_assert_r(rslt, 0);
	return data;
}


AosDataCacherObjPtr 
AosTaskDataDir::createDataCacher(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	AosDataScannerObjPtr scanner;
	OmnTagFuncInfo << endl;
	if (!mIsFiles)
	{
		scanner	= AosDataScannerObj::createDirScannerStatic(
			mDirPaths, mPhysicalId, mRecursion, mIgnoreHead,
			mCharacterType, mRowDelimiter, rdata);
	}
	else
	{
		scanner	= AosDataScannerObj::createDirScannerStatic(
			mFileInfos, mPhysicalId, mIgnoreHead,
			mCharacterType, mRowDelimiter, rdata);
	}

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
AosTaskDataDir::createDataScanner(const AosRundataPtr &rdata)
{
	AosDataScannerObjPtr scanner;
	OmnTagFuncInfo << endl;
	if (!mIsFiles)
	{
		scanner	= AosDataScannerObj::createDirScannerStatic(
			mDirPaths, mPhysicalId, mRecursion, mIgnoreHead,
			mCharacterType, mRowDelimiter, rdata);
	}
	else
	{
		if (mIsUnicomFile)
		{
			scanner = AosDataScannerObj::createUnicomDirScannerStatic(
				mFileInfos, mPhysicalId, mCharacterType, mRowDelimiter, rdata);
		}
		else
		{
			scanner	= AosDataScannerObj::createDirScannerStatic(
				mFileInfos, mPhysicalId, mIgnoreHead,
				mCharacterType, mRowDelimiter, rdata);
		}
	}
	return scanner;
}


bool
AosTaskDataDir::isDataExist(const AosRundataPtr &rdata)
{
	bool rslt = false;
	for(u32 i=0; i< mDirPaths.size(); i++)
	{
		rslt = AosNetFileCltObj::dirIsGoodStatic(
			mDirPaths[i], mPhysicalId, true, rdata.getPtr());
		if(rslt) return true;
	}
	return false;
}

#endif
