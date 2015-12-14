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
// 2014/05/05 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_Jimos_DatasetByStatQuerySingle_h
#define Aos_Dataset_Jimos_DatasetByStatQuerySingle_h

#include "Dataset/Dataset.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Dataset/Dataset.h"
#include "StatUtil/Ptrs.h"
#include "StatUtil/StatQueryInfo.h"
#include "StatUtil/StatCond.h"
#include "StatEngine/Ptrs.h"
#include "Dataset/Jimos/DatasetByQuery.h"
//#include "SEInterfaces/QueryReqObj.h"
#include "GroupQuery/Ptrs.h"

class AosDatasetByStatQuerySingle : public AosDatasetByQuery
{
	OmnDefineRCObject;

	struct InputStatFieldDef
	{
		OmnString	mStatFname;
		int		mStatFieldIdx;
		int			mInputFieldIdx;
	};
	
	struct StatQryProcer
	{
		AosDataScannerObjPtr mDataScanner;
		AosGroupQueryObjPtr	mIILQuery;
		AosStatisticPtr		mStat;
		AosStatQueryInfo	mStatQryInfo;
		bool				mNeedIILQuery;
	
		AosBuffPtr			mStatDocidsBuff;
		u32					mTotalDocidNum;
		u32					mQryIILTime;
		u32					mQryStatTime;
		
		void	reset()
		{
			mDataScanner = 0;
			mIILQuery = 0;
			mStat = 0;
			//mStatQryInfo;
			mNeedIILQuery = true;
	
			mStatDocidsBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			mTotalDocidNum = 0;
			mQryIILTime = 0;
			mQryStatTime = 0;
		}
	};

private:
	vector<StatQryProcer>		mStatQryProcers;
	AosStatQueryInfo			mStatQryInfo;
	AosStatQryEnginePtr			mStatQryEngine;
	
	AosDataRecordObjPtr			mInputDataRecord;
	vector<InputStatFieldDef>	mInputStatFieldDefs;

	bool						mDataReady;
	AosRecordsetObjPtr			mTmpRecordset;
	
	AosStatTimeUnit::E 			mTimeUnit;
	
	OmnString 					mTableName;
	
	map<int,AosDataType::E>		mKeyIdxTypeMap;
	map<int,AosDataType::E>		mGroupByKeyType;
public:
	AosDatasetByStatQuerySingle(const int version);
	~AosDatasetByStatQuerySingle();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo()  const;
	virtual bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
	
	virtual bool reset(const AosRundataPtr &rdata);
	
	virtual void setTaskDocid(const u64 task_docid);
	
	virtual bool sendStart(const AosRundataPtr &rdata);

	virtual bool sendFinish(const AosRundataPtr &rdata);
	
	virtual bool nextRecordset(
				const AosRundataPtr &rdata, 
				AosRecordsetObjPtr &recordset);

	// DatasetByQuery Internface
	virtual bool checkIsIndexQuery(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &def){ return false; };
	
	virtual bool getRsltInfor(
				AosXmlTagPtr &infor,
				const AosRundataPtr &rdata);

	virtual AosRecordsetObjPtr  cloneRecordset(AosRundata *rdata){return mTmpRecordset->clone(rdata);}

private:
	bool 	initStatQryProcers(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);

	bool 	initIILQuery(
				const AosRundataPtr &rdata,
				AosStatQueryInfo &stat_qry_info,
				AosGroupQueryObjPtr &iil_query,
				bool &need_iil_query);

	bool 	setIILQryGroupFieldsP1(
				AosStatQueryInfo &stat_qry_info,
				AosGroupQueryObjPtr &iil_query,
				bool &need_full_scan);
	bool 	setIILQryGroupFieldsP2(
				AosStatQueryInfo &stat_qry_info,
				AosGroupQueryObjPtr &iil_query,
				bool &need_full_scan);
	bool 	setIILQryGroupFieldsP3(
				AosStatQueryInfo &stat_qry_info,
				AosGroupQueryObjPtr &iil_query,
				bool &need_full_scan);

	bool 	configDataScanner(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf,
				const AosStatisticPtr &stat,
				AosStatQueryInfo &stat_qry_info,
				AosDataScannerObjPtr & data_scanner);

	bool 	initTmpRecordset(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);

	bool 	initInputDataRecord(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);
	
	bool 	initInputStatRcdFieldDef(const AosRundataPtr &rdata);

	bool 	queryIIL(const AosRundataPtr &rdata, bool &finished);

	bool 	queryEachIIL(
				const AosRundataPtr &rdata,
				const AosGroupQueryObjPtr &iil_query,
				const AosBuffPtr &rslt_buff,
				u32 &docid_num);

	//bool 	directStatQuery(const AosRundataPtr &rdata);

	bool 	readDataByDataScanner(const AosRundataPtr &rdata);

	bool 	readDataByEachDataScanner(
				const AosRundataPtr &rdata,
				const AosDataScannerObjPtr &data_scanner,
				const AosBuffPtr &stat_docids_buff);

	//bool 	getStatRecordsFromBuff(
	//			const AosRundataPtr &rdata,
	//			const AosBuffDataPtr &buffdata,
	//			vector<AosStatRecord *> &stat_rcds);

	//bool 	getNextRecord(
	//			const AosRundataPtr &rdata,
	//			const u32 rcd_idx,
	//			AosDataRecordObjPtr &rcd);

	bool 	getNextInputRecord(
				const AosRundataPtr &rdata,
				const u32 rcd_idx,
				AosDataRecordObjPtr &record);

/*	bool 	checkIsTimeField(
			const AosRundataPtr &rdata,
			const OmnString &field_name);
*/
};
#endif

