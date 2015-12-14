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
// This is a super class for data cubes that contain multiple files.
//
// Modification History:
// 2013/12/23 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataCube/Jimos/DataCubeUnicomDocument.h"
#include "DataCube/Jimos/DataCubeDir.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/CodeConvertion.h"
#include "JobTrans/UnicomReadFileTrans.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeUnicomDocument_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeUnicomDocument(version);
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


AosDataCubeUnicomDocument::AosDataCubeUnicomDocument(const u32 version)
:
AosDataCube(AOS_DATACUBETYPE_UNICOMDOCUMENT, version),
mReadBlockSize(0),
mCrtFileIdx(0),
mReadTotalLen(0)
{
}


AosDataCubeUnicomDocument::AosDataCubeUnicomDocument(
		const AosFileInfo &file_info,
		const AosXmlTagPtr &metadata)
:
mFileInfo(file_info),
mMetadata(metadata)
{
}


AosDataCubeUnicomDocument::~AosDataCubeUnicomDocument()
{
}


bool
AosDataCubeUnicomDocument::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	// 'worker_doc' serves as the metadata
	aos_assert_rr(worker_doc, rdata, false);
	mMetadata = worker_doc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(mMetadata, false);

	mLock = OmnNew OmnMutex(); 
	mReadBlockSize = mMetadata->getAttrInt64("read_block_size", eDftReadBlockSize);
	mCharset = mMetadata->getAttrStr(AOSTAG_CHARACTER, "UTF8");

	bool rslt = getFileInfo(rdata.getPtr(), worker_doc);
	aos_assert_r(rslt, false);
	return true;
}

bool	
AosDataCubeUnicomDocument::getFileInfo(
		AosRundata *rdata,
		const AosXmlTagPtr &worker_doc)
{
	AosXmlTagPtr dirs_xml = worker_doc->getFirstChild("dirs");
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

		int physical_id = dir_xml->getAttrInt("physical_id", -1);
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
			mFileList.push_back(file_info);
		}
		dir_xml = dirs_xml->getNextChild();
	}

	return true;
}


AosJimoPtr 
AosDataCubeUnicomDocument::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeUnicomDocument(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool
AosDataCubeUnicomDocument::readData(const u64 reqid, AosRundata *rdata)
{
	OmnString fname = mFileInfo.mFileName;
	int physical_id = mFileInfo.mPhysicalId;
	
	aos_assert_r(fname != "", false);
	aos_assert_r(physical_id != -1, false);

	AosTransPtr trans = OmnNew AosUnicomReadFileTrans(fname, physical_id);
	AosBuffPtr resp;

	bool timeout = false;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	//aos_assert_r(rslt, false);
	if (!rslt)
	{
		OmnAlarm << "send trans error!" << enderr;
		return false;
	}

	if (timeout)
	{
		AosSetErrorU(rdata, "Faild to add the trans, timeout");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	if (!resp|| resp->dataLen() < 0)
	{
		AosSetErrorU(rdata, "no response");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosBuffDataPtr buffdata = OmnNew AosBuffData();
	buffdata->setBuff(resp);

OmnScreen  << " ###### DataCubeUnicomDocument readFile : " << fname << endl;
	mCaller->callBack(reqid, buffdata, true);
	return true;
	/*
	aos_assert_rr(mReadBlockSize > 0, rdata, false);
	if (!mDataCubeFileObj)
	{
		mDataCubeFileObj = nextDataCube();
		if (!mDataCubeFileObj) return true;
	}
	mDataCubeFileObj->readData(reqid, rdata);

	return true;
	*/
}

	
void
AosDataCubeUnicomDocument::callBack(
		const u64 &reqid,
		const AosBuffDataPtr &buff, 
		bool finished)
{
	bool allfinished = false;
	aos_assert(buff);
	if (finished)
	{
		mDataCubeFileObj = nextDataConnector();
		if (!mDataCubeFileObj) allfinished = true;
	}

	mCaller->callBack(reqid, buff, allfinished);
}


AosDataConnectorObjPtr
AosDataCubeUnicomDocument::nextDataConnector()
{
	mLock->lock();
	if (mCrtFileIdx >= mFileList.size()) 
	{
		mLock->unlock();
		return 0;
	}

	mCrtFileInfo = mFileList[mCrtFileIdx];
	mCrtFileIdx++;

	AosDataConnectorObjPtr cubefile = OmnNew AosDataCubeUnicomDocument(mCrtFileInfo, mMetadata);
	AosDataConnectorCallerObjPtr thisptr(this, false);
	cubefile->setCaller(thisptr);
	mLock->unlock();
	return cubefile;
}


void
AosDataCubeUnicomDocument::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	mCaller = caller;	
}

AosDataConnectorObjPtr 
AosDataCubeUnicomDocument::cloneDataConnector()
{
	return OmnNew AosDataCubeUnicomDocument(*this);
}


