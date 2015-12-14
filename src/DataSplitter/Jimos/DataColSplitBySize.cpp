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
// 2015/01/05 Created by Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#include "DataSplitter/Jimos/DataColSplitBySize.h"

#include "API/AosApi.h"
#include "DataCollector/DataCollectorMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "VirtualFile/VirtualFile.h"
#include "XmlUtil/Ptrs.h"
#include "SEInterfaces/JobObj.h"
#include "SEInterfaces/JobMgrObj.h"

extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosDataColSplitBySize_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataColSplitBySize(version);
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


AosDataColSplitBySize::AosDataColSplitBySize(const u32 version)
:
AosDataColSplitByGroup(AOSDATASPLITTER_DATACOL_BY_SIZE, version)
{
}


AosDataColSplitBySize::AosDataColSplitBySize(
		const OmnString &type,
		const u32 version)
:
AosDataColSplitByGroup(type, version)
{
}


bool
AosDataColSplitBySize::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	// worker_doc format:
	// <dataconnector jimo_objid="xxx" zky_file_defname="xxx">
	//		<split size="100000" type="fixbin/buff" jimo_objid="dataspliter_datacolbysize_jimodoc_v0"/>
	//</dataconnector>
	
	aos_assert_r(worker_doc, false);

	OmnString col_id = worker_doc->getAttrStr(AOSTAG_DATACOLLECTOR_ID, "");
	aos_assert_r(col_id != "", false);

	AosXmlTagPtr split_config = worker_doc->getFirstChild("split");
	aos_assert_r(split_config, false);

	mRecordSize = split_config->getAttrInt("entry_size", 1);
	//Jozhi this is bug for andy, this file contents may be (fixed record or buff record).
	//if buff record, this split is wrong.
	mGroupSize = split_config->getAttrInt64("group_size", 10000000000);
	mGroupSize = mGroupSize / mRecordSize * mRecordSize;

	aos_assert_r(mJobDocid != 0, false);
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);  
	aos_assert_r(job, false);
	AosDataCollectorMgrPtr colMgr = job->getDataColMgr();
	aos_assert_r(colMgr, false);

	AosXmlTagPtr file_xml;
	vector<AosXmlTagPtr> files;
	bool rslt = colMgr->getOutputFilesLocked(col_id, files, rdata);
	aos_assert_r(rslt, false); 
	//aos_assert_r(files.size() == 1, false);
	if (files.size() == 0)
	{
		return true;
	}
	file_xml = files[0];

	mFileInfo.serializeFrom(file_xml);
	if (mFileInfo.mFileId == 0)
	{
		OmnAlarm << "error ." << enderr;
		AosSetErrorUser(rdata, "missing_fileid") << file_xml->toString() << enderr;
		return false;
	}
	if (mFileInfo.mPhysicalId == -1)
	{
		OmnAlarm << "error ." << enderr;
		AosSetErrorUser(rdata, "missing_fileid") << file_xml->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosDataColSplitBySize::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosDataColSplitBySize::~AosDataColSplitBySize()
{
}


bool 
AosDataColSplitBySize::split(AosRundata *rdata, vector<AosXmlTagPtr> &data_units)
{
	// each data_units conf format
	// <dataconnector type="idfiles" ... cube_id="xxx">
	// 	<files>
	// 		<file zky_physicalid=\"0\" zky_storage_fileid="xx" />
	// 		...
	// 	</files>
	// </dataconnector>
	
	OmnString str;
	str << "<dataconnector zky_objid=\"dataconnector_idfiles_jimodoc_v0\" "
		<< "zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_datacube\" " 
		<< "jimo_type=\"jimo_datacube\" current_version=\"0\" "
		<< "zky_classname=\"AosDataCubeIdFiles\" type=\"idfiles\" >"
		<< "<files>";

	int64_t length = mFileInfo.mFileLen;

	if (length == 0)
	{
		str << "</files><versions><ver_0>libDataCubicJimos.so</ver_0>"
			<< "</versions></dataconnector>";
		AosXmlTagPtr data_unit = AosXmlParser::parse(str AosMemoryCheckerArgs); 
		data_units.push_back(data_unit);
		return true;
	}
	int64_t pos = mFileInfo.mStartOffset;
	while (length > 0)
	{
		OmnString cubefileConfig = str;
		AosFileInfo info = mFileInfo;

		if (length > mGroupSize)
		{
			//cubefileConfig << "start_pos=\"" << pos << "\" "
			//	<< "file_length=\"" << mGroupSize << "\">";
			info.mStartOffset = pos;
			info.mFileLen = mGroupSize;

			pos += mGroupSize;
			length -= mGroupSize;
		}
		else
		{
			//cubefileConfig << "start_pos=\"" << pos << "\" "
			//	<< "file_length=\"" << length << "\">";
			info.mStartOffset = pos;
			info.mFileLen = length;

			length = 0;
		}
		
		cubefileConfig << info.serializeToXmlStr();

		cubefileConfig 
			<< "</files><versions><ver_0>libDataCubicJimos.so</ver_0>"
			<< "</versions></dataconnector>";

		AosXmlTagPtr data_unit = AosXmlParser::parse(cubefileConfig AosMemoryCheckerArgs); 
		data_units.push_back(data_unit);
	}
	return true; 
}


AosJimoPtr 
AosDataColSplitBySize::cloneJimo() const
{
	try
	{
		return OmnNew AosDataColSplitBySize(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

