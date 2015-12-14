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
// 2015/03/12 Created by Ketty Guo
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeMgr_CubeMap_h
#define Aos_CubeMgr_CubeMap_h

#include "IDO/IDO.h"
#include "SEInterfaces/CubeMapObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilData/RepPolicy.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class OmnMutex;
class OmnCondVar;
class AosRundata;
class AosCubeMapObj;

using namespace std;

class AosCubeMap : public AosCubeMapObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxCubeBackups = 100000
	};

	bool						mMayUseLocalCopy;
	u64							mIdoDocid;
	u32							mCubeIdx;
	OmnMutex *					mLock;
	OmnCondVar *				mCondVar;
	vector< vector<CubeInfo> > 	mCubes;

public:
	AosCubeMap(AosRundata *rdata, 
			const AosXmlTagPtr &conf,
			const int num_endpoints);
	~AosCubeMap();

	static AosCubeMapObj * retrieveCubeMap(AosRundata *rdata);

	virtual bool getTargetCubes(AosRundata *rdata, 
							const u32 cube_id, 
							vector<CubeInfo> &cube_infos); 

	virtual bool getTargetCubes(AosRundata *rdata, 
							vector<CubeInfo> &cube_infos);

	bool config(AosRundata *rdata, const AosXmlTagPtr &conf, const int num_endpoints);

	// AosIDO interface
	virtual bool serializeFrom(AosRundata *rdata, AosBuff *buff){ return false; }
	virtual bool serializeTo(AosRundata *rdata, AosBuff *buff) const{ return false; }
	virtual bool jimoCall(AosRundata*, AosJimoCall&) { return false; }

	// Jimo interface
	virtual AosJimoPtr cloneJimo() const;

	virtual AosCubeMapObjPtr createCubeMap(AosRundata *rdata, const AosXmlTagPtr &doc);
	virtual AosCubeMapObjPtr createCubeMap(AosRundata *rdata, AosBuff *buff);
	virtual int getNumCubes() const {return mCubes.size();}

	// Chen Ding, 2015/07/16
	virtual bool getCubeGroup(AosRundata *rdata, 
							const u32 cube_id, 
							vector<CubeInfo> &cubes);

	virtual bool getHostedCubes(AosRundata *rdata, 
					vector<AosCubeMapObj::CubeInfo> &cubes);

	//by White, 2015-08-28 10:54:41
	virtual bool updateLeadership(
			AosRundata			*rdata,
			const u32			cube_id,
			const i32			leader_id,	//I wish i32 shall be not enough for our system, ^_^
			vector<CubeInfo>	&cube_infos);

	//by White, 2015-09-10 16:41:58
	virtual bool putTimeoutEndpointToEnd(
			AosRundata			*rdata,
			const u32			cube_id,
			const i32			endpoint_id,	//I wish i32 shall be not enough for our system, ^_^
			vector<CubeInfo>	&cube_infos);

private:
	bool updateFromIDO(AosRundata *rdata);
};


#endif
