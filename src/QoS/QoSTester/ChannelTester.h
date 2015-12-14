////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ServerTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_QoSTester_QoSChannelTester_h
#define Omn_QoSTester_QoSChannelTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/TcpCltGrpListener.h"
#include "UtilComm/Ptrs.h"


class AosQoSChannelTester : public OmnTestPkg,
 						   public OmnTcpCltGrpListener 
{
private:

public:
	AosQoSChannelTester()
	{
		mName = "AosQoSChannelTester";
	}
	~AosQoSChannelTester() {}

	virtual bool		start();

    virtual OmnString   getTcpListenerName() const {return "ServerTester";}
    virtual void        msgRecved(const OmnTcpCltGrpPtr &group,
                                const OmnConnBuffPtr &buff,
                                const OmnTcpClientPtr &conn);
    virtual void        newConn(const OmnTcpClientPtr &conn);
    virtual bool        connClosed(const OmnTcpCltGrpPtr &,
                                const OmnTcpClientPtr &client);

private:
	bool	basicTest();
};
#endif

