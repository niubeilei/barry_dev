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
// 01/09/2014	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/GetDirListTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"


AosGetDirListTrans::AosGetDirListTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetDirList, regflag)
{
}


AosGetDirListTrans::AosGetDirListTrans(
		const OmnString &path,
		const int svr_id)
:
AosTaskTrans(AosTransType::eGetDirList, svr_id, false, true),
mPath(path)
{
}


AosGetDirListTrans::~AosGetDirListTrans()
{
}


bool
AosGetDirListTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mPath = buff->getOmnStr("");
	return true;
}


bool
AosGetDirListTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setOmnStr(mPath);
	return true;
}


AosTransPtr
AosGetDirListTrans::clone()
{
	return OmnNew AosGetDirListTrans(false);
}


bool
AosGetDirListTrans::proc()
{
	vector<AosDirInfo> dir_list;
	bool rslt = AosNetFileMgrObj::getDirListLocalStatic(mPath, dir_list, mRdata.getPtr());

	int num = dir_list.size();
	AosBuffPtr temp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);

	if (rslt)
	{
		for(int i=0; i<num; i++)
		{
			rslt = dir_list[i].serializeTo(temp_buff);
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

