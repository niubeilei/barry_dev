////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OCSPServerConfig.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_OCSPServer_OCSPServerConfig_h
#define Omn_OCSPServer_OCSPServerConfig_h

#include "Util/RCObject.h"
#include "Util/String.h"

OmnString sgOCSPServerConfigStr("<OCSPServerConfig>\
	<OCSPTcpServer> \
		<LocalPort>3904</LocalPort> \
		<LocalNumPorts>1</LocalNumPorts> \
	</OCSPTcpServer> \
	<OCSPData> \
		<SNs>\
			<SN>A0200000000001</SN>		 \
			<SN>A0200000000002</SN>		 \
			<SN>A0200000000003</SN>		 \
			<SN>A0200000000007</SN>		 \
			<SN>A0200000000008</SN>		 \
			<SN>A0200000000009</SN>		 \
		</SNs>\
	</OCSPData> \
	<CRLData> \
		<CRLFile> \
			<No>1</No> \
			<FileName>/home/lxx/temp/aa.txt</FileName> \
		</CRLFile> \
		<CRLFilePos>/home/lxx/temp/crl1</CRLFilePos> \
	</CRLData> \
</OCSPServerConfig> \
<CliClientConfig>      \
	<RemoteIpAddr>127.0.0.1</RemoteIpAddr>  \
	<RemotePort>30000</RemotePort>                      \
	<RemoteNumPorts>1</RemoteNumPorts>             \
	<LocalAddr>127.0.0.1</LocalAddr>                \
	<LocalPort>30002</LocalPort>                              \
	<LocalNumPorts>1</LocalNumPorts>                     \
</CliClientConfig>") ;

#endif
