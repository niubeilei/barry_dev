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
//
// Modification History:
// 2013/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCube_DataCube_h
#define Aos_DataCube_DataCube_h

#include "alarm_c/alarm.h"
#include "SEInterfaces/DataConnectorObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/Ptrs.h"


class AosDataCube : public AosDataConnectorObj
{
	OmnDefineRCObject;
	enum
	{
		eDftReadBlockSize = 10 * 1000 * 1000
	};


protected:
	OmnString		mJimoSubType;
	u64				mTaskDocid;

public:
	AosDataCube();
	AosDataCube(
			const OmnString &type,
			const u32 version);
	~AosDataCube();

	virtual bool config(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc);

	virtual bool start(AosRundata *rdata) {return true;} // Ketty 2014/05/26

	virtual OmnString getFileName() const;
	virtual int64_t getFileLength() const;
	virtual int getPhysicalId() const;
	virtual AosJimoPtr cloneJimo() const;
	virtual AosDataConnectorObjPtr cloneDataConnector();
	virtual bool serializeFrom(AosRundata *rdata, const AosBuffPtr &buff);
	virtual bool serializeTo(AosRundata *rdata, const AosBuffPtr &buff);

	virtual bool appendBlock(
					AosRundata *rdata, 
					AosBuffDataPtr &buff_data); 

	virtual bool writeBlock(
					AosRundata *rdata, 
					const int64_t pos,
					AosBuffDataPtr &buff_data); 

	virtual bool copyData(
					AosRundata *rdata, 
					const OmnString &from_name,
					const OmnString &to_name);

	virtual bool removeData(
					AosRundata *rdata, 
					const OmnString &name);

	virtual void setCaller(const AosDataConnectorCallerObjPtr &caller);

	virtual bool readData(const u64 reqid, AosRundata *rdata);
	
	virtual AosDataConnectorObjPtr	nextDataConnector();

	virtual bool setValueBuff(
					const AosBuffPtr &buff,
					AosRundata *rdata){ return false; };
	
	virtual void setTaskDocid(const u64 task_docid){mTaskDocid = task_docid;}

};
#endif



