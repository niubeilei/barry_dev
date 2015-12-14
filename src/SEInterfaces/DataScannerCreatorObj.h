////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataScannerCreatorObj_h
#define Aos_SEInterfaces_DataScannerCreatorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/NetFileObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/CodeConvertion.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosDataScannerCreatorObj : virtual public OmnRCObject
{
public:
	virtual bool registerScanner(
							const OmnString &name, 
							const AosDataScannerObjPtr &scanner) = 0;

	virtual AosDataScannerObjPtr createDataScanner(
							const AosRundataPtr &rdata,
							const AosBuffPtr &buff) = 0;

	virtual AosDataScannerObjPtr createDataScanner(
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &worker_doc, 
							const AosXmlTagPtr &jimo_doc) = 0;

	virtual AosDataScannerObjPtr createFileScanner(
							const OmnString &fname,
							const int physical_id,
							const bool ignore_head,
							const OmnString &character_type,
							const OmnString &row_delimiter,
							const int record_len,
							//JACKIE-HADOOP
							const bool &ishadoop,
							const AosRundataPtr &rdata) = 0;

	virtual AosDataScannerObjPtr createVirtualFileScanner(
							const OmnString &objid,
							const AosRundataPtr &rdata) = 0;

	virtual AosDataScannerObjPtr createDirScanner(
							vector<OmnString> &dirnames,
							const int physical_id,
							const bool recursion,
							const bool ignore_head,
							const OmnString &character_type,
							const OmnString &row_delimiter,
							const AosRundataPtr &rdata) = 0;

	virtual AosDataScannerObjPtr createDirScanner(
							vector<AosFileInfo> &fileinfos,
							const int physical_id,
							const bool ignore_head,
							const OmnString &character_type,
							const OmnString &row_delimiter,
							const AosRundataPtr &rdata) = 0;

	virtual AosDataScannerObjPtr createUnicomDirScanner(
							vector<AosFileInfo> &fileinfos,
							const int physical_id,
							const OmnString &character_type,
							const OmnString &row_delimiter,
							const AosRundataPtr &rdata) = 0;

	virtual AosDataScannerObjPtr createDocScanner(
							const vector<u64> &docids,  
							const AosRundataPtr &rdata) = 0;

	virtual AosDataScannerObjPtr createMultilFileScanner(
							const AosRundataPtr &rdata) = 0;

	virtual AosDataScannerObjPtr createBinaryDocScanner(
							const vector<u64> &mDocids,
							const int mPhysicalId,
							const AosRundataPtr &rdata) = 0;

	virtual AosDataScannerObjPtr createDocScanner(
	 						const AosRundataPtr &rdata,
							const u64 *docids,
							const int64_t num_docids,
							const AosBuffPtr &field_info) = 0;
};
#endif

