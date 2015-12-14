///////////////////////////////////////////////////////////////////////////
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
// 05/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataScanner_DataScanner_h
#define Aos_DataScanner_DataScanner_h

#include "DataScanner/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Buff.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataScannerType.h"
#include "SEInterfaces/BuffData.h"
#include "SEInterfaces/Ptrs.h"

class AosDataScanner : public AosDataScannerObj
{
protected:
	u64			mInstanceId;
	OmnString 	mScannerType;
	bool		mDiskError;

	u64 		mTaskDocid;

public:
	AosDataScanner(
			const OmnString &type,
			const u32 version);
	AosDataScanner(
			const OmnString &name, 
			const AosDataScannerType::E type,
			const bool flag);
	~AosDataScanner();

	virtual bool readBuffToQueue(OmnThrdStatus::E &state, const AosRundataPtr &rdata);
	virtual bool readBuffFromQueue(AosBuffDataPtr &info, const AosRundataPtr &rdata);
	virtual bool isFinished(const AosRundataPtr &rdata);
	virtual bool getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool getNextBlock(AosBuffDataPtr &info, const AosRundataPtr &rdata);
	virtual bool getNextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata);
	virtual bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool reset(const AosRundataPtr &rdata) { return true;}

	// for DataScanner Jimo
	virtual bool addDataConnector(
						const AosRundataPtr &rdata, 
						const AosDataConnectorObjPtr &cube);


	virtual bool split(
					const AosRundataPtr &rdata, 
					vector<AosXmlTagPtr> &datasets) const;
	
	virtual	bool	setRemainingBuff(
						const AosBuffDataPtr &buffdata,
						const bool incomplete,
						const int64_t proc_size,
						const AosRundataPtr &rdata);

	virtual bool	startReadData(const AosRundataPtr &rdata);
	
//	virtual void	callBack(
//						const u64 &reqId,
//						const AosBuffDataPtr &buffdata,
//						const bool isFinished);
	
	// virtual int64_t getTotalFileLength();
	virtual int64_t getTotalSize() const;
	virtual int64_t getTotalFileLengthByDir();
	
	static AosDataScannerObjPtr createScannerFromBuff(
					const AosRundataPtr &rdata,
					const AosBuffPtr &buff);

	static AosDataScannerObjPtr createScannerFromXml(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc, 
					const AosXmlTagPtr &jimo_doc);

	virtual void setFiles(const vector<AosNetFileObjPtr> &vv);
	virtual void setActionsCaller(const AosActionCallerPtr &caller);
	virtual AosBuffPtr getNextBlock(const u64 &reqId, const int64_t &expected_size);
	virtual void    addRequest(const u64 &reqId, const int64_t &blocksize);
	virtual void 	destroyedMember();
	virtual OmnString getCrtFileName(){ return ""; }
	virtual u64 getInstanceId() const;

	virtual int getPhysicalId() const;
	virtual AosDataConnectorObjPtr firstDataConnector();
	virtual AosDataConnectorObjPtr nextDataConnector();
	virtual bool getDiskError() const { return mDiskError; }
	
	virtual bool setValueBuff(
					const AosBuffPtr &buff,
					const AosRundataPtr &rdata){ return false; };
	
	virtual void setTaskDocid(const u64 task_docid){mTaskDocid = task_docid;}

	virtual bool config(const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc, 
			const AosXmlTagPtr &jimodoc) {return true;}
	virtual bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc);

protected:
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	bool registerScanner(const OmnString &name, const AosDataScannerObjPtr  &scanner);
	static bool init(const AosRundataPtr &rdata);
};

#endif

