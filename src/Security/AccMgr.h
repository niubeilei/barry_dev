////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AccMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Security_AccMgr_h
#define OMN_Security_AccMgr_h

struct aos_app_proc_req;

extern int aos_check_permit(struct aos_user *user, 
							char *domain, 
							unsigned short port, 
							char *resource, 
							struct aos_app_proc_req *request,
							int (*callback)(struct aos_app_proc_req *req));

#endif

