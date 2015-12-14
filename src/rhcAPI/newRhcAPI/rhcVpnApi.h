////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: newRhcApi.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_newRhcApi_h
#define aos_newRhcApi_h

#define MAX_IP_STRING_LEN 32
#define MAX_PORT_STRING_LEN 8
#define MAX_MAC_STRING_LEN 32
#define MAX_NAME_STRING_LEN 64
#define MAX_STATUS_STRING_LEN 8
#define MAX_MODID_STRING_LEN 32

extern int fwIpsecPass(char *command, char *rslt);
extern int fwL2tpPass(char *command, char *rslt);
extern int fwPptpPass(char *command, char *rslt);

#endif

