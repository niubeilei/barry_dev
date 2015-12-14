////////////////////////////////////////////////////////////////////////////

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
// 05/18/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ImageServer_ImageReqProc_h
#define Aos_ImageServer_ImageReqProc_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/Server/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "Util/DynArray.h"


class AosImageReqProc : public AosNetReqProc
{
	OmnDefineRCObject;

private:

	OmnMutexPtr		mLock;
	bool			mIsStopping;
	OmnString		mUsername;
	OmnString		mAppname;
	AosWebRequestPtr mReq;
public:
	AosImageReqProc();
	~AosImageReqProc();

	static 	bool 	config(const AosXmlTagPtr &config);
	bool 	stop();
	bool	procRequest(const OmnConnBuffPtr &buff);
	AosNetReqProcPtr	 clone();
	void	sendResponse(
			const AosWebRequestPtr &req,
			const AosXmlRc errcode,
			const OmnString &errmsg,
			const OmnString &contents);

};
#endif

