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
// A Bitmap Dataset is a dataset whose contents are set by a bitmap.
// Each bit is a docid. 
//
// Modification History:
// 2015/01/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Dataset/Jimos/DatasetByDocids.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetByDocids_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetByDocids(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosDatasetByDocids::AosDatasetByDocids(const int version)
:
AosDataset(AOSDATASET_DOCIDS, version)
{
}


AosDatasetByDocids::~AosDatasetByDocids()
{
}


bool
AosDatasetByDocids::config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc) 
{
	aos_assert_r(worker_doc, false);
	return true;
}


bool 
AosDatasetByDocids::reset(const AosRundataPtr &rdata)
{
	OmnDelete mDocids;
	mDocids = 0;
	return true;
}


bool
AosDatasetByDocids::nextRecordset(
		const AosRundataPtr &rdata,
		AosRecordsetObjPtr &recordset)
{
	int remaining = mNumDocs - mCrtIdx;
	if (recordset)
	{
		recordset->reset();
	}
	else
	{
		if (remaining <= 0) return true;
		recordset = mRecordset->clone(rdata);
	}
	aos_assert_rr(recordset, rdata, false);
	if (remaining <= 0) return true;

	int num_docids = (remaining >= mRecordsetSize)?mRecordsetSize:remaining;
	recordset->setData(&mDocids[mCrtIdx], num_docids*sizeof(u64));
	mCrtIdx += num_docids;
	return true;
}


bool
AosDatasetByDocids::sendStart(const AosRundataPtr &rdata)
{
	return true;
}


bool
AosDatasetByDocids::sendFinish(const AosRundataPtr &rdata)
{
	return true;
}


int64_t 
AosDatasetByDocids::getNumEntries()
{
	// It assumes the number of entries is stored in the table doc.
	return mNumDocids;
}


int64_t
AosDatasetByDocids::getTotalSize() const
{
	return mNumDocids * sizeof(u64);
}

	
AosJimoPtr
AosDatasetByDocids::cloneJimo()  const
{
	try
	{
		return OmnNew AosDatasetByDocids(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
#endif
