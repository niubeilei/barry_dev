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
//
// Modification History:
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SysMsg/RecoverFileTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEUtil/SeConfig.h"
#include "Util/File.h"

AosRecoverFileTrans::AosRecoverFileTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eRecoverFile, regflag)
{
	setIsSystemTrans(); 
}


AosRecoverFileTrans::AosRecoverFileTrans(
		const int svr_id,
		const u32 cube_grp_id,
		const u64 file_id,
		const u64 file_off)
:
AosTaskTrans(AosTransType::eRecoverFile, svr_id,
		AosProcessType::eCube, cube_grp_id, false, true),
mFileId(file_id),
mFileOff(file_off)
{
	setIsSystemTrans(); 
}


AosRecoverFileTrans::~AosRecoverFileTrans()
{
}


bool
AosRecoverFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mFileId = buff->getU64(0);
	mFileOff = buff->getU64(0);
	return true;
}


bool
AosRecoverFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mFileId);
	buff->setU64(mFileOff);
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosRecoverFileTrans::clone()
{
	return OmnNew AosRecoverFileTrans(false);
}


bool
AosRecoverFileTrans::proc()
{
	bool finish;
	AosBuffPtr data;
	AosBuffPtr resp_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	bool rslt = readFile(data, finish);
	if(!rslt)
	{
		resp_buff->setU8(false);	
		return sendResp(resp_buff);
	}
	
	resp_buff->setU8(true);
	resp_buff->setU8(finish);
	if(data)
	{
		resp_buff->setU64(data->dataLen());
		resp_buff->setBuff(data);
	}
	else
	{
		resp_buff->setU64(0);
	}

	sendResp(resp_buff);
	return true;
}


bool
AosRecoverFileTrans::readFile(AosBuffPtr &data, bool &finish)
{
	AosVfsMgrObjPtr vfs_mgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfs_mgr, false);
	
	OmnFilePtr ff = vfs_mgr->openRawFile(mFileId, mRdata.getPtr());
	aos_assert_r(ff && ff->isGood(), false);
	
	ff->lock();
	u32 file_len = ff->getLength();
	if(mFileOff > file_len)
	{
		ff->unlock();
		finish = true;	
		return true;	
	}

	u64 remain = file_len - mFileOff;
	finish = (remain < eAosSizePerRead);
	
	u32 read_size = remain < eAosSizePerRead ? remain : eAosSizePerRead; 
	data = OmnNew AosBuff(read_size, 0 AosMemoryCheckerArgs);	
	int bytes = ff->readToBuff(mFileOff, read_size, data->data());
	ff->unlock();

	aos_assert_r((u32)bytes == read_size, false);
	data->setDataLen(read_size);
	return true;
}


