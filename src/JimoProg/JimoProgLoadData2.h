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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JPLoadData_JPLoadData2_h
#define AOS_JPLoadData_JPLoadData2_h

//#include "JimoProg/Ptrs.h"
#include "JimoProg/JimoProg.h"
#include "Thread/Ptrs.h"
#include "SEInterfaces/GenericObj.h"
#include "JSON/JSON.h"

#include <vector>


class AosJPLoadData2 : public AosGenericObj,
						public AosJimoProg
{
	OmnDefineRCObject
private:
	string 					mTableName;
	vector<OmnString>			mTableNames;
	string 					mSwitchCaseProcName;

	vector<OmnString>			mDefaultProcs;
	map<OmnString, vector<OmnString> > 	mSubTableNameProcsMap;

	vector<OmnString>			mConfList;
	vector<OmnString>			mDataProcNameList;
public:
	AosJPLoadData2(const int ver);
	~AosJPLoadData2();

	virtual AosJimoPtr cloneJimo() const;
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	bool composeConf(
			const AosRundataPtr &rdata,
			const string obj_name,
			const string input_opt,
			const AosJimoProgObjPtr &prog);

	virtual bool createByJql(
			AosRundata *rdata,
			const OmnString &obj_name,
			const OmnString &jsonstr,
			const AosJimoProgObjPtr &prog);

	virtual bool runByJql(
			AosRundata *rdata,
			const OmnString &obj_name,
			const OmnString &jsonstr);

	virtual bool showByJql(
			AosRundata *rdata,
			const OmnString &obj_name,
			const OmnString &jsonstr);

	virtual bool dropByJql(
			AosRundata *rdata,
			const OmnString &obj_name,
			const OmnString &jsonstr);

	virtual AosXmlTagPtr createConfig();


	//
	virtual OmnString getObjType(AosRundata* rdata)
	{
		OmnShouldNeverComeHere;
		return "";
	}

	virtual bool parseJQL(
			AosRundata *rdata,
			AosJimoParserObj *jimo_parser,
			AosJimoProgObj *prog,
			bool &parsed,
			bool dft = false)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	virtual bool setJimoName(const OmnString& name)
	{
		OmnShouldNeverComeHere;
		return false;
	}

	virtual OmnString getJimoName() const
	{
		OmnShouldNeverComeHere;
		return "";
	}

    virtual OmnString getEnv(OmnString name)
    {
        return mParmName[name];
    }

private:
    map<OmnString, OmnString>   mParmName;

	bool				collectTableInfo(
							AosRundata *rdata,
							const OmnString &tablename,
							const OmnString &inputname,
							bool prime_table = true);

	map<string, string> useOutputfilterInput(
							const AosRundataPtr &rdata,
							map<string, string> &outputs,
							vector<JSONValue> &tasks,
							vector<JSONValue> &filtered_tasks);

	vector<JSONValue> 	composeTasks(
							const AosRundataPtr &rdata,
							vector<JSONValue> &tasks);

	map<string, vector<JSONValue> > getSameTypeTasks(
							const AosRundataPtr &rdata,
							const vector<JSONValue> &tasks);

	vector<JSONValue>	 divideTasksWithInputs(
							const AosRundataPtr &rdata,
							const vector<JSONValue> &tasks);

	bool				 composeDataprocs(
							const AosRundataPtr &rdata,
							JSONValue &task1,
							const JSONValue &task2);

	map<string, string> getInputsFromTask(
							const AosRundataPtr &rdata,
							const JSONValue &task);

	bool				getOutputsWithInputFromTask(
							const AosRundataPtr &rdata,
							const JSONValue &task,
							map<string, string> &key_inputs,
							vector<string> &value_outputs);

	AosXmlTagPtr		createRunStatDoc(
							AosRundata *rdata,
							const AosXmlTagPtr &statistic_def,
							const AosXmlTagPtr &def_doc,
							const AosXmlTagPtr &stat_doc);

	bool 				saveNewRunStatDoc(
							AosRundata *rdata,
							const AosXmlTagPtr &statistic_def,
							const OmnString &stat_name,
							AosXmlTagPtr &new_run_stat_doc);

	bool 				replaceFirstTask(
							vector<JSONValue> &repaired_tasks, 
							const AosRundataPtr rdata);
	string 				createSwitchCaseProc();
	
	virtual bool setConf(OmnString& confList);
	virtual vector<OmnString> getConfList();

	virtual bool setDataProcName(OmnString& name);
	virtual vector<OmnString> getDataProcNameList();  


};

#endif

