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
#include "DataSplitter/Jimos/DataColSplitByGroup.h"

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

AosJimoPtr AosCreateJimoFunc_AosDataColSplitByGroup_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataColSplitByGroup(version);
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


AosDataColSplitByGroup::AosDataColSplitByGroup(const u32 version)
:
AosDataSplitter(AOSDATASPLITTER_DATACOL_BY_CUBE, version)
{
}


AosDataColSplitByGroup::AosDataColSplitByGroup(
		const OmnString &type,
		const u32 version)
:
AosDataSplitter(type, version)
{
}


bool
AosDataColSplitByGroup::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	// worker_doc format:
	// <dataconnector jimo_objid="xxx" zky_file_defname="xxx">
	//		<split jimo_objid="dataspliter_datacolbygroup_jimodoc_v0"/>
	//</dataconnector>
	
	aos_assert_r(worker_doc, false);

	OmnString col_id = worker_doc->getAttrStr(AOSTAG_DATACOLLECTOR_ID, "");
	aos_assert_r(col_id != "", false);

	//AosXmlTagPtr split = worker_doc->getFirstChild("split");
	//u16 group_size = split->getAttrInt("group_num", 1);

	aos_assert_r(mJobDocid != 0, false);
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);  
	aos_assert_r(job, false);
	AosDataCollectorMgrPtr colMgr = job->getDataColMgr();
	aos_assert_r(colMgr, false);

	vector<AosXmlTagPtr> files;
	bool rslt = colMgr->getOutputFilesLocked(col_id, files, rdata);
	aos_assert_r(rslt, false); 

	AosLogicTaskObjPtr col = colMgr->getDataColLocked(col_id,rdata);
	int shuffle_type = col->getShuffleType();
	if(shuffle_type == -1)
	{
		mShuffleType = AosDataCollectorGroup::eInvalid;
	}
	else 
	{
		mShuffleType = AosDataCollectorGroup::E(shuffle_type);
	}

	map<int64_t, map<int, vector<AosFileInfo> > >::iterator itr;
	map<int, vector<AosFileInfo> >::iterator itr_phy;
	AosFileInfo	info;
	AosXmlTagPtr file_xml;
	vector<AosFileInfo> v;
	map<int, vector<AosFileInfo> > phyId_map;
	for (u32 i = 0; i < files.size(); i++)
	{
		file_xml = files[i];
		info.serializeFrom(file_xml);
		if (info.mFileBlocks.empty())
		{
			if (info.mFileId == 0)
			{
				AosSetErrorUser(rdata, "missing_fileid") << file_xml->toString() << enderr;
				return false;
			}
		}
		itr = mFileList.find(info.mShuffleId);
		if (itr == mFileList.end())
		{
			v.clear();
			phyId_map.clear();
			v.push_back(info);
			phyId_map.insert(make_pair(info.mPhysicalId, v));
			mFileList.insert(make_pair(info.mShuffleId, phyId_map));
		}
		else
		{
			phyId_map = itr->second;
			itr_phy = phyId_map.find(info.mPhysicalId);
			if (itr_phy == phyId_map.end())
			{
				v.clear();
				v.push_back(info);
				phyId_map.insert(make_pair(info.mPhysicalId, v));
			}
			else
			{
				itr_phy->second.push_back(info);
			}
			itr->second = phyId_map;
		}
	}
	return true;
}


bool
AosDataColSplitByGroup::config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosDataColSplitByGroup::~AosDataColSplitByGroup()
{
}


bool 
AosDataColSplitByGroup::split(AosRundata *rdata, vector<AosXmlTagPtr> &data_units)
{
	// each data_units conf format
	// <dataconnector type="idfiles" ... cube_id="xxx">
	// 	<files>
	// 		<file zky_physicalid=\"0\" zky_storage_fileid="xx" />
	// 		...
	// 	</files>
	// </dataconnector>
	
	OmnString str,file_conf;
	AosXmlParser parser;                  
	AosXmlTagPtr conf;
	AosFileInfo info;

	str << "<dataconnector zky_objid=\"dataconnector_idfiles_jimodoc_v0\" "
		<< "zky_otype=\"zkyotp_jimo\" jimo_name=\"jimo_datacube\" " 
		<< "jimo_type=\"jimo_datacube\" current_version=\"0\" "
		<< "zky_classname=\"AosDataCubeIdFiles\" type=\"idfiles\" ";

	map<int64_t, map<int, vector<AosFileInfo> > >::iterator itr = mFileList.begin();
	vector<AosFileInfo> files;
	map<int, vector<AosFileInfo> > phy_map;
	map<int, vector<AosFileInfo> >::iterator itr_phy;
	int64_t file_total_len = 0;
	bool flag = true;
	if (mFileList.empty())
	{
		file_conf = str;
		file_conf << AOSTAG_PHYSICALID "=\"" << AosGetIdealSvrIdByCubeId(0) << "\" "
			<< AOSTAG_SHUFFLE_ID << "=\"" << 0 << "\">" 
			<< "<files>";
		file_conf << "</files>"
			<< "<versions><ver_0>libDataCubicJimos.so</ver_0>"
			<< "</versions></dataconnector>";
		conf = parser.parse(file_conf, "" AosMemoryCheckerArgs); 
		data_units.push_back(conf);
	}
	else
	{
		int64_t shuffle_id;
		int svr_id;
		for (; itr != mFileList.end(); ++itr)
		{
			phy_map = itr->second;
			shuffle_id = itr->first;
			for(itr_phy = phy_map.begin();itr_phy!=phy_map.end(); ++itr_phy)
			{
				svr_id = itr_phy->first;
				flag = true;
				file_total_len = 0;
				file_conf = str;
				//  JIMODB-1044 
				//  by barry 2015/10/26
				file_conf << AOSTAG_PHYSICALID "=\"" << getSvrId(shuffle_id, svr_id) << "\" "
					<< AOSTAG_SHUFFLE_ID << "=\"" << shuffle_id << "\">" 
					<< "<files>";
				files = itr_phy->second;
				for (size_t i = 0; i < files.size(); i++)
				{
					info = files[i];
					if (i == 0 && info.mFileLen >= eMaxSplitSize)
					{
						file_conf << info.serializeToXmlStr();
						file_conf << "</files>"
							<< "<versions><ver_0>libDataCubicJimos.so</ver_0>"
							<< "</versions></dataconnector>";
						conf = parser.parse(file_conf, "" AosMemoryCheckerArgs); 
						data_units.push_back(conf);
						if (i != files.size() - 1)
						{
							file_conf = str;
							file_conf << AOSTAG_PHYSICALID "=\"" << getSvrId(shuffle_id, svr_id) << "\" "
								<< AOSTAG_SHUFFLE_ID << "=\"" << shuffle_id << "\">" 
								<< "<files>";
						}
						else
						{
							flag = false;
						}
						continue;
					}
					if (file_total_len + info.mFileLen <= eMaxSplitSize)
					{
						file_conf << info.serializeToXmlStr();
						file_total_len += info.mFileLen;
					}
					else
					{
						file_conf << "</files>"
							<< "<versions><ver_0>libDataCubicJimos.so</ver_0>"
							<< "</versions></dataconnector>";
						conf = parser.parse(file_conf, "" AosMemoryCheckerArgs); 
						data_units.push_back(conf);

						file_conf = str;
						file_conf << AOSTAG_PHYSICALID "=\"" << getSvrId(shuffle_id, svr_id) << "\" "
							<< AOSTAG_SHUFFLE_ID << "=\"" << shuffle_id << "\">" 
							<< "<files>";
						file_conf << info.serializeToXmlStr();
						file_total_len = info.mFileLen;
					}
				}
				if (flag)
				{
					file_conf << "</files>"
						<< "<versions><ver_0>libDataCubicJimos.so</ver_0>"
						<< "</versions></dataconnector>";
					conf = parser.parse(file_conf, "" AosMemoryCheckerArgs); 
					data_units.push_back(conf);
				}
			}
		}
	}
	return true;
}


//  JIMODB-1044 
//  by barry 2015/10/26
int
AosDataColSplitByGroup::getSvrId(
		const int64_t shuffle_id,
		const int phy_id)
{
	switch (mShuffleType)
	{
	case AosDataCollectorGroup::eInvalid:
		return phy_id;
	case AosDataCollectorGroup::eCubeShuffler:
		return AosGetIdealSvrIdByCubeId(shuffle_id);
	case AosDataCollectorGroup::ePhyShuffler:
		return shuffle_id;
	default:
		return phy_id;
	}
	return phy_id;
}


AosJimoPtr 
AosDataColSplitByGroup::cloneJimo() const
{
	try
	{
		return OmnNew AosDataColSplitByGroup(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}
