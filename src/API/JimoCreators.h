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
#ifndef AOS_API_JimoCreator_h
#define AOS_API_JimoCreator_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Ptrs.h"

extern AosDatasetObjPtr AosCreateDataset(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc);

extern AosDatasetObjPtr AosCreateDataset(
		AosRundata *rdata,
		const AosTaskObjPtr &task,
		const AosXmlTagPtr &worker_doc);

extern AosDatasetObjPtr AosCreateDataset(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc);

extern AosDatasetObjPtr AosCreateDataset(
		AosRundata *rdata, 
		const AosDataSourceObjPtr &data_source);

extern AosDatasetObjPtr AosCreateDataset(
		AosRundata *rdata, 
		const AosXmlTagPtr &def);

extern AosDataSplitterObjPtr AosCreateDataSplitter(
		AosRundata *rdata, 
		const AosXmlTagPtr &def);

extern AosDataConnectorObjPtr AosCreateDataConnector(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc);

extern AosDataConnectorObjPtr AosCreateDataConnector(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc); 

extern AosDataConnectorObjPtr AosCreateDataConnectorFromBuff(
		AosRundata *rdata,
		const AosBuffPtr &buff);

extern AosDataScannerObjPtr AosCreateDataScanner(
			AosRundata *rdata, 
			const AosBuffPtr &buff);

extern AosDataScannerObjPtr AosCreateDataScanner(
			AosRundata *rdata, 
			const AosXmlTagPtr &def);

extern AosDataScannerObjPtr AosCreateDataScanner(
			AosRundata *rdata, 
			const AosXmlTagPtr &worke_doc,
			const AosXmlTagPtr &jimo_doc);


#endif
