////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_WebServerTorturer_WebClient_h
#define AOS_WebServerTorturer_WebClient_h

#include "SEUtil/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "WebServerTorturer/Ptrs.h"
#include "XmlUtil/Ptrs.h"

OmnDefineSingletonClass(AosWebClientSingleton,
						AosWebClient,
						AosWebClientSelf,
						OmnSingletonObjId::eWebClient,
						"WebClient");


class AosWebClient : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	enum
	{
		eInitTransId = 100,
		eReadTimeoutSec = 1
	};

	OmnMutexPtr			mLock;
	OmnString			mRemoteAddr;
	int					mRemotePort;
	int					mNumPorts;
	OmnTcpClientPtr 	mConn;

public:
	AosWebClient();
	~AosWebClient();

    //
    // Singleton class interface
    //
    static 	AosWebClient*  		getSelf();
    virtual bool      			start();
    virtual bool        		stop();
    virtual OmnString   		getSysObjName() const {return "AosWebClient";}
    virtual OmnRslt     		config(const OmnXmlParserPtr &def);
			bool				start(const AosXmlTagPtr &config);
			bool 				sendRequestPublic(
									const OmnString &uri,
					 				const OmnString &method,
					  				const OmnString &parms,
					   				const OmnString &cookies,
					    			OmnTcpClientPtr &conn,
									OmnConnBuffPtr &buff);
private:
	void		connect();
	bool		reconnect(const OmnTcpClientPtr &conn);
   	void		createRequest(
		   			OmnString &request,
					const OmnString	&url,
					const OmnString &method,
					const OmnString	&parms,
					const OmnString	&cookies,
					const int connType);

	bool 		sendRequest(
					const OmnString &url,
					const OmnString &method,
					const OmnString &parms,
					const OmnString &cookies,
					const OmnTcpClientPtr &conn);
};
#endif

