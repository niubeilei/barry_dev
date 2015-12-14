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
#include "SeReqProc/FinishIMSession.h"

#include "InstantMsg/IMManager.h"
#include "InstantMsg/IMSessionMgr.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"


AosFinishIMSession::AosFinishIMSession(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_FINISH_IMSESSION, 
		AosSeReqid::eFinishIMSession, rflag)
{
}


bool 
AosFinishIMSession::proc(const AosRundataPtr &rdata)
{
	/*
	//<request reqid = "finishimsession" >
	//  <objdef>
	//  	<Contents>
	//      	<notes/>
	//  	</Contents>
	//  </objdef>
	//</request>
	

	AOSLOG_ENTER_R(rdata, false);		
	AosXmlTagPtr root = rdata->getRequestRoot();
	aos_assert_rr(root, rdata, false);

	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	aos_assert_rr(objdef, rdata, false);

	AosXmlTagPtr contents = objdef->getFirstChild("Contents");
	aos_assert_rr(contents, rdata, false);

	AosXmlTagPtr notes = contents->getFirstChild("notes");
	aos_assert_rr(notes, rdata, false);

	bool finish = AosIMSessionMgr::getSelf()->finishSession(notes, rdata);
	aos_assert_rr(finish, rdata, false);
	AOSLOG_LEAVE(rdata);
	*/
	OmnNotImplementedYet;
	return true;
}

