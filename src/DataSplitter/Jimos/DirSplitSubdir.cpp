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
#include "DataSplitter/Jimos/DirSplitSubdir.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDirSplitSubdir_0(
 		const AosRundataPtr &rdata, 
		const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDirSplitSubdir(version);
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


AosDirSplitSubdir::AosDirSplitSubdir(const u32 version)
:
AosDirSplit(AOSDATASPLITTER_DIR_FILES, version)
{
}


AosDirSplitSubdir::~AosDirSplitSubdir()
{
}


bool
AosDirSplitSubdir::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
{
	return true;
}

bool
AosDirSplitSubdir::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc)
{
	// worker_doc:
	// <data_cube read_block_size="10000" zky_character="UTF8">
	// 		<split jimo...>
	// 			<versions current_version="1.0">
	// 				<ver_0 version="1.0">libDataSplitter.so</ver_0>
	// 			</versions>
	// 		</split>
	// 		<dirs>
	//			<dir dir_name="xxx" AOSTAG_PHYSICALID="xxx" />
	//			<dir dir_name="xxx" AOSTAG_PHYSICALID="xxx" />
	//			...
	//		</dirs>
	// </data_cube>
	aos_assert_rr(worker_doc, rdata, false);
	bool rslt = AosDirSplit::config(rdata, worker_doc, 0);
	aos_assert_r(rslt, false);
	AosXmlTagPtr split_xml = worker_doc->getFirstChild("split");
	aos_assert_r(split_xml, false);

	if (!split_xml)
	{
		AosSetErrorUser(rdata, "missing_split_node") << worker_doc->toString() << enderr;
		return false;
	}
	//mGroupSize = split_xml->getAttrInt64("group_size", eDftGroupSize);
	//if (mGroupSize <= 0) mGroupSize = eDftGroupSize;

	AosXmlTagPtr dirs_xml = worker_doc->getFirstChild("dirs");
	if (!dirs_xml) 
	{
		AosSetErrorUser(rdata, "missing_dirs") << worker_doc->toString() << enderr;
		return false;
	}

	AosXmlTagPtr dir_xml = dirs_xml->getFirstChild("dir");
	while (dir_xml)
	{
		int physicalId = dir_xml->getAttrInt(AOSTAG_PHYSICALID, -1);
		OmnString dir_name = dir_xml->getAttrStr("dir_name");
		vector<AosDirInfo> v;
		AosNetFileCltObj::getDirListStatic(dir_name, physicalId, v, rdata.getPtr());
		for (u64 i=0; i<v.size(); i++) 
		{
			v[i].mPhysicalId = physicalId;
			v[i].mCharset = mCharset;
		}
		mDirs.push_back(v);
		dir_xml = dirs_xml->getNextChild();
	}
	
	return true;
}


bool 
AosDirSplitSubdir::split(AosRundata *rdata, vector<AosXmlTagPtr> &v)
{
	if (mDirs.empty()) return true;
	vector<AosDirInfo> dirsV;

	int64_t crtNumDirs = 0;
	while (!mDirs.empty())
	{
		for (u64 i=0; i<mDirs.size(); i++)	
		{
			if (mDirs[i].empty()) 
			{
				mDirs.erase(mDirs.begin()+i);
				i--;
				break;
			}
			dirsV.push_back(mDirs[i].back());
			mDirs[i].pop_back();
			crtNumDirs++;
			if (crtNumDirs == eDftMaxNumDirsOfGroup) 
			{
				AosXmlTagPtr cubedir_config = getCubeDirConfig(rdata, dirsV);
				if (cubedir_config) v.push_back(cubedir_config);
				dirsV.clear();
				crtNumDirs = 0;
				break;
			}
		}
	}
	if (!dirsV.empty()) 
	{
		AosXmlTagPtr cubedir_config = getCubeDirConfig(rdata, dirsV);
		if (cubedir_config) v.push_back(cubedir_config);
	}

	return true;
}


AosXmlTagPtr
AosDirSplitSubdir::getCubeDirConfig(
		AosRundata *rdata,
		vector<AosDirInfo> &v)
{
	OmnString str = "<data_cube zky_objid=\"dataconnector_dir_jimodoc_v0\" ";
	str << "zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_datacube\" " 
		<< "jimo_type=\"jimo_datacube\" current_version=\"0\" "
		<< "zky_classname=\"AosDataCubeDir\" type=\"dir\" "
		<< AOSTAG_CHARACTER << "=\"" << mCharset << "\" "
		<< "read_block_size=\"" << mReadBlockSize << "\">"
		<< "<versions>"
			<< "<ver_0>libDataCubicJimos.so</ver_0>"
		<< "</versions>"
		<< "<dirs>";
	for (u64 i=0; i<v.size(); i++)
	{
		OmnString crtDirStr = "<dir ";
		crtDirStr << "dir_name=\"" << v[i].mDirName << "\" "
			<< AOSTAG_PHYSICALID << "=\"" << v[i].mPhysicalId << "\">";
		crtDirStr << "</dir>";
		str << crtDirStr;
	}
	str << "</dirs></data_cube>";
	AosXmlParser parser;
	AosXmlTagPtr cubeDirConfig = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(cubeDirConfig, 0);
	return cubeDirConfig;
}


AosJimoPtr 
AosDirSplitSubdir::cloneJimo() const
{
	try
	{
		return OmnNew AosDirSplitSubdir(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

