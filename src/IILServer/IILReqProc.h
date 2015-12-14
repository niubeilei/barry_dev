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
// 11/21/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILServer_IILReqProc_h
#define Aos_IILServer_IILReqProc_h

#include "Database/Ptrs.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "SEServer/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Security/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/MetaExtension.h"
#include "Thread/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/XmlRc.h"

#include "TransUil/TransReqId.h"
#include "IILServer/Ptrs.h"

class AosIILReqProc : public AosNetReqProc
{
	OmnDefineRCObject;

public:

private:
	
	bool            mIsStopping;
	AosTransReqId::E	mOperation;

public:
	AosIILReqProc();
	~AosIILReqProc();

	static bool		config(const AosXmlTagPtr &config);
	static bool		stop();
	virtual bool	procRequest(const OmnConnBuffPtr &req);
	virtual AosNetReqProcPtr	clone();
	virtual std::vector<OmnString>  getCookie() {std::vector<OmnString> c; return c;}

	void 	sendResp(const AosWebRequestPtr &req, 
				const AosXmlRc errcode,
				const OmnString &errmsg, 
				const OmnString &contents); 

private:

	bool	addDoc(const AosWebRequestPtr &req, 
				const OmnTcpClientPtr  &conn,
				const AosXmlTagPtr &root);
	
	bool	retrieveIIL(const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root);
	
	bool	queryIIL(const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root);

};
#endif
