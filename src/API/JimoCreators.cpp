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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 2013/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/JimoCreators.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataConnectorObj.h"
#include "SEInterfaces/DataSplitterObj.h"
#include "SEInterfaces/DatasetObj.h"
#include "XmlUtil/XmlTag.h"



AosDatasetObjPtr AosCreateDataset(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc)
{
	AosXmlTagPtr jimo_doc;
	return AosCreateDataset(rdata, worker_doc, jimo_doc);
}


AosDatasetObjPtr AosCreateDataset(
		AosRundata *rdata,
		const AosTaskObjPtr &task,
		const AosXmlTagPtr &worker_doc)
{
	AosXmlTagPtr jimo_doc;
	AosDatasetObjPtr dataset = AosCreateDataset(rdata, worker_doc, jimo_doc);
	aos_assert_rr(dataset, rdata, 0);
	dataset->setTaskDocid(task->getTaskDocid());
	return dataset;
}


AosDatasetObjPtr AosCreateDataset(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	aos_assert_rr(worker_doc, rdata, 0);
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc);
	if (!jimo)
	{
		OmnAlarm << "fail to create jimo dataset" << enderr;
		AosSetErrorU(rdata, "failed_creating_jimo") << worker_doc->toString() << enderr;
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eDataset)
	{
		AosSetErrorU(rdata, "jimo_type_mismatch") << worker_doc->toString() << enderr;
		return 0;
	}

	AosDatasetObjPtr dataset = dynamic_cast<AosDatasetObj*>(jimo.getPtr());
	if (!dataset)
	{
		AosSetErrorU(rdata, "jimo_type_mismatch") << worker_doc->toString() << enderr;
		return 0;
	}

	return dataset;
}


AosDataScannerObjPtr AosCreateDataScanner(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc)
{
	aos_assert_rr(worker_doc, rdata, 0);
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc);
	if (!jimo)
	{
		OmnAlarm << "fail to create jimo datascanner" << enderr;
		AosSetErrorU(rdata, "failed_creating_jimo") << worker_doc->toString() << enderr;
		return 0;
	}

	
	if (jimo->getJimoType() != AosJimoType::eDataScanner)
	{
		AosSetErrorU(rdata, "jimo_type_mismatch") << worker_doc->toString() << enderr;
		return 0;
	}

	AosDataScannerObjPtr datascanner = dynamic_cast<AosDataScannerObj*>(jimo.getPtr());
	if (!datascanner)
	{
		AosSetErrorU(rdata, "jimo_type_mismatch") << worker_doc->toString() << enderr;
		return 0;
	}

	return datascanner;
}


AosDataScannerObjPtr AosCreateDataScanner(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	return AosDataScannerObj::createDataScannerStatic(rdata, worker_doc, jimo_doc);
}


AosDataScannerObjPtr AosCreateDataScanner(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	return AosDataScannerObj::createDataScannerStatic(rdata, buff);
}


AosDataSplitterObjPtr AosCreateDataSplitter(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	return AosDataSplitterObj::createDataSplitterStatic(rdata, def);
}


AosDataConnectorObjPtr AosCreateDataConnector(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	return AosDataConnectorObj::createDataConnectorStatic(rdata, worker_doc, jimo_doc);
}

AosDataConnectorObjPtr AosCreateDataConnector(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc) 
{
	return AosDataConnectorObj::createDataConnectorStatic(rdata, worker_doc);
}


AosDataConnectorObjPtr AosCreateDataConnectorFromBuff(
		AosRundata *rdata,
		const AosBuffPtr &buff)
{
	aos_assert_rr(buff, rdata, 0);
	
	AosXmlTagPtr jimo_doc = AosXmlParser::parse(buff->getOmnStr("") AosMemoryCheckerArgs);
	aos_assert_rr(jimo_doc, rdata, 0);

	AosJimoPtr jimo = AosCreateJimo(rdata, jimo_doc);
	
	if (!jimo)
	{
		OmnAlarm << "fail to create jimo datacube " << enderr;
		AosSetErrorUser(rdata, "failed_creating_data_cube") << enderr;
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eDataConnector)
	{
		AosSetErrorUser(rdata, "not_a_data_cube_jimo") << enderr;
		return 0;
	}

	AosDataConnectorObjPtr cube = dynamic_cast<AosDataConnectorObj*>(jimo.getPtr());
	if (!cube)
	{
		AosSetErrorUser(rdata, "invalid_data_cube") << enderr;
		return 0;
	}

	cube->serializeFrom(rdata, buff);
	return cube;
}

