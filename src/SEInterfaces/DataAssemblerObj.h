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
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataAssemblerObj_h
#define Aos_SEInterfaces_DataAssemblerObj_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DLLObj.h"
#include "SEInterfaces/TaskObj.h"
#include "Rundata/Ptrs.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
#include <map>
using namespace std;

class AosDataAssemblerObj : virtual public OmnRCObject, public AosTaskReporter
{
private:
	static AosDataAssemblerObjPtr smObject;

protected:
	OmnString	mName;

public:
	static void setDataAssembler(const AosDataAssemblerObjPtr &obj) {smObject = obj;}
	static AosDataAssemblerObjPtr getDataAssembler() {return smObject;}

	virtual u64 	getNextDocid(AosRundata *rdata) = 0;
	virtual AosDataRecordObjPtr cloneDataRecord(AosRundata *rdata) = 0;
	//virtual void 	setTask(const AosTaskObjPtr &task) = 0; 
	virtual bool	sendStart(AosRundata *rdata) = 0;
	virtual bool	sendFinish(AosRundata *rdata) = 0;
	virtual bool	waitFinish(AosRundata *rdata) = 0;
	virtual bool	reset(AosRundata *rdata) {return true; }
	virtual bool	appendEntry(
						const AosValueRslt &value,
						AosRundata *rdata) = 0;

	virtual bool addEntry( 		
						AosRundata *rdata, 
						AosDataRecordObj *record, 
						const u64 docid) = 0;

	OmnString getAssemblerName() const {return mName;}
	void setDataAssemblerName(const OmnString &name) {mName = name;}
	virtual AosXmlTagPtr getConfig () = 0;

	virtual bool isHadoop() const = 0;
	
	virtual AosDataRecordObjPtr	getOutputTemplate() = 0;	
	virtual bool appendEntry(
					AosRundata *rdata,
					AosDataRecordObj *record) = 0;

	virtual bool appendSortedFile(const u64 &file_id, const AosRundataPtr &rdata) = 0;
	virtual void tmpFilePost() = 0;
	virtual void outPutPost() = 0;
	virtual void streamPost() = 0;
	virtual void mergeFilePost() = 0;


};
#endif

