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
// 10/13/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef	Aos_WordSrver_WordReqProc_h 
#define Aos_WordSrver_WordReqProc_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Proggie/ReqDistr/Ptrs.h"
#include "SEServer/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Security/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "XmlParser/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "Thread/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/WebRequest.h"
#include "WordServer/WordOpr.h"

#include <vector>
#include <map>
#include <iterator>


using namespace std;
class AosWordReqProc : public AosNetReqProc
{
	OmnDefineRCObject;

private:
	vector<OmnString>   mCookies;
	bool				mIsStopping;
	AosWordOpr::E		mOperation;
	OmnMutexPtr         mLock;	

public:
	AosWordReqProc();
	~AosWordReqProc();

	static bool		config(const AosXmlTagPtr &config);
	static bool		stop();
	virtual bool	procRequest(const OmnConnBuffPtr &req);
	virtual AosNetReqProcPtr	clone();
	virtual std::vector<OmnString>  getCookie() {return mCookies;}

	void 	sendResp(const AosWebRequestPtr &req, 
				const AosXmlRc errcode,
				const OmnString &errmsg, 
				const OmnString &contents);
	bool	procReq(
			 	const AosWebRequestPtr &req,
			    const AosXmlTagPtr &root,
			    const AosXmlTagPtr &child);
	bool    getWordid(
				const AosWebRequestPtr &req,
				const AosXmlTagPtr &root,
				const AosXmlTagPtr &child);
};
#endif

