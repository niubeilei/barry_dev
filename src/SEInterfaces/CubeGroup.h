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
// 2013/07/12	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEInterfaces_CubeGroup_h
#define Omn_SEInterfaces_CubeGroup_h

#include "aosUtil/Types.h"
#include "SEUtil/SeConfig.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "XmlUtil/XmlTag.h"

#include <vector>
using namespace std;

class AosCubeGroup: virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	int			 	mGroupId;
	vector<u32>		mCubeIds;
	//map<u32, u32>	mSvrProcMap;
	vector<u32>		mSvrIds;

public:
	AosCubeGroup(const AosXmlTagPtr &conf);
	AosCubeGroup(const int group_id, const vector<u32> &cube_ids);
	~AosCubeGroup();
	
	void 	addSvrId(const u32 svr_id);
	int		getGroupId() const { return mGroupId; };
	vector<u32> &  getCubeIds(){ return mCubeIds; };
	vector<u32> &  getSvrIds(){ return mSvrIds; };
	int 	getNextSvrId(const int crt_sid);

	//void 	addSvrProc(const u32 svr_id, const u32 proc_id);
	//map<u32, u32> & getSvrProcMap(){ return mSvrProcMap; };
	//bool	getSvrIds(vector<u32> &svr_ids);
	//u32		getProcId(const u32 svr_id);
	//bool	getNextSvrInfo(int &svr_id, u32 &proc_id);

private:
	bool 	config(const AosXmlTagPtr &conf);

};

#endif

