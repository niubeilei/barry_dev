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
#include "DataCube/Jimos/DataCubeIdFiles.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/CodeConvertion.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeIdFiles_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeIdFiles(version);
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


AosDataCubeIdFiles::AosDataCubeIdFiles(const u32 version)
:
AosDataCubeCtnr(AOS_DATACUBETYPE_IDFILES, version)
{
}

AosDataCubeIdFiles::~AosDataCubeIdFiles()
{
}


bool
AosDataCubeIdFiles::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	// 'worker_doc' serves as the metadata
	// 	<data_cube data_cube_name="xxx" virtual_id="xxx" AOSTAG_CHARSET="xxx">
	// 		<files>
	// 			<file zky_storage_fileid="xxx" zky_physicalid="xxx"/>
	// 			...
	// 		<files>
	// 	</data_cube>
	
	aos_assert_rr(worker_doc, rdata, false);
	mMetadata = worker_doc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(mMetadata, false);

	mReadBlockSize = mMetadata->getAttrInt64("read_block_size", eDftReadBlockSize);

	AosXmlTagPtr files_xml = worker_doc->getFirstChild("files");
	aos_assert_r(files_xml, rdata);
	bool rslt = getFileInfo(rdata.getPtr(), files_xml);
	aos_assert_r(rslt, false);
	return true;
}


bool	
AosDataCubeIdFiles::getFileInfo(
		AosRundata *rdata,
		const AosXmlTagPtr &files_xml)
{
	aos_assert_r(files_xml, rdata);
	AosXmlTagPtr file_xml = files_xml->getFirstChild();
	while (file_xml)
	{
		AosFileInfo	info;
		info.serializeFrom(file_xml);

		if (info.mFileBlocks.empty())
		{
			if (info.mFileId == 0)
			{
				AosSetErrorUser(rdata, "missing_fileid") << file_xml->toString() << enderr;
				return false;
			}

			if (!AosIsValidPhysicalIdNorm(info.mPhysicalId))
			{
				AosSetErrorUser(rdata, "invalid_physical_id") << file_xml->toString() << enderr;
				return false;
			}
		}

		AosDataCubeFilePtr cube_file = OmnNew AosDataCubeFile(info, mReadBlockSize, mMetadata);
		cube_file->setTaskDocid(mTaskDocid);
		addDataConnector(cube_file);
		file_xml = files_xml->getNextChild();
	}
	return true;
}


AosDataConnectorObjPtr 
AosDataCubeIdFiles::cloneDataConnector()
{
	return OmnNew AosDataCubeIdFiles(*this);
}


AosJimoPtr 
AosDataCubeIdFiles::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeIdFiles(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}
