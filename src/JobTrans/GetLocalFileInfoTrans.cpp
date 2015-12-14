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
// 2014/08/15	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/GetLocalFileInfoTrans.h"

#include "DocFileMgr/RaidFile.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "XmlUtil/XmlTag.h"


AosGetLocalFileInfoTrans::AosGetLocalFileInfoTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetLocalFileInfo, regflag)
{
}


AosGetLocalFileInfoTrans::AosGetLocalFileInfoTrans(
		const int svr_id,
		const u64 &fileId)
:
AosTaskTrans(AosTransType::eGetLocalFileInfo, svr_id, false, true),
mFileId(fileId)
{
}


AosGetLocalFileInfoTrans::~AosGetLocalFileInfoTrans()
{
}


bool
AosGetLocalFileInfoTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mFileId = buff->getU64(0);

	return true;
}


bool
AosGetLocalFileInfoTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mFileId);

	return true;
}


AosTransPtr
AosGetLocalFileInfoTrans::clone()
{
	return OmnNew AosGetLocalFileInfoTrans(false);
}


bool
AosGetLocalFileInfoTrans::proc()
{
	OmnLocalFilePtr local_file = AosNetFileMgrObj::openLocalFileStatic(
		mFileId, mRdata.getPtr());
	
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);

	if (local_file && local_file->isGood())
	{
		OmnString filetype = local_file->getFileType();

		resp_buff->setU8(true);

		if (filetype == "AosRaidFile")
		{
			AosRaidFile * file = dynamic_cast<AosRaidFile *>(local_file.getPtr());
			bool rslt = file->serializeTo(resp_buff);
			if (!rslt) 
			{
				resp_buff->reset();
				resp_buff->setU8(false);
				resp_buff->setDataLen(sizeof(u8));
			}
		}
		else if (filetype == "OmnFile")
		{
			OmnString filename = local_file->getFileName();
			resp_buff->setOmnStr(filename);
		}
		else
		{
			resp_buff->reset();
			resp_buff->setU8(false);
			resp_buff->setDataLen(sizeof(u8));
		}
	}
	else
	{
		resp_buff->setU8(false);
	}

	sendResp(resp_buff);
	return true;
}

