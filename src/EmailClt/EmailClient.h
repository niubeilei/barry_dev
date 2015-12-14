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
// 	Created: 05/26/2011 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_EmailClient_EmailClient_h
#define AOS_EmailClient_EmailClient_h

#include "TransClient/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "XmlUtil/Ptrs.h"


OmnDefineSingletonClass(AosEmailClientSingleton,
						AosEmailClient,
						AosEmailClientSelf,
						OmnSingletonObjId::eEmailClt,
						"AosEmailClient");

class AosEmailClient:	virtual public OmnRCObject 
{
	OmnDefineRCObject;


public:
	AosEmailClient();
	~AosEmailClient();

    //
    // Singleton class interface
    //
    static AosEmailClient *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	virtual bool		config(const AosXmlTagPtr &def);

	bool    login(
				const OmnString &email_addr,
				const OmnString &passwd,
				const OmnString &email_dir,
				const AosRundataPtr &rdata);

	bool     retr(
				 const int start_idx,
				 const int psize,
				 const bool reverse,
			 	 const AosRundataPtr &rdata);

	bool     dele(
				 const OmnString &index,
				 const OmnString &deleOpr,
			 	 const AosRundataPtr &rdata);

	bool     send(
				 const OmnString &to,
				 const OmnString &from,
			 	 const OmnString &senderpasswd,
				 const OmnString &subject,
				 const OmnString &body,
				 const OmnString count,
				 const AosRundataPtr &rdata);
};
#endif

