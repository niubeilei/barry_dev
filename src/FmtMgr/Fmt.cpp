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
#include "FmtMgr/Fmt.h"

#include "Util/Buff.h"
#include "StorageMgr/FileId.h"
#include "StorageMgr/SystemId.h"
#include "SEInterfaces/FmtMgrObj.h"


bool AosFmt::smShowLog = false;

AosFmt::AosFmt()
:
mFmtId(0)
{
	mFmtBuff = OmnNew AosBuff(100, 0 AosMemoryCheckerArgs);
}


AosFmt::AosFmt(const AosBuffPtr &fmt_buff)
:
mFmtBuff(fmt_buff)
{
	mFmtBuff->setCrtIdx(0);
	mFmtId = mFmtBuff->getU64(0);
	aos_assert(mFmtId);
}


AosFmt::~AosFmt()
{
}


bool
AosFmt::add(const AosStorageFileInfo &file_info)
{
	aos_assert_r(mFmtBuff, false);
	
	u64 file_id = file_info.fileId;
	u32 crt_len = mFmtBuff->dataLen();
	u32 crt_idx = crt_len < eEntryStartOff ? eEntryStartOff : crt_len;
	mFmtBuff->setCrtIdx(crt_idx);

	mFmtBuff->setU32(eCreateFile);
	mFmtBuff->setU64(file_id);
	mFmtBuff->setU32(file_info.siteId);
	//mFmtBuff->setU32(file_info.deviceId);
	mFmtBuff->setU64(file_info.requestSize);
	mFmtBuff->setOmnStr(file_info.fname);
	
	if(smShowLog)
	{
		OmnScreen << "Fmt; create_file:"
			<< "; fileId:" << file_id 
			<< "; fname:" << file_info.fname 
			<< "; virtual_id:" << (file_id >> 32)
			<< endl;
	}
	return true;
}


bool
AosFmt::add(
		const u64 file_id,
		const u64 offset,
		const char *data,
		const int len,
		const bool flushflag)
{
	aos_assert_r(mFmtBuff, false);

	u32 crt_len = mFmtBuff->dataLen();
	u32 crt_idx = crt_len < eEntryStartOff ? eEntryStartOff : crt_len;
	mFmtBuff->setCrtIdx(crt_idx);

	mFmtBuff->setU32(eModifyFile);
	mFmtBuff->setU64(file_id);
	mFmtBuff->setU64(offset);
	mFmtBuff->setInt(len);
	mFmtBuff->setBuff(data, len);
	mFmtBuff->setU8(flushflag);

	if(smShowLog)
	{
		OmnScreen << "Fmt; modify_file:"
			<< "; fileId:" << file_id 
			<< "; offset:" << offset
			<< "; len:" << len
			<< "; virtual_id:" << (file_id >> 32) 
			<< endl;
	}
	return true;
}


bool
AosFmt::addDeleteFile(const u64 file_id)
{
	aos_assert_r(mFmtBuff, false);

	u32 vid = file_id >> 32;
	if (vid == AOS_TEMPFILE_VIRTUALID) return true;

	u32 crt_len = mFmtBuff->dataLen();
	u32 crt_idx = crt_len < eEntryStartOff ? eEntryStartOff : crt_len;
	mFmtBuff->setCrtIdx(crt_idx);

	mFmtBuff->setU32(eDeleteFile);
	mFmtBuff->setU64(file_id);
	
	return true;
}


bool
AosFmt::add(vector<AosTransId> &trans_ids)
{
	aos_assert_r(mFmtBuff, false);
	
	u32 crt_len = mFmtBuff->dataLen();
	u32 crt_idx = crt_len < eEntryStartOff ? eEntryStartOff : crt_len;
	mFmtBuff->setCrtIdx(crt_idx);

	mFmtBuff->setU32(eTransIds);
	mFmtBuff->setU32(trans_ids.size());
	for(u32 i=0; i<trans_ids.size(); i++)
	{
		trans_ids[i].serializeTo(mFmtBuff);
	}
	//mTransIds = trans_ids;
	vector<AosTransId>::iterator itr = mTransIds.end();
	mTransIds.insert(itr, trans_ids.begin(), trans_ids.end());

	return true;
}


bool
AosFmt::appendFmt(const AosFmtPtr &fmt)
{
	aos_assert_r(fmt, false);	
	AosBuffPtr fmt_buff = fmt->getFmtBuff();
	if(!fmt_buff || fmt_buff->dataLen() < eEntryStartOff)	return true;

	u32 crt_len = mFmtBuff->dataLen();
	u32 crt_idx = crt_len < eEntryStartOff ? eEntryStartOff : crt_len;
	mFmtBuff->setCrtIdx(crt_idx);
	
	char * data = fmt_buff->data() + eEntryStartOff;
	int len = fmt_buff->dataLen() - eEntryStartOff;
	aos_assert_r(len >0, false);
	mFmtBuff->setBuff(data, len);

	if(smShowLog)
	{
		OmnScreen << "Fmt; append Fmt:"
			<< "; crt_len:" << crt_len
			<< "; this:" << this
			<< "; append_len:" << len
			<< "; append_fmt:" << fmt.getPtr()
			<< "; new_len:" << mFmtBuff->dataLen()
			<< ";" << endl;
	}	
	return true;
}


void
AosFmt::setFmtId(const u64 fmt_id)
{ 
	mFmtId = fmt_id; 
	aos_assert(mFmtBuff);
	
	mFmtBuff->setCrtIdx(eFmtIdOff);
	mFmtBuff->setU64(mFmtId);
}


u64
AosFmt::getFmtIdFromBuff(const AosBuffPtr &buff)
{
	u32 crt_idx = buff->getCrtIdx();
	buff->setCrtIdx(crt_idx + eFmtIdOff);
	
	u64 fmt_id = buff->getU64(0);
	buff->setCrtIdx(crt_idx);
	return fmt_id;
}


AosFmtPtr
AosFmt::getNextFmt(const AosBuffPtr &cont)
{
	aos_assert_r(cont, 0);
	u32 fmt_size = cont->getU32(0);
	if(!fmt_size)	return 0;

	int remain_size = cont->dataLen() - cont->getCrtIdx();
	if(remain_size < (int)fmt_size)
	{
		OmnScreen << "read fmt failed. remain_size:" << remain_size
			<< "; fmt_size:" << fmt_size << endl;
		return 0;
	}

	AosBuffPtr fmt_buff = cont->getBuff(fmt_size, true AosMemoryCheckerArgs);
	AosFmtPtr fmt = OmnNew AosFmt(fmt_buff);
	return fmt;
}


AosFmt::FmtOpr
AosFmt::nextFmtOpr()
{
	aos_assert_r(mFmtBuff, eInvalid);
	if(mFmtBuff->getCrtIdx() < eEntryStartOff)
	{
		mFmtBuff->setCrtIdx(eEntryStartOff);	
	}
	
	if(mFmtBuff->getCrtIdx() >= mFmtBuff->dataLen())	return eInvalid;
	
	FmtOpr opr = (FmtOpr)mFmtBuff->getU32(0);
	aos_assert_r(opr > eInvalid && opr < eMax, eInvalid);
	return opr;	
}


bool 
AosFmt::getOprInfo(AosStorageFileInfo &file_info)
{
	bool rslt = getOprInfoCheck(eCreateFile);
	aos_assert_r(rslt, false);

	file_info.fileId = mFmtBuff->getU64(0);
	file_info.siteId = mFmtBuff->getU32(0);
	//file_info.deviceId = mFmtBuff->getInt(-1);
	file_info.requestSize = mFmtBuff->getU64(0);
	file_info.fname = mFmtBuff->getOmnStr("");
	aos_assert_r(file_info.fileId && file_info.siteId, false);
	return true;
}

	
bool 
AosFmt::getOprInfo(
		u64 &file_id, 
		u64 &file_offset, 
		AosBuffPtr &data, 
		bool &flushflag)
{
	bool rslt = getOprInfoCheck(eModifyFile);
	aos_assert_r(rslt, false);
	
	file_id = mFmtBuff->getU64(0);
	file_offset = mFmtBuff->getU64(0);
	int len = mFmtBuff->getInt(-1);
	data = mFmtBuff->getBuff(len, true AosMemoryCheckerArgs);
	flushflag = mFmtBuff->getU8(0);
	return true;
}


bool 
AosFmt::getOprInfo(vector<AosTransId> &trans_ids)
{
	bool rslt = getOprInfoCheck(eTransIds);
	aos_assert_r(rslt, false);

	u32 num = mFmtBuff->getU32(0);
	for(u32 i=0; i<num; i++)
	{
		AosTransId tid = AosTransId::serializeFrom(mFmtBuff);
		trans_ids.push_back(tid);
	}
	return true;
}

	
bool
AosFmt::getOprInfo(u64 &file_id)
{
	bool rslt = getOprInfoCheck(eDeleteFile);
	aos_assert_r(rslt, false);
	
	file_id = mFmtBuff->getU64(0);
	return true;
}

bool
AosFmt::getOprInfoCheck(const FmtOpr &check_opr)
{
	aos_assert_r(mFmtBuff, false);

	u32 crt_idx = mFmtBuff->getCrtIdx();
	aos_assert_r(crt_idx > eEntryStartOff && crt_idx > 4, false); 

	mFmtBuff->setCrtIdx(crt_idx - 4);
	FmtOpr opr = (FmtOpr)mFmtBuff->getU32(0);
	aos_assert_r(opr == check_opr, false);
	
	return true;
}

