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
// 02/22/2008 : Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef aos_net_comm_mgr_h
#define aos_net_comm_mgr_h

#ifdef __cplusplus
extern "C" {
#endif

struct aos_action;

extern int aos_comm_mgr_recv_msg(
		const int sock, 
		struct aos_action *callback);

#ifdef __cplusplus
}
#endif


#endif


