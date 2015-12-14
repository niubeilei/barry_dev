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
// 12/17/2009: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SiteMgr_SiteReqProc_h
#define AOS_SiteMgr_SiteReqProc_h
#if 0
#include "Database/Ptrs.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"


class AosSiteReqProc : public AosNetReqProc
{
	OmnDefineRCObject;

public:
	enum
	{
		eGetDataid = 'a',
		eXmlObjCreated = 'b' 
	};

private:

public:
	AosSiteReqProc();
	~AosSiteReqProc();

	bool	procRequest(const OmnConnBuffPtr &buff);
	bool 	getDataid(
				const AosNetRequestPtr &req, 
				const AosXmlTagPtr &cmd,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	xmlObjCreated(
				const AosNetRequestPtr &req, 
				const AosXmlTagPtr &cmd,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	void 	sendResp(
				const AosNetRequestPtr &req, 
				const AosXmlRc errcode,
				const OmnString &errmsg,
				const OmnString &contents);
};
#endif
#endif

