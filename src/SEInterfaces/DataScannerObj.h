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
#ifndef Aos_SEInterfaces_DataScannerObj_h
#define Aos_SEInterfaces_DataScannerObj_h

#include "DataScanner/Ptrs.h"
#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataScannerType.h"
#include "SEInterfaces/DataScannerCreatorObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "Thread/Thread.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/CodeConvertion.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosDataScannerObj : public AosJimo, public AosTaskReporter
{
protected:
	static AosDataScannerCreatorObjPtr	smCreator;
	AosDataScannerType::E				mType;

public:
	AosDataScannerObj(const u32 version);
	
	virtual ~AosDataScannerObj();

	virtual AosDataScannerType::E getType() const {return mType;}

	virtual bool 	getNextBlock(AosBuffDataPtr &info, const AosRundataPtr &rdata) = 0;
	virtual int		getPhysicalId() const = 0;
	virtual AosDataScannerObjPtr clone(
						const AosXmlTagPtr &def,  
						const AosRundataPtr &rdata) = 0;
	virtual u64 getInstanceId() const = 0;
	virtual bool 	serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;
	virtual bool 	serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;
	virtual AosDataConnectorObjPtr firstDataConnector() = 0;
	virtual AosDataConnectorObjPtr nextDataConnector() = 0;
	virtual bool reset(const AosRundataPtr &rdata);	// Used to reset the getNextRecord() loop.
	
	// used by DataScannerCube
	virtual bool addDataConnector(
						const AosRundataPtr &rdata, 
						const AosDataConnectorObjPtr &cube) = 0;
	
	
	virtual	bool	setRemainingBuff(
						const AosBuffDataPtr &buffdata,
						const bool incomplete,
						const int64_t proc_size,
						const AosRundataPtr &rdata) = 0;

	virtual bool	startReadData(const AosRundataPtr &rdata) = 0;

	/*
	virtual void	callBack(
						const u64 &reqId,
						const AosBuffDataPtr &buffdata,
						const bool isFinished) = 0;
	*/
	// Used by ActCreateHitIIL
	virtual void setFiles(const vector<AosNetFileObjPtr> &vv) = 0;
	virtual void setActionsCaller(const AosActionCallerPtr &caller) = 0;
	virtual void addRequest(const u64 &reqId, const int64_t &blocksize) = 0;
	virtual AosBuffPtr getNextBlock(const u64 &reqId, const int64_t &expected_size) = 0;
	virtual void    destroyedMember() = 0;
	virtual bool	isFinished(const AosRundataPtr &rdata) = 0;

	// Used by ActScanFile
	virtual bool	getNextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata) = 0;
	virtual bool	readBuffToQueue(OmnThrdStatus::E &state, const AosRundataPtr &rdata) = 0;
	virtual bool	readBuffFromQueue(AosBuffDataPtr &info, const AosRundataPtr &rdata) = 0;
	virtual int64_t getTotalFileLengthByDir() = 0;
	// Chen Ding, 2013/11/24
	// virtual int64_t getTotalFileLength() = 0;
	virtual int64_t getTotalSize() const = 0;
	virtual bool getDiskError() const = 0;

	virtual bool setValueBuff(
					const AosBuffPtr &buff,
					const AosRundataPtr &rdata) = 0;

	virtual void setTaskDocid(const u64 task_docid) = 0;
	virtual bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc) = 0;

	/*
	virtual bool	getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata) = 0;

	virtual OmnString	getCrtFileName() = 0;


	// Chen Ding, 2013/10/29
	AosDataRecordObjPtr getNextRecord(const AosRundataPtr &rdata)
	{
		AosDataRecordObjPtr record;
		if (!getNextRecord(record, rdata)) return 0;
		return record;
	}
	*/

	static AosDataScannerObjPtr createDataScannerStatic(
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &worker_doc, 
							const AosXmlTagPtr &jimo_doc);

	static AosDataScannerObjPtr createDataScannerStatic(
							const AosRundataPtr &rdata,
							const AosBuffPtr &buff);

	static AosDataScannerObjPtr createFileScannerStatic(
	 						const OmnString &fname,
	 						const int physical_id,
	 						const bool ignore_head,
	 						const OmnString &character_type,
	 						const OmnString &row_delimiter,
	 						const int record_len,
							//JACKIE-HADOOP
							const bool &ishadoop,
	 						const AosRundataPtr &rdata);

	static AosDataScannerObjPtr createDirScannerStatic(
							vector<OmnString> &dirnames,
							const int physical_id,
							const bool recursion,
							const bool ignore_head,
							const OmnString &character_type,
							const OmnString &row_delimiter,
							const AosRundataPtr &rdata);

	static AosDataScannerObjPtr createDirScannerStatic(
							vector<AosFileInfo> &fileinfos,
							const int physical_id,
							const bool ignore_head,
							const OmnString &character_type,
							const OmnString &row_delimiter,
							const AosRundataPtr &rdata);

	static AosDataScannerObjPtr createUnicomDirScannerStatic(
	 						vector<AosFileInfo> &fileinfos,
	 						const int physical_id,
	 						const OmnString &character_type,
	 						const OmnString &row_delimiter,
	 						const AosRundataPtr &rdata);

	static AosDataScannerObjPtr createDocScannerStatic(
	 						const vector<u64> &docids,  
	 						const AosRundataPtr &rdata);

	static AosDataScannerObjPtr createDocScannerStatic(
							const AosRundataPtr &rdata,
							const u64 *docids,
							const int64_t num_docids,
							const AosBuffPtr &field_info);

	static AosDataScannerObjPtr createMultiFileScannerStatic(
							const AosRundataPtr &rdata);

	static AosDataScannerObjPtr createBinaryDocScannerStatic(
							const vector<u64> &docids,
							const int physicalId,
							const AosRundataPtr &rdata);

	static void setCreator(const AosDataScannerCreatorObjPtr &creator) {smCreator = creator;}
	static AosDataScannerCreatorObjPtr getCreator() {return smCreator;}

};

#endif

