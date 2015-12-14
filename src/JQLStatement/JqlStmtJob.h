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
#ifndef AOS_JQLStatement_JqlStmtJob_H
#define AOS_JQLStatement_JqlStmtJob_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtJob : public AosJqlStatement
{
private:
	//data from JQLParser
	OmnString		mJobName;
	OmnString		mJobFileName;
	AosExprList*  	mTaskNames; 
	bool			mIsJobLog;
	AosJqlLimit*	mLimit;

public:
	OmnString		mErrmsg;

public:
	AosJqlStmtJob(const OmnString &errmsg);
	AosJqlStmtJob();
	~AosJqlStmtJob();

	//getter/setters
	void setJobName(OmnString name);
	void setJobFileName(OmnString name);
	void setTaskNames(AosExprList *task_names);
	void setIsLog(bool is_log);
	void setLimit(AosJqlLimit* limit);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createJob(const AosRundataPtr &rdata);  
	bool runJob(const AosRundataPtr &rdata);  
	bool waitJob(const AosRundataPtr &rdata);  
	bool runJobByFileName(const AosRundataPtr &rdata);
	bool showJobs(const AosRundataPtr &rdata);   
	bool describeJob(const AosRundataPtr &rdata);
	bool restartJob(const AosRundataPtr &rdata);   
	bool stopJob(const AosRundataPtr &rdata);   
	bool dropJob(const AosRundataPtr &rdata);   
	bool showJobStat(const AosRundataPtr &rdata);// Young: 2014/07/07
	bool showJobStatus(const AosRundataPtr &rdata);// Young: 2014/07/07
	bool showJobLogStat(const AosRundataPtr &rdata);// Young: 2014/07/07

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);

	static AosXmlTagPtr createTaskConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static bool createMapTaskConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def,
			const AosXmlTagPtr &rslt_tag);

	static bool createMapActionConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def,
			const AosXmlTagPtr &rslt_tag);

	static bool createReduceTaskConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def,
			const AosXmlTagPtr &rslt_tag);

	static bool createReduceActionConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def,
			const AosXmlTagPtr &rslt_tag);

	static AosXmlTagPtr createMapDataProcConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static AosXmlTagPtr createReduceDataProcConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static bool createDatasetsConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def,
			const AosXmlTagPtr &rslt_tag,
			const bool input_dataset);

	static AosXmlTagPtr createOutputDatasetConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static AosXmlTagPtr createInputDatasetConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static AosXmlTagPtr createDataScannerConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static AosXmlTagPtr createDataSchemaConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static AosXmlTagPtr createDataRecordConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static AosXmlTagPtr createDataFieldConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static AosXmlTagPtr createConnectorConf(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	static void modifyName(
			const AosXmlTagPtr &def,
			const AosXmlTagPtr &doc,
			const OmnString &aname,
			const OmnString &avalue);

	static AosXmlTagPtr getDoc(
			const AosRundataPtr &rdata,
			const OmnString &attrname, 
			const OmnString &avalue,
			const OmnString &otype);
};

#endif
