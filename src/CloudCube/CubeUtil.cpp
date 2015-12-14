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
// 2013/03/02 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "CloudCube/CubeUtil.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"

/*
bool 
AosCubeUtil::createMsg(
		OmnString &msg, 
		const u32 send_cubeid,
		const u32 recv_cubeid,
		const OmnString &msg_id, 
		const bool is_req)
{
	int send_phyid = AosCubeId2PhysicalId(rdata, send_cubeid);
	int recv_phyid = AosCubeId2PhysicalId(rdata, recv_cubeid);
	msg << AOSTAG_MESSAGE_TYPE << "=\"" << msg_type
		<< "\" " << AOSTAG_REQUEST_FLAG << "=\"" << (is_req)?"true":"false" 
		<< "\" " << AOSTAG_SEND_CUBEID << "=\"" << send_cubeid
		<< "\" " << AOSTAG_RECV_CUBEID << "=\"" << recv_cubeid
		<< "\" " << AOSTAG_SEND_PHYSICALID << "=\"" << send_phyid
		<< "\" " << AOSTAG_RECV_PHYSICALID << "=\"" << recv_phyid
		<< "\" ";
	return true;
}
*/
