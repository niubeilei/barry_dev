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
// 2012/05/23	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEInterfaces_SvrInfo_h
#define Omn_SEInterfaces_SvrInfo_h

#include "TransServer/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/ProcessType.h"

#include <map>
using namespace std;

#define AOSSERVER_NORMAL	"normal"
#define AOSSERVER_MSG		"msg"
	
struct ProcInfo
{
	AosProcessType::E mType;
	int			   mCubeGrpId;
};

class AosServerInfo: virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	int			mServerId;
	OmnString	mAddr;
	int			mPort;

	vector<ProcInfo>	mProcInfos;
	
public:
	AosServerInfo(const AosXmlTagPtr &conf);
	~AosServerInfo();
	
	int			getServerId() const { return mServerId; };
	OmnString	getAddr() const { return mAddr; };
	int			getPort() const { return mPort; };
	
	//bool 		addDefaultProcs(
	//				const AosXmlTagPtr procs_conf,
	//				map<u32, AosCubeGroupPtr> &cube_grps);
	void		addProc(ProcInfo &info){ mProcInfos.push_back(info); };
	void		addBkpCubeProc(const int cube_grp_id);

	vector<ProcInfo> & getDefProcs(){ return mProcInfos; };

	//AosProcInfo::Status getProcStatus(const AosProcessType::E type);
	//AosProcInfo::Status getProcStatus(const u32 proc_id);
private:
	bool 	config(const AosXmlTagPtr &svr_conf);

};
#endif

