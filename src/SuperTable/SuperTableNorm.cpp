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
// 2013/10/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SuperTable/SuperTableNorm.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSuperTableNorm(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &sdoc) 
{
	try
	{
		OmnScreen << "To create SuperTableNorm Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSuperTableNorm(rdata, sdoc);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
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


AosSuperTableNorm::AosSuperTableNorm(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &doc)
:
AosSuperTable(rdata, doc)
{
	if (!init(rdata)) OmnThrowException(rdata->getErrmsg());
}


bool
AosSuperTableNorm::init(const AosRundataPtr &rdata)
{
	if (mObjid == "")
	{
		AosSetErrorU(rdata, "missing_objid") << enderr;
		return false;
	}

	if (!retrieveTableDoc(rdata)) return false;
	if (!createDataset(rdata)) return false;
	return true;
}


bool 
AosSuperTableNorm::reset(const AosRundataPtr &rdata)
{
	aos_assert_rr(mDataset, rdata, false);
	return mDataset->reset(rdata);
}


bool
AosSuperTableNorm::nextRecord(
		const AosRundataPtr &rdata,
		AosDataRecordObjPtr &record)
{
	aos_assert_rr(mDataset, rdata, 0);
	return mDataset->getNextRecord(rdata);
}


int64_t 
AosSuperTableNorm::getNumEntries(const AosRundataPtr &rdata)
{
	// It assumes the number of entries is stored in the table doc.
	AosXmlTagPtr doc = retrieveTableDoc(rdata);
	if (!doc) return -1;
	return doc->getAttrInt64(AOSTAG_NUM_ENTRIES, -1);
}


bool
AosSuperTableNorm::createDataset(const AosRundataPtr &rdata)
{
	// Data scanner is defined by the table doc:
	// 	<supertable ...>
	// 		<data_scanner ...>
	// 			...
	// 		</data_scanner>
	// 	</supertable>
	AosXmlTagPtr doc = retrieveTableDoc(rdata);
	if (!doc) return false;
	AosXmlTagPtr def = doc->getFirstChild(AOSTAG_DATASET);
	if (!def)
	{
		AosSetErrorUser(rdata, "no_dataset_defined") << mObjid << enderr;
		return false;
	}

	mDataset = AosCreateDataset(rdata, def);
	if (!mDataset) return false;
	return true;
}

