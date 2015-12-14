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
// 2011/04/18	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocProcServer_h
#define Aos_SmartDoc_SdocProcServer_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "Util/TimeUtil.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/LockMonitor.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

class AosSdocProcServer : public AosSmartDoc
{

private:
	OmnMutexPtr     mLock;
public:
	AosSdocProcServer(const bool flag);
	~AosSdocProcServer();

	virtual AosSmartDocObjPtr	clone() {return OmnNew AosSdocProcServer(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	bool doActions(
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &actions, 
		const AosXmlTagPtr &entries,
		const AosRundataPtr &rdata);
	bool connectToProcServer(
		const AosXmlTagPtr &connxml,
		const OmnString &send_req,
		OmnString &resp,
		const AosRundataPtr &rdata);

};


#endif
