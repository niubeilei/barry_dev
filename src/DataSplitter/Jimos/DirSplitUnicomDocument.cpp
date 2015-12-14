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
#include "DataSplitter/Jimos/DirSplitUnicomDocument.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDirSplitUnicomDocument_0(
 		const AosRundataPtr &rdata, 
		const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDirSplitUnicomDocument(version);
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


AosDirSplitUnicomDocument::AosDirSplitUnicomDocument(const u32 version)
:
AosDirSplit(AOSDATASPLITTER_DIR_UNICOMDOCUMENT, version)
{
}


AosDirSplitUnicomDocument::~AosDirSplitUnicomDocument()
{
}


bool
AosDirSplitUnicomDocument::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
{
	return true;
}


bool
AosDirSplitUnicomDocument::config(
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

	AosXmlTagPtr dirs_xml = worker_doc->getFirstChild("dirs");
	if (!dirs_xml) 
	{
		AosSetErrorUser(rdata, "missing_dirs") << worker_doc->toString() << enderr;
		return false;
	}

	AosXmlTagPtr dir_xml = dirs_xml->getFirstChild(true);
	vector<AosFileInfo> zip_list;
	mXmlList.clear();
	if (dir_xml)
	{
		mPhysicalId = dir_xml->getAttrInt(AOSTAG_PHYSICALID, -1);
		aos_assert_r(mPhysicalId != -1, false);

		OmnString dir_name = dir_xml->getAttrStr("dir_name", "");
		aos_assert_r(dir_name != "", false);

		bool rslt = AosNetFileCltObj::getFileListByAssignExtStatic(
			"xml", dir_name, mPhysicalId, -1, mXmlList, rdata.getPtr());
		aos_assert_r(rslt, false);
		
		for (u64 i=0; i<mXmlList.size(); i++) 
		{
			mXmlList[i].mPhysicalId = mPhysicalId;
			mXmlList[i].mCharset = mCharset;
		}
		
		rslt = AosNetFileCltObj::getFileListByAssignExtStatic(
			"eippack", dir_name, mPhysicalId, -1, zip_list, rdata.getPtr());
		aos_assert_r(rslt, false);

		for (u64 i=0; i<zip_list.size(); i++)
		{
			zip_list[i].mPhysicalId = mPhysicalId;
			zip_list[i].mCharset = mCharset;
			mZipList.insert(make_pair(zip_list[i].mFileName, zip_list[i]));
		}
	}
	
	return true;
}


bool 
AosDirSplitUnicomDocument::split(
		AosRundata *rdata, 
		vector<AosXmlTagPtr> &v)
{
	if (mXmlList.empty()) return true;
	
	vector<AosFileInfo> filesV;
	AosFileInfo xmlfile_info;
	int64_t groupSize = 0;
	int64_t crtNumFiles = 0;
	OmnString fname;
	AosXmlTagPtr fileConfig;

	map<OmnString, AosFileInfo>::iterator zip_itr;
	OmnString zip_name;
	for (u64 i=0; i<mXmlList.size(); i++)
	{
		xmlfile_info = mXmlList[i];
		groupSize += xmlfile_info.mFileLen;
		
		fname = xmlfile_info.mFileName;
		AosStrSplit split;
		bool finished;
		OmnString pair[2];
		split.splitStr(fname.data(),".", pair, 2, finished);    

		zip_name = pair[0];
		zip_name << ".eippack";
		zip_itr = mZipList.find(zip_name);
		if (zip_itr != mZipList.end())
		{
			groupSize += zip_itr->second.mFileLen;
			mZipList.erase(zip_itr);
		}
		filesV.push_back(xmlfile_info);
		if (crtNumFiles == eDftMaxGroupNumFiles || groupSize >= mGroupSize)
		{
			fileConfig = getCubeFilesConfig(rdata, filesV);
			if (fileConfig) v.push_back(fileConfig);
			filesV.clear();
			crtNumFiles = 0;
			groupSize = 0;
		}
	}

	if (!filesV.empty()) 
	{
		fileConfig = getCubeFilesConfig(rdata, filesV);
		if (fileConfig) v.push_back(fileConfig);
		filesV.clear();
	}

	return true;
}


AosXmlTagPtr
AosDirSplitUnicomDocument::getCubeFilesConfig(
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

	OmnString str = "<dataconnector zky_objid=\"dataconnector_unicom_document_jimodoc_v0\" ";
	str << "zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_datacube\" " 
		<< "jimo_type=\"jimo_datacube\" current_version=\"0\" "
		<< "zky_classname=\"AosDataCubeUnicomDocument\" type=\"files\" "
		<< AOSTAG_CHARACTER "=\"" << mCharset << "\" "
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
AosDirSplitUnicomDocument::cloneJimo() const
{
	try
	{
		return OmnNew AosDirSplitUnicomDocument(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

