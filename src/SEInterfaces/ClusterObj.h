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
// 2015/03/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ClusterObj_h
#define Aos_SEInterfaces_ClusterObj_h

#include "IDO/IDO.h"
#include "JimoCall/Ptrs.h"
#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/CubeMapObj.h"


class AosRundata;

class AosClusterObj : public AosJimoCaller 
{
public:
	AosClusterObj();

	u32 getDistrID(AosRundata *rdata, const u64 docid);
	u32 getDistrID(AosRundata *rdata, const OmnString &name);

	virtual bool getTargetCubes(AosRundata *rdata, 
							const i64 cube_id, 
							vector<AosCubeMapObj::CubeInfo> &cube_infos) const = 0;


	virtual bool getTargetCubes(AosRundata *rdata, 
							vector<AosCubeMapObj::CubeInfo> &cube_infos) const = 0;

	virtual int getNumCubes() const = 0;
	virtual OmnString toString() const = 0;
	virtual OmnString getClusterName() const = 0;
	virtual u64 nextJimoCallID(AosRundata *rdata) = 0;
	virtual bool eraseJimoCall(AosRundata *rdata, const u64 call_id) = 0;
	virtual bool makeJimoCall(AosRundata *rdata, 
							//const AosCubeMapObj::CubeInfo endpoint_id,
							const int endpoint_id,
							const AosJimoCallPtr &jimo_call, 
							AosBuff *buff) = 0;
	virtual bool modifyCubeAllocation(
								AosRundata *rdata, 
								const int cube_id, 
								const vector<AosCubeMapObj::CubeInfo> &endpoint_ids) = 0;

	virtual bool moveCube(AosRundata *rdata, 
								const int cube_id,
								const int from_endpoint_id,
								const int to_endpoint_id) = 0;

	virtual bool isRetrieving() const = 0;
	virtual bool isValid() const = 0;
	virtual void setRetrieving() = 0;
	virtual void setInvalid() = 0;
	virtual void setValid() = 0;
	virtual bool retrieveFromAdmin(AosRundata *) = 0;

	// Chen Ding, 2015/07/16
	virtual bool getHostedCubes(AosRundata *rdata, 
					vector<AosCubeMapObj::CubeInfo> &cubes) = 0;
	virtual bool getCubeGroup(AosRundata *rdata, 
					const u32 cube_id,
					vector<AosCubeMapObj::CubeInfo> &cubes) = 0;

	//by White, 2015-08-28 10:54:41
	virtual bool updateLeadership(
			AosRundata						*rdata,
			const u32						cube_id,
			const i32						leader_id,	//wish i32 shall be not enough for our system, ^_^
			vector<AosCubeMapObj::CubeInfo> &cube_infos) = 0;

	//by White, 2015-09-10 16:41:58
	virtual bool putTimeoutEndpointToEnd(
			AosRundata						*rdata,
			const u32						cube_id,
			const i32						endpoint_id,	//I wish i32 shall be not enough for our system, ^_^
			vector<AosCubeMapObj::CubeInfo>	&cube_infos) = 0;
};
#endif
