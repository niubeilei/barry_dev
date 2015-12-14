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
// 07/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataScanner_DataScannerCreator_h
#define Aos_DataScanner_DataScannerCreator_h

#include "SEInterfaces/DataScannerCreatorObj.h"


class AosDataScannerCreator : virtual public AosDataScannerCreatorObj
{
	OmnDefineRCObject;

public:
	virtual bool	registerScanner(
							const OmnString &name, 
							const AosDataScannerObjPtr &scanner);

	virtual AosDataScannerObjPtr createDataScanner(
							const AosRundataPtr &rdata,
							const AosBuffPtr &buff);

	virtual AosDataScannerObjPtr createDataScanner(
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &worker_doc, 
							const AosXmlTagPtr &jimo_doc);

	virtual AosDataScannerObjPtr createFileScanner(
							const OmnString &fname,
							const int physical_id,
							const bool ignore_head,
							const OmnString &character_type,
							const OmnString &row_delimiter,
							const int record_len,
							//JACKIE-HADOOP
							const bool &ishadoop,
							const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr createVirtualFileScanner(
							const OmnString &objid,
							const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr createDirScanner(
							vector<OmnString> &dirnames,
							const int physical_id,
							const bool recursion,
							const bool ignore_head,
							const OmnString &character_type,
							const OmnString &row_delimiter,
							const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr createDirScanner(
							vector<AosFileInfo> &fileinfos,
							const int physical_id,
							const bool ignore_head,
							const OmnString &character_type,
							const OmnString &row_delimiter,
							const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr createUnicomDirScanner(
							vector<AosFileInfo> &fileinfos,
							const int physical_id,
							const OmnString &character_type,
							const OmnString &row_delimiter,
							const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr createDocScanner(
							const vector<u64> &docids,
							const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr createMultilFileScanner(
							const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr createBinaryDocScanner(
							const vector<u64> &docids,
							const int mPhysicalId,
							const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr createDocScanner(
	 						const AosRundataPtr &rdata,
							const u64 *docids,
							const int64_t num_docids,
							const AosBuffPtr &field_info);


};
#endif

