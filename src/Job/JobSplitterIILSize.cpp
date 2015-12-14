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
// This job splitter splits jobs based on IIL size. The input is an IIL.
// It creates one task for each N number of entries in the IIL.
//
// Modification History:
// 04/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Job/JobSplitterIILSize.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

/*
AosJobSplitterIILSize::AosJobSplitterIILSize(const bool flag)
:
AosJobSplitter(AOSJOBSPLITTER_IILSIZE, AosJobSplitterId::eIILSize, flag)
{
}


AosJobSplitterIILSize::AosJobSplitterIILSize(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
:
AosJobSplitter(AOSJOBSPLITTER_IILSIZE, AosJobSplitterId::eIILSize, false),
mSdoc(sdoc)
{
}


AosJobSplitterIILSize::~AosJobSplitterIILSize()
{
}


bool 
AosJobSplitterIILSize::resetLoop(const AosRundataPtr &rdata)
{
	// This job splitter splits jobs based on IIL size. The input is an IIL.
	// It creates one task for each N number of entries in the IIL.
	
	aos_assert_rr(mSdoc, rdata, false);

	// 1. Retrieve the IILID
	u64 iilid = mSdoc->getAttrU64(AOSTAG_IILID, 0);
	if (iilid == 0)
	{
		// The IILID is not there yet. Retrieve the IILName.
		OmnString iilname = mSdoc->getAttrStr(AOSTAG_IILNAME);
		if (iilname == "")
		{
			AosSetErrorU(rdata, "missing_iil") << ": " << mSdoc->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		mIIL = AosRetrieveIIL(iilname, true, mIILHoldingTimerSec, rdata);
		if (!mIIL)
		{
			AosSetErrorU(rdata, "failed_retrieve_iil") << ": " << iilname;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	else
	{
		mIIL = AosRetrieveIIL(iilid, true, mIILHoldingTimerSec, rdata);
		if (!mIIL)
		{
			AosSetErrorU(rdata, "failed_retrieve_iil") << ": " << iilid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	// When it comes to this point, the IIL is retrieved (in 'iil') and it is
	// local. 
	aos_assert_rr(mIIL, rdata, false);

	// Retrieve the partition size
	mPartitionSize = mSdoc->getAttrInt64(AOSTAG_PARTITION_SIZE, -1);
	if (!mPartitionSize <= 0)
	{
		AosSetErrorU(rdata, "partition_size_invalid") << ": " << mSdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mIILIdx.setRestart();
	return true;
}


bool
AosJobSplitterIILSize::createTasks(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosJobSplitterObjPtr
AosJobSplitterIILSize::create(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) const
{
	try                                             
	{
		AosJobSplitterObjPtr splitter = OmnNew AosJobSplitterIILSize(sdoc, rdata);
		return splitter;
	}
	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_JobSplitterIILSize");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
*/

#endif
