////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AttackMgrConfig.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AttackMgr_AttackMgrConfig_h
#define Omn_AttackMgr_AttackMgrConfig_h

#include "Util/RCObject.h"
#include "Util/String.h"

OmnString sgAttackMgrConfigStr("<CliClientConfig>      \
	<RemoteIpAddr>127.0.0.1</RemoteIpAddr>  \
	<RemotePort>30000</RemotePort>                    \
	<RemoteNumPorts>1</RemoteNumPorts>             \
	<LocalAddr>127.0.0.1</LocalAddr>               \
	<LocalPort>30003</LocalPort>                           \ 
	<LocalNumPorts>1</LocalNumPorts>                     \
</CliClientConfig>") ;

#endif
