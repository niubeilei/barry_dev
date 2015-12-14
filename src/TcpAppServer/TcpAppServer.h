////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpAppServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_TcpAppServer_TcpAppServer_h
#define Aos_TcpAppServer_TcpAppServer_h

#include "Message/Msg.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/TcpMsgReader.h"

class AosTcpAppServer: public OmnTcpListener, public OmnTcpMsgReader
{
	OmnDefineRCObject;

protected:
	OmnTcpServerPtr			mServer;
	bool					mFinished;
	bool					mUsePrivateLenthType;
	OmnTcpListenerPtr		mListener;
	OmnTcpMsgReaderPtr	mReader;

public:
	AosTcpAppServer();
	~AosTcpAppServer();

	virtual bool		start();
	virtual bool		stop();
	virtual bool		config(const OmnXmlItemPtr &def,const OmnIpAddr &localIP = OmnIpAddr::eInvalidIpAddr);

	virtual OmnIpAddr	getLocalIpAddr() const;
	virtual int			getLocalPort() const;

	virtual void		setLocalIpAddr(const OmnIpAddr addr);
	virtual void		setLocalPort(const int port);

	virtual OmnString	getTcpListenerName() const;
	virtual void		msgRecved(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);

	virtual OmnString	getTcpMsgReaderName() const;
	virtual int			nextMsg(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);
	virtual void		connClosed(const OmnTcpClientPtr &conn);
 	virtual void    	connAccepted(const OmnTcpClientPtr &conn);

	virtual void		setListener(const OmnTcpListenerPtr &listener);
	virtual void		setReader(const OmnTcpMsgReaderPtr &reader);

	virtual OmnRslt	writeTo(const char *data, const int length, const OmnTcpClientPtr client);
	virtual OmnRslt	writeTo(const OmnString &msg, const OmnTcpClientPtr client);

	virtual bool		getClientByRemoteAddr(const OmnIpAddr addr, const int port, OmnTcpClientPtr & client);

	void				setConnFlag(const bool connFlag);

private:

};
#endif
