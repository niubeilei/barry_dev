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

#ifndef Omn_SslTester_SslServerTester_h
#define Omn_SslTester_SslServerTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/TcpBouncerListener.h"
#include "UtilComm/TcpCltGrpListener.h"
#include "UtilComm/Ptrs.h"


// class AosSslServerTester : public OmnTestPkg,
// 						   public AosTcpBouncerListener
class AosSslServerTester : public OmnTestPkg,
 						   public OmnTcpCltGrpListener 
{
private:

public:
	AosSslServerTester()
	{
		mName = "AosSslServerTester";
	}
	~AosSslServerTester() {}

	virtual bool		start();

	// 
	// TcpBouncerListener Interface
	//
	/*
    virtual void    clientCreated(const AosTcpBouncerClientPtr &client);
    virtual AosTcpBouncer::Action msgReceived(
                        const AosTcpBouncerClientPtr &client,
                        const OmnConnBuffPtr &buff);
    virtual AosTcpBouncer::Action recvFailed(
                        const AosTcpBouncerClientPtr &client,
                        char *data,
                        u32 dataLen);
    virtual AosTcpBouncer::Action readyToBounce(
                        const AosTcpBouncerClientPtr &client,
                        char *data,
                        u32 dataLen);
    virtual AosTcpBouncer::Action bounceFailed(
                        const AosTcpBouncerClientPtr &client,
                        char *data,
                        u32 dataLen);
    virtual AosTcpBouncer::Action clientClosed(
                        const AosTcpBouncerClientPtr &client);
	*/

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

