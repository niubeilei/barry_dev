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
// This action get a function from a dll lib,
// then run the function and get the return value.
//
// Modification History:
// 2012/05/11	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActMergeFile.h"


#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Thread/ThreadShellMgr.h"
#include "Thread/ThreadPool.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/AppendFile.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "NetFile/NetFile.h"
#include "XmlUtil/XmlTag.h"
#include "Job/JobMgr.h"



static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("ActMergeFile", __FILE__, __LINE__);


AosActMergeFile::AosActMergeFile(const bool flag)
:
AosSdocAction(AOSACTTYPE_MERGEFILE, AosActionType::eMergeFile, flag)
{
}


AosActMergeFile::~AosActMergeFile()
{
}


bool
AosActMergeFile::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if(!task)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mOutPutSem = OmnNew OmnSem(0);

	int level = sdoc->getAttrInt(AOSTAG_LEVEL, -1);
	aos_assert_r(level!= -1, false);

	bool last_merge = sdoc->getAttrBool(AOSTAG_LAST_MERGE, false);

	int crt_block_idx = sdoc->getAttrInt(AOSTAG_BLOCK_IDX, 0);

	AosXmlTagPtr dataCol_tag = sdoc->getFirstChild("datacollector");
	aos_assert_r(dataCol_tag, false);

	OmnString datacol_id = dataCol_tag->getAttrStr(AOSTAG_NAME);
	aos_assert_r(datacol_id != "", false);

	OmnString str_type = dataCol_tag->getAttrStr("record_type", AOSRECORDTYPE_FIXBIN);
	mRecordType = AosDataRecordType::toEnum(str_type);
	aos_assert_r(AosDataRecordType::isValid(mRecordType), false);

	AosXmlTagPtr asm_tag = dataCol_tag->getFirstChild("asm");
	aos_assert_r(asm_tag, false);
	AosXmlTagPtr comp_tag = asm_tag->getFirstChild("CompareFun");
	aos_assert_r(comp_tag, false);
	AosCompareFunPtr comp = AosCompareFun::getCompareFunc(comp_tag);
	aos_assert_r(comp, false);

	OmnString opr = dataCol_tag->getAttrStr(AOSTAG_OPR, "index");
	AosDataColOpr::E oper = AosDataColOpr::toEnum(opr);
	aos_assert_r(AosDataColOpr::isValid(oper), false);
	
	vector<AosNetFileObjPtr> net_files;
	getFiles(net_files, sdoc, level);
	aos_assert_r(net_files.size() > 0 , false);

	AosTaskDataObjPtr taskData = mTask->getOutPut(
		AosTaskDataType::eOutPutIIL, datacol_id, rdata);

	/*
	if (net_files.size() == 1)
	{
		if (!taskData)
		{
			int server_id = net_files[0]->getServerId();
			u64 file_id = net_files[0]->getFileId();
			taskData = AosTaskDataObj::createTaskDataOutPutIIL(
				server_id, datacol_id, file_id, level+1, crt_block_idx, last_merge);
			aos_assert_r(taskData, false);
			mTask->updateTaskOutPut(taskData, rdata);                                     
		}
		AosActionObjPtr thisptr(this, false);
		mTask->actionFinished(thisptr, rdata);
		mTask = 0;
	    return true;		
	}
	*/

	OmnLocalFilePtr output_file;
	if (!taskData)
	{
		OmnString fname = createFileName(datacol_id, level);
		aos_assert_r(fname != "", false);

		u64 file_id = 0;
		output_file = AosNetFileCltObj::createRaidFileStatic(
			rdata.getPtr(), file_id, fname, 0, false);
		aos_assert_r(output_file && output_file->isGood(), false);

		taskData = AosTaskDataObj::createTaskDataOutPutIIL(
				AosGetSelfServerId(), datacol_id, file_id, level+1, crt_block_idx, last_merge);
		aos_assert_r(taskData, false);

		bool rslt = mTask->updateTaskOutPut(taskData, rdata);
		aos_assert_r(rslt, false);
	}
	else
	{
		AosXmlTagPtr output;
		bool rslt = taskData->serializeTo(output, rdata);
		aos_assert_r(rslt && output, false);
		
		u64 file_id = output->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
		output_file = AosNetFileCltObj::openLocalFileStatic(file_id, rdata.getPtr());
		aos_assert_r(output_file && output_file->isGood(), false);

		output_file->resetFile();
	}

	AosMergeFileSorterPtr sorter = OmnNew AosMergeFileSorter(2000000, comp.getPtr(), net_files, rdata);
	AosActionObjPtr thisptr(this, false);
	OmnThrdShellProcPtr runner = OmnNew AosGenerateOutPutThrd(
			thisptr, sorter, output_file, rdata);
	sgThreadPool->proc(runner); 
	sorter->sort();

	mOutPutSem->wait();

	bool is_delete_file = (task && task->getIsDeleteFile());
	if (is_delete_file && !last_merge)
	{
		int server_id = -1;
		u64 file_id = 0;
		bool rslt, svr_death;
		for(u32 i=0; i<net_files.size(); i++)
		{
			server_id = net_files[i]->getServerId();
			file_id = net_files[i]->getFileId();
			rslt =  AosNetFileCltObj::deleteFileStatic(
				file_id, server_id, svr_death, rdata.getPtr());
			aos_assert_r(rslt, false);
		}
	}
	net_files.clear();

	aos_assert_r(task, false);
	task->actionFinished(thisptr, rdata);
	return true;
}


bool
AosActMergeFile::getFiles(
		vector<AosNetFileObjPtr> &net_files,
		const AosXmlTagPtr &sdoc, 
		const int crt_level)
{
	AosXmlTagPtr fileids_tag = sdoc->getFirstChild("files");
	aos_assert_r(fileids_tag, false);
	
	AosXmlTagPtr tag = fileids_tag->getFirstChild();
	while(tag)
	{
		AosFileInfo info;
		info.serializeFrom(tag);
		aos_assert_r(info.mFileId != 0, false);
		aos_assert_r(info.mPhysicalId != -1, false);
		aos_assert_r(info.mLevel != -1, false);

		if (info.mLevel != crt_level)
		{
			OmnAlarm << "level is error! level:" << info.mLevel << " crt_level:" << crt_level << enderr;
		}
		
		//AosNetFileObjPtr file = OmnNew AosIdNetFile(data_serverid, fileid);
		AosNetFileObjPtr file = OmnNew AosIdNetFile(info);
		aos_assert_r(file, false);

		net_files.push_back(file);
		tag = fileids_tag->getNextChild();
	}
	return true;
}


bool
AosActMergeFile::AosGenerateOutPutThrd::run()
{
	bool rslt = false;
	aos_assert_r(mSorter, false);
	mTmpBuff = OmnNew AosBuff(1000 AosMemoryCheckerArgs);
	while(1)
	{
		AosBuffPtr buff = mSorter->nextBuff();
		if (!buff)
		{
			break;
		}
		rslt = AosMergeFileSorter::sanitycheck(mTmpBuff, buff, mSorter);
		aos_assert_r(rslt, false);

		rslt = mOutPutFile->append(buff->data(), buff->dataLen(), true);
		aos_assert_r(rslt, false);
	}


	mCaller->outPutPost();
	return true;
}


bool
AosActMergeFile::AosGenerateOutPutThrd::procFinished()
{
	return true;
}

void
AosActMergeFile::outPutPost()
{
	mOutPutSem->post();
}

AosActionObjPtr
AosActMergeFile::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);
	try
	{
		return OmnNew AosActMergeFile(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
