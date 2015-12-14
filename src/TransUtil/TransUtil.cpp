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
// 07/20/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/TransUtil.h"

#include "Rundata/Rundata.h"
#include "UtilData/ModuleId.h"

ofstream AosTransUtil::mLogFile;


/*
// Chen Ding, 2013/03/02
bool 
AosTransUtil::createMsg(
		OmnString &msg, 
		const u32 send_cubeid,
		const u32 recv_cubeid,
		const AosModuleId::E send_modid,
		const AosModuleId::E recv_modid,
		const OmnString &msg_id, 
		const bool is_req)
{
	msg << AOSTAG_MESSAGE_TYPE << "=\"" << msg_type
		<< "\" " << AOSTAG_REQUEST_FLAG << "=\"" << (is_req)?"true":"false" 
		<< "\" " << AOSTAG_SEND_CUBEID << "=\"" << send_cubeid
		<< "\" " << AOSTAG_RECV_CUBEID << "=\"" << recv_cubeid
		<< "\" " << AOSTAG_SEND_PHYSICALID << "=\"" << send_phyid
		<< "\" " << AOSTAG_RECV_PHYSICALID << "=\"" << recv_phyid
		<< "\" " << AOSTAG_SEND_MODULEID << "=\"" << send_modid
		<< "\" " << AOSTAG_RECV_MODULEID << "=\"" << recv_modid
		<< "\" ";
	return true;
}
*/

