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
// 2014/11/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/Jimo.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Datalet/DataletFormat.h"
#include "XmlUtil/XmlTag.h"


namespace Jimo
{
bool jimoInit(AosRundata *rdata, const AosXmlTagPtr &conf)
{
	if (!conf) return true;

	return true;
}


/*
bool 
Jimo::getCubeEndpointIDs(
		AosRundata *rdata, 
		const u64 docid, 
		vector<int> &endpoint_ids, 
		const AosReplicPolicy &replic_policy)
{
	AosCubeMapIDOPtr cube_map = AosCubeMgrObj::getCubeMap(rdata, docid);
	aos_assert_rr(cube_map, rdata, false);

	bool rslt = cube_map->getCubeEndpointIDs(rdata, endpoint_ids, replic_policy);
	aos_assert_rr(rslt, rdata, false);
	return true;
}
*/
};


