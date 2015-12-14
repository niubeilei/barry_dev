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
//
// Modification History:
// 2013/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataSplitter/Jimos/FilesSplit.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosFilesSplit_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosFilesSplit(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosFilesSplit::AosFilesSplit(const u32 version)
:
AosDataSplitter(AOSDATASPLITTER_FILES, version)
{
}


AosFilesSplit::AosFilesSplit(
		const OmnString &type,
		const u32 version)
:
AosDataSplitter(type, version)
{
}


bool
AosFilesSplit::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	AosXmlTagPtr split_config = worker_doc->getFirstChild("split");
	aos_assert_r(split_config, false);

	AosXmlTagPtr files_xml = worker_doc->getFirstChild("files");
	if (!files_xml) 
	{
		AosSetErrorUser(rdata, "missing_files") << worker_doc->toString() << enderr;
		return false;
	}

	AosXmlTagPtr file_xml = files_xml->getFirstChild();
	while(file_xml)
	{
		AosFileInfo	info;
		info.mFileId = file_xml->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
		if (info.mFileId == 0)
		{
			AosSetErrorUser(rdata, "missing_fileid") << file_xml->toString() << enderr;
			return false;
		}

		info.mPhysicalId = file_xml->getAttrInt(AOSTAG_PHYSICALID, -1);
		if (!AosIsValidPhysicalIdNorm(info.mPhysicalId))
		{
			AosSetErrorUser(rdata, "invalid_physical_id") << worker_doc->toString() << enderr;
			return false;
		}

		mFileList.push_back(info);
		file_xml = files_xml->getNextChild();
	}

	return true;
}


bool
AosFilesSplit::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosFilesSplit::~AosFilesSplit()
{
}


bool 
AosFilesSplit::split(AosRundata *rdata, vector<AosXmlTagPtr> &data_units)
{
	OmnString str = "<dataconnector zky_objid=\"dataconnector_idfiles_jimodoc_v0\" ";
	str << "zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_datacube\" " 
		<< "jimo_type=\"jimo_datacube\" current_version=\"0\" "
		<< "zky_classname=\"AosDataCubeIdFiles\" type=\"idfiles\" ";

	for (u64 i = 0; i < mFileList.size(); i++)
	{
		OmnString cubefileConfig = str;
		cubefileConfig << AOSTAG_PHYSICALID "=\"" << mFileList[i].mPhysicalId << "\" >"
			<< "<files><file "
			<< AOSTAG_PHYSICALID << "=\"" << mFileList[i].mPhysicalId << "\" "
			<< AOSTAG_STORAGE_FILEID << "=\"" << mFileList[i].mFileId << "\" "
			<< "/></files>"
			<< "<versions><ver_0>libDataCubicJimos.so</ver_0>"
			<< "</versions></dataconnector>";

		AosXmlParser parser;                  
		AosXmlTagPtr data_unit = parser.parse(cubefileConfig, "" AosMemoryCheckerArgs); 
		data_units.push_back(data_unit);
	}
	return true;
}


AosJimoPtr 
AosFilesSplit::cloneJimo() const
{
	try
	{
		return OmnNew AosFilesSplit(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}
