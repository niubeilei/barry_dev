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
#include "DataSplitter/Jimos/FileSplitSize.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosFileSplitFixSize_0(
		const AosRundataPtr &rdata,
		const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosFileSplitFixSize(version);
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


AosFileSplitFixSize::AosFileSplitFixSize(const u32 version)
:
AosFileSplit(AOSDATASPLITTER_FILE_FIX_SIZE, version)
{
}


AosFileSplitFixSize::~AosFileSplitFixSize()
{
}


bool
AosFileSplitFixSize::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc)
{
	aos_assert_rr(worker_doc, rdata, false);
	bool rslt = AosFileSplit::config(rdata, worker_doc);
	aos_assert_r(rslt, false);

	AosXmlTagPtr split_config = worker_doc->getFirstChild("split");
	aos_assert_r(split_config, false);

	mRecordSize = split_config->getAttrInt("entry_size", 1);
	
	mGroupSize = mGroupSize / mRecordSize * mRecordSize;

	return true;
}


bool 
AosFileSplitFixSize::split( 
		AosRundata *rdata, 
		vector<AosXmlTagPtr> &data_units)
{ 
	OmnString str = "<dataconnector zky_objid=\"dataconnector_file_jimodoc_v0\" ";
	str << "zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_datacube\" " 
		<< "jimo_type=\"jimo_datacube\" current_version=\"0\" "
	//	<< "zky_classname=\"AosDataCubeFile\" type=\"file\" ";
		<< "zky_classname=\"AosDataCubeFile\" type=\"file\">";

	//str << AOSTAG_CHARACTER << "=\"" << mFileInfo.mCharset << "\" " 
	//	<< AOSTAG_PHYSICALID << "=\"" << mFileInfo.mPhysicalId << "\" "
	//	<< "file_name=\"" << mFileInfo.mFileName << "\" "
	//	<< "file_id=\"" << mFileInfo.mFileId << "\" ";

	int64_t length = mFileInfo.mFileLen;
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

		cubefileConfig << "<versions><ver_0>libDataCubicJimos.so</ver_0>"
			<< "</versions></dataconnector>";

		AosXmlTagPtr data_unit = AosXmlParser::parse(cubefileConfig AosMemoryCheckerArgs); 
		data_units.push_back(data_unit);
	}
	return true; 
}


AosJimoPtr 
AosFileSplitFixSize::cloneJimo() const
{
	try
	{
		return OmnNew AosFileSplitFixSize(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

