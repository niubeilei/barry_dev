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
#include "SEInterfaces/DataScannerObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/BuffData.h"


AosDataScannerCreatorObjPtr AosDataScannerObj::smCreator;


AosDataScannerObj::AosDataScannerObj(const u32 version)
:
AosJimo(AosJimoType::eDataScanner, version)
{
}


AosDataScannerObj::~AosDataScannerObj()
{
}


AosDataScannerObjPtr 
AosDataScannerObj::createDataScannerStatic(
 		const AosRundataPtr &rdata,
 		const AosXmlTagPtr &worker_doc,
 		const AosXmlTagPtr &jimo_doc)
{
 	aos_assert_r(smCreator, 0);
 	return smCreator->createDataScanner(rdata, worker_doc, jimo_doc);
}


AosDataScannerObjPtr 
AosDataScannerObj::createDataScannerStatic(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createDataScanner(rdata, buff);
}


AosDataScannerObjPtr
AosDataScannerObj::createFileScannerStatic(
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
	aos_assert_r(smCreator, 0);
	return smCreator->createFileScanner(fname, physical_id,
		ignore_head, character_type, row_delimiter, record_len, ishadoop, rdata);
}


// AosDataScannerObjPtr 
// AosDataScannerObj::createVirtualFileScannerStatic(
// 		const OmnString &objid,
// 		const AosRundataPtr &rdata)
// {
// 	aos_assert_r(smCreator, 0);
// 	return smCreator->createVirtualFileScanner(objid, rdata);
// }


AosDataScannerObjPtr
AosDataScannerObj::createDirScannerStatic(
		vector<OmnString> &dirnames,
		const int physical_id,
		const bool recursion, 
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createDirScanner(dirnames, physical_id,
		recursion, ignore_head, character_type, row_delimiter, rdata);
}


AosDataScannerObjPtr
AosDataScannerObj::createDirScannerStatic(
		vector<AosFileInfo> &fileinfos,
		const int physical_id,
		const bool ignore_head,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createDirScanner(fileinfos, physical_id,
		ignore_head, character_type, row_delimiter, rdata);
}


AosDataScannerObjPtr
AosDataScannerObj::createUnicomDirScannerStatic(
		vector<AosFileInfo> &fileinfos,
		const int physical_id,
		const OmnString &character_type,
		const OmnString &row_delimiter,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createUnicomDirScanner(fileinfos,
		physical_id, character_type, row_delimiter, rdata);
}


AosDataScannerObjPtr 
AosDataScannerObj::createBinaryDocScannerStatic(
		const vector<u64> &docids,
		const int physical_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createBinaryDocScanner(docids, physical_id, rdata);
}


AosDataScannerObjPtr
AosDataScannerObj::createDocScannerStatic(
		const vector<u64> &docids,  
		const AosRundataPtr &rdata)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createDocScanner(docids, rdata);
}


AosDataScannerObjPtr
AosDataScannerObj::createMultiFileScannerStatic(
		const AosRundataPtr &rdata)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createMultilFileScanner(rdata);
}


// bool
// AosDataScannerObj::registerDataScannerStatic(
// 		const OmnString &name, 
// 		const AosDataScannerObjPtr &scanner)
// {
// 	aos_assert_r(smCreator, false);
// 	return smCreator->registerDataScanner(name, scanner);
// }


// Chen Ding, 2013/10/29
bool
AosDataScannerObj::reset(const AosRundataPtr &rdata)
{
	return true;
	//OmnShouldNeverComeHere;
	//return false;
}


// 2013/09/23, Jackie
AosDataScannerObjPtr 
AosDataScannerObj::createDocScannerStatic(
	 	const AosRundataPtr &rdata,
		const u64 *docids,
		const int64_t num_docids,
		const AosBuffPtr &field_info)
{
	return smCreator->createDocScanner(rdata, docids, num_docids, field_info);
}
