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
// Modification History:
// 02/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RemoteBackupSvr/DocBackupProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "TransClient/TransClient.h"
#include "TransUtil/IdRobin.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosDocBackupProc::AosDocBackupProc()
:
mLock(OmnNew OmnMutex())
{
}


AosDocBackupProc::~AosDocBackupProc()
{
}


bool
AosDocBackupProc::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	
	AosXmlTagPtr conf = config->getFirstChild("doc_backup_proc");
	if (!conf) return true;

	int num_virtuals = conf->getAttrInt(AOSTAG_NUM_VIRTUALS, -1);
	if (num_virtuals < 0)
	{
		OmnAlarm << "Invalid number of virtuals: " << num_virtuals << enderr;
		exit(-1);
	}

	AosXmlTagPtr trans_config = conf->getFirstChild(AOSCONFIG_TRANS);
	if (!trans_config)
	{
		OmnAlarm << "Missing " << AOSCONFIG_TRANS 
			<< " tag. Please correct the configurations" << enderr;
			exit(-1);
	}	

	AosIdRobinPtr idRobin = OmnNew AosIdRobin(num_virtuals);
	mTransClient = OmnNew AosTransClient(trans_config, (AosTransDistributorPtr)idRobin);
	AosTransCltProcPtr thisptr(this, false);
	mTransClient->setCltProc(thisptr);
	//AosTransClient::sRecover();
	return true;
}


bool
AosDocBackupProc::addReq(
			const AosRundataPtr &rdata,
			const OmnString &request,
			const u64 &docid)
{
	aos_assert_rr(request !="", rdata, 0);
	AosXmlParser parser;
	AosXmlTagPtr transxml = parser.parse(request, "" AosMemoryCheckerArgs);
	aos_assert_r(transxml, 0);	
	aos_assert_rr(mTransClient, rdata, 0);
	bool rslt =  mTransClient->addTrans(rdata, transxml, true, docid);
	if(rslt) rdata->setOk();
	return rslt;
}

bool
AosDocBackupProc::procRecover(
			const AosXmlTagPtr &trans_doc,
			const AosTaskTransPtr &task_trans,
		    const AosRundataPtr &rdata)
{
	return true;
}

void 
AosDocBackupProc::notifyInfo(const AosXmlTagPtr &msg)
{
}
