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
// 09/20/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Snapshot/Testers/NormalDoc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "QueryRslt/QueryRslt.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryContextObj.h"
#include "DocTrans/BatchCreateDocTrans.h"
#include "DocClient/DocidMgr.h"

AosNormalDoc::AosNormalDoc(const u64 &task_docid, const AosRundataPtr &rdata)
:
mStartDocid(0),
mEndDocid(0),
mTaskDocid(task_docid)
{
}


AosNormalDoc::~AosNormalDoc()
{
}

bool
AosNormalDoc::createData(const u64 &docid, const AosRundataPtr &rdata)
{
	OmnString objid = "";
	//u64 docid = AosDocidMgr::getSelf()->nextDocid(objid, rdata);
	//u64 docid = ++smDocid;

	OmnString docstr;
	docstr << "<vpd zky_a = \"11\"  zky_otype=\"doc\" zky_public_doc=\"true\" zky_public_ctnr=\"true\" "
			<< "zky_objid = \"" << objid << "\" zky_docid =\"" << docid << " \"/>";

	char *doc = docstr.getBuffer(); 
	int record_size = docstr.length();

	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid((docid));
	aos_assert_r(mSnapIds.count(vid) != 0, false);
	AosTransPtr trans = OmnNew AosBatchCreateDocTrans(
			docid, doc, record_size, mSnapIds[vid], 
			mTaskDocid, true, false);
	AosSendTrans(rdata, trans);

	if (mStartDocid == 0) mStartDocid = docid;
	mEndDocid = docid;
	return true;
}


bool
AosNormalDoc::checkData(
		const AosRundataPtr &rdata, 
		const u64 &start_docid, 
		const u64 &end_docid)
{
	u64 idx = start_docid;
	while (idx <= end_docid)
	{
		u64 did = idx;
		AosXmlTagPtr dd = AosGetDocByDocid(did, rdata);
		//aos_assert_r(dd, false);
		if (!dd)
		{
			dd = AosGetDocByDocid(did, rdata);
			if (dd)
			{
				OmnScreen << "AAA; docid:" << did << ";" << endl;
			}
			else
			{
				OmnScreen << "BBB; docid:" << did << ";" << endl;
				dd = AosGetDocByDocid(did, rdata);
				aos_assert_r(dd, false);
			}
		}
		idx ++;
	}
	return true;
}
