////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// 06/17/2013	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "FmtMgr/FmtSvr.h"

#include "FmtMgr/Fmt.h"
#include "FmtMgr/FmtFile.h"
#include "StorageMgr/SystemId.h"


AosFmtSvr::AosFmtSvr(const bool is_master)
:
mIsMaster(is_master)
{
}

AosFmtSvr::~AosFmtSvr()
{
}

bool
AosFmtSvr::config(const AosXmlTagPtr &app_conf)
{
	AosXmlTagPtr conf = app_conf->getFirstChild(AOSCONFIG_TRANS_SERVER);
	if(conf) mShowLog = conf->getAttrBool(AOSCONFIG_SHOWLOG, false);
	AosFmt::setShowLog(mShowLog);

	mDirname = OmnApp::getAppBaseDir() << "FmtLog";
	OmnApp::createDir(mDirname);
	
	OmnString fname = "cube_grp_";
	fname << AosGetSelfCubeGrpId();
	mFmtFile = OmnNew AosFmtFile(mDirname, fname, eMaxLogFileSize, mShowLog);
	bool rslt = mFmtFile->init();
	aos_assert_r(rslt, false);

	//initSvrIds();

	mRdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRdata->setSiteid(AOS_SYS_SITEID);
	
	return true;
}

bool
AosFmtSvr::start()
{
	return true;
}


bool
AosFmtSvr::stop()
{
	return true;
}

bool
AosFmtSvr::switchFrom(const AosFmtSvrPtr &from)
{
	mDirname = from->mDirname;
	mFmtFile = from->mFmtFile;
	
	mRdata = from->mRdata;
	mShowLog = from->mShowLog;
	return true;
}


u64
AosFmtSvr::getMaxFmtIdInFile()
{
	int max_fseq = mFmtFile->endSeq();
	if(max_fseq < 0)	return 0;
	
	return mFmtFile->getMaxFmtIdByFileSeq(max_fseq);
}


bool
AosFmtSvr::readFmtBySync(
		const AosRundataPtr &rdata,
		const u32 cube_grp_id,
		const u64 beg_fmt_id,
		const u64 end_fmt_id,
		int &file_seq,
		bool &finish,
		AosBuffPtr &fmt_buff)
{
	aos_assert_r(cube_grp_id == (u32)AosGetSelfCubeGrpId(), false);
	
	if(mFmtFile->empty())
	{
		finish = true;
		return true;
	}

	fmt_buff = OmnNew AosBuff(eMaxLogFileSize, 0 AosMemoryCheckerArgs);
	while(1)
	{
		bool rslt = readFmtBySyncPriv(rdata, beg_fmt_id, end_fmt_id,
				file_seq, finish, fmt_buff);
		aos_assert_r(rslt, false);
		if(finish || fmt_buff->dataLen())	break;
	}

	return true;
}

	
bool
AosFmtSvr::readFmtBySyncPriv(
		const AosRundataPtr &rdata,
		const u64 beg_fmt_id,
		const u64 end_fmt_id,
		int &file_seq,
		bool &finish,
		AosBuffPtr &fmt_buff)
{
	int next_seq = mFmtFile->getNextSeqno(file_seq);	
	aos_assert_r(next_seq >=-1, false);
	if(next_seq == -1)
	{
		finish = true;
		return true;
	}

	if(file_seq != -1) aos_assert_r(next_seq == file_seq + 1, false);
	file_seq = next_seq;

	int active_seq = mFmtFile->activeSeq();
	if(file_seq == active_seq) mFmtFile->startNextActive();

	finish = false;
	AosBuffPtr cont;
	bool rslt = mFmtFile->readNormFile(file_seq, cont);	
	aos_assert_r(rslt && cont, false);

	bool check = true;

	u32 fmt_len;
	u64 crt_fmt_id = 0;
	u32 crt_idx;
	while(1)
	{
		fmt_len = cont->getU32(0);
		if(fmt_len == 0)	break;
	
		crt_idx = cont->getCrtIdx();
		crt_fmt_id = AosFmt::getFmtIdFromBuff(cont);
		aos_assert_r(crt_fmt_id, false);
		cont->setCrtIdx(crt_idx + fmt_len);
	
		if(crt_fmt_id < beg_fmt_id)	continue;
		
		if(check)
		{
			check = false;
			if(beg_fmt_id !=0 && beg_fmt_id != crt_fmt_id)
			{
				OmnScreen << "!!! need beg_fmt_id:" << beg_fmt_id
					<< "; but crt_fmt_id:" << crt_fmt_id << endl;
			}
		}

		fmt_buff->setU32(fmt_len);
		char *data = cont->data() + crt_idx;
		fmt_buff->setBuff(data, fmt_len);

		if(crt_fmt_id >= end_fmt_id)
		{
			// add this buff to fmt_buff. is for check.
			finish = true;
			break;
		}
	}
	return true;
}


/*
bool
AosFmtSvr::readFmtBySyncPriv(
		const AosRundataPtr &rdata,
		const u64 beg_fmt_id,
		const u64 end_fmt_id,
		int &file_seq,
		bool &finish,
		AosBuffPtr &fmt_buff)
{
	if(file_seq == -1)
	{
		// file_seq == -1. means start read log.
		file_seq = mFmtFile->begSeq();
	}
	else
	{
		int next_seq = mFmtFile->getNextSeqno(file_seq);	
		aos_assert_r(next_seq >=-1, false);
		if(next_seq == -1)
		{
			finish = true;
			return true;
		}

		aos_assert_r(next_seq == file_seq + 1, false);
		file_seq = next_seq;
	}

	int active_seq = mFmtFile->activeSeq();
	if(file_seq == active_seq) mFmtFile->startNextActive();

	finish = false;
	AosBuffPtr cont;
	bool rslt = mFmtFile->readNormFile(file_seq, cont);	
	aos_assert_r(rslt && cont, false);

	bool check = true;
	while(1)
	{
		AosFmtPtr fmt = AosFmt::getNextFmt(cont);
		if(!fmt) break;
	
		u64 fmt_id = fmt->getFmtId();
		if(fmt_id < beg_fmt_id)	continue;
	
		if(check)
		{
			check = false;
			if(beg_fmt_id !=0 && beg_fmt_id != fmt_id)
			{
				OmnScreen << "!!! need beg_fmt_id:" << beg_fmt_id
					<< "; but fmt_id:" << fmt_id << endl;
			}
		}
		
		fmt_buff->setBuff(fmt->getFmtBuff());

		if(end_fmt_id != 0 && fmt_id >= end_fmt_id)
		{
			// add this buff to fmt_buff. is for check.
			finish = true;
			break;
		}
	}
	return true;
}
*/


bool
AosFmtSvr::recvFinishedFmtId(const u64 max_fmt_id)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosFmtSvr::getNextSendFmtId(
		const AosRundataPtr &rdata,
		const u32 cube_grp_id,
		const int from_svr_id,
		u64 &next_fmt_id)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosFmtSvr::sendFmt(
		const AosRundataPtr &rdata,
		const AosFmtPtr	&fmt)
{
	OmnShouldNeverComeHere;
	return false;
}


void
AosFmtSvr::setFmt(const AosFmtPtr &fmt)
{
	OmnShouldNeverComeHere;
}

AosFmtPtr
AosFmtSvr::getFmt()
{
	OmnShouldNeverComeHere;
	return 0;
}

bool
AosFmtSvr::addToGlobalFmt(
		const AosRundataPtr &rdata, 
		const u64 file_id,
		const u64 offset,
		const char *data,
		const int len,
		const bool flushflag)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosFmtSvr::addToGlobalFmt(
		const AosRundataPtr &rdata,
		const AosStorageFileInfo &file_info)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosFmtSvr::addDeleteFileToGlobalFmt(
		const AosRundataPtr &rdata, 
		const u64 &file_id)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosFmtSvr::addToGlobalFmt(
		const AosRundataPtr &rdata, 
		const AosFmtPtr &fmt)
{
	OmnShouldNeverComeHere;
	return false;
}


