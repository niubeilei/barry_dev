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
// 2013/11/24 Created by Chen Ding
// 2015/01/28 Renamed from DataCube.
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataConnectorObj_h
#define Aos_SEInterfaces_DataConnectorObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskDataObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/DiskStat.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

#define AOS_DATACUBETYPE_DUMMY				"dummy"
#define AOS_DATACUBETYPE_FILE				"file"
#define AOS_DATACUBETYPE_FILES				"files"
#define AOS_DATACUBETYPE_DIR				"dir"
#define AOS_DATACUBETYPE_HADOOP_FILE		"hadoop_file"
#define AOS_DATACUBETYPE_HADOOP_QUERY		"hadoop_query"
#define AOS_DATACUBETYPE_IDFILES			"idfiles"
#define AOS_DATACUBETYPE_READDOC_BYSORT		"readdoc_bysort"
#define AOS_DATACUBETYPE_READDOC_NOSORT		"readdoc_nosort"
#define AOS_DATACUBETYPE_READDOC_BYFILTER	"readdoc_byfilter"
#define AOS_DATACUBETYPE_SQLDATA 			"sql_data"
#define AOS_DATACUBETYPE_UNICOMDOCUMENT		"unicom_document"
#define AOS_DATACUBETYPE_HBASE				"hbase"
#define AOS_DATACUBETYPE_BUFF				"buff"
#define AOS_DATACUBETYPE_IIL				"iil"
#define AOS_DATACUBETYPE_DATACOLLECTOR		"datacol"

class AosDataConnectorCallerObj : public OmnRCObject
{
public:
	virtual void callBack(
					const u64 &reqid,
					const AosBuffDataPtr &buff, 
					bool finished) = 0;
	
};

class AosDataConnectorObj : public AosJimo, public AosTaskReporter
{
protected:
	OmnString			mDataConnectorType;
	AosDiskStat			mDiskStat;

public:
	AosDataConnectorObj();
	AosDataConnectorObj(const u32 version);
	~AosDataConnectorObj();

	virtual bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &worker_doc) = 0;
	virtual bool start(AosRundata *rdata) = 0; 		// Ketty 2014/05/26

	virtual OmnString getFileName() const = 0;
	virtual int64_t getFileLength() const = 0;
	virtual int getPhysicalId() const = 0;
	virtual AosDataConnectorObjPtr cloneDataConnector() = 0;
	virtual bool serializeFrom(AosRundata *rdata, const AosBuffPtr &buff) = 0;
	virtual bool serializeTo(AosRundata *rdata, const AosBuffPtr &buff) = 0;

	virtual void setCaller(const AosDataConnectorCallerObjPtr &caller) = 0;

	virtual bool readData(const u64 reqid, AosRundata *rdata) = 0;

	virtual bool appendBlock(
					AosRundata *rdata, 
					AosBuffDataPtr &buff_data) = 0; 

	virtual bool writeBlock(
					AosRundata *rdata, 
					const int64_t pos,
					AosBuffDataPtr &buff_data) = 0; 

	virtual bool copyData(
					AosRundata *rdata, 
					const OmnString &from_name,
					const OmnString &to_name) = 0;

	virtual bool removeData(
					AosRundata *rdata, 
					const OmnString &name) = 0;
	
	virtual AosDataConnectorObjPtr	nextDataConnector() = 0;

	virtual bool setValueBuff(
					const AosBuffPtr &buff,
					AosRundata *rdata) = 0;

	OmnString getDataConnectorType() const {return mDataConnectorType;}
	AosDiskStat getDiskStat() const {return mDiskStat;}

	virtual void setTaskDocid(const u64 task_docid) = 0;
	
	static AosDataConnectorObjPtr createDataConnectorStatic(
					AosRundata *rdata, 
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);
	
	static AosDataConnectorObjPtr createDataConnectorStatic(
					AosRundata *rdata, 
					const AosXmlTagPtr &worker_doc);
};
#endif
