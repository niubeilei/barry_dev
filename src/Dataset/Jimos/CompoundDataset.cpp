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
// This dataset uses one data scanner as its inputs. The data scanner
// can be any type. Most datasets can be implemented by this class.
//
// Modification History:
// 2013/11/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Dataset/CompoundDataset.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "XmlUtil/XmlTag.h"



AosCompoundDataset::AosCompoundDataset(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
:
AosDataset(rdata, worker_doc, jimo_doc),
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr())
{
	if (!config(rdata, worker_doc, jimo_doc)) OmnThrowException(rdata->getErrmsg());
}


bool
AosCompoundDataset::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


bool 
AosCompoundDataset::reset(const AosRundataPtr &rdata)
{
	aos_assert_rr(mDataScanner, rdata, false);
	return mDataScanner->reset(rdata);
}


bool
AosCompoundDataset::nextRecordset(
		const AosRundataPtr &rdata,
		AosRecordsetObjPtr &recordset)
{
	// This function retrieves the next record set. If the 
	// dataset has finished or failed, it returns. Otherwise,
	// if there are some results, it gets the first one and returns.
	// Otherwise, it waits until either the dataset finished or
	// some recordsets are available.

	return true;
}
#endif
