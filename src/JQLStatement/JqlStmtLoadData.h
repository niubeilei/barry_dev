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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtLoadData_H
#define AOS_JQLStatement_JqlStmtLoadData_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"
#include "AosConf/Job.h"
#include "AosConf/JobTask.h"
#include "AosConf/MapTask.h"
#include "AosConf/ReduceTask.h"
#include "AosConf/DataProc.h"
#include "AosConf/DataEngine.h"
#include "AosConf/DataRecord.h"
#include "AosConf/DataScanner.h"
#include "AosConf/DataSchema.h"
#include "AosConf/DataSet.h"
#include "AosConf/DataEngineScanMul.h"
#include "AosConf/DataEngineJoin.h"
using namespace AosConf;

class AosJqlStmtLoadData : public AosJqlStatement
{
private:
	enum
	{
		eMaxThreads = 4
	};

	//data from JQLParser
	OmnString							mLoadDataName;
	OmnString							mFromDataSet;
	OmnString							mToDataSet;
	OmnString							mDataRecordName;
	bool								mIsTable;
	bool								mIsHbase;
	u64 								mOffSet;
	u32									mMaxThread;
	AosExprList*						mRawKeyList;
	OmnString	 						mRecordType;
	OmnString							mRecordName;
	int 								mRecordLen;
	u32 								mMaxTask;
	deque<OmnString>					mIndexObjids;
	vector<OmnString>					mOptionFields;
	map<OmnString, OmnString>			mOptionFieldsMap;
	vector<OmnString>					mVirtualFieldOprType;
	map<OmnString, OmnString>			mInputDataFieldConfMap;
	vector<pair<OmnString, OmnString> >	mVirtualFieldNames;
	vector<pair<OmnString, OmnString> >	mVirtualFieldIONames;
	vector<pair<u32, u32> >				mVirtualFieldIOLens;
	int									mFieldSeqno;		// Chen Ding, 2014/07/30
	u32 								mVersionNum;
	OmnString 							mErrmsg;

public:
	AosJqlStmtLoadData();
	AosJqlStmtLoadData(const OmnString &errmsg);
	~AosJqlStmtLoadData();

	//getter/setters
	void setLoadDataName(OmnString name);
	void setFromDataSet(OmnString name);
	void setToDataSet(OmnString name);
	void setIsHbase(bool is_hbase);
	void setRawkeyList(AosExprList* rawkey_list);
	void setMaxThread(u32 num){ mMaxThread = num;}
	void setTaskNum(u32 num) { mMaxTask = num; }
	void setVersionNum(u32 num) { mVersionNum = num; }
	void setOptionFields(AosExprList *fieldnames);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createLoadData(const AosRundataPtr &rdata);  
	bool runLoadData(const AosRundataPtr &rdata);  
	bool showLoadDatas(const AosRundataPtr &rdata);   
	bool describeLoadData(const AosRundataPtr &rdata);
	bool dropLoadData(const AosRundataPtr &rdata);   

	AosXmlTagPtr getInputDatasetConf(const AosRundataPtr &rdata);
	AosXmlTagPtr getOutputDatasetsConf(const AosRundataPtr &rdata);
	AosXmlTagPtr getJobConf(const AosRundataPtr &rdata);
	AosXmlTagPtr getMapTaskConf(const AosRundataPtr &rdata, const AosXmlTagPtr &inputRecordConf);
	AosXmlTagPtr getReduceTaskConf(const AosRundataPtr &rdata, const AosXmlTagPtr &inputRecordConf);
	vector<AosXmlTagPtr> getVirtualFieldInputDataFieldConf(const AosRundataPtr &rdata);
	vector<AosXmlTagPtr> getMapDatafieldConf(const AosRundataPtr &rdata, const AosXmlTagPtr &mapNode);
	int getValueLength(const OmnString &map_name);
	vector<AosXmlTagPtr> getVirtualFieldConf(const AosRundataPtr &rdata, const AosXmlTagPtr &colnumNode);

	void checkOutputIsTableOrDataset(const AosRundataPtr &rdata);
	static OmnString getObjid(OmnString const name);

private:
	OmnString getNextValueFieldName();

};

#endif

