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
// 2014/08/20 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DataSplitter/Jimos/FilesSplitByCube.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "VirtualFile/VirtualFile.h"
#include "XmlUtil/Ptrs.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosFilesSplitByCube_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosFilesSplitByCube(version);
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


AosFilesSplitByCube::AosFilesSplitByCube(const u32 version)
:
AosDataSplitter(AOSDATASPLITTER_FILES_BY_CUBE, version)
{
}


AosFilesSplitByCube::AosFilesSplitByCube(
		const OmnString &type,
		const u32 version)
:
AosDataSplitter(type, version)
{
}


bool
AosFilesSplitByCube::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	// worker_doc format:
	// <dataconnector jimo_objid="xxx" zky_file_defname="xxx">
	//		<split jimo_objid="dataspliter_filesbycube_jimodoc_v0"/>
	//</dataconnector>
	
	AosXmlTagPtr split_config = worker_doc->getFirstChild("split");
	aos_assert_r(split_config, false);

	AosXmlTagPtr files_xml = worker_doc->getFirstChild("files");
	if (!files_xml) 
	{
		OmnString file_defname = worker_doc->getAttrStr("zky_file_defname", "");	
		if(file_defname == "")
		{
			AosSetErrorUser(rdata, "missing_fileid")
				<< worker_doc->toString() << enderr;
			return false;
		}
		files_xml = AosGetDocByObjid(file_defname, rdata); 
	}
	if (files_xml)
	{
		AosXmlTagPtr file_xml = files_xml->getFirstChild(true);
		while(file_xml)
		{
			//AosFileInfo	info;
			FileDef file;	
			file.mFileInfo.mFileId = file_xml->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
			if (file.mFileInfo.mFileId == 0)
			{
				AosSetErrorUser(rdata, "missing_fileid") << file_xml->toString() << enderr;
				return false;
			}

			file.mFileInfo.mPhysicalId = file_xml->getAttrInt(AOSTAG_PHYSICALID, -1);
			if (file.mFileInfo.mPhysicalId == -1)
			{
				AosSetErrorUser(rdata, "missing_fileid") << file_xml->toString() << enderr;
				return false;
			}

			int shuffle_id = file_xml->getAttrInt(AOSTAG_CUBE_ID, -1);
			if(shuffle_id == -1)
			{
				AosSetErrorUser(rdata, "invalid_physical_id") << files_xml->toString() << enderr;
				return false;
			}
			file.mShuffleId = shuffle_id;

			//mFileList.push_back(info);
			mFileList.push_back(file);
			file_xml = files_xml->getNextChild();
		}
	}

	return true;
}


bool
AosFilesSplitByCube::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosFilesSplitByCube::~AosFilesSplitByCube()
{
}


bool 
AosFilesSplitByCube::split(AosRundata *rdata, vector<AosXmlTagPtr> &data_units)
{
	// each data_units conf format
	// <dataconnector type="idfiles" ... shuffle_id_"xxx">
	// 	<files>
	// 		<file zky_physicalid=\"0\" zky_storage_fileid="xx" />
	// 		...
	// 	</files>
	// </dataconnector>
	
	if (mFileList.empty())
		return true;

	OmnString str = "<dataconnector zky_objid=\"dataconnector_idfiles_jimodoc_v0\" ";
	str << "zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_datacube\" " 
		<< "jimo_type=\"jimo_datacube\" current_version=\"0\" "
		<< "zky_classname=\"AosDataCubeIdFiles\" type=\"idfiles\" ";

	map<u32, OmnString> conf_str_map;
	map<u32, OmnString>::iterator itr;
	OmnString file_conf;
	int svr_id;
	for (u64 i = 0; i < mFileList.size(); i++)
	{
		itr = conf_str_map.find(mFileList[i].mShuffleId);
		if(itr == conf_str_map.end())
		{
			file_conf = str;
			svr_id = AosGetIdealSvrIdByCubeId(mFileList[i].mShuffleId);
			file_conf << AOSTAG_PHYSICALID "=\"" << svr_id << "\" "
				<< AOSTAG_CUBE_ID << "=\"" << mFileList[i].mShuffleId << "\" >"
				<< "<files>";

			pair< map<u32, OmnString>::iterator, bool> pr;
			pr = conf_str_map.insert(make_pair(mFileList[i].mShuffleId , file_conf));
			aos_assert_r(pr.second, false);	
			itr = pr.first;
		}

		itr->second << "<file "
			<< AOSTAG_PHYSICALID << "=\"" << mFileList[i].mFileInfo.mPhysicalId << "\" "
			<< AOSTAG_STORAGE_FILEID << "=\"" << mFileList[i].mFileInfo.mFileId << "\" "
			<< "></file>";
	}

	AosXmlParser parser;                  
	AosXmlTagPtr conf;
	for(itr = conf_str_map.begin(); itr != conf_str_map.end(); itr++)
	{
		OmnString & crt_conf_str = itr->second;
		crt_conf_str << "</files>"
			<< "<versions><ver_0>libDataCubicJimos.so</ver_0>"
			<< "</versions></dataconnector>";
		
		conf = parser.parse(crt_conf_str, "" AosMemoryCheckerArgs); 
		data_units.push_back(conf);
	}	
	return true;
}


AosJimoPtr 
AosFilesSplitByCube::cloneJimo() const
{
	try
	{
		return OmnNew AosFilesSplitByCube(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}
