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
#include "Dataset/Jimos/DatasetByBitmap.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetByBitmap_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetByBitmap(version);
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


AosDatasetByBitmap::AosDatasetByBitmap(const int version)
:
AosDataset(AOSDATASET_BITMAP, version)
{
}


AosDatasetByBitmap::~AosDatasetByBitmap()
{
}


bool
AosDatasetByBitmap::config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc) 
{
	aos_assert_r(worker_doc, false);
	return true;
}


bool 
AosDatasetByBitmap::reset(const AosRundataPtr &rdata)
{
	OmnDelete mDocids;
	mDocids = 0;
	return true;
}


bool
AosDatasetByBitmap::nextRecordset(
		const AosRundataPtr &rdata,
		AosRecordsetObjPtr &recordset)
{
	if (recordset)
	{
		recordset->reset();
	}
	else
	{
		if (!mBitmap->hasMore()) return true;
		recordset = mRecordset->clone(rdata);
	}
	aos_assert_rr(recordset, rdata, false);
	if (!mBitmap->hasMore()) return true;

	AosBuffPtr buff = OmnNew AosBuff(mRecordsetSize*sizeof(u64));
	u64 *docids = (u64 *)buff->data();
	int idx = 0;
	while ((docid = mBitmap->nextDocid()) != 0)
	{
		docids[idx++] = docid;	
	}
	return true;
}


bool
AosDatasetByBitmap::sendStart(const AosRundataPtr &rdata)
{
	return true;
}

bool
AosDatasetByBitmap::sendFinish(const AosRundataPtr &rdata)
{
	return true;
}

int64_t 
AosDatasetByBitmap::getNumEntries()
{
	// It assumes the number of entries is stored in the table doc.
	if (!mBitmap) return 0;
	return mBitmap->getNumBits();
}


int64_t
AosDatasetByBitmap::getTotalSize() const
{
	if (!mBitmap) return 0;
	return mBitmap->getNumBits();
}

	
AosJimoPtr
AosDatasetByBitmap::cloneJimo()  const
{
	try
	{
		return OmnNew AosDatasetByBitmap(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
#endif
