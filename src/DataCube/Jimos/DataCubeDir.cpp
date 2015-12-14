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
// This data cube manages multiple files. Files are specified by a directory.
// If recursive is true, all files in all its sub-directories are selected.
// If a file extension is specified, only the files that match the extension
// are selected.
//
// Modification History:
// 2013/12/23 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "DataCube/Jimos/DataCubeDir.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/CodeConvertion.h"
extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeDir_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeDir(version);
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


AosDataCubeDir::AosDataCubeDir(const u32 version)
:
AosDataCubeCtnr(AOS_DATACUBETYPE_DIR, version)
{
}

AosDataCubeDir::~AosDataCubeDir()
{
}


bool
AosDataCubeDir::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	// 'worker_doc' serves as the metadata
	//<data_cubedir  read_block_size="xxx" zky_character="xxx" >
	//	<dirs>
	//		<dir AOSTAG_PHYSICALID="xxx" dirname="xxxx">
	//		<dir AOSTAG_PHYSICALID="xxx" dirname="xxxx">
	//		<dir AOSTAG_PHYSICALID="xxx" dirname="xxxx">
	//	</dirs>
	//</data_cubedir>
	
	aos_assert_rr(worker_doc, rdata, false);
	mMetadata = worker_doc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(mMetadata, false);

	mReadBlockSize = mMetadata->getAttrInt64("read_block_size", eDftReadBlockSize);
	mCharset = mMetadata->getAttrStr(AOSTAG_CHARACTER, "UTF8");
	
	AosXmlTagPtr dirs_xml = worker_doc->getFirstChild("dirs");
	aos_assert_r(dirs_xml, false);
	AosXmlTagPtr dir_xml = dirs_xml->getFirstChild("dir");
	while (dir_xml)
	{
		int physical_id = dir_xml->getAttrInt(AOSTAG_PHYSICALID, -1);
		if (!AosIsValidPhysicalIdNorm(physical_id))
		{
			AosSetErrorUser(rdata, "invalid_physical_id") << dir_xml->toString() << enderr;
			return false;
		}

		OmnString dir_name = dir_xml->getAttrStr("dir_name", "");
		if (dir_name == "")
		{
			AosSetErrorUser(rdata, "missing_dirname") << worker_doc->toString() << enderr;
			return false;
		}
		//bool recursive = worker_doc->getAttrBool("recursive", false);

		vector<AosFileInfo> v;
		AosNetFileCltObj::getFileListStatic(dir_name, physical_id, -1, v, rdata.getPtr());
		for (u32 i=0; i<v.size(); i++)
		{
			AosFileInfo file_info = v[i];
			file_info.mCharset = mCharset;
			file_info.mPhysicalId = physical_id;
			
			// Ketty 2014/01/21
			//mFileList.push_back(file_info);
			AosDataCubeFilePtr cube_file = OmnNew AosDataCubeFile(file_info, mReadBlockSize, mMetadata);
			cube_file->setTaskDocid(mTaskDocid);
			addDataConnector(cube_file);
		}

		dir_xml = dirs_xml->getNextChild();
	}

	return true;
}


AosDataConnectorObjPtr 
AosDataCubeDir::cloneDataConnector()
{
	return OmnNew AosDataCubeDir(*this);
}


AosJimoPtr 
AosDataCubeDir::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeDir(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


