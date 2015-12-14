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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Dataset/Jimos/DatasetByExternalDoc.h"

#include "API/AosApi.h"
//#include "DataAssembler/DocVarAssembler.h"
#include "DocClient/DocidMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataSplitterObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/SchemaObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetByExternalDoc_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetByExternalDoc(version);
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


AosDatasetByExternalDoc::AosDatasetByExternalDoc(
		const OmnString &type, 
		const int version)
:
AosDatasetByDataScanner(type, version)
{
}


AosDatasetByExternalDoc::AosDatasetByExternalDoc(const int version)
:
AosDatasetByDataScanner(AOSDATASET_EXTERNALDOC, version)
{
}


AosDatasetByExternalDoc::~AosDatasetByExternalDoc()
{
}


bool
AosDatasetByExternalDoc::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc) 
{
	bool rslt = AosDatasetByDataScanner::config(rdata, worker_doc);
	aos_assert_r(rslt, false);

	AosXmlTagPtr inputdataset = worker_doc->getFirstChild("inputdataset");
	aos_assert_r(inputdataset, false);

	mExternalField = inputdataset->getAttrStr("external_field");
	aos_assert_r(mExternalField != "", false);

	mExternalDataset = AosCreateDataset(rdata.getPtr(), inputdataset);
	aos_assert_r(mExternalDataset, false);
	rslt = mExternalDataset->config(rdata, inputdataset);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDatasetByExternalDoc::setValueBuff(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mExternalDataset, false);

	if (!buff || buff->dataLen() <= 0) return true;

	buff->reset();
	bool rslt = mExternalDataset->setValueBuff(buff, rdata);
	aos_assert_r(rslt, false);

	rslt = mExternalDataset->sendStart(rdata);
	aos_assert_r(rslt, false);

	AosBuffPtr raw_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);

	AosDataRecordObj * rcd;
	AosRecordsetObjPtr rcd_set;
	AosValueRslt value_rslt;
	OmnString key;

	while (1)
	{
		rslt = mExternalDataset->nextRecordset(rdata, rcd_set);
		aos_assert_r(rslt, false);

		if (AosRecordsetObj::checkEmpty(rcd_set)) break;
	
		while (1)
		{
			rslt = rcd_set->nextRecord(rdata.getPtrNoLock(), rcd);
			aos_assert_r(rslt, false);

			if (!rcd) break;

			rslt = rcd->getFieldValue(
				mExternalField, value_rslt, false, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);

			key = value_rslt.getStr();
			key.normalizeWhiteSpace(true, true);  // trim

			rslt = raw_buff->setOmnStr(key);
			aos_assert_r(rslt, false);
		}
	}

	return AosDatasetByDataScanner::setValueBuff(raw_buff, rdata);
}


AosJimoPtr
AosDatasetByExternalDoc::cloneJimo()  const
{
	try
	{
		return OmnNew AosDatasetByExternalDoc(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

