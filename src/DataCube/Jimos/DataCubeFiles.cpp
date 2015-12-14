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
// This data cube has a number of files. The files are listed through
// the config. All files belong to the same physical/virtual.
//
// Modification History:
// 2013/12/23 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataCube/Jimos/DataCubeFiles.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/CodeConvertion.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeFiles_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeFiles(version);
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


AosDataCubeFiles::AosDataCubeFiles(const u32 version)
:
AosDataCubeCtnr(AOS_DATACUBETYPE_FILES, version)
{
}

AosDataCubeFiles::~AosDataCubeFiles()
{
}


bool
AosDataCubeFiles::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	// 'worker_doc' serves as the metadata
	// 	<data_cube data_cube_name="xxx" virtual_id="xxx" AOSTAG_CHARSET="xxx">
	// 		<dirs>
	// 			<dir dirname="xxx" AOSTAG_PHYSICALID="xxx">
	// 				file,file,...,file
	// 			</dir>
	// 			...
	// 		<dirs>
	// 	</data_cube>
	
	aos_assert_rr(worker_doc, rdata, false);
	mMetadata = worker_doc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(mMetadata, false);

	mReadBlockSize = mMetadata->getAttrInt64("read_block_size", eDftReadBlockSize);
	mCharset = mMetadata->getAttrStr(AOSTAG_CHARACTER, "UTF8");

	// DDDDDDDDDD need to configure the rest.
	AosXmlTagPtr dirs_xml = worker_doc->getFirstChild("dirs");
	aos_assert_r(dirs_xml, rdata);
	bool rslt = getFileInfo(rdata.getPtr(), dirs_xml);
	aos_assert_r(rslt, false);

	return true;
}


bool	
AosDataCubeFiles::getFileInfo(
		AosRundata *rdata,
		const AosXmlTagPtr &dirs_xml)
{
	aos_assert_r(dirs_xml, rdata);
	AosXmlTagPtr dir_xml = dirs_xml->getFirstChild("dir");
	while (dir_xml)
	{
		OmnString dirname = dir_xml->getAttrStr("dir_name", "");
		if (dirname == "")
		{
			AosSetErrorUser(rdata, "missing_dirname") << dir_xml->toString() << enderr;
			return false;
		}

		int physical_id = dir_xml->getAttrInt(AOSTAG_PHYSICALID, -1);
		if (!AosIsValidPhysicalIdNorm(physical_id))
		{
			AosSetErrorUser(rdata, "invalid_physical_id") << dir_xml->toString() << enderr;
			return false;
		}

		OmnString files = dir_xml->getNodeText().data();
		AosStrSplit split(files, ",");
		vector<OmnString> filenameVector = split.entriesV();
		for (u32 i=0; i<filenameVector.size(); i++)
		{
			OmnString file_name = dirname;
			file_name << "/" << filenameVector[i];
			AosFileInfo file_info;
			bool rslt = AosNetFileCltObj::getFileInfoStatic(
				file_name, physical_id, file_info, rdata);
			if (!rslt)
			{
				AosSetErrorUser(rdata, "error_filename") << dir_xml->toString() << enderr;
				return false;
			}
			
			// Ketty 2014/01/21
			//mFileList.push_back(file_info);
			file_info.mPhysicalId = physical_id;
			file_info.mCharset = mCharset;

			AosDataCubeFilePtr cube_file = OmnNew AosDataCubeFile(file_info, mReadBlockSize, mMetadata);
			cube_file->setTaskDocid(mTaskDocid);
			addDataConnector(cube_file);
//			mFileList.push_back(file_info);
		}
		dir_xml = dirs_xml->getNextChild();
	}
	
	return true;
}


AosDataConnectorObjPtr 
AosDataCubeFiles::cloneDataConnector()
{
	return OmnNew AosDataCubeFiles(*this);
}


AosJimoPtr 
AosDataCubeFiles::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeFiles(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


