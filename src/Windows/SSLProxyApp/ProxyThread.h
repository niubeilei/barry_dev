////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ProxyThread.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#pragma once

#define MAX_BUFFER_SIZE 16384
#define PROXY_TIME_OUT 180000 // milliseconds

VOID ProxyThreadProc(PVOID lpParameter);
VOID ProxyServerThread(PVOID lpParameter);