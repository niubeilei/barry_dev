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
// 05/24/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/GetOnlineUser.h"

#include "EventMgr/EventMgr.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosGetOnlineUser::AosGetOnlineUser(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GET_ONLINE_USER, 
		AosSeReqid::eGetOnlineUser, rflag)
{
}


bool 
AosGetOnlineUser::proc(const AosRundataPtr &rdata)
{
	// The request is identifies by the arguments:
	//
	//   <request ...>
	//       <command>
	//           <cmd start_idx="nnn"
	//               psize="nnn"
	//           >
	//           </cmd>
	//       </command>
	//   </request>
	
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString errmsg;
	OmnString contents = "";

	OmnString args = root->getChildTextByAttr("name", "args");

	contents = "<Users>";
	/*mLock->lock();
	for (int i=0; i<mNumReqs; i++)
	{
		    contents << "<user><![CDATA[" << mReqNames[i] << "]]></user>";
	}
	contents << "</Users>";
	mLock->unlock();*/
	rdata->setResults(contents);
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}
