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
// 02/29/2008 Copied from Porting/SystemError.h
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_porting_error_msg_h
#define Omn_porting_error_msg_h

extern char * aos_translate_system_error(const int errcode);
extern int	  aos_is_select_error_recoverable(const int errcode);
extern char * aos_connectError(const int errcode);
extern char * aos_write_error(const int errcode);
extern char * aos_read_error(const int errcode);

#endif

