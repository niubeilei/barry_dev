////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelInterface.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelInterface_KernelInterface_h
#define Omn_KernelInterface_KernelInterface_h

#include "aos/KernelEnum.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommListener.h"
#include "UtilComm/TcpListener.h"
#include "XmlParser/Ptrs.h"

OmnDefineSingletonClass(OmnKernelInterfaceSingleton,
						OmnKernelInterface,
						OmnKernelInterfaceSelf,
						OmnSingletonObjId::eKernelInterface, 
						"KernelInterface");



class OmnKernelInterface : public OmnCommListener, 
						   public OmnTcpListener
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxUdpConns = 5,
		eMaxTcpConns = 5
	};

	int				mUdpIndex;
	int				mTcpIndex;
	OmnIpAddr		mUdpAddr[eMaxUdpConns];
	int				mUdpPort[eMaxUdpConns];
	OmnIpAddr		mTcpAddr[eMaxTcpConns];
	int				mTcpPort[eMaxTcpConns];
	OmnUdpCommPtr	mUdpComm[eMaxUdpConns];
	OmnTcpCommSvrPtr	mTcpComm[eMaxTcpConns];

public:
	OmnKernelInterface();
	~OmnKernelInterface();

	bool		start();
	bool		stop();
	OmnRslt		config(const OmnXmlParserPtr &conf);

	static OmnKernelInterface *	getSelf();

	// 
	// OmnCommListner Interface
	//
	virtual bool		msgRead(const OmnConnBuffPtr &buff);
	virtual OmnString	getCommListenerName() const;

	// 
	// TcpListener interface
	//
	virtual OmnString	getTcpListenerName() const;
	virtual void		msgRecved(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);

private:
};

#endif

