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
// 2013/07/12	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/CubeGroup.h"

#include "Util/StrParser.h"

AosCubeGroup::AosCubeGroup(const AosXmlTagPtr &conf)
{
	bool rslt = config(conf);
	if(!rslt)
	{
		OmnThrowException("error!");
	}
}

AosCubeGroup::AosCubeGroup(const int group_id, const vector<u32> &cube_ids)
:
mGroupId(group_id),
mCubeIds(cube_ids)
{
}


AosCubeGroup::~AosCubeGroup()
{
}


bool
AosCubeGroup::config(const AosXmlTagPtr &conf)
{
	mGroupId = conf->getAttrInt("grp_id", -1);
	aos_assert_r(mGroupId >=0, false);
	
	OmnString str_vids = conf->getAttrStr("vids", "");
	if(str_vids == "")	return false;
	
	OmnStrParser1 parser(str_vids, ", ");
	OmnString svid;
	while ((svid = parser.nextWord()) != "")
	{
		u32 vid = atoi(svid.data());
		aos_assert_r(vid < eAosMaxVirtualId, false);

		mCubeIds.push_back(vid);
	}
	return true;
}

void
AosCubeGroup::addSvrId(const u32 svr_id)
{
	mSvrIds.push_back(svr_id);
}

int
AosCubeGroup::getNextSvrId(const int crt_sid)
{
	if(crt_sid == -1)
	{
		if(mSvrIds.size() == 0)	return -1;
		return mSvrIds[0];
	}

	u32 idx = 0;
	for(idx =0; idx<mSvrIds.size(); idx++)
	{
		if((u32)crt_sid == mSvrIds[idx])
		{
			idx++;
			break;	
		}
	}
	if(idx == mSvrIds.size())	return -1;
	
	return mSvrIds[idx];
}

/*
void
AosCubeGroup::addSvrProc(const u32 svr_id, const u32 proc_id)
{
	mSvrProcMap.insert(make_pair(svr_id, proc_id));
}


bool
AosCubeGroup::getSvrIds(vector<u32> &svr_ids)
{
	map<u32, u32>::iterator itr = mSvrProcMap.begin();
	for(; itr != mSvrProcMap.end(); itr++)
	{
		svr_ids.push_back(itr->first);
	}
	
	return true;
}

u32
AosCubeGroup::getProcId(const u32 svr_id)
{
	map<u32, u32>::iterator itr = mSvrProcMap.find(svr_id);
	aos_assert_r(itr != mSvrProcMap.end(), false);	
	return itr->second;
}

bool
AosCubeGroup::getNextSvrInfo(int &svr_id, u32 &proc_id)
{
	map<u32, u32>::iterator itr;
	if(svr_id == -1)
	{
		itr = mSvrProcMap.begin();	
	}
	else
	{
		itr = mSvrProcMap.find(svr_id);
		aos_assert_r(itr != mSvrProcMap.end(), false);
		itr++;
	}
	
	svr_id = -1;
	proc_id = 0;
	if(itr != mSvrProcMap.end())
	{
		svr_id = itr->first;
		proc_id = itr->second;
	}
	return true;
}
*/
