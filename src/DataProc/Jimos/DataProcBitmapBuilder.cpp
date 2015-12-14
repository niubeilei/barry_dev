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
// 2013/12/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataProc/Jimos/DataProcBitmapBuilder.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataProcBuildBitmap_0(const AosRundataPtr &rdata, const OmnString &version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataProcBuildBitmap(version);
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


AosDataProcBuildBitmap::AosDataProcBuildBitmap(const OmnString &version)
:
AosJimoDataProc("dataproc_bitmap_builder", version)
{
}


AosDataProcBuildBitmap::~AosDataProcBuildBitmap()
{
}


bool
AosDataProcBuildBitmap::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataProcStatus::E
AosDataProcBuildBitmap::procData(
		const AosDataRecordObjPtr &record,			
		const u64 &docid,
		const AosDataRecordObjPtr &output,			
		const AosRundataPtr &rdata)
{
	// This proc evaluates the record. If it is true, the bit is 
	// added to the bitmap. 
	aos_assert_r(mExprRaw, AosDataProcStatus::eError);
	if (mFilterRaw)
	{
		if (mFilterRaw->evalCond(record, rdata)) return AosDataProcStatus::eContinue;
	}

	if (!mExprRaw->evaluate(...)) return AosDataProcStatus::eContinue;

	rslt = mDataAssemblerRaw->appendEntry(docid, rdata);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	return AosDataProcStatus::eContinue;
}


AosDataProcObjPtr
AosDataProcBuildBitmap::clone()
{
	return OmnNew AosDataProcBuildBitmap(*this);
}


bool
AosDataProcBuildBitmap::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	if (!mExprRaw)
	{
		AosSetEntityError(rdata, "dataprocbitmapbuilder_missing_expr", 
			"Data Proc", mType) << enderr;
		return false;
	}

	bool rslt = AosDataProc::resolveIILAssembler(asms, record, rdata);
	aos_assert_r(rslt, false);
	
	aos_assert_r(record, false);
	if (mFilterRaw) mFilterRaw->resolveFieldNames(rdata, record);
	mExprRaw->resolveFieldName(rdata, record);	
	return true;
}
#endif
