////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/06/18	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DfmLog.h"

#include "API/AosApi.h"  
#include "DocFileMgr/DfmLogProcThrd.h"  
#include "DocFileMgr/SlabBlockFile.h"  
#include "DocFileMgr/DocFileMgr.h"
#include "DocFileMgr/DfmHeader.h"
#include "DocFileMgr/DfmBody.h"
#include "DocFileMgr/DfmLogUtil.h"
#include "DocFileMgr/FileGroupMgr.h"
#include "DocFileMgr/DfmLogFile.h"
#include "FmtMgr/Fmt.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/FmtMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"

AosDfmLogProcThrdPtr AosDfmLog::smProcThrdObj = OmnNew AosDfmLogProcThrd();

AosDfmLog::AosDfmLog(
		const AosRundataPtr &rdata,
		const AosDfmDocType::E doc_type,
		const AosDocFileMgrObjPtr &mgr, 
		const u32 virtual_id,
		const u32 dfm_id,
		const bool show_log)
:
mVirtualId(virtual_id),
mDfmId(dfm_id),
mDocFileMgr(mgr),
mBody(0),
mHeader(0),
mShowLog(show_log)
{
	u32 maxFileSize = smProcThrdObj->getMaxFileSize(); 

	AosDocFileMgr * docmgr = (AosDocFileMgr *)mDocFileMgr.getPtr();
	mHeader = docmgr->getDfmHeaderObj();
	aos_assert(mHeader);

	mBody = docmgr->getDfmBodyObj();
	aos_assert(mBody);

	AosDfmLogUtil::createVidDir(mVirtualId);
	AosDfmLogUtil::createNormDir(mVirtualId);

	AosDfmLogPtr thisPtr(this, false);
	mDfmLogFile = OmnNew AosDfmLogFile(doc_type, 
			maxFileSize, mVirtualId, mDfmId, thisPtr, show_log);
}


AosDfmLog::~AosDfmLog()
{
}


bool
AosDfmLog::config(const AosXmlTagPtr &app_config)
{
	return smProcThrdObj->config(app_config);
}


AosDfmDocPtr
AosDfmLog::readDoc(
		const AosRundataPtr &rdata,
		const u64 docid)
{
	AosDfmDocPtr doc = mDfmLogFile->readDoc(rdata, docid);
	return doc;
}


bool
AosDfmLog::addEntry(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc,
		vector<AosTransId> &trans_ids)
{
	bool rslt = mDfmLogFile->addEntry(rdata, doc, trans_ids);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDfmLog::addEntrys(
		const AosRundataPtr &rdata,
		map<u64, u64> &index_map,
		const AosBuffPtr &buff)
{
	bool rslt = mDfmLogFile->addEntrys(rdata, index_map, buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDfmLog::addRequest(
		const AosRundataPtr &rdata,
		const u64 &file_id)
{
	AosDfmLogPtr thisPtr(this, false);
	return smProcThrdObj->addRequest(rdata, thisPtr, file_id);
}


bool
AosDfmLog::saveDoc(
		const AosRundataPtr &rdata,
		const u64 &file_id,
		const bool init_flag)
{
	if (!init_flag) aos_assert_r(mDfmLogFile->readFileIdFromQueue(file_id), false);

	bool rslt = saveDocPriv(rdata, file_id);
	aos_assert_r(rslt, false);

	if (!init_flag) aos_assert_r(mDfmLogFile->removeFileIdFromQueue(file_id), false);

	mDfmLogFile->removeFile(file_id);
	return true;
}


bool
AosDfmLog::saveDocPriv(
		const AosRundataPtr &rdata,
		const u64 &file_id)
{
	//Linda, 2013/08/22 LINDAFMT
	//// fmt create 
	AosFmtPtr fmt = OmnNew AosFmt();
	AosFmtMgrObjPtr fmt_mgr;
	if(AosIsSelfCubeSvr()) 
	{
		fmt_mgr = AosFmtMgrObj::getFmtMgr();
		aos_assert_r(fmt_mgr, false);
		fmt_mgr->setFmt(fmt);
	}

	bool no_send_fmt = false;
	AosDfmCompareFunType::set_f vv;
	bool rslt = readDfmLogFile(rdata, file_id, vv, no_send_fmt);
	aos_assert_r(rslt, false);

	rslt = mHeader->readHeaders(rdata, vv);
	aos_assert_r(rslt, false);

	aos_assert_r(vv.size() > 0, false);
	// sort by seqno, offset, opr, size
	AosDfmCompareFunType::set_fs docs;
	docs.insert(vv.begin(), vv.end());
	aos_assert_r(docs.size() == vv.size(), false);

	rslt = mBody->procBody(rdata, mDocFileMgr, docs);
	aos_assert_r(rslt, false);
	
	rslt = mHeader->saveHeaders(rdata, mDocFileMgr, vv);
	aos_assert_r(rslt, false);
	
	if(AosIsSelfCubeSvr())
	{
		//if (no_send_fmt)
		{
			//Linda, 2013/08/22 LINDAFMT
			//// fmt finish
			aos_assert_r(fmt_mgr, false);
			fmt_mgr->sendFmt(rdata, fmt);
			fmt_mgr->setFmt(0);
		}
		
		// finish trans.
		vector<AosTransId>  trans_ids = fmt->getTransIds();
		AosFinishTrans(trans_ids);
	}
	OmnScreen << "QQQ vid:" << mVirtualId << "; finished file_id: " << file_id << endl;
	return true;
}


bool
AosDfmLog::readDfmLogFile(
		const AosRundataPtr &rdata,
		const u64 &file_id,
		AosDfmCompareFunType::set_f &vv,
		bool &no_send_fmt)
{
	//Linda, 2013/08/22 LINDAFMT
	AosFmtPtr fmt;
	if(AosIsSelfCubeSvr())
	{
		fmt = AosFmtMgrObj::getFmtMgr()->getFmt();
		aos_assert_r(fmt, false);
	}

	AosBuffPtr buff = mDfmLogFile->readDfmLogFile(file_id, true, no_send_fmt);
	aos_assert_r(buff, false);
	buff->reset();
	int64_t buff_len = buff->dataLen();
	while(buff->getCrtIdx() < buff_len)
	{
		u32 rcd_total = buff->getU32(0);
		if(rcd_total == 0 || (u64)rcd_total > buff_len)
		{
			OmnAlarm << " fileid == 0" << enderr;
			return false;
		}

		int64_t crt_idx = buff->getCrtIdx();
		vector<AosTransId> trans_ids;
		AosDfmDocPtr doc = AosDfmLogUtil::getDocInfoFromBuff(mDfmLogFile->getTempDoc(), buff, trans_ids);
		if(!doc)
		{
			OmnAlarm << "failed read file" << enderr; 
			return false;
		}
		aos_assert_r(doc.getPtr(), false);

		if(AosIsSelfCubeSvr())
		{
			//Linda, 2013/08/22 LINDAFMT
			fmt->add(trans_ids);
		}

		buff->setCrtIdx(crt_idx + rcd_total);

		pair<AosDfmCompareFunType::setitr_f, bool> pr;
		pr = vv.insert(doc);
		if (pr.second) continue;

		if ((*pr.first)->getOpr() == AosDfmDoc::eSave)
		{
			vv.erase(*pr.first);
			if (doc->getOpr() == AosDfmDoc::eSave)
			{
				vv.insert(doc);
			}
		}
		else
		{
			OmnScreen << "failed delete!" << endl;
			//OmnAlarm << "failed delete!" << enderr;
		}

	}
	return true;
}


bool
AosDfmLog::init(
		const AosRundataPtr &rdata,
		const u32 &virtual_id,
		const AosFileGroupMgrPtr &fgm)
{
	if (OmnApp::eNewDFMVersion == OmnApp::getDFMVersion()) return true;

	vector<OmnString> file_path;
	AosDfmLogUtil::getNormDirFileNames(rdata, file_path, virtual_id);

	sort(file_path.begin(), file_path.end());
	for (u32 i = 0; i < file_path.size(); i++)
	{
		OmnString fname = file_path[i];
		aos_assert_r(fname != "" , false);
		OmnScreen << "LLLLLLLLLLLLLLLLLLLLLLLLLLLLL: " << virtual_id << ";" << fname << endl;

		OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
		aos_assert_r(file && file->isGood(), false);
		
		file->lock();
		u32 dfm_id = file->readBinaryU32(AosDfmLogFile::eDfmIdOff, 0);
		u64 file_id = file->readBinaryU64(AosDfmLogFile::eFileIdOff, 0);
		file->unlock();
		aos_assert_r(file_id, false);

		AosDocFileMgrObjPtr dfm = fgm->retrieveDfm(rdata, dfm_id); 
		aos_assert_r(dfm, false);
		bool rslt = dfm->recoverDfmLog(rdata, file_id);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosDfmLog::recoverDfmLog(
		const AosRundataPtr &rdata,
		const u64 &file_id)
{
	return saveDoc(rdata, file_id, true);
}


bool
AosDfmLog::startStop(const AosRundataPtr &rdata)
{
	OmnScreen << "DfmLog start stop virtualid: " << mVirtualId << endl;
	mDfmLogFile->addCrtRequest(rdata);
	return true;
}


bool
AosDfmLog::stop()
{
	smProcThrdObj->stop();
	return true;
}

bool
AosDfmLog::continueProcCache()
{
	return smProcThrdObj->continueProcCache();
}

bool
AosDfmLog::stopProcCache()
{
	return smProcThrdObj->stopProcCache();
}


bool
AosDfmLog::clear(
		const AosRundataPtr &rdata,
		const u32 &virtual_id)
{
	vector<OmnString> file_path;
	AosDfmLogUtil::getNormDirFileNames(rdata, file_path, virtual_id);

	sort(file_path.begin(), file_path.end());
	for (u32 i = 0; i < file_path.size(); i++)
	{
		OmnString fname = file_path[i];
		aos_assert_r(fname != "" , false);
		OmnScreen << "clear LLLLLLLLLLLLLLLLLLLLLLLLLLLLL: " << virtual_id << ";" << fname << endl;

		OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
		aos_assert_r(file && file->isGood(), false);
		
		file->lock();
		u32 flag = file->readBinaryU32(AosDfmLogFile::eFlagOff, 0);
		u64 file_id = file->readBinaryU64(AosDfmLogFile::eFileIdOff, 0);
		file->unlock();
		
		// ketty 2013/12/14
		OmnScreen << "removeFile file_id:" << file_id << "; fname:" << file->getFileName() << endl;
		//AosVfsMgrObj::getVfsMgr()->removeFile(file_id, rdata);	
		AosDfmLogUtil::removeFile(file_id);
		/*
		if (flag == 0)
		{
			OmnScreen << "removeFile file_id:" << file_id << "; fname:" << file->getFileName() << endl;
			//AosVfsMgrObj::getVfsMgr()->removeFile(file_id, rdata);	
			AosDfmLogUtil::removeFile(file_id);
		}
		else
		{
			file->setU32(AosDfmLogFile::eFmtFlagOff, 1, true);
			OmnScreen << " llllllllllllllllllll file_id: " << file_id << " , flag: " << flag << endl;
		}
		*/
	}

	return true;
}


u32
AosDfmLog::getMaxFileSize()
{
	return smProcThrdObj->getMaxFileSize();
}


bool
AosDfmLog::flushContents(const AosRundataPtr &rdata)
{
	OmnScreen << "DfmLog start flush virtualid: " << mVirtualId << endl;
	mDfmLogFile->addCrtRequest(rdata);
	smProcThrdObj->flushContentsByVirtualId(mVirtualId);
	OmnScreen << "DfmLog stop flush virtualid: " << mVirtualId << endl;
	return true;
}



//Linda, 2014/03/04
//bool
//AosDfmLog::commit(
//		const AosRundataPtr &rdata, 
//		const AosDfmLogFilePtr &dfmlogfile)
//{
//	return mDfmLogFile->commit(rdata, dfmlogfile);
//}
