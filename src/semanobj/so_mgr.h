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
// 12/11/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_semanobj_so_mgr_h
#define aos_semanobj_so_mgr_h

struct aos_so;
#define AOS_SO_MGR_TABLE_SIZE 10000
#define AOS_SO_MGR_TABLE_MAX 10000

struct aos_so_mgr
{
};

extern int aos_somgr_add_so(struct aos_so *ptr);
extern int aos_somgr_remove_so(struct aos_so *ptr);

#endif // End of Include

