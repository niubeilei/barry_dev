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
// 2015/03/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_CubeMapObj_h
#define Aos_SEInterfaces_CubeMapObj_h

#include "IDO/IDO.h"
//#include "JimoObj/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include <vector>

class AosRundata;

class AosCubeMapObj : public AosIDO
{
public:
	enum
	{
		eMethodGetByDocid = 1,
	};

	enum Status
	{
		eInvalid,

		eActive,
		eWriteOnly,
		eSuspended,
	};
	
	struct CubeInfo
	{
		int			cube_id;
		int			physical_id;
		u8			dupid;
		Status		status;
		
		CubeInfo(const int cube_id, 
				const int physical_id, 
				const u8 dupid, 
				const Status status)
		:
		cube_id(cube_id),
		physical_id(physical_id),
		dupid(dupid),
		status(status)
		{
		}

		CubeInfo()
		:
		cube_id(-1),
		physical_id(-1),
		dupid(0),
		status(eInvalid)
		{
		}
	};

	struct CubeInfoFull
	{
		int			cube_id;
		Status		status;
		vector<int>	backups;
	};

public:
	AosCubeMapObj();
	AosCubeMapObj(const u64 docid);

	virtual AosCubeMapObjPtr createCubeMap(AosRundata *rdata, const AosXmlTagPtr &doc) = 0;
	virtual AosCubeMapObjPtr createCubeMap(AosRundata *rdata, AosBuff *buff) = 0;
	virtual int getNumCubes() const = 0;

	virtual bool getTargetCubes(AosRundata *rdata, 
							const u32 cube_id, 
							vector<CubeInfo> &cubes) = 0;

	virtual bool getTargetCubes(AosRundata *rdata, 
							vector<CubeInfo> &cubes) = 0;

	// Chen Ding, 2015/07/16
	virtual bool getCubeGroup(AosRundata *rdata, 
							const u32 cube_id, 
							vector<CubeInfo> &cubes) = 0;

	//by White, 2015-08-28 10:54:41
	virtual bool updateLeadership(
			AosRundata			*rdata,
			const u32			cube_id,
			const i32			leader_id,	//I wish i32 shall be not enough for our system, ^_^
			vector<CubeInfo>	&cube_infos) = 0;

	//by White, 2015-09-10 16:41:58
	virtual bool putTimeoutEndpointToEnd(
			AosRundata			*rdata,
			const u32			cube_id,
			const i32			endpoint_id,	//I wish i32 shall be not enough for our system, ^_^
			vector<CubeInfo>	&cube_infos) = 0;
};
#endif
