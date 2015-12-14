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
// 01/06/2011	Created by Ken
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_EmailServer_SmtpServer_h
#define AOS_EmailServer_SmtpServer_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

#include <python2.6/Python.h>


class AosSmtpServer : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
		OmnMutexPtr         mLock;
		PyObject *			mSendFunc;
		static PyObject *			mSmtpServerModule;

public:
	AosSmtpServer(const AosXmlTagPtr &config);
	~AosSmtpServer();

	bool		send(
				const AosXmlTagPtr &data,
				const AosRundataPtr &rdata);

private:
	void  		svrInit(const AosXmlTagPtr &config);
	void		moduleInit(const AosXmlTagPtr &config);
	OmnString   getServer(
				const OmnString &from,
				const AosRundataPtr &rdata);
};
#endif

