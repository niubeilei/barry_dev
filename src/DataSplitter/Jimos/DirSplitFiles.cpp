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
#include "DataSplitter/Jimos/DirSplitFiles.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDirSplitFiles_0(
 		const AosRundataPtr &rdata, 
		const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDirSplitFiles(version);
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


AosDirSplitFiles::AosDirSplitFiles(const u32 version)
:
AosDirSplit(AOSDATASPLITTER_DIR_FILES, version)
{
}


AosDirSplitFiles::~AosDirSplitFiles()
{
}


bool
AosDirSplitFiles::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
{
	return true;
}

bool
AosDirSplitFiles::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc)
{
	// worker_doc:
	// <dataconnector read_block_size="10000" zky_character="UTF8">
	// 		<split group_size="xxx" jimo...>
	// 			<versions current_version="1.0">
	// 				<ver_0 version="1.0">libDataSplitter.so</ver_0>
	// 			</versions>
	// 		</split>
	// 		<dirs>
	//			<dir dir_name="xxx" AOSTAG_PHYSICALID="xxx" />
	//			<dir dir_name="xxx" AOSTAG_PHYSICALID="xxx" />
	//			...
	//		</dirs>
	// </dataconnector>
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
	mGroupSize = split_xml->getAttrInt64("group_size", eDftGroupSize);

	if (mGroupSize <= 0) mGroupSize = eDftGroupSize;
	mGroupSize = mGroupSize*1000000;

	AosXmlTagPtr dirs_xml = worker_doc->getFirstChild("dirs");
	if (!dirs_xml) 
	{
		AosSetErrorUser(rdata, "missing_dirs") << worker_doc->toString() << enderr;
		return false;
	}

	AosXmlTagPtr dir_xml = dirs_xml->getFirstChild(true);
	while (dir_xml)
	{
		int physicalId = dir_xml->getAttrInt(AOSTAG_PHYSICALID, -1);
		aos_assert_r(physicalId != -1, false);
		OmnString dir_name = dir_xml->getAttrStr("dir_name", "");
		vector<AosFileInfo> v;
		AosNetFileCltObj::getFileListStatic(dir_name, physicalId, -1, v, rdata.getPtr());
		for (u64 i=0; i<v.size(); i++) 
		{
			v[i].mPhysicalId = physicalId;
			v[i].mCharset = mCharset;
		}
	
		if (mFiles.count(physicalId) > 0)	
		{
			mFiles[physicalId].push_back(v);
		}
		else
		{
			vector<vector<AosFileInfo> > vTemp;
			vTemp.push_back(v);
			mFiles[physicalId] = vTemp;
		}

		//mFiles.push_back(v);
		dir_xml = dirs_xml->getNextChild();
	}
	
	return true;
}


bool 
AosDirSplitFiles::split(AosRundata *rdata, vector<AosXmlTagPtr> &v)
{
	if (mFiles.empty()) return true;
	vector<AosFileInfo> filesV;
	vector<vector<AosFileInfo> > sameNodeFiles;

	for (map<int, vector<vector<AosFileInfo> > >::iterator itr=mFiles.begin(); 
			itr != mFiles.end(); itr++)
	{
		sameNodeFiles = itr->second;
		int64_t crtNumFiles = 0;
		int64_t groupSize = 0;

		while (!sameNodeFiles.empty())
		{
			for (u64 i=0; i<sameNodeFiles.size(); i++)	
			{
				if (sameNodeFiles[i].empty()) 
				{
					sameNodeFiles.erase(sameNodeFiles.begin()+i);
					i--;
					break;
				}
				AosFileInfo file_info = sameNodeFiles[i].back();
				groupSize += file_info.mFileLen;
				filesV.push_back(sameNodeFiles[i].back());
				sameNodeFiles[i].pop_back();
				crtNumFiles++;
				if (crtNumFiles == eDftMaxGroupNumFiles || groupSize >= mGroupSize) 
				{
					AosXmlTagPtr fileConfig = getCubeFilesConfig(rdata, filesV);
					if (fileConfig) v.push_back(fileConfig);
					filesV.clear();
					crtNumFiles = 0;
					groupSize = 0;
					break;
				}
			}
		}
		if (!filesV.empty()) 
		{
			AosXmlTagPtr fileConfig = getCubeFilesConfig(rdata, filesV);
			if (fileConfig) v.push_back(fileConfig);
			filesV.clear();
		}
	}

	return true;
}


AosXmlTagPtr
AosDirSplitFiles::getCubeFilesConfig(
		AosRundata *rdata,
		vector<AosFileInfo> &v)
{
	map<OmnString, vector<OmnString> > fileGroups;
	for (u64 i=0; i<v.size(); i++)
	{
		AosFileInfo file_info = v[i];	
		OmnString filename = file_info.mFileName;
		int idx = filename.indexOf(0, '/', true);
		aos_assert_r(idx >= 0, 0);
		OmnString name = filename.substr(idx + 1);
		OmnString path = filename.substr(0, idx - 1);
		OmnString mapkey = path;
		map<OmnString, vector<OmnString> >::iterator itr = fileGroups.find(mapkey);
		if (itr == fileGroups.end())
		{
			vector<OmnString> filesV;
			filesV.push_back(name);
			fileGroups[mapkey] = filesV;
		}
		else
		{
			(itr->second).push_back(name);
		}
	}

	aos_assert_r(v.back().mPhysicalId != -1, 0);

	OmnString skipFirstLine = mIsSkipFirstLine ? "true" : "false";
	OmnString str = "<dataconnector zky_objid=\"dataconnector_files_jimodoc_v0\" ";
	str << "zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_datacube\" " 
		<< "jimo_type=\"jimo_datacube\" current_version=\"0\" "
		<< "zky_classname=\"AosDataCubeFiles\" type=\"files\" "
		<< "skip_first_line" << "=\"" << skipFirstLine << "\" "
		<< AOSTAG_CHARACTER << "=\"" << mCharset << "\" "
		<< AOSTAG_PHYSICALID << "=\"" << v.back().mPhysicalId << "\" " 
		<< "read_block_size=\"" << mReadBlockSize << "\">"
		<< "<versions>"
			<< "<ver_0>libDataCubicJimos.so</ver_0>"
		<< "</versions>"
		<< "<dirs>";
	
	map<OmnString, vector<OmnString> >::iterator itr = fileGroups.begin();
	aos_assert_r(itr != fileGroups.end(), 0);
	for (u64 i=0; i<fileGroups.size(); i++)
	{
		OmnString dir_name = itr->first;
		OmnString crtDirStr = "<dir ";
		crtDirStr << "dir_name=\"" << dir_name << "\" "
			  << AOSTAG_PHYSICALID << "=\"" << v.back().mPhysicalId << "\">";
		vector<OmnString> filenameV = itr->second;
		for (u64 j=0; j<filenameV.size(); j++)
		{
			crtDirStr << filenameV[j];
			if (j != filenameV.size()-1) crtDirStr << ",";
		}
		crtDirStr << "</dir>";
		str << crtDirStr;
		itr++;
	}
	str << "</dirs></dataconnector>";
	AosXmlParser parser;
	AosXmlTagPtr fileGroupConfig = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(fileGroupConfig, 0);
	return fileGroupConfig;
}


AosJimoPtr 
AosDirSplitFiles::cloneJimo() const
{
	try
	{
		return OmnNew AosDirSplitFiles(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

