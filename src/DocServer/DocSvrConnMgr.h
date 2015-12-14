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
// 06/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/22
/*
#ifndef AOS_DocServer_DocSvrConnMgr_h
#define AOS_DocServer_DocSvrConnMgr_h

#include "TransServer/TransProc.h"
#include "DocServer/DocSvr.h"


class AosDocSvrConnMgr: public AosTransProc
{
	OmnDefineRCObject;
	
public:
	enum
	{
		eError = 0,
		eOk,
	};
	
private:
	AosTransModuleSvrPtr		mTransServer;

public:

	AosDocSvrConnMgr();
	~AosDocSvrConnMgr();

	void 		setTransServer(const AosTransModuleSvrPtr &trans_server);
	
	//virtual bool proc(const AosIILTransPtr &trans, const AosRundataPtr &rdata);
	//virtual bool proc(const AosBigTransPtr &trans,
	//		        const AosBuffPtr &content,
	//				const AosRundataPtr &rdata);
private:
	void	sendResp(
			const AosXmlRc errcode,
			const OmnString &errmsg,
			const AosRundataPtr &rdata);
			//const AosBigTransPtr &trans);
};
#endif
*/

