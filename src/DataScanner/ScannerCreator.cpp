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
#include "DataScanner/ScannerCreator.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

#include "DataScanner/FileScanner.h"
#include "DataScanner/VirtualFileScanner.h"
#include "DataScanner/DataDclScanner.h"
#include "DataScanner/DirScanner.h"
#include "DataScanner/DocScanner.h"
#include "DataScanner/UnicomDirScanner.h"
#include "DataScanner/BinaryDocScanner.h"
#include "DataScanner/MultiFileScanner.h"



bool 
AosDataScannerCreator::registerScanner(
		const OmnString &name, 
		const AosDataScannerObjPtr &scanner)
{
	OmnNotImplementedYet;
	return false;
}


AosDataScannerObjPtr 
AosDataScannerCreator::createDataScanner(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	return AosDataScanner::createScannerFromBuff(rdata, buff);
}

 
AosDataScannerObjPtr 
AosDataScannerCreator::createDataScanner(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	return AosDataScanner::createScannerFromXml(rdata, worker_doc, jimo_doc);
}

AosDataScannerObjPtr 
AosDataScannerCreator::createFileScanner(
		const OmnString &fname,
		const int physical_id,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const int record_len,
		//JACKIE-HADOOP
		const bool &ishadoop,
		const AosRundataPtr &rdata)
{
//	AosFileScanner *scanner = OmnNew AosFileScanner(false);
//	scanner->initFileScanner(fname, physical_id, ignore_head,
//		character_type, row_delimiter, record_len, ishadoop, rdata);
//	if (scanner->getDiskError())
//	{
		return 0;
//	}
//	return scanner;
}

AosDataScannerObjPtr 
AosDataScannerCreator::createBinaryDocScanner(
		const vector<u64> &docids,
		const int mPhysicalId,
		const AosRundataPtr &rdata)
{
//	AosBinaryDocScanner *scanner = OmnNew AosBinaryDocScanner(false);
//	scanner->initBinaryDocScanner(docids, mPhysicalId, rdata);
//	return scanner;	
	return 0;
}

AosDataScannerObjPtr
AosDataScannerCreator::createVirtualFileScanner(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	return 0;
	//return OmnNew AosVirtualFileScanner(objid, rdata);
}

AosDataScannerObjPtr
AosDataScannerCreator::createDirScanner(
		vector<OmnString> &dirnames,
		const int physical_id,
		const bool recursion,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
//	AosDirScanner *scanner = OmnNew AosDirScanner(false);
//	scanner->initDirScanner(dirnames, physical_id, recursion, 
//		ignore_head, character_type, row_delimiter, rdata);
//	return scanner;
	return 0;
}

AosDataScannerObjPtr
AosDataScannerCreator::createDirScanner(
		vector<AosFileInfo> &fileinfos,
		const int physical_id,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
//	AosDirScanner *scanner = OmnNew AosDirScanner(false);
//	scanner->initDirScanner(fileinfos, physical_id, 
//		ignore_head, character_type, row_delimiter, rdata);
//	return scanner;
	return 0;
}

AosDataScannerObjPtr
AosDataScannerCreator::createUnicomDirScanner(
		vector<AosFileInfo> &fileinfos,
		const int physical_id,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
//	AosUnicomDirScanner *scanner = OmnNew AosUnicomDirScanner(false);
//	scanner->initUnicomDirScanner(fileinfos,
//		physical_id, character_type, row_delimiter, rdata);
//	return scanner;
	return 0;
}

AosDataScannerObjPtr
AosDataScannerCreator::createDocScanner(
					const vector<u64> &docids,
	 				const AosRundataPtr &rdata)
{
	AosDocScanner *scanner = OmnNew AosDocScanner(false);
	scanner->initDocScanner(docids, rdata);
	return scanner;
}

AosDataScannerObjPtr
AosDataScannerCreator::createDocScanner(
	 				const AosRundataPtr &rdata,
					const u64 *docids,
					const int64_t num_docids,
					const AosBuffPtr &field_info)
{
	AosDocScanner *scanner = OmnNew AosDocScanner(false);
	scanner->initDocScanner(rdata, docids, num_docids, field_info);
	return scanner;
}

AosDataScannerObjPtr
AosDataScannerCreator::createMultilFileScanner(const AosRundataPtr &rdata)
{
	AosMultiFileScanner *scanner = OmnNew AosMultiFileScanner(rdata);
	return scanner;
}

