////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliFwMisc.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef AOS_OLD_CLI
#ifndef aos_CliFwMisc_h
#define aos_CliFwMisc_h

#include "KernelInterface/Cli.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/List.h"  
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/CliCmd.h"
#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include <string.h>

//kevin 07/28/2006 for uname
#include <sys/utsname.h>
#include "KernelInterface_exe/version.h"

#define MAX_INTERFACE 16
#define MAX_LIMIT_RATE 600000
#define MAX_PORTS 16

extern unsigned int parse_port(const char *port);

extern unsigned int parse_multi_ports(char *portstring, unsigned int *ports);

extern int parse_mac(char *mac, int *macarray);

extern int portlen(int port);

extern int ratelen(int port);

#endif

#endif
