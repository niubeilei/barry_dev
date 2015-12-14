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
#include "DataScanner/Testers3/NormalDoc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocClient/DocClient.h"
#include "Random/RandomUtil.h"
#include "DocServer/ProcNames.h"
#include "Rundata/Rundata.h"
#include "QueryRslt/QueryRslt.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryContextObj.h"

AosNormalDoc::AosNormalDoc()
:
mStartDocid(0),
mCrtDocid(0)
{
}


AosNormalDoc::~AosNormalDoc()
{
}

bool
AosNormalDoc::createData(const AosRundataPtr &rdata)
{
	OmnString docstr;
	//docstr << "<vpd zky_a = '11' zky_pctrs='vpd' zky_public_doc='true' zky_publc_ctnr='true'/>";
	docstr << "<vpd zky_a = \"11\"  zky_otype=\"doc\" zky_public_doc=\"true\" zky_public_ctnr=\"true\"/>";
	AosXmlTagPtr dd = AosCreateDoc(docstr, true, rdata);
	aos_assert_r(dd, false);
	u64 docid = dd->getAttrU64(AOSTAG_DOCID, 0);
	if (mStartDocid == 0) mStartDocid = docid;
	if (mCrtDocid != 0)
	{
		if (mCrtDocid +1 != docid)
		{
			OmnScreen << " mCrtDocid +1 :" << mCrtDocid +1 << " , docid: " << docid << endl;
		}
		//aos_assert_r(mCrtDocid +1 == docid, false);
	}
	mCrtDocid = docid;
	return true;
}
