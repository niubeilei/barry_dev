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
// 2013/04/30 Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#include "BitmapMgr/BitmapUtil.h"

#include "API/AosApi.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/BitmapEngineObj.h"


u32
AosBitmapUtil::getLocalSectionId(const u32 section_id)
{
	static int lsNumCubes = AosGetNumCubes();

	aos_assert_r(lsNumCubes > 0, 0);
	return section_id / lsNumCubes;
}

u64 
AosBitmapUtil::getMaxLocalSectionId()
{
	static int lsNumCubes = AosGetNumCubes();

	aos_assert_r(lsNumCubes > 0, 0);

	u64 nn = 0xffff / lsNumCubes;
	if (nn > eMaxSectionsPerCube) return eMaxSectionsPerCube;
	return nn;
}

bool 
AosBitmapUtil::splitBitmapToCubes(
		const AosRundataPtr &rdata, 
		const AosBitmapObjPtr &bitmap,
		vector<AosBitmapObjPtr> &cube_list)
{
	// This function splits the bitmap 'bitmap' based on cubes.
	//
	int num_cubes = AosGetNumCubes();
	aos_assert_rr(num_cubes > 0, rdata, false);
	cube_list.clear();
	cube_list.resize(num_cubes);

	AosBitmapObj::map_t bitmap_maps;
	bool rslt = bitmap->getSections(rdata, bitmap_maps);
	aos_assert_rr(rslt, rdata, false);
	AosBitmapObj::mapitr_t itr = bitmap_maps.begin();
	while (itr != bitmap_maps.end())
	{
		u64 section_id = itr->second->getSectionId();
		int cube_id = AosBitmapEngineObj::getCubeId(section_id);
		aos_assert_rr(cube_id >= 0 && cube_id < num_cubes, rdata, false);
		AosBitmapObjPtr bmp = cube_list[cube_id];
		if (bmp)
		{
			bmp->orBitmap(itr->second);
		}
		else
		{
			cube_list[cube_id] = itr->second;
		}
		itr++;
	}
	return true;

	OmnNotImplementedYet;
	return false;		
}


u64 
AosBitmapUtil::getGlobalSectionId(const u64 local_sid)
{
	static int lsNumCubes = AosGetNumCubes();

	aos_assert_r(lsNumCubes > 0, 0);
	return local_sid * lsNumCubes + AosGetSelfServerId();
}

