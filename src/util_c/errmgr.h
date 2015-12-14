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
// 12/12/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_aos_util_errmgr_h
#define Omn_aos_util_errmgr_h

#define aos_errmgr_add(errmsg) aos_errmgr_add_err(__FILE__, __LINE__, errmsg)

extern int aos_errmgr_add_err(const char * const filename, 
							  const int lineno, 
							  const char * const errmsg);
extern int aos_errmgr_nullptr(const char * const filename, 
							  const int lineno);

#endif
