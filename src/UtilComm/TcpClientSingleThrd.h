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
// 2015/08/21 Created By White
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpClientSingleThrd_h
#define Omn_UtilComm_TcpClientSingleThrd_h

#include "UtilComm/TcpClient.h"

class OmnTcpClientSingleThrd : public OmnTcpClient
{
	OmnTcpClientSingleThrd(const OmnTcpClientSingleThrd &rhs);
	OmnTcpClientSingleThrd & operator = (const OmnTcpClientSingleThrd &rhs);

public:
	OmnTcpClientSingleThrd(const OmnIpAddr &remoteIpAddr,
			     const int remotePort,
				 const int numPort,
				 const AosTcpLengthType lt);
	OmnTcpClientSingleThrd(const OmnString &name,
				 const OmnIpAddr &remoteIpAddr,
			     const int remotePort,
				 const int numPort,
				 const AosTcpLengthType lt = eAosTLT_LengthIndicator);
	OmnTcpClientSingleThrd(const OmnIpAddr &remoteIpAddr,
		   const int remotePort,
		   const int remoteNumPorts,
		   const OmnIpAddr &localIpAddr,
		   const int localPort,
		   const int localNumPorts,
		   const OmnString &name,
		   const AosTcpLengthType lt = eAosTLT_LengthIndicator);

	OmnTcpClientSingleThrd(const OmnString &name,
		   const OmnString &uPath,
		   const AosTcpLengthType lt = eAosTLT_LengthIndicator);

    virtual ~OmnTcpClientSingleThrd();

	virtual int	smartRead(OmnConnBuffPtr &buff);
};
#endif
