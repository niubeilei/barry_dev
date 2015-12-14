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
#ifndef Aos_DataCube_DataCubeSQLData_h
#define Aos_DataCube_DataCubeSQLData_h

#include "DataCube/DataCube.h"
#include "SEInterfaces/FileReadListener.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEUtil/AsyncRespCaller.h"
#include "TransUtil/Ptrs.h"
#include "Database/Ptrs.h"
#include <vector> 

class AosDataCubeSQLData : public AosDataCube, public AosAsyncRespCaller 
{
	OmnDefineRCObject;

	enum                       
	{                          
		    eDftBlockSize = 1000000
	};                         
	
private:
	AosDataConnectorCallerObjPtr         mCaller;
	u32								mNumOfField;   
	u64								mCrtIdx;       
	int								mNumValues;    
	vector<OmnString>				mHeads;        
	OmnString						mSql;          
	OmnDbTablePtr					mRaw;          
	u32                             mBlockSize; 
	OmnDataStoreMySQLPtr 			mMySql;

public:
	AosDataCubeSQLData(const u32 version);
	~AosDataCubeSQLData();


	virtual OmnString getFileName() const;    
	virtual int64_t getFileLength() const;    
	virtual int getPhysicalId() const;        
	virtual AosDataConnectorObjPtr cloneDataConnector();

	virtual bool serializeFrom(AosRundata *rdata, const AosBuffPtr &buff);
	virtual bool serializeTo(AosRundata *rdata, const AosBuffPtr &buff);
	
	virtual void setCaller(const AosDataConnectorCallerObjPtr &caller);

	virtual bool readData(const u64 reqid, AosRundata *rdata);
	
	virtual bool readBlock(
					AosRundata *rdata, 
					AosBuffDataPtr &buff_data);
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
	
	// jimo, interface
	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);

	virtual void callback(                 
					const AosTransPtr &trans,  
					const AosBuffPtr &resp,
					const bool svr_death);     
};
#endif



