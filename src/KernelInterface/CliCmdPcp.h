////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCmdPcp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelInterface_CliCmdPcp_h
#define Omn_KernelInterface_CliCmdPcp_h

#include "KernelInterface/CliCmd.h"

/*
<Cmd>
	<OprID>pcpOnOff</OprId>
	<Prefix>pcp status</Prefix>
	<bufsize>0</bufsize>
	<Parms>
		<Parm>
			<type>string</type>
			<help>The status: on|off</help>
			<values>on, off</values>
		</Parm>
		<Parm>
			<help>The PCP</help>
			<value>netinput</value>
		</Parm>
	</Parms>
	<Usage>Usage: pcp status <on|off> [<pcp-name>]</Usage>
</Cmd>

OmnDeclareCliCmd(OmnCliPcpStatus,			"pcp status");
*/

#endif

