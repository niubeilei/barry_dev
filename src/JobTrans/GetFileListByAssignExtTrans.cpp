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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/GetFileListByAssignExtTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosGetFileListByAssignExtTrans::AosGetFileListByAssignExtTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetFileListByAssignExt, regflag)
{
}


AosGetFileListByAssignExtTrans::AosGetFileListByAssignExtTrans(
		const OmnString &ext,
		const OmnString &path,
		const int svr_id,
		const int rec_level)
:
AosTaskTrans(AosTransType::eGetFileListByAssignExt, svr_id, false, true),
mExt(ext),
mPath(path),
mRecLevel(rec_level)
{
}


AosGetFileListByAssignExtTrans::~AosGetFileListByAssignExtTrans()
{
}


bool
AosGetFileListByAssignExtTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mExt = buff->getOmnStr("");
	mPath = buff->getOmnStr("");
	mRecLevel = buff->getInt(-1);
	return true;
}


bool
AosGetFileListByAssignExtTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mExt);
	buff->setOmnStr(mPath);
	buff->setInt(mRecLevel);
	return true;
}


AosTransPtr
AosGetFileListByAssignExtTrans::clone()
{
	return OmnNew AosGetFileListByAssignExtTrans(false);
}


bool
AosGetFileListByAssignExtTrans::proc()
{
	vector<AosFileInfo> file_list;
	bool rslt = AosNetFileMgrObj::getFileListByAssignExtLocalStatic(
		mExt, mPath, mRecLevel, file_list, mRdata.getPtr());

	int num = file_list.size();
	AosBuffPtr temp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);

	if (rslt)
	{
		for(int i=0; i<num; i++)
		{
			rslt = file_list[i].serializeTo(temp_buff);
			if (!rslt) break;
		}
	}

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	if(rslt)
	{
		resp_buff->setInt(num);
		resp_buff->setBuff(temp_buff);
	}

	sendResp(resp_buff);
	return true;
}

